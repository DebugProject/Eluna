#ifndef __ELUNA__H
#define __ELUNA__H

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};
#include "HookMgr.h"
#include "ScriptPCH.h"
#include "Channel.h"
#include "GameObjectAI.h"
#include "Config.h"
#include "ObjectAccessor.h"
#include "World.h"
#include "swigluarun.h"

typedef std::set<std::string> LoadedScripts;
template<typename T> const char* GetTName();

class Eluna
{
public:
    friend class ScriptMgr;
    lua_State* LuaState;

    typedef std::map<int, int> ElunaBindingMap;
    typedef UNORDERED_MAP<uint32, ElunaBindingMap> ElunaEntryMap;
    struct ElunaBind;
    std::map<int, std::vector<int> > ServerEventBindings;
    ElunaBind* CreatureEventBindings;
    ElunaBind* CreatureGossipBindings;
    ElunaBind* GameObjectEventBindings;
    ElunaBind* GameObjectGossipBindings;
    ElunaBind* ItemEventBindings;
    ElunaBind* ItemGossipBindings;
    ElunaBind* playerGossipBindings;

    void StartEluna(bool restart);
    void report();
    void Register(uint8 reg, uint32 id, uint32 evt, int func);
    void BeginCall(int funcRef);
    bool ExecuteCall();
    void EndCall();
    static void LoadDirectory(char* directory, LoadedScripts* scr);
    // Pushes
    void PushValue(); // nil
    void PushValue(uint64);
    void PushValue(int64);
    void PushValue(int);
    void PushValue(uint32);
    void PushValue(bool);
    void PushValue(float);
    void PushValue(double);
    void PushValue(const char*);
    template<class T> void PushValue(const T* ptr, int gc = 0)
    {
        swig_type_info * pTypeInfo = SWIG_TypeQuery(LuaState, GetTName<T>());
        if (pTypeInfo)
            SWIG_NewPointerObj(LuaState, ptr, pTypeInfo, gc);
        else
            PushValue();
    }

    // Usage: Type var = sEluna->CheckPointer<Type>(narg);
    // Example:
    // Player* var = sEluna->CheckPointer<Player*>(narg);
    // uint32 var = sEluna->CheckPointer<uint32>(narg);
    template<typename T> T CheckValue(int narg)
    {
        T* ptrHold = static_cast<T*>(lua_touserdata(LuaState, narg));
        if (!ptrHold)
            return NULL;
        return *ptrHold;
    }
    template<> uint64 CheckValue<uint64>(int narg)
    {
        return strtoul(luaL_optstring(LuaState, narg, "0x0"), NULL, 0);
    }
    template<> int64 CheckValue<int64>(int narg)
    {
        return strtol(luaL_optstring(LuaState, narg, "0x0"), NULL, 0);
    }
    template<> int CheckValue<int>(int narg)
    {
        return luaL_checkinteger(LuaState, narg);
    }
    template<> uint32 CheckValue<uint32>(int narg)
    {
        return luaL_checkunsigned(LuaState, narg);
    }
    template<> bool CheckValue<bool>(int narg)
    {
        if (lua_isboolean(LuaState, narg))
            return lua_toboolean(LuaState, narg);
        return luaL_optint(LuaState, narg, false);
    }
    template<> float CheckValue<float>(int narg)
    {
        return luaL_checknumber(LuaState, narg);
    }
    template<> double CheckValue<double>(int narg)
    {
        return luaL_checknumber(LuaState, narg);
    }
    template<> const char* CheckValue<const char*>(int narg)
    {
        return luaL_checkstring(LuaState, narg);
    }


    template<typename T> T OptValue(int narg, T opt = NULL)
    {
        T* ptrHold = static_cast<T*>(lua_touserdata(LuaState, narg));
        if (!ptrHold)
            return opt;
        return *ptrHold;
    }
    template<> uint64 OptValue<uint64>(int narg, uint64 opt)
    {
        if (!lua_isstring(LuaState, narg))
            return opt;
        return strtoul(luaL_optstring(LuaState, narg, "0x0"), NULL, 0);
    }
    template<> int64 OptValue<int64>(int narg, int64 opt)
    {
        if (!lua_isstring(LuaState, narg))
            return opt;
        return strtol(luaL_optstring(LuaState, narg, "0x0"), NULL, 0);
    }
    template<> int OptValue<int>(int narg, int opt)
    {
        return luaL_optinteger(LuaState, narg, opt);
    }
    template<> uint32 OptValue<uint32>(int narg, uint32 opt)
    {
        return luaL_optunsigned(LuaState, narg, opt);
    }
    template<> bool OptValue<bool>(int narg, bool opt)
    {
        if (lua_isboolean(LuaState, narg))
            return lua_toboolean(LuaState, narg);
        return luaL_optint(LuaState, narg, opt);
    }
    template<> float OptValue<float>(int narg, float opt)
    {
        return luaL_optnumber(LuaState, narg, opt);
    }
    template<> double OptValue<double>(int narg, double opt)
    {
        return luaL_optnumber(LuaState, narg, opt);
    }
    template<> const char* OptValue<const char*>(int narg, const char* opt)
    {
        return luaL_optstring(LuaState, narg, opt);
    }

