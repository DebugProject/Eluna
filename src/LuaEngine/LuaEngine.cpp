#include "LuaEngine.h"
#include "HookMgr.h"

#if PLATFORM == PLATFORM_UNIX
#include <dirent.h>
#endif

template<typename T> const char* GetTName() { return "UNK"; }
template<> const char* GetTName<Corpse>() { return "Corpse *"; }
template<> const char* GetTName<Unit>() { return "Unit *"; }
template<> const char* GetTName<Player>() { return "Player *"; }
template<> const char* GetTName<Creature>() { return "Creature *"; }
template<> const char* GetTName<TempSummon>() { return "TempSummon *"; }
template<> const char* GetTName<Pet>() { return "Pet *"; }
template<> const char* GetTName<GameObject>() { return "GameObject *"; }
template<> const char* GetTName<Group>() { return "Group *"; }
template<> const char* GetTName<Guild>() { return "Guild *"; }
template<> const char* GetTName<QueryResult>() { return "QueryResult *"; }
template<> const char* GetTName<Aura>() { return "Aura *"; }
template<> const char* GetTName<WorldPacket>() { return "WorldPacket *"; }
template<> const char* GetTName<Item>() { return "Item *"; }
template<> const char* GetTName<Spell>() { return "Spell *"; }
template<> const char* GetTName<Quest const>() { return "Quest *"; } // Needs testing
template<> const char* GetTName<Map>() { return "Map *"; }

extern "C" extern int luaopen_Eluna(lua_State* L);

void Eluna::StartEluna(bool restart)
{
    if (restart)
    {
        sHookMgr->OnEngineRestart();
        TC_LOG_INFO("misc", "Eluna::Restarting Lua Engine");

        if (LuaState)
        {
            // Unregisters and stops all timed events
            LuaEventMap::ScriptEventsResetAll();
            LuaEventData::RemoveAll();

            // Remove bindings
            for (std::map<int, std::vector<int> >::iterator itr = ServerEventBindings.begin(); itr != ServerEventBindings.end(); ++itr)
            {
                for (std::vector<int>::const_iterator it = itr->second.begin(); it != itr->second.end(); ++it)
                    luaL_unref(LuaState, LUA_REGISTRYINDEX, (*it));
                itr->second.clear();
            }
            CreatureEventBindings->Clear();
            CreatureGossipBindings->Clear();
            GameObjectEventBindings->Clear();
            GameObjectGossipBindings->Clear();
            ItemEventBindings->Clear();
            ItemGossipBindings->Clear();
            playerGossipBindings->Clear();

            lua_close(LuaState);
        }
    }
    else
        AddScriptHooks();

    LuaState = luaL_newstate();
    TC_LOG_INFO("server.loading", "Eluna Lua Engine loaded.");

    LoadedScripts loadedScripts;
    LoadDirectory("scripts", &loadedScripts);
    luaL_openlibs(LuaState);
    luaopen_Eluna(LuaState);

    uint32 count = 0;
    char filename[200];
    for (LoadedScripts::const_iterator itr = loadedScripts.begin(); itr !=  loadedScripts.end(); ++itr)
    {
        strcpy(filename, itr->c_str());
        if (luaL_loadfile(LuaState, filename) != 0)
        {
            TC_LOG_ERROR("server.loading", "Eluna::Error loading `%s`.", itr->c_str());
            report(LuaState);
        }
        else
        {
            int err = lua_pcall(LuaState, 0, 0, 0);
            if (err != 0 && err == LUA_ERRRUN)
            {
                TC_LOG_ERROR("server.loading", "Eluna::Error loading `%s`.", itr->c_str());
                report(LuaState);
            }
        }
        ++count;
    }

    if (restart)
    {
        //! Iterate over every supported source type (creature and gameobject)
        //! Not entirely sure how this will affect units in non-loaded grids.
        {
            TRINITY_READ_GUARD(HashMapHolder<Creature>::LockType, *HashMapHolder<Creature>::GetLock());
            HashMapHolder<Creature>::MapType const& m = ObjectAccessor::GetCreatures();
            for (HashMapHolder<Creature>::MapType::const_iterator iter = m.begin(); iter != m.end(); ++iter)
                if (iter->second->IsInWorld()) // must check?
                    // if(sEluna->CreatureEventBindings->GetBindMap(iter->second->GetEntry())) // update all AI or just Eluna?
                        iter->second->AIM_Initialize();
        }
    }

    TC_LOG_INFO("server.loading", "Eluna::Loaded %u Lua scripts..", count);
}

