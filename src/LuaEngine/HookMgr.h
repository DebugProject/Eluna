#include "ScriptMgr.h"
#include "EventProcessor.h"
#include "Item.h"
#include "Unit.h"

#ifndef LUAHOOKS_H
#define LUAHOOKS_H

#define SCRIPTLOOP_START(E) \
    for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[E].begin(); \
    itr != sEluna->ServerEventBindings[E].end(); ++itr) \
{ \
    sEluna->BeginCall((*itr)); \
    sEluna->PushValue(E);
#define SCRIPTLOOP_END() \
    }

enum RegisterTypes
{
    REGTYPE_SERVER,
    REGTYPE_CREATURE,
    REGTYPE_CREATURE_GOSSIP,
    REGTYPE_GAMEOBJECT,
    REGTYPE_GAMEOBJECT_GOSSIP,
    REGTYPE_ITEM,
    REGTYPE_ITEM_GOSSIP,
    REGTYPE_PLAYER_GOSSIP,
    REGTYPE_COUNT
};

// RegisterServerHook(EventId, function)
enum ServerEvents
{
    // Player
    PLAYER_EVENT_ON_CHARACTER_CREATE        =     1,            // (event, player)
    PLAYER_EVENT_ON_CHARACTER_DELETE        =     2,            // (event, guid)
    PLAYER_EVENT_ON_LOGIN                   =     3,            // (event, player)
    PLAYER_EVENT_ON_LOGOUT                  =     4,            // (event, player)
    PLAYER_EVENT_ON_SPELL_CAST              =     5,            // (event, player, spell, skipCheck)
    PLAYER_EVENT_ON_KILL_PLAYER             =     6,            // (event, killer, killed)
    PLAYER_EVENT_ON_KILL_CREATURE           =     7,            // (event, killer, killed)
    PLAYER_EVENT_ON_KILLED_BY_CREATURE      =     8,            // (event, killer, killed)
    PLAYER_EVENT_ON_DUEL_REQUEST            =     9,            // (event, target, challenger)
    PLAYER_EVENT_ON_DUEL_START              =     10,           // (event, player1, player2)
    PLAYER_EVENT_ON_DUEL_END                =     11,           // (event, winner, loser, type)
    PLAYER_EVENT_ON_GIVE_XP                 =     12,           // (event, player, amount, victim)
    PLAYER_EVENT_ON_LEVEL_CHANGE            =     13,           // (event, player, oldLevel)
    PLAYER_EVENT_ON_MONEY_CHANGE            =     14,           // (event, player, amount)
    PLAYER_EVENT_ON_REPUTATION_CHANGE       =     15,           // (event, player, factionId, standing, incremental)
    PLAYER_EVENT_ON_TALENTS_CHANGE          =     16,           // (event, player, points)
    PLAYER_EVENT_ON_TALENTS_RESET           =     17,           // (event, player, noCost)
    PLAYER_EVENT_ON_CHAT                    =     18,           // (event, player, msg, Type, lang) - Can return false
    PLAYER_EVENT_ON_WHISPER                 =     19,           // (event, player, msg, Type, lang, receiver)
    PLAYER_EVENT_ON_GROUP_CHAT              =     20,           // (event, player, msg, Type, lang, group) - Can return false
    PLAYER_EVENT_ON_GUILD_CHAT              =     21,           // (event, player, msg, Type, lang, guild) - Can return false
    PLAYER_EVENT_ON_CHANNEL_CHAT            =     22,           // (event, player, msg, Type, lang, channel) - Can return false
    PLAYER_EVENT_ON_EMOTE                   =     23,           // (event, player, emote) - Not triggered on any known emote
    PLAYER_EVENT_ON_TEXT_EMOTE              =     24,           // (event, player, textEmote, emoteNum, guid)
    PLAYER_EVENT_ON_SAVE                    =     25,           // (event, player)
    PLAYER_EVENT_ON_BIND_TO_INSTANCE        =     26,           // (event, player, difficulty, mapid, permanent)
    PLAYER_EVENT_ON_UPDATE_ZONE             =     27,           // (event, player, newZone, newArea)
    PLAYER_EVENT_ON_MAP_CHANGE              =     28,           // (event, player)