    // Creates new binding stores
    Eluna()
    {
        LuaState = NULL;

        for (int i = 0; i < SERVER_EVENT_COUNT; ++i)
        {
            std::vector<int> _vector;
            ServerEventBindings.insert(std::pair<int, std::vector<int> >(i, _vector));
        }
        CreatureEventBindings = new ElunaBind;
        CreatureGossipBindings = new ElunaBind;
        GameObjectEventBindings = new ElunaBind;
        GameObjectGossipBindings = new ElunaBind;
        ItemEventBindings = new ElunaBind;
        ItemGossipBindings = new ElunaBind;
        playerGossipBindings = new ElunaBind;
    }

    ~Eluna()
    {
        for (std::map<int, std::vector<int> >::iterator itr = ServerEventBindings.begin(); itr != ServerEventBindings.end(); ++itr)
        {
            for (std::vector<int>::iterator it = itr->second.begin(); it != itr->second.end(); ++it)
                luaL_unref(LuaState, LUA_REGISTRYINDEX, (*it));
            itr->second.clear();
        }
        ServerEventBindings.clear();
        CreatureEventBindings->Clear();
        CreatureGossipBindings->Clear();
        GameObjectEventBindings->Clear();
        GameObjectGossipBindings->Clear();
        ItemEventBindings->Clear();
        ItemGossipBindings->Clear();
        playerGossipBindings->Clear();

        lua_close(LuaState); // Closing
    }

    struct ElunaBind
    {
        bool Call(uint32 entryId, uint32 eventId);
        void Clear(); // unregisters all registered functions and clears all registered events from the bind std::maps (reset)
        void Insert(uint32 entryId, uint32 eventId, int funcRef); // Inserts a new registered event

        // Gets the function ref of an entry for an event
        int GetBind(uint32 entryId, uint32 eventId)
        {
            ElunaEntryMap::iterator itr = Bindings.find(entryId);
            if (itr == Bindings.end())
                return 0;

            return itr->second[eventId];
        }

        // Gets the binding std::map containing all registered events with the function refs for the entry
        ElunaBindingMap* GetBindMap(uint32 entryId) 
        {
            ElunaEntryMap::iterator itr = Bindings.find(entryId);
            if (itr == Bindings.end())
                return NULL;

            return &itr->second;
        }

        ElunaEntryMap Bindings; // Binding store Bindings[entryId][eventId] = funcRef;
    };

    class NearestTypeWithEntryInRangeCheck // not self
    {
    public:
        NearestTypeWithEntryInRangeCheck(WorldObject const* obj, float range, TypeID typeId, uint32 entry = 0) : i_obj(obj), i_range(range), i_typeId(typeId), i_entry(entry)
        {
        }

        bool operator()(WorldObject* u)
        {
            if (u->GetTypeId() == i_typeId && i_obj->GetGUID() != u->GetGUID() && (!i_entry || u->GetEntry() == i_entry) && i_obj->IsWithinDistInMap(u, i_range))
            {
                if (Unit* unit = u->ToUnit())
                {
                    if (!unit->IsAlive())
                        return false;
                }

                i_range = i_obj->GetDistance(u);
                return true;
            }

            return false;
        }
    private:
        WorldObject const* i_obj;
        float i_range;
        TypeID i_typeId;
        uint32 i_entry;

        NearestTypeWithEntryInRangeCheck(NearestTypeWithEntryInRangeCheck const&);
    };
};
#define sEluna ACE_Singleton<Eluna, ACE_Null_Mutex>::instance()

class LuaTaxiMgr
{
private:
    static uint32 nodeId;
public:
    static void StartTaxi(Player* player, uint32 pathid);
    static uint32 AddPath(std::list<TaxiPathNodeEntry> nodes, uint32 mountA, uint32 mountH, uint32 price = 0, uint32 pathId = 0);
};
#endif
