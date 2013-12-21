#include "HookMgr.h"
#include "LuaEngine.h"

UNORDERED_MAP<uint64, LuaEventMap*> LuaEventMap::LuaEventMaps;
UNORDERED_MAP<int, LuaEventData*> LuaEventData::LuaEvents;
UNORDERED_MAP<uint64, std::set<int> > LuaEventData::EventIDs;

struct Eluna_CreatureScript::ScriptCreatureAI : ScriptedAI
{
    ScriptCreatureAI(Creature* creature) : ScriptedAI(creature) { }
    ~ScriptCreatureAI() { }

    //Called at World update tick
    void UpdateAI(uint32 diff) OVERRIDE
    {
        ScriptedAI::UpdateAI(diff);
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_AIUPDATE);
        sEluna->PushValue(me);
        sEluna->PushValue(diff);
        sEluna->ExecuteCall();
    }

    //Called for reaction at enter to combat if not in combat yet (enemy can be NULL)
    //Called at creature aggro either by MoveInLOS or Attack Start
    void EnterCombat(Unit* target) OVERRIDE
    {
        ScriptedAI::EnterCombat(target);
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_ENTER_COMBAT);
        sEluna->PushValue(me);
        sEluna->PushValue(target);
        sEluna->ExecuteCall();
    }

    // Called at any Damage from any attacker (before damage apply)
    void DamageTaken(Unit* attacker, uint32& damage) OVERRIDE
    {
        ScriptedAI::DamageTaken(attacker, damage);
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_DAMAGE_TAKEN);
        sEluna->PushValue(me);
        sEluna->PushValue(attacker);
        sEluna->PushValue(damage);
        sEluna->ExecuteCall();
    }

    //Called at creature death
    void JustDied(Unit* killer) OVERRIDE
    {
        ScriptedAI::JustDied(killer);
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_DIED);
        sEluna->PushValue(me);
        sEluna->PushValue(killer);
        sEluna->ExecuteCall();
    }

    //Called at creature killing another unit
    void KilledUnit(Unit* victim) OVERRIDE
    {
        ScriptedAI::KilledUnit(victim);
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_TARGET_DIED);
        sEluna->PushValue(me);
        sEluna->PushValue(victim);
        sEluna->ExecuteCall();
    }

    // Called when the creature summon successfully other creature
    void JustSummoned(Creature* summon) OVERRIDE
    {
        ScriptedAI::JustSummoned(summon);
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_JUST_SUMMONED_CREATURE);
        sEluna->PushValue(me);
        sEluna->PushValue(summon);
        sEluna->ExecuteCall();
    }

    // Called when a summoned creature is despawned
    void SummonedCreatureDespawn(Creature* summon) OVERRIDE
    {
        ScriptedAI::SummonedCreatureDespawn(summon);
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_SUMMONED_CREATURE_DESPAWN);
        sEluna->PushValue(me);
        sEluna->PushValue(summon);
        sEluna->ExecuteCall();
    }

    // Called when hit by a spell
    void SpellHit(Unit* caster, SpellInfo const* spell) OVERRIDE
    {
        ScriptedAI::SpellHit(caster, spell);
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_HIT_BY_SPELL);
        sEluna->PushValue(me);
        sEluna->PushValue(caster);
        sEluna->PushValue(spell->Id); // Pass spell object?
        sEluna->ExecuteCall();
    }

    // Called when spell hits a target
    void SpellHitTarget(Unit* target, SpellInfo const* spell) OVERRIDE
    {
        ScriptedAI::SpellHitTarget(target, spell);
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_SPELL_HIT_TARGET);
        sEluna->PushValue(me);
        sEluna->PushValue(target);
        sEluna->PushValue(spell->Id); // Pass spell object?
        sEluna->ExecuteCall();
    }

    //Called at waypoint reached or PointMovement end
    void MovementInform(uint32 type, uint32 id) OVERRIDE
    {
        ScriptedAI::MovementInform(type, id);
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_REACH_WP);
        sEluna->PushValue(me);
        sEluna->PushValue(type);
        sEluna->PushValue(id);
        sEluna->ExecuteCall();
    }

    // Called when AI is temporarily replaced or put back when possess is applied or removed
    void OnPossess(bool apply) OVERRIDE
    {
        ScriptedAI::OnPossess(apply);
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_POSSESS);
        sEluna->PushValue(me);
        sEluna->PushValue(apply);
        sEluna->ExecuteCall();
    }

    //Called at creature reset either by death or evade
    void Reset() OVERRIDE
    {
        ScriptedAI::Reset();
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_RESET);
        sEluna->PushValue(me);
        sEluna->ExecuteCall();
    }

    // Called before EnterCombat even before the creature is in combat.
    void AttackStart(Unit* target) OVERRIDE
    {
        ScriptedAI::AttackStart(target);
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_PRE_COMBAT);
        sEluna->PushValue(me);
        sEluna->PushValue(target);
        sEluna->ExecuteCall();
    }

    // Called in Creature::Update when deathstate = DEAD. Inherited classes may maniuplate the ability to respawn based on scripted events.
    bool CanRespawn() OVERRIDE
    {
        ScriptedAI::CanRespawn();
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_CAN_RESPAWN);
        sEluna->PushValue(me);
        sEluna->ExecuteCall();
        return true;
    }

    // Called for reaction at stopping attack at no attackers or targets
    void EnterEvadeMode() OVERRIDE
    {
        ScriptedAI::EnterEvadeMode();
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_LEAVE_COMBAT);
        sEluna->PushValue(me);
        sEluna->ExecuteCall();
    }

    // Called when the creature is summoned successfully by other creature
    void IsSummonedBy(Unit* summoner) OVERRIDE
    {
        ScriptedAI::IsSummonedBy(summoner);
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_SUMMONED);
        sEluna->PushValue(me);
        sEluna->PushValue(summoner);
        sEluna->ExecuteCall();
    }

    void SummonedCreatureDies(Creature* summon, Unit* killer) OVERRIDE
    {
        ScriptedAI::SummonedCreatureDies(summon, killer);
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_SUMMONED_CREATURE_DIED);
        sEluna->PushValue(me);
        sEluna->PushValue(summon);
        sEluna->PushValue(killer);
        sEluna->ExecuteCall();
    }

    // Called when the creature is target of hostile action: swing, hostile spell landed, fear/etc)
    void AttackedBy(Unit* attacker) OVERRIDE
    {
        ScriptedAI::AttackedBy(attacker);
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_ATTACKED_AT);
        sEluna->PushValue(me);
        sEluna->PushValue(attacker);
        sEluna->ExecuteCall();
    }

    // Called when creature is spawned or respawned (for reseting variables)
    void JustRespawned() OVERRIDE
    {
        ScriptedAI::JustRespawned();
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_SPAWN);
        sEluna->PushValue(me);
        sEluna->ExecuteCall();
    }

    void OnCharmed(bool apply) OVERRIDE
    {
        ScriptedAI::OnCharmed(apply);
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_CHARMED);
        sEluna->PushValue(me);
        sEluna->PushValue(apply);
        sEluna->ExecuteCall();
    }

    // Called at reaching home after evade
    void JustReachedHome() OVERRIDE
    {
        ScriptedAI::JustReachedHome();
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_REACH_HOME);
        sEluna->PushValue(me);
        sEluna->ExecuteCall();
    }

    // Called at text emote receive from player
    void ReceiveEmote(Player* player, uint32 emoteId) OVERRIDE
    {
        ScriptedAI::ReceiveEmote(player, emoteId);
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_RECEIVE_EMOTE);
        sEluna->PushValue(me);
        sEluna->PushValue(player);
        sEluna->PushValue(emoteId);
        sEluna->ExecuteCall();
    }

    // Called when owner takes damage
    void OwnerAttackedBy(Unit* attacker) OVERRIDE
    {
        ScriptedAI::OwnerAttackedBy(attacker);
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_OWNER_ATTACKED_AT);
        sEluna->PushValue(me);
        sEluna->PushValue(attacker);
        sEluna->ExecuteCall();
    }

    // Called when owner attacks something
    void OwnerAttacked(Unit* target) OVERRIDE
    {
        ScriptedAI::OwnerAttacked(target);
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_OWNER_ATTACKED);
        sEluna->PushValue(me);
        sEluna->PushValue(target);
        sEluna->ExecuteCall();
    }

    // called when the corpse of this creature gets removed
    void CorpseRemoved(uint32& respawnDelay) OVERRIDE
    {
        ScriptedAI::CorpseRemoved(respawnDelay);
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_CORPSE_REMOVED);
        sEluna->PushValue(me);
        sEluna->PushValue(respawnDelay);
        sEluna->ExecuteCall();
    }

    void PassengerBoarded(Unit* passenger, int8 seatId, bool apply) OVERRIDE
    {
        ScriptedAI::PassengerBoarded(passenger, seatId, apply);
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_PASSANGER_BOARDED);
        sEluna->PushValue(me);
        sEluna->PushValue(passenger);
        sEluna->PushValue(seatId);
        sEluna->PushValue(apply);
        sEluna->ExecuteCall();
    }

    void OnSpellClick(Unit* clicker, bool& result) OVERRIDE
    {
        ScriptedAI::OnSpellClick(clicker, result);
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_SPELL_CLICK);
        sEluna->PushValue(me);
        sEluna->PushValue(clicker);
        sEluna->PushValue(result);
        sEluna->ExecuteCall();
    }

    void MoveInLineOfSight(Unit* who) OVERRIDE
    {
        ScriptedAI::MoveInLineOfSight(who);
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_MOVE_IN_LOS);
        sEluna->PushValue(me);
        sEluna->PushValue(who);
        sEluna->ExecuteCall();
    }

    // Called if IsVisible(Unit* who) is true at each who move, reaction at visibility zone enter
    void MoveInLineOfSight_Safe(Unit* who)
    {
        ScriptedAI::MoveInLineOfSight_Safe(who);
        sEluna->CreatureEventBindings->Call(me->GetEntry(), CREATURE_EVENT_ON_VISIBLE_MOVE_IN_LOS);
        sEluna->PushValue(me);
        sEluna->PushValue(who);
        sEluna->ExecuteCall();
    }
};
CreatureAI* Eluna_CreatureScript::GetAI(Creature* creature) const OVERRIDE
{
    if (!sEluna->CreatureEventBindings->GetBindMap(creature->GetEntry()))
        return NULL;

    ScriptCreatureAI* luaCreatureAI = new ScriptCreatureAI(creature);
    return luaCreatureAI;
}