    // Guild
    GUILD_EVENT_ON_ADD_MEMBER               =     29,           // (event, guild, player, rank)
    GUILD_EVENT_ON_REMOVE_MEMBER            =     30,           // (event, guild, isDisbanding, isKicked)
    GUILD_EVENT_ON_MOTD_CHANGE              =     31,           // (event, guild, newMotd)
    GUILD_EVENT_ON_INFO_CHANGE              =     32,           // (event, guild, newInfo)
    GUILD_EVENT_ON_CREATE                   =     33,           // (event, guild, leader, name)
    GUILD_EVENT_ON_DISBAND                  =     34,           // (event, guild)
    GUILD_EVENT_ON_MONEY_WITHDRAW           =     35,           // (event, guild, player, amount, isRepair)
    GUILD_EVENT_ON_MONEY_DEPOSIT            =     36,           // (event, guild, player, amount)
    GUILD_EVENT_ON_ITEM_MOVE                =     37,           // (event, guild, player, item, isSrcBank, srcContainer, srcSlotId, isDestBank, destContainer, destSlotId)
    GUILD_EVENT_ON_EVENT                    =     38,           // (event, guild, eventType, plrGUIDLow1, plrGUIDLow2, newRank)
    GUILD_EVENT_ON_BANK_EVENT               =     39,           // (event, guild, eventType, tabId, playerGUIDLow, itemOrMoney, itemStackCount, destTabId)

    // Server
    SERVER_EVENT_ON_NETWORK_START           =     40,           // Not Implemented
    SERVER_EVENT_ON_NETWORK_STOP            =     41,           // Not Implemented
    SERVER_EVENT_ON_SOCKET_OPEN             =     42,           // Not Implemented
    SERVER_EVENT_ON_SOCKET_CLOSE            =     43,           // Not Implemented
    SERVER_EVENT_ON_PACKET_RECEIVE          =     44,           // Not Implemented
    SERVER_EVENT_ON_PACKET_RECEIVE_UNKNOWN  =     45,           // Not Implemented
    SERVER_EVENT_ON_PACKET_SEND             =     46,           // Not Implemented

    // World
    WORLD_EVENT_ON_OPEN_STATE_CHANGE        =     47,           // (event, open)
    WORLD_EVENT_ON_CONFIG_LOAD              =     48,           // (event, reload)
    WORLD_EVENT_ON_MOTD_CHANGE              =     49,           // (event, newMOTD)
    WORLD_EVENT_ON_SHUTDOWN_INIT            =     50,           // (event, code, mask)
    WORLD_EVENT_ON_SHUTDOWN_CANCEL          =     51,           // (event)
    WORLD_EVENT_ON_UPDATE                   =     52,           // (event, diff)
    WORLD_EVENT_ON_STARTUP                  =     53,           // (event)
    WORLD_EVENT_ON_SHUTDOWN                 =     54,           // (event)

    // Eluna
    ELUNA_EVENT_ON_RESTART                  =     55,           // (event)

    // Map
    MAP_EVENT_ON_CREATE                     =     56,           // Not Implemented
    MAP_EVENT_ON_DESTROY                    =     57,           // Not Implemented
    MAP_EVENT_ON_LOAD                       =     58,           // Not Implemented
    MAP_EVENT_ON_UNLOAD                     =     59,           // Not Implemented
    MAP_EVENT_ON_PLAYER_ENTER               =     60,           // Not Implemented
    MAP_EVENT_ON_PLAYER_LEAVE               =     61,           // Not Implemented
    MAP_EVENT_ON_UPDATE                     =     62,           // Not Implemented

    // Area trigger
    TRIGGER_EVENT_ON_TRIGGER                =     63,           // (event, player, triggerId)

    // Weather
    WEATHER_EVENT_ON_CHANGE                 =     64,           // (event, weather, state, grade)