// Loads lua scripts from given directory
void Eluna::LoadDirectory(char* Dirname, LoadedScripts* lscr)
{
#ifdef WIN32
    HANDLE hFile;
    WIN32_FIND_DATA FindData;
    memset(&FindData, 0, sizeof(FindData));
    char SearchName[MAX_PATH];

    strcpy(SearchName, Dirname);
    strcat(SearchName, "\\*.*");

    hFile = FindFirstFile(SearchName, &FindData);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        TC_LOG_ERROR("server.loading", "Eluna::No `scripts` directory found! Creating a 'scripts' directory and restarting Eluna.");
        CreateDirectory("scripts", NULL);
        StartEluna(true);
        return;
    }

    FindNextFile(hFile, &FindData);
    while ( FindNextFile(hFile, &FindData) )
    {
        if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            strcpy(SearchName, Dirname);
            strcat(SearchName, "\\");
            strcat(SearchName, FindData.cFileName);
            LoadDirectory(SearchName, lscr);
        }
        else
        {
            std::string fname = Dirname;
            fname += "\\";
            fname += FindData.cFileName;
            size_t len = strlen(fname.c_str());
            int i = 0;
            char ext[MAX_PATH];
            while (len > 0)
            {
                ext[i++] = fname[--len];
                if (fname[len] == '.')
                    break;
            }
            ext[i++] = '\0';
            if (!_stricmp(ext,"aul."))
                lscr->insert(fname);
        }
    }
    FindClose(hFile);
#else    
    char* dir = strrchr(Dirname, '/');
    if (strcmp(Dirname, "..") == 0 || strcmp(Dirname, ".") == 0)
        return;

    if (dir && (strcmp(dir, "/..") == 0 || strcmp(dir, "/.") == 0 || strcmp(dir, "/.svn") == 0))
        return;

    struct dirent** list;
    int fileCount = scandir(Dirname, &list, 0, 0);

    if (fileCount <= 0 || !list)
        return;

    struct stat attributes;
    bool error;
    while (fileCount--)
    {
        char _path[200];
        sprintf(_path, "%s/%s", Dirname, list[fileCount]->d_name);
        if (stat(_path, &attributes) == -1)
        {
            error = true;
            TC_LOG_ERROR(LOG_FILTER_SERVER_LOADING, "Eluna::Error opening `%s`", _path);
        }
        else
            error = false;

        if (!error && S_ISDIR(attributes.st_mode))
            LoadDirectory((char*)_path, lscr);
        else
        {
            char* ext = strrchr(list[fileCount]->d_name, '.');
            if (ext && !strcmp(ext, ".lua"))
                lscr->luaFiles.insert(_path);
        }
        free(list[fileCount]);
    }
    free(list);
#endif
}

void Eluna::report(lua_State* L)
{
    const char* msg = lua_tostring(L, -1);
    while (msg)
    {
        lua_pop(L, -1);
        printf("\t%s\n",msg);
        msg = lua_tostring(L, -1);
    }
}

void Eluna::BeginCall(int fReference)
{
    lua_settop(LuaState, 0); //stack should be empty
    lua_rawgeti(LuaState, LUA_REGISTRYINDEX, (fReference));
}

bool Eluna::ExecuteCall(uint8 params, uint8 res)
{
    bool ret = true;
    int top = lua_gettop(LuaState);

    if (lua_type(LuaState, top-params) == LUA_TFUNCTION) // is function
    {
        if (lua_pcall(LuaState,params,res,0) )
        {
            report(LuaState);
            ret = false;
        }
    }
    else
    {
        ret = false;
        if (params > 0)
        {
            for (int i = top; i >= (top-params); i--)
            {
                if (!lua_isnone(LuaState, i) )
                    lua_remove(LuaState, i);
            }
        }
    }
    return ret;
}

void Eluna::EndCall(uint8 res)
{
    for (int i = res; i > 0; i--)
    {
        if (!lua_isnone(LuaState,res))
            lua_remove(LuaState,res);
    }
}