struct Eluna_GameObjectScript::ScriptGameObjectAI : public GameObjectAI, public LuaEventMap
{
    ScriptGameObjectAI(GameObject* _go) : GameObjectAI(_go), LuaEventMap() { }
    ~ScriptGameObjectAI()
    {
        LuaEventMap::LuaEventMaps.erase(go->GetGUID());
    }

    void UpdateAI(uint32 diff) OVERRIDE
    {
        ScriptEventsUpdate(diff);
        ScriptEventsExecute();
        sEluna->GameObjectEventBindings->Call(go->GetEntry(), GAMEOBJECT_EVENT_ON_AIUPDATE);
        sEluna->PushValue(go);
        sEluna->PushValue(diff);
        sEluna->ExecuteCall();
    }

    // executed when a timed event fires
    void OnScriptEvent(int funcRef, uint32 delay, uint32 calls) OVERRIDE
    {
        sEluna->BeginCall(funcRef);
        sEluna->PushValue(funcRef);
        sEluna->PushValue(delay);
        sEluna->PushValue(calls);
        sEluna->PushValue(go);
        sEluna->ExecuteCall();
    }

    void Reset() OVERRIDE
    {
        sEluna->GameObjectEventBindings->Call(go->GetEntry(), GAMEOBJECT_EVENT_ON_RESET);
        sEluna->PushValue(GAMEOBJECT_EVENT_ON_RESET);
        sEluna->PushValue(go);
        sEluna->ExecuteCall();
    }
};

GameObjectAI* Eluna_GameObjectScript::GetAI(GameObject* gameObject) const OVERRIDE
{
    if (!sEluna->GameObjectEventBindings->GetBindMap(gameObject->GetEntry()))
        return NULL;

    ScriptGameObjectAI* luaGameObjectAI = new ScriptGameObjectAI(gameObject);
    LuaEventMap::LuaEventMaps[gameObject->GetGUID()] = luaGameObjectAI;
    return luaGameObjectAI;
}

class Eluna_WorldScript : public WorldScript, public LuaEventMap
{
public:
    Eluna_WorldScript() : WorldScript("SmartEluna_WorldScript"), LuaEventMap() { }
    ~Eluna_WorldScript() { }

    void OnOpenStateChange(bool open) OVERRIDE
    {
        SCRIPTLOOP_START(WORLD_EVENT_ON_OPEN_STATE_CHANGE)
            sEluna->PushValue(open);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }

    void OnConfigLoad(bool reload) OVERRIDE
    {
        SCRIPTLOOP_START(WORLD_EVENT_ON_CONFIG_LOAD)
            sEluna->PushValue(reload);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }

    void OnMotdChange(std::string& newMotd) OVERRIDE
    {
        SCRIPTLOOP_START(WORLD_EVENT_ON_MOTD_CHANGE)
            sEluna->PushValue(newMotd.c_str());
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }

    void OnShutdownInitiate(ShutdownExitCode code, ShutdownMask mask) OVERRIDE
    {
        SCRIPTLOOP_START(WORLD_EVENT_ON_SHUTDOWN_INIT)
            sEluna->PushValue(code);
        sEluna->PushValue(mask);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }

    void OnShutdownCancel() OVERRIDE
    {
        SCRIPTLOOP_START(WORLD_EVENT_ON_SHUTDOWN_CANCEL)
            sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }

    void OnUpdate(uint32 diff) OVERRIDE
    {
        ScriptEventsUpdate(diff);
        ScriptEventsExecute();
        SCRIPTLOOP_START(WORLD_EVENT_ON_UPDATE)
            sEluna->PushValue(diff);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    // executed when a  timed event fires
    void OnScriptEvent(int funcRef, uint32 delay, uint32 calls) OVERRIDE
    {
        sEluna->BeginCall(funcRef);
        sEluna->PushValue(funcRef);
        sEluna->PushValue(delay);
        sEluna->PushValue(calls);
        sEluna->ExecuteCall();
    }

    void OnStartup() OVERRIDE
    {
        SCRIPTLOOP_START(WORLD_EVENT_ON_STARTUP)
            sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }

    void OnShutdown() OVERRIDE
    {
        SCRIPTLOOP_START(WORLD_EVENT_ON_SHUTDOWN)
            sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
};

void LuaEventMap::ScriptEventsResetAll()
{
    // GameObject events reset
    if (!LuaEventMaps.empty())
        for (UNORDERED_MAP<uint64, LuaEventMap*>::const_iterator itr = LuaEventMaps.begin(); itr != LuaEventMaps.end(); ++itr)
            if (itr->second)
                itr->second->ScriptEventsReset();
    // Global events reset
    sHookMgr->LuaWorldAI->ScriptEventsReset();
}
void LuaEventMap::ScriptEventsReset()
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
void LuaEventMap::ScriptEventCancel(int funcRef)
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
void LuaEventMap::ScriptEventsExecute()
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

LuaEventData::~LuaEventData()
{
    luaL_unref(sEluna->LuaState, LUA_REGISTRYINDEX, funcRef);
    LuaEvents.erase(funcRef);
    EventIDs[GUID].erase(funcRef);
}
bool LuaEventData::Execute(uint64 time, uint32 diff)
{
    sEluna->BeginCall(funcRef);
    sEluna->PushValue(funcRef);
    sEluna->PushValue(delay);
    sEluna->PushValue(calls);
    sEluna->PushValue(_unit);
    sEluna->ExecuteCall();
    if (calls && !--calls) // dont repeat anymore
    {
        Remove(GUID, funcRef);
        return true; // destory event
    }
    _unit->m_Events.AddEvent(this, _unit->m_Events.CalculateTime(delay));
    return false; // dont destory event
}

// Custom
class Eluna_HookScript : public HookScript
{
public:
    Eluna_HookScript() : HookScript() { }
    // misc
    void OnLootItem(Player* player, Item* item, uint32 count, uint64 guid)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_LOOT_ITEM)
            sEluna->PushValue(player);
        sEluna->PushValue(item);
        sEluna->PushValue(count);
        sEluna->PushValue(guid);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnFirstLogin(Player* player)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_FIRST_LOGIN)
            sEluna->PushValue(player);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnRepop(Player* player)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_REPOP)
            sEluna->PushValue(player);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnResurrect(Player* player)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_RESURRECT)
            sEluna->PushValue(player);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnEquip(Player* player, Item* item, uint8 bag, uint8 slot)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_EQUIP)
            sEluna->PushValue(player);
        sEluna->PushValue(item);
        sEluna->PushValue(bag);
        sEluna->PushValue(slot);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    InventoryResult OnCanUseItem(Player* player, uint32 itemEntry)
    {
        InventoryResult result = EQUIP_ERR_OK;
        SCRIPTLOOP_START(PLAYER_EVENT_ON_CAN_USE_ITEM)
            sEluna->PushValue(player);
        sEluna->PushValue(itemEntry);
        if (sEluna->ExecuteCall())
        {
            if (!lua_isnoneornil(sEluna->LuaState, 1))
                result = InventoryResult(sEluna->OptValue<int>(1, result));
            sEluna->EndCall();
        }
        SCRIPTLOOP_END()
            return result;
    }
    bool OnChat(Player* player, uint32 type, uint32 lang, std::string& msg)
    {
        bool Result = false;
        SCRIPTLOOP_START(PLAYER_EVENT_ON_CHAT)
            sEluna->PushValue(player);
        sEluna->PushValue(msg.c_str());
        sEluna->PushValue(type);
        sEluna->PushValue(lang);
        if (sEluna->ExecuteCall())
        {
            Result = !sEluna->OptValue<bool>(1, Result);
            sEluna->EndCall();
        }
        SCRIPTLOOP_END()
            return Result;
    }
    bool OnChat(Player* player, uint32 type, uint32 lang, std::string& msg, Group* group)
    {
        bool Result = true;
        SCRIPTLOOP_START(PLAYER_EVENT_ON_GROUP_CHAT)
            sEluna->PushValue(player);
        sEluna->PushValue(msg.c_str());
        sEluna->PushValue(type);
        sEluna->PushValue(lang);
        sEluna->PushValue(group);
        if (sEluna->ExecuteCall())
        {
            Result = sEluna->OptValue<bool>(1, Result);
            sEluna->EndCall();
        }
        SCRIPTLOOP_END()
            return Result;
    }
    bool OnChat(Player* player, uint32 type, uint32 lang, std::string& msg, Guild* guild)
    {
        bool Result = true;
        SCRIPTLOOP_START(PLAYER_EVENT_ON_GUILD_CHAT)
            sEluna->PushValue(player);
        sEluna->PushValue(msg.c_str());
        sEluna->PushValue(type);
        sEluna->PushValue(lang);
        sEluna->PushValue(guild);
        if (sEluna->ExecuteCall())
        {
            Result = sEluna->OptValue<bool>(1, Result);
            sEluna->EndCall();
        }
        SCRIPTLOOP_END()
            return Result;
    }
    bool OnChat(Player* player, uint32 type, uint32 lang, std::string& msg, Channel* channel)
    {
        bool Result = true;
        SCRIPTLOOP_START(PLAYER_EVENT_ON_CHANNEL_CHAT)
            sEluna->PushValue(player);
        sEluna->PushValue(msg.c_str());
        sEluna->PushValue(type);
        sEluna->PushValue(lang);
        sEluna->PushValue(channel->GetChannelId());
        if (sEluna->ExecuteCall())
        {
            Result = sEluna->OptValue<bool>(1, Result);
            sEluna->EndCall();
        }
        SCRIPTLOOP_END()
            return Result;
    }
    void OnEngineRestart()
    {
        SCRIPTLOOP_START(ELUNA_EVENT_ON_RESTART)
            SCRIPTLOOP_END()
    }
    // item
    bool OnDummyEffect(Unit* caster, uint32 spellId, SpellEffIndex effIndex, Item* target)
    {
        if (!sEluna->ItemEventBindings->Call(target->GetEntry(), ITEM_EVENT_ON_DUMMY_EFFECT))
            return false;
        sEluna->PushValue(caster);
        sEluna->PushValue(spellId);
        sEluna->PushValue(effIndex);
        sEluna->PushValue(target);
        sEluna->ExecuteCall();
        return true;
    }

    void TEST(const Item* item)
    {
    }

    bool OnQuestAccept(Player* player, Item* item, Quest const* quest)
    {
        TEST(item);
        if (!sEluna->ItemEventBindings->Call(item->GetEntry(), ITEM_EVENT_ON_QUEST_ACCEPT))
            return false;
        sEluna->PushValue(player);
        sEluna->PushValue(item);
        sEluna->PushValue(quest);
        sEluna->ExecuteCall();
        return true;
    }
    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets)
    {
        if (!sEluna->ItemEventBindings->Call(item->GetEntry(), ITEM_EVENT_ON_USE))
            return false;
        sEluna->PushValue(player);
        sEluna->PushValue(item);
        if (GameObject* target = targets.GetGOTarget())
            sEluna->PushValue(target);
        else if (Item* target = targets.GetItemTarget())
            sEluna->PushValue(target);
        else if (Unit* target = targets.GetUnitTarget())
            sEluna->PushValue(target);
        else
            sEluna->PushValue();
        sEluna->ExecuteCall();
        // player->SendEquipError((InventoryResult)83, item, NULL);
        return false;
    }
    bool OnExpire(Player* player, ItemTemplate const* proto)
    {
        if (!sEluna->ItemEventBindings->Call(proto->ItemId, ITEM_EVENT_ON_EXPIRE))
            return false;
        sEluna->PushValue(player);
        sEluna->PushValue(proto->ItemId);
        sEluna->ExecuteCall();
        return true;
    }
    bool OnGossipSelect(Player* player, Item* item, uint32 sender, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (!sEluna->CreatureGossipBindings->Call(item->GetEntry(), GOSSIP_EVENT_ON_SELECT))
            return false;
        sEluna->PushValue(player);
        sEluna->PushValue(item);
        sEluna->PushValue(sender);
        sEluna->PushValue(action);
        sEluna->ExecuteCall();
        return true;
    }
    bool OnGossipSelectCode(Player* player, Item* item, uint32 sender, uint32 action, const char* code)
    {
        player->PlayerTalkClass->ClearMenus();
        if (!sEluna->CreatureGossipBindings->Call(item->GetEntry(), GOSSIP_EVENT_ON_SELECT))
            return false;
        sEluna->PushValue(player);
        sEluna->PushValue(item);
        sEluna->PushValue(sender);
        sEluna->PushValue(action);
        sEluna->PushValue(code);
        sEluna->ExecuteCall();
        return true;
    }
    // creature
    bool OnDummyEffect(Unit* caster, uint32 spellId, SpellEffIndex effIndex, Creature* target)
    {
        if (!sEluna->CreatureEventBindings->Call(target->GetEntry(), CREATURE_EVENT_ON_DUMMY_EFFECT))
            return false;
        sEluna->PushValue(caster);
        sEluna->PushValue(spellId);
        sEluna->PushValue(effIndex);
        sEluna->PushValue(target);
        sEluna->ExecuteCall();
        return true;
    }
    bool OnGossipHello(Player* player, Creature* creature)
    {
        player->PlayerTalkClass->ClearMenus();
        if (!sEluna->CreatureGossipBindings->Call(creature->GetEntry(), GOSSIP_EVENT_ON_HELLO))
            return false;
        sEluna->PushValue(player);
        sEluna->PushValue(creature);
        sEluna->ExecuteCall();
        return true;
    }
    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (!sEluna->CreatureGossipBindings->Call(creature->GetEntry(), GOSSIP_EVENT_ON_SELECT))
            return false;
        sEluna->PushValue(player);
        sEluna->PushValue(creature);
        sEluna->PushValue(sender);
        sEluna->PushValue(action);
        sEluna->ExecuteCall();
        return true;
    }
    bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code)
    {
        player->PlayerTalkClass->ClearMenus();
        if (!sEluna->CreatureGossipBindings->Call(creature->GetEntry(), GOSSIP_EVENT_ON_SELECT))
            return false;
        sEluna->PushValue(player);
        sEluna->PushValue(creature);
        sEluna->PushValue(sender);
        sEluna->PushValue(action);
        sEluna->PushValue(code);
        sEluna->ExecuteCall();
        return true;
    }
    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (!sEluna->CreatureEventBindings->Call(creature->GetEntry(), CREATURE_EVENT_ON_QUEST_ACCEPT))
            return false;
        sEluna->PushValue(player);
        sEluna->PushValue(creature);
        sEluna->PushValue(quest);
        sEluna->ExecuteCall();
        return true;
    }
    bool OnQuestSelect(Player* player, Creature* creature, Quest const* quest)
    {
        if (!sEluna->CreatureEventBindings->Call(creature->GetEntry(), CREATURE_EVENT_ON_QUEST_SELECT))
            return false;
        sEluna->PushValue(player);
        sEluna->PushValue(creature);
        sEluna->PushValue(quest);
        sEluna->ExecuteCall();
        return true;
    }
    bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
    {
        if (!sEluna->CreatureEventBindings->Call(creature->GetEntry(), CREATURE_EVENT_ON_QUEST_COMPLETE))
            return false;
        sEluna->PushValue(player);
        sEluna->PushValue(creature);
        sEluna->PushValue(quest);
        sEluna->ExecuteCall();
        return true;
    }
    bool OnQuestReward(Player* player, Creature* creature, Quest const* quest, uint32 opt)
    {
        if (!sEluna->CreatureEventBindings->Call(creature->GetEntry(), CREATURE_EVENT_ON_QUEST_REWARD))
            return false;
        sEluna->PushValue(player);
        sEluna->PushValue(creature);
        sEluna->PushValue(quest);
        sEluna->PushValue(opt);
        sEluna->ExecuteCall();
        return true;
    }
    uint32 GetDialogStatus(Player* player, Creature* creature)
    {
        if (!sEluna->CreatureEventBindings->Call(creature->GetEntry(), CREATURE_EVENT_ON_DIALOG_STATUS))
            return 0;
        sEluna->PushValue(player);
        sEluna->PushValue(creature);
        sEluna->ExecuteCall();
        return 100;
    }
    // gameobject
    bool OnDummyEffect(Unit* caster, uint32 spellId, SpellEffIndex effIndex, GameObject* target)
    {
        if (!sEluna->GameObjectEventBindings->Call(target->GetEntry(), GAMEOBJECT_EVENT_ON_DUMMY_EFFECT))
            return false;
        sEluna->PushValue(caster);
        sEluna->PushValue(spellId);
        sEluna->PushValue(effIndex);
        sEluna->PushValue(target);
        sEluna->ExecuteCall();
        return true;
    }
    bool OnGossipHello(Player* player, GameObject* go)
    {
        player->PlayerTalkClass->ClearMenus();
        if (!sEluna->GameObjectGossipBindings->Call(go->GetEntry(), GOSSIP_EVENT_ON_HELLO))
            return false;
        sEluna->PushValue(player);
        sEluna->PushValue(go);
        sEluna->ExecuteCall();
        return true;
    }
    bool OnGossipSelect(Player* player, GameObject* go, uint32 sender, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (!sEluna->GameObjectGossipBindings->Call(go->GetEntry(), GOSSIP_EVENT_ON_SELECT))
            return false;
        sEluna->PushValue(player);
        sEluna->PushValue(go);
        sEluna->PushValue(sender);
        sEluna->PushValue(action);
        sEluna->ExecuteCall();
        return true;
    }
    bool OnGossipSelectCode(Player* player, GameObject* go, uint32 sender, uint32 action, const char* code)
    {
        player->PlayerTalkClass->ClearMenus();
        if (!sEluna->GameObjectGossipBindings->Call(go->GetEntry(), GOSSIP_EVENT_ON_SELECT))
            return false;
        sEluna->PushValue(player);
        sEluna->PushValue(go);
        sEluna->PushValue(sender);
        sEluna->PushValue(action);
        sEluna->PushValue(code);
        sEluna->ExecuteCall();
        return true;
    }
    bool OnQuestAccept(Player* player, GameObject* go, Quest const* quest)
    {
        if (!sEluna->GameObjectEventBindings->Call(go->GetEntry(), GAMEOBJECT_EVENT_ON_QUEST_ACCEPT))
            return false;
        sEluna->PushValue(player);
        sEluna->PushValue(go);
        sEluna->PushValue(quest);
        sEluna->ExecuteCall();
        return true;
    }
    bool OnQuestReward(Player* player, GameObject* go, Quest const* quest, uint32 opt)
    {
        if (!sEluna->GameObjectEventBindings->Call(go->GetEntry(), GAMEOBJECT_EVENT_ON_QUEST_REWARD))
            return false;
        sEluna->PushValue(player);
        sEluna->PushValue(go);
        sEluna->PushValue(quest);
        sEluna->PushValue(opt);
        sEluna->ExecuteCall();
        return true;
    }
    uint32 GetDialogStatus(Player* player, GameObject* go)
    {
        if (!sEluna->GameObjectEventBindings->Call(go->GetEntry(), GAMEOBJECT_EVENT_ON_DIALOG_STATUS))
            return 0;
        sEluna->PushValue(player);
        sEluna->PushValue(go);
        sEluna->ExecuteCall();
        return 100;
    }
    void OnDestroyed(GameObject* go, Player* player)
    {
        sEluna->GameObjectEventBindings->Call(go->GetEntry(), GAMEOBJECT_EVENT_ON_DESTROYED);
        sEluna->PushValue(go);
        sEluna->PushValue(player);
        sEluna->ExecuteCall();
    }
    void OnDamaged(GameObject* go, Player* player)
    {
        sEluna->GameObjectEventBindings->Call(go->GetEntry(), GAMEOBJECT_EVENT_ON_DAMAGED);
        sEluna->PushValue(go);
        sEluna->PushValue(player);
        sEluna->ExecuteCall();
    }
    void OnLootStateChanged(GameObject* go, uint32 state, Unit* unit)
    {
        sEluna->GameObjectEventBindings->Call(go->GetEntry(), GAMEOBJECT_EVENT_ON_LOOT_STATE_CHANGE);
        sEluna->PushValue(go);
        sEluna->PushValue(state);
        sEluna->PushValue(unit);
        sEluna->ExecuteCall();
    }
    void OnGameObjectStateChanged(GameObject* go, uint32 state)
    {
        sEluna->GameObjectEventBindings->Call(go->GetEntry(), GAMEOBJECT_EVENT_ON_GO_STATE_CHANGED);
        sEluna->PushValue(go);
        sEluna->PushValue(state);
        sEluna->ExecuteCall();
    }
    // areatrigger
    bool OnTrigger(Player* player, AreaTriggerEntry const* trigger)
    {
        SCRIPTLOOP_START(TRIGGER_EVENT_ON_TRIGGER)
            sEluna->PushValue(player);
        sEluna->PushValue(trigger->id);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
            return false;
    }
    // weather
    void OnChange(Weather* weather, WeatherState state, float grade)
    {
        SCRIPTLOOP_START(WEATHER_EVENT_ON_CHANGE)
            sEluna->PushValue((weather->GetZone()));
        sEluna->PushValue(state);
        sEluna->PushValue(grade);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    // condition
    bool OnConditionCheck(Condition* condition, ConditionSourceInfo& sourceInfo)
    {
        return true;
    }
    // transport
    void OnAddPassenger(Transport* transport, Player* player)
    {
    }
    void OnAddCreaturePassenger(Transport* transport, Creature* creature)
    {
    }
    void OnRemovePassenger(Transport* transport, Player* player)
    {
    }
    void OnRelocate(Transport* transport, uint32 waypointId, uint32 mapId, float x, float y, float z)
    {
    }
};