    // Auction house
    AUCTION_EVENT_ON_ADD                    =     65,           // Not Implemented
    AUCTION_EVENT_ON_REMOVE                 =     66,           // Not Implemented
    AUCTION_EVENT_ON_SUCCESFUL              =     67,           // Not Implemented
    AUCTION_EVENT_ON_EXPIRE                 =     68,           // Not Implemented

    // Group
    GROUP_EVENT_ON_MEMBER_ADD               =     69,           // (event, group, guid)
    GROUP_EVENT_ON_MEMBER_INVITE            =     70,           // (event, group, guid)
    GROUP_EVENT_ON_MEMBER_REMOVE            =     71,           // (event, group, guid, method, kicker, reason)
    GROUP_EVENT_ON_LEADER_CHANGE            =     72,           // (event, group, newLeaderGuid, oldLeaderGuid)
    GROUP_EVENT_ON_DISBAND                  =     73,           // (event, group)

    // Custom
    PLAYER_EVENT_ON_EQUIP                   =     74,           // (event, player, item, bag, slot)
    PLAYER_EVENT_ON_FIRST_LOGIN             =     75,           // (event, player)
    PLAYER_EVENT_ON_CAN_USE_ITEM            =     76,           // (event, player, itemEntry)
    PLAYER_EVENT_ON_LOOT_ITEM               =     77,           // (event, player, item, count)
    PLAYER_EVENT_ON_ENTER_COMBAT            =     78,           // (event, player, enemy)
    PLAYER_EVENT_ON_LEAVE_COMBAT            =     79,           // (event, player)
    PLAYER_EVENT_ON_REPOP                   =     80,           // (event, player)
    PLAYER_EVENT_ON_RESURRECT               =     81,           // (event, player)

    SERVER_EVENT_COUNT
};

// RegisterCreatureEvent(entry, EventId, function)
enum CreatureEvents
{
    CREATURE_EVENT_ON_ENTER_COMBAT                    = 1,      // (event, creature, target)
    CREATURE_EVENT_ON_LEAVE_COMBAT                    = 2,      // (event, creature)
    CREATURE_EVENT_ON_TARGET_DIED                     = 3,      // (event, creature, victim)
    CREATURE_EVENT_ON_DIED                            = 4,      // (event, creature, killer)
    CREATURE_EVENT_ON_SPAWN                           = 5,      // (event, creature)
    CREATURE_EVENT_ON_REACH_WP                        = 6,      // (event, creature, type, id)
    CREATURE_EVENT_ON_AIUPDATE                        = 7,      // (event, creature, diff)
    CREATURE_EVENT_ON_RECEIVE_EMOTE                   = 8,      // (event, creature, player, emoteid)
    CREATURE_EVENT_ON_DAMAGE_TAKEN                    = 9,      // (event, creature, attacker, damage)
    CREATURE_EVENT_ON_PRE_COMBAT                      = 10,     // (event, creature, target)
    CREATURE_EVENT_ON_ATTACKED_AT                     = 11,     // (event, creature, attacker)
    CREATURE_EVENT_ON_OWNER_ATTACKED                  = 12,     // (event, creature, target)
    CREATURE_EVENT_ON_OWNER_ATTACKED_AT               = 13,     // (event, creature, attacker)
    CREATURE_EVENT_ON_HIT_BY_SPELL                    = 14,     // (event, creature, caster, spellid)
    CREATURE_EVENT_ON_SPELL_HIT_TARGET                = 15,     // (event, creature, target, spellid)
    CREATURE_EVENT_ON_SPELL_CLICK                     = 16,     // (event, creature, clicker)
    CREATURE_EVENT_ON_CHARMED                         = 17,     // (event, creature, apply)
    CREATURE_EVENT_ON_POSSESS                         = 18,     // (event, creature, apply)
    CREATURE_EVENT_ON_JUST_SUMMONED_CREATURE          = 19,     // (event, creature, summon)
    CREATURE_EVENT_ON_SUMMONED_CREATURE_DESPAWN       = 20,     // (event, creature, summon)
    CREATURE_EVENT_ON_SUMMONED_CREATURE_DIED          = 21,     // (event, creature, summon, killer)
    CREATURE_EVENT_ON_SUMMONED                        = 22,     // (event, creature, summoner)
    CREATURE_EVENT_ON_RESET                           = 23,     // (event, creature)
    CREATURE_EVENT_ON_REACH_HOME                      = 24,     // (event, creature)
    CREATURE_EVENT_ON_CAN_RESPAWN                     = 25,     // (event, creature)
    CREATURE_EVENT_ON_CORPSE_REMOVED                  = 26,     // (event, creature, respawndelay)
    CREATURE_EVENT_ON_MOVE_IN_LOS                     = 27,     // (event, creature, unit)
    CREATURE_EVENT_ON_VISIBLE_MOVE_IN_LOS             = 28,     // (event, creature, unit)
    CREATURE_EVENT_ON_PASSANGER_BOARDED               = 29,     // (event, creature, passanger, seatid, apply)
    CREATURE_EVENT_ON_DUMMY_EFFECT                    = 30,     // (event, caster, spellid, effindex, creature)
    CREATURE_EVENT_ON_QUEST_ACCEPT                    = 31,     // (event, player, creature, quest)
    CREATURE_EVENT_ON_QUEST_SELECT                    = 32,     // (event, player, creature, quest)
    CREATURE_EVENT_ON_QUEST_COMPLETE                  = 33,     // (event, player, creature, quest)
    CREATURE_EVENT_ON_QUEST_REWARD                    = 34,     // (event, player, creature, quest, opt)
    CREATURE_EVENT_ON_DIALOG_STATUS                   = 35,     // (event, player, creature)
    CREATURE_EVENT_COUNT
};