void Eluna::PushValue()
{
    lua_pushnil(LuaState);
}
void Eluna::PushValue(uint64 l)
{
    std::ostringstream ss;
    ss << "0x" << std::hex << l;
    PushValue(ss.str().c_str());
}
void Eluna::PushValue(int64 l)
{
    std::ostringstream ss;
    if (l < 0)
        ss << "-0x" << std::hex << -l;
    else
        ss << "0x" << std::hex << l;
    PushValue(ss.str().c_str());
}
void Eluna::PushValue(int i)
{
    lua_pushinteger(LuaState, i);
}
void Eluna::PushValue(uint32 u)
{
    lua_pushunsigned(LuaState, u);
}
void Eluna::PushValue(float f)
{
    lua_pushnumber(LuaState, f);
}
void Eluna::PushValue(double d)
{
    lua_pushnumber(LuaState, d);
}
void Eluna::PushValue(bool b)
{
    lua_pushboolean(LuaState, b);
}
void Eluna::PushValue(const char* str)
{
    lua_pushstring(LuaState, str);
}

/*
template<uint64> uint64 Eluna::CheckValue(int narg)
{
const char* c_str;
if (!L)
c_str = luaL_optstring(LuaState, narg, "0x0");
else
c_str = luaL_optstring(L, narg, "0x0");
return strtoul(c_str, NULL, 0);
}

int64 Eluna::CheckValue(lua_State* L, int narg)
{
const char* c_str;
if (!L)
c_str = luaL_optstring(LuaState, narg, "0x0");
else
c_str = luaL_optstring(L, narg, "0x0");
return strtol(c_str, NULL, 0);
}
*/

/*
Object* Eluna::CHECK_OBJECT(lua_State* L, int narg)
{
if (!L)
return ElunaTemplate<Object>::check(LuaState, narg);
else
return ElunaTemplate<Object>::check(L, narg);
}

WorldObject* Eluna::CHECK_WORLDOBJECT(lua_State* L, int narg)
{
if (!L)
return ElunaTemplate<WorldObject>::check(LuaState, narg);
else
return ElunaTemplate<WorldObject>::check(L, narg);
}

Unit* Eluna::CHECK_UNIT(lua_State* L, int narg)
{
WorldObject* obj = CheckValue<WorldObject*>(narg);
if(!obj)
return NULL;
return obj->ToUnit();
}

Player* Eluna::CHECK_PLAYER(lua_State* L, int narg)
{
WorldObject* obj = CheckValue<WorldObject*>(narg);
if(!obj)
return NULL;
return obj->ToPlayer();
}

Creature* Eluna::CHECK_CREATURE(lua_State* L, int narg)
{
WorldObject* obj = CheckValue<WorldObject*>(narg);
if(!obj)
return NULL;
return obj->ToCreature();
}

GameObject* Eluna::CHECK_GAMEOBJECT(lua_State* L, int narg)
{
WorldObject* obj = CheckValue<WorldObject*>(narg);
if(!obj)
return NULL;
return obj->ToGameObject();
}

Corpse* Eluna::CHECK_CORPSE(lua_State* L, int narg)
{
WorldObject* obj = CheckValue<WorldObject*>(narg);
if (!obj)
return NULL;
return obj->ToCorpse();
}

WorldPacket* Eluna::CHECK_PACKET(lua_State* L, int narg)
{
if (!L)
return ElunaTemplate<WorldPacket>::check(LuaState, narg);
else
return ElunaTemplate<WorldPacket>::check(L, narg);
}

Quest* Eluna::CHECK_QUEST(lua_State* L, int narg)
{
if (!L)
return ElunaTemplate<Quest>::check(LuaState, narg);
else
return ElunaTemplate<Quest>::check(L, narg);
}

Spell* Eluna::CHECK_SPELL(lua_State* L, int narg)
{
if (!L)
return ElunaTemplate<Spell>::check(LuaState, narg);
else
return ElunaTemplate<Spell>::check(L, narg);
}
*/