// TC
class Eluna_ServerScript : public ServerScript
{
public:
    Eluna_ServerScript() : ServerScript("Eluna_ServerScript") { }
    void OnNetworkStart()
    {
    }
    void OnNetworkStop()
    {
    }
    void OnSocketOpen(WorldSocket* socket)
    {
    }
    void OnSocketClose(WorldSocket* socket, bool wasNew)
    {
    }
    void OnPacketSend(WorldSocket* socket, WorldPacket& packet)
    {
    }
    void OnPacketReceive(WorldSocket* socket, WorldPacket& packet)
    {
    }
    void OnUnknownPacketReceive(WorldSocket* socket, WorldPacket& packet)
    {
    }
};
class Eluna_FormulaScript : public FormulaScript
{
public:
    Eluna_FormulaScript() : FormulaScript("Eluna_FormulaScript") { }
    void OnHonorCalculation(float& honor, uint8 level, float multiplier)
    {
    }
    void OnGrayLevelCalculation(uint8& grayLevel, uint8 playerLevel)
    {
    }
    void OnColorCodeCalculation(XPColorChar& color, uint8 playerLevel, uint8 mobLevel)
    {
    }
    void OnZeroDifferenceCalculation(uint8& diff, uint8 playerLevel)
    {
    }
    void OnBaseGainCalculation(uint32& gain, uint8 playerLevel, uint8 mobLevel, ContentLevels content)
    {
    }
    void OnGainCalculation(uint32& gain, Player* player, Unit* unit)
    {
    }
    void OnGroupRateCalculation(float& rate, uint32 count, bool isRaid)
    {
    }
};
/*
class Eluna_MapScript : public WorldMapScript
{
public:
Eluna_MapScript() : WorldMapScript("Eluna_MapScript", 123) { }
void OnCreate(Map* map)
{
}
void OnDestroy(Map* map)
{
}
void OnLoadGridMap(Map* map, GridMap* gmap, uint32 gx, uint32 gy)
{
}
void OnUnloadGridMap(Map* map, GridMap* gmap, uint32 gx, uint32 gy)
{
}
void OnPlayerEnter(Map* map, Player* player)
{
}
void OnPlayerLeave(Map* map, Player* player)
{
}
void OnUpdate(Map* map, uint32 diff)
{
}
};
*/
/*
class Eluna_InstanceMapScript : public InstanceMapScript
{
public:
Eluna_InstanceMapScript() : InstanceMapScript("Eluna_InstanceMapScript") { }
InstanceScript* GetInstanceScript(InstanceMap* map)
{
}
void OnCreate(Map* map)
{
}
void OnDestroy(Map* map)
{
}
void OnLoadGridMap(Map* map, GridMap* gmap, uint32 gx, uint32 gy)
{
}
void OnUnloadGridMap(Map* map, GridMap* gmap, uint32 gx, uint32 gy)
{
}
void OnPlayerEnter(Map* map, Player* player)
{
}
void OnPlayerLeave(Map* map, Player* player)
{
}
void OnUpdate(Map* map, uint32 diff)
{
}
};
*/
class Eluna_AuctionHouseScript : public AuctionHouseScript
{
public:
    Eluna_AuctionHouseScript() : AuctionHouseScript("Eluna_AuctionHouseScript") { }
    void OnAuctionAdd(AuctionHouseObject* ah, AuctionEntry* entry)
    {
    }
    void OnAuctionRemove(AuctionHouseObject* ah, AuctionEntry* entry)
    {
    }
    void OnAuctionSuccessful(AuctionHouseObject* ah, AuctionEntry* entry)
    {
    }
    void OnAuctionExpire(AuctionHouseObject* ah, AuctionEntry* entry)
    {
    }
};
/*class Eluna_ConditionScript : public ConditionScript
{
public:
Eluna_ConditionScript() : ConditionScript("Eluna_ConditionScript") { }
bool OnConditionCheck(Condition* condition, ConditionSourceInfo& sourceInfo)
{
return false;
}
};
*/
class Eluna_VehicleScript : public VehicleScript
{
public:
    Eluna_VehicleScript() : VehicleScript("Eluna_VehicleScript") { }
    void OnInstall(Vehicle* veh)
    {
    }
    void OnUninstall(Vehicle* veh)
    {
    }
    void OnReset(Vehicle* veh)
    {
    }
    void OnInstallAccessory(Vehicle* veh, Creature* accessory)
    {
    }
    void OnAddPassenger(Vehicle* veh, Unit* passenger, int8 seatId)
    {
    }
    void OnRemovePassenger(Vehicle* veh, Unit* passenger)
    {
    }
};
/*class Eluna_TransportScript : public TransportScript
{
public:
Eluna_TransportScript() : TransportScript("Eluna_TransportScript") { }
void OnAddPassenger(Transport* transport, Player* player)
{
}
void OnAddCreaturePassenger(Transport* transport, Creature* creature)
{
}
void OnRemovePassenger(Transport* transport, Player* player)
{
}
void OnRelocate(Transport* transport, uint32 waypointId, uint32 mapId, float x, float y, float z)
{
}
};
*/
class Eluna_PlayerScript : public PlayerScript
{
public:
    Eluna_PlayerScript() : PlayerScript("Eluna_PlayerScript") { }
    void OnPlayerEnterCombat(Player* player, Unit* enemy)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_ENTER_COMBAT)
            sEluna->PushValue(player);
        sEluna->PushValue(enemy);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnPlayerLeaveCombat(Player* player)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_LEAVE_COMBAT)
            sEluna->PushValue(player);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnPVPKill(Player* killer, Player* killed)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_KILL_PLAYER)
            sEluna->PushValue(killer);
        sEluna->PushValue(killed);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnCreatureKill(Player* killer, Creature* killed)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_KILL_CREATURE)
            sEluna->PushValue(killer);
        sEluna->PushValue(killed);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnPlayerKilledByCreature(Creature* killer, Player* killed)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_KILLED_BY_CREATURE)
            sEluna->PushValue(killer);
        sEluna->PushValue(killed);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnLevelChanged(Player* player, uint8 oldLevel)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_LEVEL_CHANGE)
            sEluna->PushValue(player);
        sEluna->PushValue(oldLevel);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnFreeTalentPointsChanged(Player* player, uint32 newPoints)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_TALENTS_CHANGE)
            sEluna->PushValue(player);
        sEluna->PushValue(newPoints);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnTalentsReset(Player* player, bool noCost)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_TALENTS_RESET)
            sEluna->PushValue(player);
        sEluna->PushValue(noCost);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnMoneyChanged(Player* player, int32& amount)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_MONEY_CHANGE)
            sEluna->PushValue(player);
        sEluna->PushValue(amount);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnGiveXP(Player* player, uint32& amount, Unit* victim)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_GIVE_XP)
            sEluna->PushValue(player);
        sEluna->PushValue(amount);
        sEluna->PushValue(victim);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnReputationChange(Player* player, uint32 factionID, int32& standing, bool incremental)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_REPUTATION_CHANGE)
            sEluna->PushValue(player);
        sEluna->PushValue(factionID);
        sEluna->PushValue(standing);
        sEluna->PushValue(incremental);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnDuelRequest(Player* target, Player* challenger)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_DUEL_REQUEST)
            sEluna->PushValue(target);
        sEluna->PushValue(challenger);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnDuelStart(Player* player1, Player* player2)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_DUEL_START)
            sEluna->PushValue(player1);
        sEluna->PushValue(player2);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnDuelEnd(Player* winner, Player* loser, DuelCompleteType type)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_DUEL_END)
            sEluna->PushValue(winner);
        sEluna->PushValue(loser);
        sEluna->PushValue(type);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    bool OnChat(Player* player, uint32 type, uint32 lang, std::string& msg)
    {
        bool Result = false;
        SCRIPTLOOP_START(PLAYER_EVENT_ON_CHAT)
            sEluna->PushValue(player);
        sEluna->PushValue(msg.c_str());
        sEluna->PushValue(type);
        sEluna->PushValue(lang);
        if (sEluna->ExecuteCall())
        {
            Result = !sEluna->OptValue<bool>(1, Result);
            sEluna->EndCall();
        }
        SCRIPTLOOP_END()
            return Result;
    }
    bool OnChat(Player* player, uint32 type, uint32 lang, std::string& msg, Player* receiver)
    {
        bool Result = false;
        SCRIPTLOOP_START(PLAYER_EVENT_ON_WHISPER)
            sEluna->PushValue(player);
        sEluna->PushValue(msg.c_str());
        sEluna->PushValue(type);
        sEluna->PushValue(lang);
        sEluna->PushValue(receiver);
        if (sEluna->ExecuteCall())
        {
            Result = !sEluna->OptValue<bool>(1, Result);
            sEluna->EndCall();
        }
        SCRIPTLOOP_END()
            return Result;
    }
    bool OnChat(Player* player, uint32 type, uint32 lang, std::string& msg, Group* group)
    {
        bool Result = true;
        SCRIPTLOOP_START(PLAYER_EVENT_ON_GROUP_CHAT)
            sEluna->PushValue(player);
        sEluna->PushValue(msg.c_str());
        sEluna->PushValue(type);
        sEluna->PushValue(lang);
        sEluna->PushValue(group);
        if (sEluna->ExecuteCall())
        {
            Result = sEluna->OptValue<bool>(1, Result);
            sEluna->EndCall();
        }
        SCRIPTLOOP_END()
            return Result;
    }
    bool OnChat(Player* player, uint32 type, uint32 lang, std::string& msg, Guild* guild)
    {
        bool Result = true;
        SCRIPTLOOP_START(PLAYER_EVENT_ON_GUILD_CHAT)
            sEluna->PushValue(player);
        sEluna->PushValue(msg.c_str());
        sEluna->PushValue(type);
        sEluna->PushValue(lang);
        sEluna->PushValue(guild);
        if (sEluna->ExecuteCall())
        {
            Result = sEluna->OptValue<bool>(1, Result);
            sEluna->EndCall();
        }
        SCRIPTLOOP_END()
            return Result;
    }
    bool OnChat(Player* player, uint32 type, uint32 lang, std::string& msg, Channel* channel)
    {
        bool Result = true;
        SCRIPTLOOP_START(PLAYER_EVENT_ON_CHANNEL_CHAT)
            sEluna->PushValue(player);
        sEluna->PushValue(msg.c_str());
        sEluna->PushValue(type);
        sEluna->PushValue(lang);
        sEluna->PushValue(channel->GetChannelId());
        if (sEluna->ExecuteCall())
        {
            Result = sEluna->OptValue<bool>(1, Result);
            sEluna->EndCall();
        }
        SCRIPTLOOP_END()
            return Result;
    }
    void OnEmote(Player* player, uint32 emote)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_EMOTE)
            sEluna->PushValue(player);
        sEluna->PushValue(emote);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnTextEmote(Player* player, uint32 textEmote, uint32 emoteNum, uint64 guid)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_TEXT_EMOTE)
            sEluna->PushValue(player);
        sEluna->PushValue(textEmote);
        sEluna->PushValue(emoteNum);
        sEluna->PushValue(guid);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnSpellCast(Player* player, Spell* spell, bool skipCheck)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_SPELL_CAST)
            sEluna->PushValue(player);
        sEluna->PushValue(spell);
        sEluna->PushValue(skipCheck);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnLogin(Player* player)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_LOGIN)
            sEluna->PushValue(player);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnLogout(Player* player)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_LOGOUT)
            sEluna->PushValue(player);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnCreate(Player* player)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_CHARACTER_CREATE)
            sEluna->PushValue(player);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnDelete(uint64 guid)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_CHARACTER_DELETE)
            sEluna->PushValue(guid);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnSave(Player* player)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_SAVE)
            sEluna->PushValue(player);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnBindToInstance(Player* player, Difficulty difficulty, uint32 mapid, bool permanent)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_BIND_TO_INSTANCE)
            sEluna->PushValue(player);
        sEluna->PushValue(difficulty);
        sEluna->PushValue(mapid);
        sEluna->PushValue(permanent);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnUpdateZone(Player* player, uint32 newZone, uint32 newArea)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_UPDATE_ZONE)
            sEluna->PushValue(player);
        sEluna->PushValue(newZone);
        sEluna->PushValue(newArea);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnMapChanged(Player* player)
    {
        SCRIPTLOOP_START(PLAYER_EVENT_ON_MAP_CHANGE)
            sEluna->PushValue(player);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnGossipSelect(Player* player, uint32 menuid, uint32 sender, uint32 action)
    {
        sEluna->playerGossipBindings->Call(menuid, GOSSIP_EVENT_ON_SELECT);
        sEluna->PushValue(player);
        sEluna->PushValue(menuid);
        sEluna->PushValue(sender);
        sEluna->PushValue(action);
        sEluna->ExecuteCall();
    }
    void OnGossipSelectCode(Player* player, uint32 menuid, uint32 sender, uint32 action, const char* code)
    {
        sEluna->playerGossipBindings->Call(menuid, GOSSIP_EVENT_ON_SELECT);
        sEluna->PushValue(player);
        sEluna->PushValue(menuid);
        sEluna->PushValue(sender);
        sEluna->PushValue(action);
        sEluna->PushValue(code);
        sEluna->ExecuteCall();
    }
};
class Eluna_GuildScript : public GuildScript
{
public:
    Eluna_GuildScript() : GuildScript("Eluna_GuildScript") { }
    void OnAddMember(Guild* guild, Player* player, uint8& plRank)
    {
        SCRIPTLOOP_START(GUILD_EVENT_ON_ADD_MEMBER)
            sEluna->PushValue(guild);
        sEluna->PushValue(player);
        sEluna->PushValue(plRank);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnRemoveMember(Guild* guild, Player* player, bool isDisbanding, bool isKicked)
    {
        SCRIPTLOOP_START(GUILD_EVENT_ON_REMOVE_MEMBER)
            sEluna->PushValue(guild);
        sEluna->PushValue(player);
        sEluna->PushValue(isDisbanding);
        sEluna->PushValue(isKicked);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnMOTDChanged(Guild* guild, const std::string& newMotd)
    {
        SCRIPTLOOP_START(GUILD_EVENT_ON_MOTD_CHANGE)
            sEluna->PushValue(guild);
        sEluna->PushValue(newMotd.c_str());
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnInfoChanged(Guild* guild, const std::string& newInfo)
    {
        SCRIPTLOOP_START(GUILD_EVENT_ON_INFO_CHANGE)
            sEluna->PushValue(guild);
        sEluna->PushValue(newInfo.c_str());
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnCreate(Guild* guild, Player* leader, const std::string& name)
    {
        SCRIPTLOOP_START(GUILD_EVENT_ON_CREATE)
            sEluna->PushValue(guild);
        sEluna->PushValue(leader);
        sEluna->PushValue(name.c_str());
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnDisband(Guild* guild)
    {
        SCRIPTLOOP_START(GUILD_EVENT_ON_DISBAND)
            sEluna->PushValue(guild);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnMemberWitdrawMoney(Guild* guild, Player* player, uint32 &amount, bool isRepair)
    {
        SCRIPTLOOP_START(GUILD_EVENT_ON_MONEY_WITHDRAW)
            sEluna->PushValue(guild);
        sEluna->PushValue(player);
        sEluna->PushValue(amount);
        sEluna->PushValue(isRepair);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnMemberDepositMoney(Guild* guild, Player* player, uint32 &amount)
    {
        SCRIPTLOOP_START(GUILD_EVENT_ON_MONEY_DEPOSIT)
            sEluna->PushValue(guild);
        sEluna->PushValue(player);
        sEluna->PushValue(amount);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnItemMove(Guild* guild, Player* player, Item* pItem, bool isSrcBank, uint8 srcContainer, uint8 srcSlotId,
        bool isDestBank, uint8 destContainer, uint8 destSlotId)
    {
        SCRIPTLOOP_START(GUILD_EVENT_ON_ITEM_MOVE)
            sEluna->PushValue(guild);
        sEluna->PushValue(player);
        sEluna->PushValue(pItem);
        sEluna->PushValue(isSrcBank);
        sEluna->PushValue(srcContainer);
        sEluna->PushValue(srcSlotId);
        sEluna->PushValue(isDestBank);
        sEluna->PushValue(destContainer);
        sEluna->PushValue(destSlotId);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnEvent(Guild* guild, uint8 eventType, uint32 playerGuid1, uint32 playerGuid2, uint8 newRank)
    {
        SCRIPTLOOP_START(GUILD_EVENT_ON_EVENT)
            sEluna->PushValue(guild);
        sEluna->PushValue(eventType);
        sEluna->PushValue(playerGuid1);
        sEluna->PushValue(playerGuid2);
        sEluna->PushValue(newRank);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnBankEvent(Guild* guild, uint8 eventType, uint8 tabId, uint32 playerGuid, uint32 itemOrMoney, uint16 itemStackCount, uint8 destTabId)
    {
        SCRIPTLOOP_START(GUILD_EVENT_ON_BANK_EVENT)
            sEluna->PushValue(guild);
        sEluna->PushValue(eventType);
        sEluna->PushValue(tabId);
        sEluna->PushValue(playerGuid);
        sEluna->PushValue(itemOrMoney);
        sEluna->PushValue(itemStackCount);
        sEluna->PushValue(destTabId);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
};
class Eluna_GroupScript : public GroupScript
{
public:
    Eluna_GroupScript() : GroupScript("Eluna_GroupScript") { }
    void OnAddMember(Group* group, uint64 guid)
    {
        SCRIPTLOOP_START(GROUP_EVENT_ON_MEMBER_ADD)
            sEluna->PushValue(group);
        sEluna->PushValue(guid);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnInviteMember(Group* group, uint64 guid)
    {
        SCRIPTLOOP_START(GROUP_EVENT_ON_MEMBER_INVITE)
            sEluna->PushValue(group);
        sEluna->PushValue(guid);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnRemoveMember(Group* group, uint64 guid, RemoveMethod method, uint64 kicker, const char* reason)
    {
        SCRIPTLOOP_START(GROUP_EVENT_ON_MEMBER_REMOVE)
            sEluna->PushValue(group);
        sEluna->PushValue(guid);
        sEluna->PushValue(method);
        sEluna->PushValue(kicker);
        sEluna->PushValue(reason);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnChangeLeader(Group* group, uint64 newLeaderGuid, uint64 oldLeaderGuid)
    {
        SCRIPTLOOP_START(GROUP_EVENT_ON_LEADER_CHANGE)
            sEluna->PushValue(group);
        sEluna->PushValue(newLeaderGuid);
        sEluna->PushValue(oldLeaderGuid);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
    void OnDisband(Group* group)
    {
        SCRIPTLOOP_START(GROUP_EVENT_ON_DISBAND)
            sEluna->PushValue(group);
        sEluna->ExecuteCall();
        SCRIPTLOOP_END()
    }
};

void HookMgr::AddScriptHooks()
{
    // AI
    LuaCreatureAI = new Eluna_CreatureScript;
    LuaGameObjectAI = new Eluna_GameObjectScript;
    LuaWorldAI = new Eluna_WorldScript;
    // Custom
    new Eluna_HookScript;
    // TC
    new Eluna_ServerScript;
    new Eluna_FormulaScript;
    //new Eluna_MapScript;
    //new Eluna_InstanceMapScript;
    new Eluna_AuctionHouseScript;
    //new Eluna_ConditionScript;
    new Eluna_VehicleScript;
    //new Eluna_TransportScript;
    new Eluna_PlayerScript;
    new Eluna_GuildScript;
    new Eluna_GroupScript;
}