// RegisterGameObjectEvent(entry, EventId, function)
enum GameObjectEvents
{
    GAMEOBJECT_EVENT_ON_AIUPDATE                    = 1,        // (event, go, diff)
    GAMEOBJECT_EVENT_ON_RESET                       = 2,        // (event, go)
    GAMEOBJECT_EVENT_ON_DUMMY_EFFECT                = 3,        // (event, caster, spellid, effindex, go)
    GAMEOBJECT_EVENT_ON_QUEST_ACCEPT                = 4,        // (event, player, go, quest)
    GAMEOBJECT_EVENT_ON_QUEST_REWARD                = 5,        // (event, player, go, quest, opt)
    GAMEOBJECT_EVENT_ON_DIALOG_STATUS               = 6,        // (event, player, go)
    GAMEOBJECT_EVENT_ON_DESTROYED                   = 7,        // (event, go, player)
    GAMEOBJECT_EVENT_ON_DAMAGED                     = 8,        // (event, go, player)
    GAMEOBJECT_EVENT_ON_LOOT_STATE_CHANGE           = 9,        // (event, go, state, unit)
    GAMEOBJECT_EVENT_ON_GO_STATE_CHANGED            = 10,       // (event, go, state)
    GAMEOBJECT_EVENT_COUNT
};

// RegisterItemEvent(entry, EventId, function)
enum ItemEvents
{
    ITEM_EVENT_ON_DUMMY_EFFECT                      = 1,        // (event, caster, spellid, effindex, item)
    ITEM_EVENT_ON_USE                               = 2,        // (event, player, item, target)
    ITEM_EVENT_ON_QUEST_ACCEPT                      = 3,        // (event, player, item, quest)
    ITEM_EVENT_ON_EXPIRE                            = 4,        // (event, player, itemid)
    ITEM_EVENT_COUNT
};

// RegisterCreatureGossipEvent(entry, EventId, function)
// RegisterGameObjectGossipEvent(entry, EventId, function)
// RegisterItemGossipEvent(entry, EventId, function)
// RegisterPlayerGossipEvent(menu_id, EventId, function)
enum GossipEvents
{
    GOSSIP_EVENT_ON_HELLO                           = 1,        // (event, player, object) - Object is the Creature/GameObject/Item
    GOSSIP_EVENT_ON_SELECT                          = 2,        // (event, player, object, sender, intid, code, menu_id) - Object is the Creature/GameObject/Item/Player, menu_id is only for player gossip
    GOSSIP_EVENT_COUNT
};

