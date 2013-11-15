#include "LuaEngine.h"

// RegisterServerHook(event, function)
static int RegisterServerHook(lua_State* L)
{
    lua_settop(L, 2);
    uint32 ev = luaL_checkunsigned(L, 1);
    if (ev == 0)
    {
        luaL_error(L, "0 is not a valid event");
        return 0;
    }
    luaL_checktype(L, lua_gettop(L), LUA_TFUNCTION);

    int functionRef  = lua_ref(L, true);
    if (functionRef > 0)
        sEluna->Register(REGTYPE_SERVER, 0, ev, functionRef);
    return 0;
}

//RegisterCreatureGossipEvent(entry, event, function)
static int RegisterCreatureGossipEvent(lua_State* L)
{
    lua_settop(L, 3);
    uint32 entry = luaL_checkunsigned(L, 1);
    uint32 ev = luaL_checkunsigned(L, 2);
    if (ev == 0)
    {
        luaL_error(L, "0 is not a valid event");
        return 0;
    }
    luaL_checktype(L, lua_gettop(L), LUA_TFUNCTION);

    int functionRef  = lua_ref(L, true);
    if (functionRef > 0)
        sEluna->Register(REGTYPE_CREATURE_GOSSIP, entry, ev, functionRef);
    return 0;
}

// RegisterGameObjectGossipEvent(entry, event, function)
static int RegisterGameObjectGossipEvent(lua_State* L)
{
    lua_settop(L, 3);
    uint32 entry = luaL_checkunsigned(L, 1);
    uint32 ev = luaL_checkunsigned(L, 2);
    if (ev == 0)
    {
        luaL_error(L, "0 is not a valid event");
        return 0;
    }
    luaL_checktype(L, lua_gettop(L), LUA_TFUNCTION);

    int functionRef  = lua_ref(L, true);
    if (functionRef > 0)
        sEluna->Register(REGTYPE_GAMEOBJECT_GOSSIP, entry, ev, functionRef);
    return 0;
}

// RegisterItemEvent(entry, event, function)
static int RegisterItemEvent(lua_State* L)
{
    lua_settop(L, 3);
    uint32 entry = luaL_checkunsigned(L, 1);
    uint32 ev = luaL_checkunsigned(L, 2);
    if (ev == 0)
    {
        luaL_error(L, "0 is not a valid event");
        return 0;
    }
    luaL_checktype(L, lua_gettop(L), LUA_TFUNCTION);

    int functionRef  = lua_ref(L, true);
    if (functionRef > 0)
        sEluna->Register(REGTYPE_ITEM, entry, ev, functionRef);
    return 0;
}

// RegisterItemGossipEvent(entry, event, function)
static int RegisterItemGossipEvent(lua_State* L)
{
    lua_settop(L, 3);
    uint32 entry = luaL_checkunsigned(L, 1);
    uint32 ev = luaL_checkunsigned(L, 2);
    if (ev == 0)
    {
        luaL_error(L, "0 is not a valid event");
        return 0;
    }
    luaL_checktype(L, lua_gettop(L), LUA_TFUNCTION);

    int functionRef  = lua_ref(L, true);
    if (functionRef > 0)
        sEluna->Register(REGTYPE_ITEM_GOSSIP, entry, ev, functionRef);
    return 0;
}

// RegisterPlayerGossipEvent(menu_id, event, function)
static int RegisterPlayerGossipEvent(lua_State* L)
{
    lua_settop(L, 3);
    uint32 menu_id = luaL_checkunsigned(L, 1);
    uint32 ev = luaL_checkunsigned(L, 2);
    if (ev == 0)
    {
        luaL_error(L, "0 is not a valid event");
        return 0;
    }
    luaL_checktype(L, lua_gettop(L), LUA_TFUNCTION);

    int functionRef  = lua_ref(L, true);
    if (functionRef > 0)
        sEluna->Register(REGTYPE_PLAYER_GOSSIP, menu_id, ev, functionRef);
    return 0;
}

// RegisterCreatureEvent(entry, event, function)
static int RegisterCreatureEvent(lua_State* L)
{
    lua_settop(L, 3);
    uint32 entry = luaL_checkunsigned(L, 1);
    uint32 ev = luaL_checkunsigned(L, 2);
    if (ev == 0)
    {
        luaL_error(L, "0 is not a valid event");
        return 0;
    }
    luaL_checktype(L, lua_gettop(L), LUA_TFUNCTION);

    int functionRef  = lua_ref(L, true);
    if (functionRef > 0)
        sEluna->Register(REGTYPE_CREATURE, entry, ev, functionRef);
    return 0;
}

// RegisterGameObjectEvent(entry, event, function)
static int RegisterGameObjectEvent(lua_State* L)
{
    lua_settop(L, 3);
    uint32 entry = luaL_checkunsigned(L, 1);
    uint32 ev = luaL_checkunsigned(L, 2);
    if (ev == 0)
    {
        luaL_error(L, "0 is not a valid event");
        return 0;
    }
    luaL_checktype(L, lua_gettop(L), LUA_TFUNCTION);

    int functionRef  = lua_ref(L, true);
    if (functionRef > 0)
        sEluna->Register(REGTYPE_GAMEOBJECT, entry, ev, functionRef);
    return 0;
}

// GetLuaEngine() - Gets lua engine name
static int GetLuaEngine(lua_State* L)
{
    sEluna->PushValue("ElunaEngine");
    return 1;
}

void RegisterGlobals(lua_State* L)
{
    SWIG_Lua_module_add_function(L, "RegisterServerHook", RegisterServerHook);
    SWIG_Lua_module_add_function(L, "RegisterCreatureGossipEvent", RegisterCreatureGossipEvent);
    SWIG_Lua_module_add_function(L, "RegisterGameObjectGossipEvent", RegisterGameObjectGossipEvent);
    SWIG_Lua_module_add_function(L, "RegisterItemEvent", RegisterItemEvent);
    SWIG_Lua_module_add_function(L, "RegisterItemGossipEvent", RegisterItemGossipEvent);
    SWIG_Lua_module_add_function(L, "RegisterPlayerGossipEvent", RegisterPlayerGossipEvent);
    SWIG_Lua_module_add_function(L, "RegisterCreatureEvent", RegisterCreatureEvent);
    SWIG_Lua_module_add_function(L, "RegisterGameObjectEvent", RegisterGameObjectEvent);
    SWIG_Lua_module_add_function(L, "GetLuaEngine", GetLuaEngine);
}