#include "HookMgr.h"
#include "LuaEngine.h"

// Custom
class Eluna_HookScript : public HookScript
{
public:
    Eluna_HookScript() : HookScript() { }
    // misc
    void OnLootItem(Player* player, Item* item, uint32 count, uint64 guid)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_LOOT_ITEM].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_LOOT_ITEM].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_LOOT_ITEM);
            sEluna->PushValue(player);
            sEluna->PushValue(item);
            sEluna->PushValue(count);
            sEluna->PushValue(guid);
            sEluna->ExecuteCall(5, 0);
        }
    }
    void OnFirstLogin(Player* player)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_FIRST_LOGIN].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_FIRST_LOGIN].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_FIRST_LOGIN);
            sEluna->PushValue(player);
            sEluna->ExecuteCall(2, 0);
        }
    }
    void OnRepop(Player* player)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_REPOP].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_REPOP].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_REPOP);
            sEluna->PushValue(player);
            sEluna->ExecuteCall(2, 0);
        }
    }
    void OnResurrect(Player* player)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_RESURRECT].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_RESURRECT].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_RESURRECT);
            sEluna->PushValue(player);
            sEluna->ExecuteCall(2, 0);
        }
    }
    void OnEquip(Player* player, Item* item, uint8 bag, uint8 slot)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_EQUIP].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_EQUIP].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_EQUIP);
            sEluna->PushValue(player);
            sEluna->PushValue(item);
            sEluna->PushValue(bag);
            sEluna->PushValue(slot);
            sEluna->ExecuteCall(5, 0);
        }
    }
    InventoryResult OnCanUseItem(Player* player, uint32 itemEntry)
    {
        InventoryResult result = EQUIP_ERR_OK;
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_CAN_USE_ITEM].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_CAN_USE_ITEM].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_CAN_USE_ITEM);
            sEluna->PushValue(player);
            sEluna->PushValue(itemEntry);
            if (sEluna->ExecuteCall(3, 1))
            {
                lua_State* L = sEluna->LuaState;
                if (!lua_isnoneornil(L, 1))
                    result = (InventoryResult)lua_tounsigned(L, 1);
                sEluna->EndCall(1);
            }
        }
        return result;
    }
    void HandleGossipSelectOption(Player* player, uint64 guid, uint32 sender, uint32 action, std::string code, uint32 menuId)
    {
        if (!player->IsAlive() || player->GetCharmerGUID())
            return;

        if (player->HasUnitState(UNIT_STATE_DIED))
            player->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);
        player->PlayerTalkClass->ClearMenus();

        if (IS_ITEM_GUID(guid))
        {
            Item* item = player->GetItemByGuid(guid);
            if (!item)
                return;

            int bind = sEluna->ItemGossipBindings->GetBind(item->GetEntry(), GOSSIP_EVENT_ON_SELECT);
            if (bind)
            {
                sEluna->BeginCall(bind);
                sEluna->PushValue(GOSSIP_EVENT_ON_SELECT);
                sEluna->PushValue(player);
                sEluna->PushValue(item);
                sEluna->PushValue(sender);
                sEluna->PushValue(action);
                if (code.empty())
                    lua_pushnil(sEluna->LuaState);
                else
                    sEluna->PushValue(code.c_str());
                sEluna->ExecuteCall(6, 0);
            }
        }
        else if (IS_PLAYER_GUID(guid))
        {
            if (player->GetGUID() != guid)
                return;

            int bind = sEluna->playerGossipBindings->GetBind(menuId, GOSSIP_EVENT_ON_SELECT);
            if (bind)
            {
                sEluna->BeginCall(bind);
                sEluna->PushValue(GOSSIP_EVENT_ON_SELECT);
                sEluna->PushValue(player); // receiver
                sEluna->PushValue(player); // sender, just not to mess up the amount of args.
                sEluna->PushValue(sender);
                sEluna->PushValue(action);
                if (code.empty())
                    lua_pushnil(sEluna->LuaState);
                else
                    sEluna->PushValue(code.c_str());
                sEluna->PushValue(menuId);
                sEluna->ExecuteCall(7, 0);
            }
        }
    }
    bool OnChat(Player* player, uint32 type, uint32 lang, std::string& msg)
    {
        bool Result = true;
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_CHAT].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_CHAT].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_CHAT);
            sEluna->PushValue(player);
            sEluna->PushValue(msg.c_str());
            sEluna->PushValue(type);
            sEluna->PushValue(lang);
            if (sEluna->ExecuteCall(5, 1))
            {
                lua_State* L = sEluna->LuaState;
                if (!lua_isnoneornil(L, 1) && !lua_toboolean(L, 1))
                    Result = false;
                sEluna->EndCall(1);
            }
        }
        return Result;
    }
    bool OnChat(Player* player, uint32 type, uint32 lang, std::string& msg, Group* group)
    {
        bool Result = true;
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_GROUP_CHAT].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_GROUP_CHAT].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_GROUP_CHAT);
            sEluna->PushValue(player);
            sEluna->PushValue(msg.c_str());
            sEluna->PushValue(type);
            sEluna->PushValue(lang);
            sEluna->PushValue(group);
            if (sEluna->ExecuteCall(6, 1))
            {
                lua_State* L = sEluna->LuaState;
                if (!lua_isnoneornil(L, 1) && !lua_toboolean(L, 1))
                    Result = false;
                sEluna->EndCall(1);
            }
        }
        return Result;
    }
    bool OnChat(Player* player, uint32 type, uint32 lang, std::string& msg, Guild* guild)
    {
        bool Result = true;
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_GUILD_CHAT].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_GUILD_CHAT].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_GUILD_CHAT);
            sEluna->PushValue(player);
            sEluna->PushValue(msg.c_str());
            sEluna->PushValue(type);
            sEluna->PushValue(lang);
            sEluna->PushValue(guild);
            if (sEluna->ExecuteCall(6, 1))
            {
                lua_State* L = sEluna->LuaState;
                if (!lua_isnoneornil(L, 1) && !lua_toboolean(L, 1))
                    Result = false;
                sEluna->EndCall(1);
            }
        }
        return Result;
    }
    bool OnChat(Player* player, uint32 type, uint32 lang, std::string& msg, Channel* channel)
    {
        bool Result = true;
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_CHANNEL_CHAT].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_CHANNEL_CHAT].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_CHANNEL_CHAT);
            sEluna->PushValue(player);
            sEluna->PushValue(msg.c_str());
            sEluna->PushValue(type);
            sEluna->PushValue(lang);
            sEluna->PushValue(channel->GetChannelId());
            if (sEluna->ExecuteCall(6, 1))
            {
                lua_State* L = sEluna->LuaState;
                if (!lua_isnoneornil(L, 1) && !lua_toboolean(L, 1))
                    Result = false;
                sEluna->EndCall(1);
            }
        }
        return Result;
    }
    void OnEngineRestart()
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[ELUNA_EVENT_ON_RESTART].begin();
            itr != sEluna->ServerEventBindings[ELUNA_EVENT_ON_RESTART].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(ELUNA_EVENT_ON_RESTART);
            sEluna->ExecuteCall(1, 0);
        }
    }
    // item
    bool OnDummyEffect(Unit* caster, uint32 spellId, SpellEffIndex effIndex, Item* target)
    {
        int bind = sEluna->ItemEventBindings->GetBind(target->GetEntry(), ITEM_EVENT_ON_DUMMY_EFFECT);
        if (!bind)
            return false;
        sEluna->BeginCall(bind);
        sEluna->PushValue(ITEM_EVENT_ON_DUMMY_EFFECT);
        sEluna->PushValue(caster);
        sEluna->PushValue(spellId);
        sEluna->PushValue(effIndex);
        sEluna->PushValue(target);
        sEluna->ExecuteCall(5, 0);
        return true;
    }
    bool OnQuestAccept(Player* player, Item* item, Quest const* quest)
    {
        int bind = sEluna->ItemEventBindings->GetBind(item->GetEntry(), ITEM_EVENT_ON_QUEST_ACCEPT);
        if (!bind)
            return false;
        sEluna->BeginCall(bind);
        sEluna->PushValue(ITEM_EVENT_ON_QUEST_ACCEPT);
        sEluna->PushValue(player);
        sEluna->PushValue(item);
        sEluna->PushValue(quest);
        sEluna->ExecuteCall(4, 0);
        return true;
    }
    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets)
    {
        int bind1 = sEluna->ItemGossipBindings->GetBind(item->GetEntry(), GOSSIP_EVENT_ON_HELLO);
        int bind2 = sEluna->ItemEventBindings->GetBind(item->GetEntry(), ITEM_EVENT_ON_USE);
        if (!bind1 && !bind2)
            return false;
        if (bind1)
        {
            player->PlayerTalkClass->ClearMenus();
            sEluna->BeginCall(bind1);
            sEluna->PushValue(GOSSIP_EVENT_ON_HELLO);
            sEluna->PushValue(player);
            sEluna->PushValue(item);
            sEluna->ExecuteCall(3, 0);
        }
        if (bind2)
        {
            sEluna->BeginCall(bind2);
            sEluna->PushValue(ITEM_EVENT_ON_USE);
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
            sEluna->ExecuteCall(4, 0);
        }
        player->SendEquipError((InventoryResult)83, item, NULL);
        return true;
    }
    bool OnExpire(Player* player, ItemTemplate const* proto)
    {
        int bind = sEluna->ItemEventBindings->GetBind(proto->ItemId, ITEM_EVENT_ON_EXPIRE);
        if (!bind)
            return false;
        sEluna->BeginCall(bind);
        sEluna->PushValue(ITEM_EVENT_ON_EXPIRE);
        sEluna->PushValue(player);
        sEluna->PushValue(proto->ItemId);
        sEluna->ExecuteCall(3, 0);
        return true;
    }
    // creature
    bool OnDummyEffect(Unit* caster, uint32 spellId, SpellEffIndex effIndex, Creature* target)
    {
        int bind = sEluna->CreatureEventBindings->GetBind(target->GetEntry(), CREATURE_EVENT_ON_DUMMY_EFFECT);
        if (!bind)
            return false;
        sEluna->BeginCall(bind);
        sEluna->PushValue(CREATURE_EVENT_ON_DUMMY_EFFECT);
        sEluna->PushValue(caster);
        sEluna->PushValue(spellId);
        sEluna->PushValue(effIndex);
        sEluna->PushValue(target);
        sEluna->ExecuteCall(5, 0);
        return true;
    }
    bool OnGossipHello(Player* player, Creature* creature)
    {
        int bind = sEluna->CreatureGossipBindings->GetBind(creature->GetEntry(), GOSSIP_EVENT_ON_HELLO);
        if (!bind)
            return false;
        player->PlayerTalkClass->ClearMenus();
        sEluna->BeginCall(bind);
        sEluna->PushValue(GOSSIP_EVENT_ON_HELLO);
        sEluna->PushValue(player);
        sEluna->PushValue(creature);
        sEluna->ExecuteCall(3, 0);
        return true;
    }
    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
    {
        int bind = sEluna->CreatureGossipBindings->GetBind(creature->GetEntry(), GOSSIP_EVENT_ON_SELECT);
        if (!bind)
            return false;
        player->PlayerTalkClass->ClearMenus();
        sEluna->BeginCall(bind);
        sEluna->PushValue(GOSSIP_EVENT_ON_SELECT);
        sEluna->PushValue(player);
        sEluna->PushValue(creature);
        sEluna->PushValue(sender);
        sEluna->PushValue(action);
        sEluna->ExecuteCall(5, 0);
        return true;
    }
    bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code)
    {
        int bind = sEluna->CreatureGossipBindings->GetBind(creature->GetEntry(), GOSSIP_EVENT_ON_SELECT);
        if (!bind)
            return false;
        player->PlayerTalkClass->ClearMenus();
        sEluna->BeginCall(bind);
        sEluna->PushValue(GOSSIP_EVENT_ON_SELECT);
        sEluna->PushValue(player);
        sEluna->PushValue(creature);
        sEluna->PushValue(sender);
        sEluna->PushValue(action);
        sEluna->PushValue(code);
        sEluna->ExecuteCall(6, 0);
        return true;
    }
    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        int bind = sEluna->CreatureEventBindings->GetBind(creature->GetEntry(), CREATURE_EVENT_ON_QUEST_ACCEPT);
        if (!bind)
            return false;
        sEluna->BeginCall(bind);
        sEluna->PushValue(CREATURE_EVENT_ON_QUEST_ACCEPT);
        sEluna->PushValue(player);
        sEluna->PushValue(creature);
        sEluna->PushValue(quest);
        sEluna->ExecuteCall(4, 0);
        return true;
    }
    bool OnQuestSelect(Player* player, Creature* creature, Quest const* quest)
    {
        int bind = sEluna->CreatureEventBindings->GetBind(creature->GetEntry(), CREATURE_EVENT_ON_QUEST_SELECT);
        if (!bind)
            return false;
        sEluna->BeginCall(bind);
        sEluna->PushValue(CREATURE_EVENT_ON_QUEST_SELECT);
        sEluna->PushValue(player);
        sEluna->PushValue(creature);
        sEluna->PushValue(quest);
        sEluna->ExecuteCall(4, 0);
        return true;
    }
    bool OnQuestComplete(Player* player, Creature* creature, Quest const* quest)
    {
        int bind = sEluna->CreatureEventBindings->GetBind(creature->GetEntry(), CREATURE_EVENT_ON_QUEST_COMPLETE);
        if (!bind)
            return false;
        sEluna->BeginCall(bind);
        sEluna->PushValue(CREATURE_EVENT_ON_QUEST_COMPLETE);
        sEluna->PushValue(player);
        sEluna->PushValue(creature);
        sEluna->PushValue(quest);
        sEluna->ExecuteCall(4, 0);
        return true;
    }
    bool OnQuestReward(Player* player, Creature* creature, Quest const* quest, uint32 opt)
    {
        int bind = sEluna->CreatureEventBindings->GetBind(creature->GetEntry(), CREATURE_EVENT_ON_QUEST_REWARD);
        if (!bind)
            return false;
        sEluna->BeginCall(bind);
        sEluna->PushValue(CREATURE_EVENT_ON_QUEST_REWARD);
        sEluna->PushValue(player);
        sEluna->PushValue(creature);
        sEluna->PushValue(quest);
        sEluna->PushValue(opt);
        sEluna->ExecuteCall(5, 0);
        return true;
    }
    uint32 GetDialogStatus(Player* player, Creature* creature)
    {
        int bind = sEluna->CreatureEventBindings->GetBind(creature->GetEntry(), CREATURE_EVENT_ON_DIALOG_STATUS);
        if (!bind)
            return 0;
        sEluna->BeginCall(bind);
        sEluna->PushValue(CREATURE_EVENT_ON_DIALOG_STATUS);
        sEluna->PushValue(player);
        sEluna->PushValue(creature);
        sEluna->ExecuteCall(3, 0);
        return 100;
    }
    // gameobject
    bool OnDummyEffect(Unit* caster, uint32 spellId, SpellEffIndex effIndex, GameObject* target)
    {
        int bind = sEluna->GameObjectEventBindings->GetBind(target->GetEntry(), GAMEOBJECT_EVENT_ON_DUMMY_EFFECT);
        if (!bind)
            return false;
        sEluna->BeginCall(bind);
        sEluna->PushValue(GAMEOBJECT_EVENT_ON_DUMMY_EFFECT);
        sEluna->PushValue(caster);
        sEluna->PushValue(spellId);
        sEluna->PushValue(effIndex);
        sEluna->PushValue(target);
        sEluna->ExecuteCall(5, 0);
        return true;
    }
    bool OnGossipHello(Player* player, GameObject* go)
    {
        int bind = sEluna->GameObjectGossipBindings->GetBind(go->GetEntry(), GOSSIP_EVENT_ON_HELLO);
        if (!bind)
            return false;
        player->PlayerTalkClass->ClearMenus();
        sEluna->BeginCall(bind);
        sEluna->PushValue(GOSSIP_EVENT_ON_HELLO);
        sEluna->PushValue(player);
        sEluna->PushValue(go);
        sEluna->ExecuteCall(3, 0);
        return true;
    }
    bool OnGossipSelect(Player* player, GameObject* go, uint32 sender, uint32 action)
    {
        int bind = sEluna->GameObjectGossipBindings->GetBind(go->GetEntry(), GOSSIP_EVENT_ON_SELECT);
        if (!bind)
            return false;
        player->PlayerTalkClass->ClearMenus();
        sEluna->BeginCall(bind);
        sEluna->PushValue(GOSSIP_EVENT_ON_SELECT);
        sEluna->PushValue(player);
        sEluna->PushValue(go);
        sEluna->PushValue(sender);
        sEluna->PushValue(action);
        sEluna->ExecuteCall(5, 0);
        return true;
    }
    bool OnGossipSelectCode(Player* player, GameObject* go, uint32 sender, uint32 action, const char* code)
    {
        int bind = sEluna->GameObjectGossipBindings->GetBind(go->GetEntry(), GOSSIP_EVENT_ON_SELECT);
        if (!bind)
            return false;
        player->PlayerTalkClass->ClearMenus();
        sEluna->BeginCall(bind);
        sEluna->PushValue(GOSSIP_EVENT_ON_SELECT);
        sEluna->PushValue(player);
        sEluna->PushValue(go);
        sEluna->PushValue(sender);
        sEluna->PushValue(action);
        sEluna->PushValue(code);
        sEluna->ExecuteCall(6, 0);
        return true;
    }
    bool OnQuestAccept(Player* player, GameObject* go, Quest const* quest)
    {
        int bind = sEluna->GameObjectEventBindings->GetBind(go->GetEntry(), GAMEOBJECT_EVENT_ON_QUEST_ACCEPT);
        if (!bind)
            return false;
        sEluna->BeginCall(bind);
        sEluna->PushValue(GAMEOBJECT_EVENT_ON_QUEST_ACCEPT);
        sEluna->PushValue(player);
        sEluna->PushValue(go);
        sEluna->PushValue(quest);
        sEluna->ExecuteCall(4, 0);
        return true;
    }
    bool OnQuestReward(Player* player, GameObject* go, Quest const* quest, uint32 opt)
    {
        int bind = sEluna->GameObjectEventBindings->GetBind(go->GetEntry(), GAMEOBJECT_EVENT_ON_QUEST_REWARD);
        if (!bind)
            return false;
        sEluna->BeginCall(bind);
        sEluna->PushValue(GAMEOBJECT_EVENT_ON_QUEST_REWARD);
        sEluna->PushValue(player);
        sEluna->PushValue(go);
        sEluna->PushValue(quest);
        sEluna->PushValue(opt);
        sEluna->ExecuteCall(5, 0);
        return true;
    }
    uint32 GetDialogStatus(Player* player, GameObject* go)
    {
        int bind = sEluna->GameObjectEventBindings->GetBind(go->GetEntry(), GAMEOBJECT_EVENT_ON_DIALOG_STATUS);
        if (!bind)
            return 0;
        sEluna->BeginCall(bind);
        sEluna->PushValue(GAMEOBJECT_EVENT_ON_DIALOG_STATUS);
        sEluna->PushValue(player);
        sEluna->PushValue(go);
        sEluna->ExecuteCall(3, 0);
        return 100;
    }
    void OnDestroyed(GameObject* go, Player* player)
    {
        int bind = sEluna->GameObjectEventBindings->GetBind(go->GetEntry(), GAMEOBJECT_EVENT_ON_DESTROYED);
        if (!bind)
            return;
        sEluna->BeginCall(bind);
        sEluna->PushValue(GAMEOBJECT_EVENT_ON_DESTROYED);
        sEluna->PushValue(go);
        sEluna->PushValue(player);
        sEluna->ExecuteCall(3, 0);
    }
    void OnDamaged(GameObject* go, Player* player)
    {
        int bind = sEluna->GameObjectEventBindings->GetBind(go->GetEntry(), GAMEOBJECT_EVENT_ON_DAMAGED);
        if (!bind)
            return;
        sEluna->BeginCall(bind);
        sEluna->PushValue(GAMEOBJECT_EVENT_ON_DAMAGED);
        sEluna->PushValue(go);
        sEluna->PushValue(player);
        sEluna->ExecuteCall(3, 0);
    }
    void OnLootStateChanged(GameObject* go, uint32 state, Unit* unit)
    {
        int bind = sEluna->GameObjectEventBindings->GetBind(go->GetEntry(), GAMEOBJECT_EVENT_ON_LOOT_STATE_CHANGE);
        if (!bind)
            return;
        sEluna->BeginCall(bind);
        sEluna->PushValue(GAMEOBJECT_EVENT_ON_LOOT_STATE_CHANGE);
        sEluna->PushValue(go);
        sEluna->PushValue(state);
        sEluna->PushValue(unit);
        sEluna->ExecuteCall(4, 0);
    }
    void OnGameObjectStateChanged(GameObject* go, uint32 state)
    {
        int bind = sEluna->GameObjectEventBindings->GetBind(go->GetEntry(), GAMEOBJECT_EVENT_ON_GO_STATE_CHANGED);
        if (!bind)
            return;
        sEluna->BeginCall(bind);
        sEluna->PushValue(GAMEOBJECT_EVENT_ON_GO_STATE_CHANGED);
        sEluna->PushValue(go);
        sEluna->PushValue(state);
        sEluna->ExecuteCall(3, 0);
    }
    // areatrigger
    bool OnTrigger(Player* player, AreaTriggerEntry const* trigger)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[TRIGGER_EVENT_ON_TRIGGER].begin();
            itr != sEluna->ServerEventBindings[TRIGGER_EVENT_ON_TRIGGER].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(TRIGGER_EVENT_ON_TRIGGER);
            sEluna->PushValue(player);
            sEluna->PushValue(trigger->id);
            sEluna->ExecuteCall(3, 0);
        }
        return false;
    }
    // weather
    void OnChange(Weather* weather, WeatherState state, float grade)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[WEATHER_EVENT_ON_CHANGE].begin();
            itr != sEluna->ServerEventBindings[WEATHER_EVENT_ON_CHANGE].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(WEATHER_EVENT_ON_CHANGE);
            sEluna->PushValue((weather->GetZone()));
            sEluna->PushValue(state);
            sEluna->PushValue(grade);
            sEluna->ExecuteCall(4, 0);
        }
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
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_ENTER_COMBAT].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_ENTER_COMBAT].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_ENTER_COMBAT);
            sEluna->PushValue(player);
            sEluna->PushValue(enemy);
            sEluna->ExecuteCall(3, 0);
        }
    }
    void OnPlayerLeaveCombat(Player* player)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_LEAVE_COMBAT].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_LEAVE_COMBAT].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_LEAVE_COMBAT);
            sEluna->PushValue(player);
            sEluna->ExecuteCall(2, 0);
        }
    }
    void OnPVPKill(Player* killer, Player* killed)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_KILL_PLAYER].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_KILL_PLAYER].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_KILL_PLAYER);
            sEluna->PushValue(killer);
            sEluna->PushValue(killed);
            sEluna->ExecuteCall(3, 0);
        }
    }
    void OnCreatureKill(Player* killer, Creature* killed)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_KILL_CREATURE].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_KILL_CREATURE].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_KILL_CREATURE);
            sEluna->PushValue(killer);
            sEluna->PushValue(killed);
            sEluna->ExecuteCall(3, 0);
        }
    }
    void OnPlayerKilledByCreature(Creature* killer, Player* killed)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_KILLED_BY_CREATURE].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_KILLED_BY_CREATURE].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_KILLED_BY_CREATURE);
            sEluna->PushValue(killer);
            sEluna->PushValue(killed);
            sEluna->ExecuteCall(3, 0);
        }
    }
    void OnLevelChanged(Player* player, uint8 oldLevel)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_LEVEL_CHANGE].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_LEVEL_CHANGE].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_LEVEL_CHANGE);
            sEluna->PushValue(player);
            sEluna->PushValue(oldLevel);
            sEluna->ExecuteCall(3, 0);
        }
    }
    void OnFreeTalentPointsChanged(Player* player, uint32 newPoints)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_TALENTS_CHANGE].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_TALENTS_CHANGE].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_TALENTS_CHANGE);
            sEluna->PushValue(player);
            sEluna->PushValue(newPoints);
            sEluna->ExecuteCall(3, 0);
        }
    }
    void OnTalentsReset(Player* player, bool noCost)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_TALENTS_RESET].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_TALENTS_RESET].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_TALENTS_RESET);
            sEluna->PushValue(player);
            sEluna->PushValue(noCost);
            sEluna->ExecuteCall(3, 0);
        }
    }
    void OnMoneyChanged(Player* player, int32& amount)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_MONEY_CHANGE].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_MONEY_CHANGE].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_MONEY_CHANGE);
            sEluna->PushValue(player);
            sEluna->PushValue(amount);
            sEluna->ExecuteCall(3, 0);
        }
    }
    void OnGiveXP(Player* player, uint32& amount, Unit* victim)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_GIVE_XP].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_GIVE_XP].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_GIVE_XP);
            sEluna->PushValue(player);
            sEluna->PushValue(amount);
            sEluna->PushValue(victim);
            sEluna->ExecuteCall(4, 0);
        }
    }
    void OnReputationChange(Player* player, uint32 factionID, int32& standing, bool incremental)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_REPUTATION_CHANGE].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_REPUTATION_CHANGE].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_REPUTATION_CHANGE);
            sEluna->PushValue(player);
            sEluna->PushValue(factionID);
            sEluna->PushValue(standing);
            sEluna->PushValue(incremental);
            sEluna->ExecuteCall(5, 0);
        }
    }
    void OnDuelRequest(Player* target, Player* challenger)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_DUEL_REQUEST].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_DUEL_REQUEST].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_DUEL_REQUEST);
            sEluna->PushValue(target);
            sEluna->PushValue(challenger);
            sEluna->ExecuteCall(3, 0);
        }
    }
    void OnDuelStart(Player* player1, Player* player2)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_DUEL_START].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_DUEL_START].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_DUEL_START);
            sEluna->PushValue(player1);
            sEluna->PushValue(player2);
            sEluna->ExecuteCall(3, 0);
        }
    }
    void OnDuelEnd(Player* winner, Player* loser, DuelCompleteType type)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_DUEL_END].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_DUEL_END].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_DUEL_END);
            sEluna->PushValue(winner);
            sEluna->PushValue(loser);
            sEluna->PushValue(type);
            sEluna->ExecuteCall(4, 0);
        }
    }
    void OnChat(Player* player, uint32 type, uint32 lang, std::string& msg)
    {
        // implemented in ChatHandler.cpp
    }
    void OnChat(Player* player, uint32 type, uint32 lang, std::string& msg, Player* receiver)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_WHISPER].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_WHISPER].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_WHISPER);
            sEluna->PushValue(player);
            sEluna->PushValue(msg.c_str());
            sEluna->PushValue(type);
            sEluna->PushValue(lang);
            sEluna->PushValue(receiver);
            sEluna->ExecuteCall(6, 0);
        }
    }
    void OnChat(Player* player, uint32 type, uint32 lang, std::string& msg, Group* group)
    {
        // implemented in ChatHandler.cpp
    }
    void OnChat(Player* player, uint32 type, uint32 lang, std::string& msg, Guild* guild)
    {
        // implemented in ChatHandler.cpp
    }
    void OnChat(Player* player, uint32 type, uint32 lang, std::string& msg, Channel* channel)
    {
        // implemented in ChatHandler.cpp
    }
    void OnEmote(Player* player, uint32 emote)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_EMOTE].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_EMOTE].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_EMOTE);
            sEluna->PushValue(player);
            sEluna->PushValue(emote);
            sEluna->ExecuteCall(3, 0);
        }
    }
    void OnTextEmote(Player* player, uint32 textEmote, uint32 emoteNum, uint64 guid)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_TEXT_EMOTE].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_TEXT_EMOTE].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_TEXT_EMOTE);
            sEluna->PushValue(player);
            sEluna->PushValue(textEmote);
            sEluna->PushValue(emoteNum);
            sEluna->PushValue(guid);
            sEluna->ExecuteCall(5, 0);
        }
    }
    void OnSpellCast(Player* player, Spell* spell, bool skipCheck)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_SPELL_CAST].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_SPELL_CAST].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_SPELL_CAST);
            sEluna->PushValue(player);
            sEluna->PushValue(spell);
            sEluna->PushValue(skipCheck);
            sEluna->ExecuteCall(4, 0);
        }
    }
    void OnLogin(Player* player)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_LOGIN].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_LOGIN].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_LOGIN);
            sEluna->PushValue(player);
            sEluna->ExecuteCall(2, 0);
        }
    }
    void OnLogout(Player* player)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_LOGOUT].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_LOGOUT].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_LOGOUT);
            sEluna->PushValue(player);
            sEluna->ExecuteCall(2, 0);
        }
    }
    void OnCreate(Player* player)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_CHARACTER_CREATE].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_CHARACTER_CREATE].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_CHARACTER_CREATE);
            sEluna->PushValue(player);
            sEluna->ExecuteCall(2, 0);
        }
    }
    void OnDelete(uint64 guid)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_CHARACTER_DELETE].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_CHARACTER_DELETE].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_CHARACTER_DELETE);
            sEluna->PushValue(guid);
            sEluna->ExecuteCall(2, 0);
        }
    }
    void OnSave(Player* player)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_SAVE].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_SAVE].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_SAVE);
            sEluna->PushValue(player);
            sEluna->ExecuteCall(2, 0);
        }
    }
    void OnBindToInstance(Player* player, Difficulty difficulty, uint32 mapid, bool permanent)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_BIND_TO_INSTANCE].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_BIND_TO_INSTANCE].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_BIND_TO_INSTANCE);
            sEluna->PushValue(player);
            sEluna->PushValue(difficulty);
            sEluna->PushValue(mapid);
            sEluna->PushValue(permanent);
            sEluna->ExecuteCall(5, 0);
        }
    }
    void OnUpdateZone(Player* player, uint32 newZone, uint32 newArea)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_UPDATE_ZONE].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_UPDATE_ZONE].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_UPDATE_ZONE);
            sEluna->PushValue(player);
            sEluna->PushValue(newZone);
            sEluna->PushValue(newArea);
            sEluna->ExecuteCall(4, 0);
        }
    }
    void OnMapChanged(Player* player)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[PLAYER_EVENT_ON_MAP_CHANGE].begin();
            itr != sEluna->ServerEventBindings[PLAYER_EVENT_ON_MAP_CHANGE].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(PLAYER_EVENT_ON_MAP_CHANGE);
            sEluna->PushValue(player);
            sEluna->ExecuteCall(2, 0);
        }
    }
};
class Eluna_GuildScript : public GuildScript
{
public:
    Eluna_GuildScript() : GuildScript("Eluna_GuildScript") { }
    void OnAddMember(Guild* guild, Player* player, uint8& plRank)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[GUILD_EVENT_ON_ADD_MEMBER].begin();
            itr != sEluna->ServerEventBindings[GUILD_EVENT_ON_ADD_MEMBER].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(GUILD_EVENT_ON_ADD_MEMBER);
            sEluna->PushValue(guild);
            sEluna->PushValue(player);
            sEluna->PushValue(plRank);
            sEluna->ExecuteCall(4, 0);
        }
    }
    void OnRemoveMember(Guild* guild, Player* player, bool isDisbanding, bool isKicked)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[GUILD_EVENT_ON_REMOVE_MEMBER].begin();
            itr != sEluna->ServerEventBindings[GUILD_EVENT_ON_REMOVE_MEMBER].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(GUILD_EVENT_ON_REMOVE_MEMBER);
            sEluna->PushValue(guild);
            sEluna->PushValue(player);
            sEluna->PushValue(isDisbanding);
            sEluna->PushValue(isKicked);
            sEluna->ExecuteCall(5, 0);
        }
    }
    void OnMOTDChanged(Guild* guild, const std::string& newMotd)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[GUILD_EVENT_ON_MOTD_CHANGE].begin();
            itr != sEluna->ServerEventBindings[GUILD_EVENT_ON_MOTD_CHANGE].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(GUILD_EVENT_ON_MOTD_CHANGE);
            sEluna->PushValue(guild);
            sEluna->PushValue(newMotd.c_str());
            sEluna->ExecuteCall(3, 0);
        }
    }
    void OnInfoChanged(Guild* guild, const std::string& newInfo)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[GUILD_EVENT_ON_INFO_CHANGE].begin();
            itr != sEluna->ServerEventBindings[GUILD_EVENT_ON_INFO_CHANGE].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(GUILD_EVENT_ON_INFO_CHANGE);
            sEluna->PushValue(guild);
            sEluna->PushValue(newInfo.c_str());
            sEluna->ExecuteCall(3, 0);
        }
    }
    void OnCreate(Guild* guild, Player* leader, const std::string& name)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[GUILD_EVENT_ON_CREATE].begin();
            itr != sEluna->ServerEventBindings[GUILD_EVENT_ON_CREATE].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(GUILD_EVENT_ON_CREATE);
            sEluna->PushValue(guild);
            sEluna->PushValue(leader);
            sEluna->PushValue(name.c_str());
            sEluna->ExecuteCall(4, 0);
        }
    }
    void OnDisband(Guild* guild)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[GUILD_EVENT_ON_DISBAND].begin();
            itr != sEluna->ServerEventBindings[GUILD_EVENT_ON_DISBAND].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(GUILD_EVENT_ON_DISBAND);
            sEluna->PushValue(guild);
            sEluna->ExecuteCall(2, 0);
        }
    }
    void OnMemberWitdrawMoney(Guild* guild, Player* player, uint32 &amount, bool isRepair)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[GUILD_EVENT_ON_MONEY_WITHDRAW].begin();
            itr != sEluna->ServerEventBindings[GUILD_EVENT_ON_MONEY_WITHDRAW].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(GUILD_EVENT_ON_MONEY_WITHDRAW);
            sEluna->PushValue(guild);
            sEluna->PushValue(player);
            sEluna->PushValue(amount);
            sEluna->PushValue(isRepair);
            sEluna->ExecuteCall(5, 0);
        }
    }
    void OnMemberDepositMoney(Guild* guild, Player* player, uint32 &amount)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[GUILD_EVENT_ON_MONEY_DEPOSIT].begin();
            itr != sEluna->ServerEventBindings[GUILD_EVENT_ON_MONEY_DEPOSIT].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(GUILD_EVENT_ON_MONEY_DEPOSIT);
            sEluna->PushValue(guild);
            sEluna->PushValue(player);
            sEluna->PushValue(amount);
            sEluna->ExecuteCall(4, 0);
        }
    }
    void OnItemMove(Guild* guild, Player* player, Item* pItem, bool isSrcBank, uint8 srcContainer, uint8 srcSlotId,
        bool isDestBank, uint8 destContainer, uint8 destSlotId)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[GUILD_EVENT_ON_ITEM_MOVE].begin();
            itr != sEluna->ServerEventBindings[GUILD_EVENT_ON_ITEM_MOVE].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(GUILD_EVENT_ON_ITEM_MOVE);
            sEluna->PushValue(guild);
            sEluna->PushValue(player);
            sEluna->PushValue(pItem);
            sEluna->PushValue(isSrcBank);
            sEluna->PushValue(srcContainer);
            sEluna->PushValue(srcSlotId);
            sEluna->PushValue(isDestBank);
            sEluna->PushValue(destContainer);
            sEluna->PushValue(destSlotId);
            sEluna->ExecuteCall(10, 0);
        }
    }
    void OnEvent(Guild* guild, uint8 eventType, uint32 playerGuid1, uint32 playerGuid2, uint8 newRank)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[GUILD_EVENT_ON_EVENT].begin();
            itr != sEluna->ServerEventBindings[GUILD_EVENT_ON_EVENT].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(GUILD_EVENT_ON_EVENT);
            sEluna->PushValue(guild);
            sEluna->PushValue(eventType);
            sEluna->PushValue(playerGuid1);
            sEluna->PushValue(playerGuid2);
            sEluna->PushValue(newRank);
            sEluna->ExecuteCall(6, 0);
        }
    }
    void OnBankEvent(Guild* guild, uint8 eventType, uint8 tabId, uint32 playerGuid, uint32 itemOrMoney, uint16 itemStackCount, uint8 destTabId)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[GUILD_EVENT_ON_BANK_EVENT].begin();
            itr != sEluna->ServerEventBindings[GUILD_EVENT_ON_BANK_EVENT].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(GUILD_EVENT_ON_BANK_EVENT);
            sEluna->PushValue(guild);
            sEluna->PushValue(eventType);
            sEluna->PushValue(tabId);
            sEluna->PushValue(playerGuid);
            sEluna->PushValue(itemOrMoney);
            sEluna->PushValue(itemStackCount);
            sEluna->PushValue(destTabId);
            sEluna->ExecuteCall(8, 0);
        }
    }
};
class Eluna_GroupScript : public GroupScript
{
public:
    Eluna_GroupScript() : GroupScript("Eluna_GroupScript") { }
    void OnAddMember(Group* group, uint64 guid)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[GROUP_EVENT_ON_MEMBER_ADD].begin();
            itr != sEluna->ServerEventBindings[GROUP_EVENT_ON_MEMBER_ADD].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(GROUP_EVENT_ON_MEMBER_ADD);
            sEluna->PushValue(group);
            sEluna->PushValue(guid);
            sEluna->ExecuteCall(3, 0);
        }
    }
    void OnInviteMember(Group* group, uint64 guid)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[GROUP_EVENT_ON_MEMBER_INVITE].begin();
            itr != sEluna->ServerEventBindings[GROUP_EVENT_ON_MEMBER_INVITE].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(GROUP_EVENT_ON_MEMBER_INVITE);
            sEluna->PushValue(group);
            sEluna->PushValue(guid);
            sEluna->ExecuteCall(3, 0);
        }
    }
    void OnRemoveMember(Group* group, uint64 guid, RemoveMethod method, uint64 kicker, const char* reason)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[GROUP_EVENT_ON_MEMBER_REMOVE].begin();
            itr != sEluna->ServerEventBindings[GROUP_EVENT_ON_MEMBER_REMOVE].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(GROUP_EVENT_ON_MEMBER_REMOVE);
            sEluna->PushValue(group);
            sEluna->PushValue(guid);
            sEluna->PushValue(method);
            sEluna->PushValue(kicker);
            sEluna->PushValue(reason);
            sEluna->ExecuteCall(6, 0);
        }
    }
    void OnChangeLeader(Group* group, uint64 newLeaderGuid, uint64 oldLeaderGuid)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[GROUP_EVENT_ON_LEADER_CHANGE].begin();
            itr != sEluna->ServerEventBindings[GROUP_EVENT_ON_LEADER_CHANGE].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(GROUP_EVENT_ON_LEADER_CHANGE);
            sEluna->PushValue(group);
            sEluna->PushValue(newLeaderGuid);
            sEluna->PushValue(oldLeaderGuid);
            sEluna->ExecuteCall(4, 0);
        }
    }
    void OnDisband(Group* group)
    {
        for (std::vector<int>::const_iterator itr = sEluna->ServerEventBindings[GROUP_EVENT_ON_DISBAND].begin();
            itr != sEluna->ServerEventBindings[GROUP_EVENT_ON_DISBAND].end(); ++itr)
        {
            sEluna->BeginCall((*itr));
            sEluna->PushValue(GROUP_EVENT_ON_DISBAND);
            sEluna->PushValue(group);
            sEluna->ExecuteCall(2, 0);
        }
    }
};

void Eluna::AddScriptHooks()
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