// Saves the function reference ID given to the register type's store for given entry under the given event
void Eluna::Register(uint8 regtype, uint32 id, uint32 evt, int functionRef)
{
    switch(regtype)
    {
    case REGTYPE_SERVER:
        if (evt < SERVER_EVENT_COUNT)
        {
            ServerEventBindings[evt].push_back(functionRef);
            return;
        }
        break;

    case REGTYPE_CREATURE:
        if (evt < CREATURE_EVENT_COUNT)
        {
            if (!sObjectMgr->GetCreatureTemplate(id))
            {
                luaL_error(LuaState, "Couldn't find a creature with (ID: %d)!", id);
                return;
            }

            sEluna->CreatureEventBindings->Insert(id, evt, functionRef);
            return;
        }
        break;

    case REGTYPE_CREATURE_GOSSIP:
        if (evt < GOSSIP_EVENT_COUNT)
        {
            if (!sObjectMgr->GetCreatureTemplate(id))
            {
                luaL_error(LuaState, "Couldn't find a creature with (ID: %d)!", id);
                return;
            }

            sEluna->CreatureGossipBindings->Insert(id, evt, functionRef);
            return;
        }
        break;

    case REGTYPE_GAMEOBJECT:
        if (evt < GAMEOBJECT_EVENT_COUNT)
        {
            if (!sObjectMgr->GetGameObjectTemplate(id))
            {
                luaL_error(LuaState, "Couldn't find a gameobject with (ID: %d)!", id);
                return;
            }

            sEluna->GameObjectEventBindings->Insert(id, evt, functionRef);
            return;
        }
        break;

    case REGTYPE_GAMEOBJECT_GOSSIP:
        if (evt < GOSSIP_EVENT_COUNT)
        {
            if (!sObjectMgr->GetGameObjectTemplate(id))
            {
                luaL_error(LuaState, "Couldn't find a gameobject with (ID: %d)!", id);
                return;
            }

            sEluna->GameObjectGossipBindings->Insert(id, evt, functionRef);
            return;
        }
        break;

    case REGTYPE_ITEM:
        if (evt < ITEM_EVENT_COUNT)
        {
            if (!sObjectMgr->GetItemTemplate(id))
            {
                luaL_error(LuaState, "Couldn't find a item with (ID: %d)!", id);
                return;
            }

            sEluna->ItemEventBindings->Insert(id, evt, functionRef);
            return;
        }
        break;

    case REGTYPE_ITEM_GOSSIP:
        if (evt < GOSSIP_EVENT_COUNT)
        {
            if (!sObjectMgr->GetItemTemplate(id))
            {
                luaL_error(LuaState, "Couldn't find a item with (ID: %d)!", id);
                return;
            }

            sEluna->ItemGossipBindings->Insert(id, evt, functionRef);
            return;
        }
        break;

    case REGTYPE_PLAYER_GOSSIP:
        if (evt < GOSSIP_EVENT_COUNT)
        {
            sEluna->playerGossipBindings->Insert(id, evt, functionRef);
            return;
        }
        break;

    default:
        luaL_error(LuaState, "Unknown register type (regtype %d, id %d, event %d)", regtype, id, evt);
        return;
    }
    luaL_error(LuaState, "Unknown event type (regtype %d, id %d, event %d)", regtype, id, evt);
}
void Eluna::ElunaBind::Clear()
{
    for (ElunaEntryMap::iterator itr = Bindings.begin(); itr != Bindings.end(); ++itr)
    {
        for (ElunaBindingMap::const_iterator it = itr->second.begin(); it != itr->second.end(); ++it)
            luaL_unref(sEluna->LuaState, LUA_REGISTRYINDEX, it->second);
        itr->second.clear();
    }
    Bindings.clear();
}
void Eluna::ElunaBind::Insert(uint32 entryId, uint32 eventId, int funcRef)
{
    if (Bindings[entryId][eventId])
    {
        luaL_error(sEluna->LuaState, "A function is already registered for entry (%d) event (%d)", entryId, eventId);
        luaL_unref(sEluna->LuaState, LUA_REGISTRYINDEX, funcRef); // free the unused ref
    }
    else
        Bindings[entryId][eventId] = funcRef;
}

UNORDERED_MAP<uint64, Eluna::LuaEventMap*> Eluna::LuaEventMap::LuaEventMaps;
UNORDERED_MAP<int, Eluna::LuaEventData*> Eluna::LuaEventData::LuaEvents;
UNORDERED_MAP<uint64, std::set<int> > Eluna::LuaEventData::EventIDs;