class Eluna_CreatureScript;
class Eluna_GameObjectScript;
class Eluna_WorldScript;
class HookScript;

class HookMgr
{
public:
    typedef std::set<HookScript*> HookPointerSet;
    HookPointerSet hookPointers;

    Eluna_CreatureScript* LuaCreatureAI;
    Eluna_GameObjectScript* LuaGameObjectAI;
    Eluna_WorldScript* LuaWorldAI;
    void AddScriptHooks();

    HookMgr()
    {
    }

    // misc
    void OnLootItem(Player* player, Item* item, uint32 count, uint64 guid);
    void OnFirstLogin(Player* player);
    void OnEquip(Player* player, Item* item, uint8 bag, uint8 slot);
    void OnRepop(Player* player);
    void OnResurrect(Player* player);
    InventoryResult OnCanUseItem(const Player* player, uint32 itemEntry);
    void HandleGossipSelectOption(Player* player, uint64 guid, uint32 sender, uint32 action, std::string code, uint32 menuId);
    void OnEngineRestart();
    // item
    bool OnDummyEffect(Unit* caster, uint32 spellId, SpellEffIndex effIndex, Item* target);
    bool OnQuestAccept(Player* player, Item* item, Quest const* quest);
    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets);
    bool OnExpire(Player* player, ItemTemplate const* proto);
    bool OnGossipSelect(Player* player, Item* item, uint32 sender, uint32 action);
    bool OnGossipSelectCode(Player* player, Item* item, uint32 sender, uint32 action, const char* code);
    // creature
    bool OnDummyEffect(Unit* caster, uint32 spellId, SpellEffIndex effIndex, Creature* target);
    bool OnGossipHello(Player* player, Creature* creature);
    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action);
    bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code);
    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest);
    bool OnQuestSelect(Player* player, Creature* creature, Quest const* quest);
    bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest);
    bool OnQuestReward(Player* player, Creature* creature, Quest const* quest, uint32 opt);
    uint32 GetDialogStatus(Player* player, Creature* creature);
    // gameobject
    bool OnDummyEffect(Unit* caster, uint32 spellId, SpellEffIndex effIndex, GameObject* target);
    bool OnGossipHello(Player* player, GameObject* go);
    bool OnGossipSelect(Player* player, GameObject* go, uint32 sender, uint32 action);
    bool OnGossipSelectCode(Player* player, GameObject* go, uint32 sender, uint32 action, const char* code);
    bool OnQuestAccept(Player* player, GameObject* go, Quest const* quest);
    bool OnQuestReward(Player* player, GameObject* go, Quest const* quest, uint32 opt);
    uint32 GetDialogStatus(Player* player, GameObject* go);
    void OnDestroyed(GameObject* go, Player* player);
    void OnDamaged(GameObject* go, Player* player);
    void OnLootStateChanged(GameObject* go, uint32 state, Unit* unit);
    void OnGameObjectStateChanged(GameObject* go, uint32 state);
    // areatrigger
    bool OnTrigger(Player* player, AreaTriggerEntry const* trigger);
    // weather
    void OnChange(Weather* weather, WeatherState state, float grade);
    // condition
    bool OnConditionCheck(Condition* condition, ConditionSourceInfo& sourceInfo);
    // transport
    void OnAddPassenger(Transport* transport, Player* player);
    void OnAddCreaturePassenger(Transport* transport, Creature* creature);
    void OnRemovePassenger(Transport* transport, Player* player);
    void OnRelocate(Transport* transport, uint32 waypointId, uint32 mapId, float x, float y, float z);
};
#define sHookMgr ACE_Singleton<HookMgr, ACE_Null_Mutex>::instance()