void Eluna::LuaEventMap::ScriptEventsResetAll()
{
    // GameObject events reset
    if (!LuaEventMaps.empty())
        for (UNORDERED_MAP<uint64, LuaEventMap*>::const_iterator itr = LuaEventMaps.begin(); itr != LuaEventMaps.end(); ++itr)
            if (itr->second)
                itr->second->ScriptEventsReset();
    // Global events reset
    sEluna->LuaWorldAI->ScriptEventsReset();
}
void Eluna::LuaEventMap::ScriptEventsReset()
{
    _time = 0;
    if (ScriptEventsEmpty())
        return;
    for (EventStore::const_iterator itr = _eventMap.begin(); itr != _eventMap.end();)
    {
        luaL_unref(sEluna->LuaState, LUA_REGISTRYINDEX, itr->second.funcRef);
        ++itr;
    }
    _eventMap.clear();
}
void Eluna::LuaEventMap::ScriptEventCancel(int funcRef)
{
    if (ScriptEventsEmpty())
        return;

    for (EventStore::iterator itr = _eventMap.begin(); itr != _eventMap.end();)
    {
        if (funcRef == itr->second.funcRef)
        {
            luaL_unref(sEluna->LuaState, LUA_REGISTRYINDEX, itr->second.funcRef);
            _eventMap.erase(itr++);
        }
        else
            ++itr;
    }
}
void Eluna::LuaEventMap::ScriptEventsExecute()
{
    if (ScriptEventsEmpty())
        return;

    for (EventStore::iterator itr = _eventMap.begin(); itr != _eventMap.end();)
    {
        if (itr->first > _time)
        {
            ++itr;
            continue;
        }

        OnScriptEvent(itr->second.funcRef, itr->second.delay, itr->second.calls);

        if (itr->second.calls != 1)
        {
            if (itr->second.calls > 1)
                itr->second.calls = itr->second.calls-1;
            _eventMap.insert(EventStore::value_type(_time + itr->second.delay, itr->second));
        }
        else
            luaL_unref(sEluna->LuaState, LUA_REGISTRYINDEX, itr->second.funcRef);
        _eventMap.erase(itr++);
    }
}

// Lua taxi helper functions
uint32 LuaTaxiMgr::nodeId = 500;
void LuaTaxiMgr::StartTaxi(Player* player, uint32 pathid)
{
    if (pathid >= sTaxiPathNodesByPath.size())
        return;

    TaxiPathNodeList const& path = sTaxiPathNodesByPath[pathid];
    if (path.size() < 2)
        return;

    std::vector<uint32> nodes;
    nodes.resize(2);
    nodes[0] = path[0].index;
    nodes[1] = path[path.size()-1].index;

    player->ActivateTaxiPathTo(nodes);
}
uint32 LuaTaxiMgr::AddPath(std::list<TaxiPathNodeEntry> nodes, uint32 mountA, uint32 mountH, uint32 price, uint32 pathId)
{
    if (nodes.size() < 2)
        return 0;
    if (!pathId)
        pathId = sTaxiPathNodesByPath.size();
    if (sTaxiPathNodesByPath.size() <= pathId)
        sTaxiPathNodesByPath.resize(pathId+1);
    sTaxiPathNodesByPath[pathId].clear();
    sTaxiPathNodesByPath[pathId].resize(nodes.size());
    uint32 startNode = nodeId;
    uint32 index = 0;
    for (std::list<TaxiPathNodeEntry>::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
    {
        TaxiPathNodeEntry entry = *it;
        entry.path = pathId;
        TaxiNodesEntry* nodeEntry = new TaxiNodesEntry();
        nodeEntry->ID = index;
        nodeEntry->map_id = entry.mapid;
        nodeEntry->MountCreatureID[0] = mountH;
        nodeEntry->MountCreatureID[1] = mountA;
        nodeEntry->x = entry.x;
        nodeEntry->y = entry.y;
        nodeEntry->z = entry.z;
        sTaxiNodesStore.SetEntry(nodeId, nodeEntry);
        entry.index = nodeId++;
        sTaxiPathNodesByPath[pathId].set(index++, TaxiPathNodePtr(new TaxiPathNodeEntry(entry)));
    }
    if (startNode >= nodeId)
        return 0;
    sTaxiPathSetBySource[startNode][nodeId-1] = TaxiPathBySourceAndDestination(pathId, price);
    return pathId;
}