class HookScript
{
public:
    HookScript()
    {
        sHookMgr->hookPointers.insert(this);
    }
    ~HookScript()
    {
        sHookMgr->hookPointers.erase(this);
    }
    // misc
    virtual void OnLootItem(Player* player, Item* item, uint32 count, uint64 guid) { }
    virtual void OnFirstLogin(Player* player) { }
    virtual void OnEquip(Player* player, Item* item, uint8 bag, uint8 slot) { }
    virtual void OnRepop(Player* player) { }
    virtual void OnResurrect(Player* player) { }
    virtual InventoryResult OnCanUseItem(Player* player, uint32 itemEntry) { return EQUIP_ERR_OK; }
    virtual void HandleGossipSelectOption(Player* player, uint64 guid, uint32 sender, uint32 action, std::string code, uint32 menuId) { }
    virtual void OnEngineRestart() { }
    // item
    virtual bool OnDummyEffect(Unit* caster, uint32 spellId, SpellEffIndex effIndex, Item* target) { return false; }
    virtual bool OnQuestAccept(Player* player, Item* item, Quest const* quest) { return false; }
    virtual bool OnUse(Player* player, Item* item, SpellCastTargets const& targets) { return false; }
    virtual bool OnExpire(Player* player, ItemTemplate const* proto) { return false; }
    virtual bool OnGossipSelect(Player* player, Item* item, uint32 sender, uint32 action) { return false; }
    virtual bool OnGossipSelectCode(Player* player, Item* item, uint32 sender, uint32 action, const char* code) { return false; }
    // creature
    virtual bool OnDummyEffect(Unit* caster, uint32 spellId, SpellEffIndex effIndex, Creature* target) { return false; }
    virtual bool OnGossipHello(Player* player, Creature* creature) { return false; }
    virtual bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) { return false; }
    virtual bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code) { return false; }
    virtual bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) { return false; }
    virtual bool OnQuestSelect(Player* player, Creature* creature, Quest const* quest) { return false; }
    virtual bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest) { return false; }
    virtual bool OnQuestReward(Player* player, Creature* creature, Quest const* quest, uint32 opt) { return false; }
    virtual uint32 GetDialogStatus(Player* player, Creature* creature) { return 100; }
    // gameobject
    virtual bool OnDummyEffect(Unit* caster, uint32 spellId, SpellEffIndex effIndex, GameObject* target) { return false; }
    virtual bool OnGossipHello(Player* player, GameObject* go) { return false; }
    virtual bool OnGossipSelect(Player* player, GameObject* go, uint32 sender, uint32 action) { return false; }
    virtual bool OnGossipSelectCode(Player* player, GameObject* go, uint32 sender, uint32 action, const char* code) { return false; }
    virtual bool OnQuestAccept(Player* player, GameObject* go, Quest const* quest) { return false; }
    virtual bool OnQuestReward(Player* player, GameObject* go, Quest const* quest, uint32 opt) { return false; }
    virtual uint32 GetDialogStatus(Player* player, GameObject* go) { return 100; }
    virtual void OnDestroyed(GameObject* go, Player* player) { }
    virtual void OnDamaged(GameObject* go, Player* player) { }
    virtual void OnLootStateChanged(GameObject* go, uint32 state, Unit* unit) { }
    virtual void OnGameObjectStateChanged(GameObject* go, uint32 state) { }
    // areatrigger
    virtual bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) { return false; }
    // weather
    virtual void OnChange(Weather* weather, WeatherState state, float grade) { }
    // condition
    virtual bool OnConditionCheck(Condition* condition, ConditionSourceInfo& sourceInfo) { return true; }
    // transport
    virtual void OnAddPassenger(Transport* transport, Player* player) { }
    virtual void OnAddCreaturePassenger(Transport* transport, Creature* creature) { }
    virtual void OnRemovePassenger(Transport* transport, Player* player) { }
    virtual void OnRelocate(Transport* transport, uint32 waypointId, uint32 mapId, float x, float y, float z) { }
};


class LuaEventMap
{
public:
    LuaEventMap() { }
    ~LuaEventMap() { ScriptEventsReset(); }

    struct eventData
    {
        int funcRef; uint32 delay; uint32 calls;
        eventData(int _funcRef, uint32 _delay, uint32 _calls) :
            funcRef(_funcRef), delay(_delay), calls(_calls) {}
    };

    typedef std::multimap<uint32, eventData> EventStore; // Not to use multimap? Can same function ref ID be used multiple times?

    virtual void OnScriptEvent(int funcRef, uint32 delay, uint32 calls) { }
    // Unregisters and stops all timed events
    static void ScriptEventsResetAll();
    void ScriptEventsReset();
    void ScriptEventCancel(int funcRef);
    void ScriptEventsExecute();

    // Gets the event map saved for a guid
    static LuaEventMap* GetEvents(WorldObject* obj)
    {
        if (!obj)
            return NULL;
        UNORDERED_MAP<uint64, LuaEventMap*>::const_iterator it = LuaEventMaps.find(obj->GetGUID());
        if (it != LuaEventMaps.end())
            return it->second;
        return NULL;
    }

    void ScriptEventsUpdate(uint32 time)
    {
        _time += time;
    }

    bool ScriptEventsEmpty() const
    {
        return _eventMap.empty();
    }

    void ScriptEventCreate(int funcRef, uint32 delay, uint32 calls)
    {
        _eventMap.insert(EventStore::value_type(_time + delay, eventData(funcRef, delay, calls)));
    }

    static UNORDERED_MAP<uint64, LuaEventMap*> LuaEventMaps; // Creature and gameobject timed events

private:
    EventStore _eventMap;
    uint32 _time;
};

struct LuaEventData : public BasicEvent, public LuaEventMap::eventData
{
    static UNORDERED_MAP<int, LuaEventData*> LuaEvents;
    static UNORDERED_MAP<uint64, std::set<int> > EventIDs;
    Unit* _unit;
    uint64 GUID;

    LuaEventData(int funcRef, uint32 delay, uint32 calls, Unit* unit) :
        _unit(unit), GUID(unit->GetGUID()), LuaEventMap::eventData(funcRef, delay, calls)
    {
        LuaEvents[funcRef] = this;
        EventIDs[unit->GetGUID()].insert(funcRef);
    }

    ~LuaEventData();

    static void RemoveAll()
    {
        for (UNORDERED_MAP<uint64, std::set<int> >::const_iterator it = EventIDs.begin(); it != EventIDs.end(); ++it)
        {
            if(it->second.empty())
                continue;
            for(std::set<int>::const_iterator itr = it->second.begin(); itr != it->second.end(); ++itr)
            {
                if(LuaEvents.find(*itr) == LuaEvents.end())
                    continue;
                if(LuaEvents[*itr]->_unit)
                {
                    LuaEvents[*itr]->_unit->m_Events.KillAllEvents(true);
                    break;
                }
            }
        }
        LuaEvents.clear();
        EventIDs.clear();
    }

    static void RemoveAll(Unit* unit)
    {
        if(!unit)
            return;
        unit->m_Events.KillAllEvents(true); // should delete the objects
        for (std::set<int>::const_iterator it = EventIDs[unit->GetGUID()].begin(); it != EventIDs[unit->GetGUID()].end(); ++it)
            LuaEvents.erase(*it); // deletes pointers
        EventIDs.erase(unit->GetGUID());
    }

    static void Remove(uint64 guid, int eventID)
    {
        if(LuaEvents.find(eventID) != LuaEvents.end())
        {
            LuaEvents[eventID]->to_Abort = true; // delete on next cycle
            LuaEvents.erase(eventID);
        }
        EventIDs[guid].erase(eventID);
    }

    bool Execute(uint64 time, uint32 diff); // Should NEVER execute on dead events
};
class Eluna_CreatureScript : public CreatureScript
{
public:
    Eluna_CreatureScript() : CreatureScript("SmartEluna_CreatureScript") { } // Smart suppressing error @startup 
    ~Eluna_CreatureScript() { }

    struct ScriptCreatureAI;
    CreatureAI* GetAI(Creature* creature) const OVERRIDE;
};
class Eluna_GameObjectScript : public GameObjectScript
{
public:
    Eluna_GameObjectScript() : GameObjectScript("SmartEluna_GameObjectScript") { } // Smart suppressing error @startup
    ~Eluna_GameObjectScript() { }

    struct ScriptGameObjectAI;
    GameObjectAI* GetAI(GameObject* gameObject) const OVERRIDE;
};
#endif
