%module Eluna
%{
    // src/server/game/Entities
    #include "Bag.h"
    #include "Corpse.h"
    #include "Creature.h"
    #include "CreatureGroups.h"
    #include "DynamicObject.h"
    #include "GameObject.h"
    #include "GossipDef.h"
    #include "Item.h"
    #include "ItemEnchantmentMgr.h"
    #include "ItemPrototype.h"
    #include "Object.h"
    #include "ObjectDefines.h"
    // #include "ObjectPosSelector.h"
    #include "Pet.h"
    #include "PetDefines.h"
    #include "Player.h"
    // #include "SocialMgr.h"
    #include "TemporarySummon.h"
    // #include "Totem.h"
    #include "Transport.h"
    #include "Unit.h"
    // #include "UpdateData.h"
    // #include "UpdateFieldFlags.h"
    #include "UpdateFields.h"
    // #include "UpdateMask.h"
    #include "Vehicle.h"
    #include "VehicleDefines.h"
    
    // Other
    #include "EventProcessor.h"
    #include "Map.h"
    #include "Spell.h"
    // #include "ByteBuffer.h"
    #include "WorldPacket.h"
    #include "SpellAuraDefines.h"
    #include "SpellAuraEffects.h"
    #include "SpellAuras.h"
    #include "Guild.h"
    #include "Group.h"
    #include "WorldSession.h"
    #include "Chat.h"
    #include "DBCStore.h"

    // DB
    #include "DatabaseWorkerPool.h"
    #include "Field.h"
    // #include "CharacterDatabase.h"
    // #include "LoginDatabase.h"
    // #include "WorldDatabase.h"
    #include "DatabaseEnv.h"
    #include "QueryResult.h"
%}

// SWIG stuff
// All needed?
%include <typemaps.i>
%include <carrays.i>
%include "std_string.i"
%include "lua_fnptr.i"
%include cpointer.i

// Ignored functions
%ignore ChatCommand;

// Not working
%ignore Unit::HasSpell;
%ignore Unit::GetFactionReactionTo;
%ignore Position::PositionXYZOStream;
%ignore Position::PositionXYZStream;
%ignore Group::CountTheRoll;
%ignore Group::GetRoll;
%ignore Group::ChangeMembersGroup;
%ignore Group::InInstance;
%ignore Spell::EffectSummonCritter;

// Not defined
%ignore Vehicle::EjectPassenger;
%ignore Unit::_RemoveNoStackAuraApplicationsDueToAura;
%ignore Unit::_IsNoStackAuraDueToAura;
%ignore Player::CheckAllAchievementCriteria;
%ignore Player::ChangeSpeakTime;
%ignore Player::CalcRage;
%ignore Player::GetSpellByProto;
%ignore Player::SetFloatValueInArray;
%ignore Object::ApplyModUInt64Value;
%ignore Bag::Clear;
%ignore ObjectAccessor::FindCreature;
%ignore WorldSession::SendItemInfo;
%ignore WorldSession::SendItemPageInfo;
%ignore WorldSession::HandleAuthSessionOpcode;
%ignore WorldSession::HandleBattleMasterHelloOpcode;
%ignore WorldSession::HandleChannelModerate;
%ignore WorldSession::HandlePingOpcode;
%ignore WorldSession::HandleMeetingStoneInfo;
%ignore WorldSession::HandleLookingForGroup;

// Shadowed
%ignore Position::Relocate(Position const* pos);
%ignore Position::GetPosition(float &x, float &y);
%ignore Position::GetPosition(float &x, float &y, float &z);
%ignore Player::HasEnoughMoney(int32 amount) const;
%ignore Unit::GetCurrentSpell(uint32 spellType) const;

// Fix C++ notes
#  define ATTR_PRINTF(F, V)
#  define OVERRIDE
#  define FINAL
//Cpp11
//#  define OVERRIDE override
//#  define FINAL final

// Fix ACE types
%apply char                      { int8 };
%apply short int                 { int16 };
%apply long int                  { int32 };
%apply long long int             { int64 };
%apply unsigned char             { uint8 };
%apply unsigned short int        { uint16 };
%apply unsigned long int         { uint32 };
%apply unsigned long long int    { uint64 };

/*
%apply int* OUTPUT {int *result}; // int *result is output
%apply int* INPUT {int *x1, int *y1}; // int *x1 and int *y1 are input
%apply int* INOUT {int *sx, int *sy}; // int *sx and int *sy are input and output
*/
// Fix GetPosition()
%apply float& OUTPUT { float &x };
%apply float& OUTPUT { float &y };
%apply float& OUTPUT { float &z };
%apply float& OUTPUT { float &o };

// %import DBCStructure.i

// src/server/game/Entities
%include "GossipDef.h"
%include "EventProcessor.h"
%include "ObjectDefines.h"
%include "Object.h"
%template(CorpseGridObject) GridObject<Corpse>;
%template(PlayerGridObject) GridObject<Player>;
%template(CreatureGridObject) GridObject<Creature>;
%template(GameObjectGridObject) GridObject<GameObject>;
%template(DynamicObjectGridObject) GridObject<DynamicObject>;
%include "UpdateFields.h"
%include "Unit.h"
%include "ItemEnchantmentMgr.h"
%include "ItemPrototype.h"
%include "Item.h"
%include "Bag.h"
%include "Corpse.h"
%include "Creature.h"
%include "CreatureGroups.h"
%include "DynamicObject.h"
%include "GameObject.h"
%include "TemporarySummon.h"
%include "PetDefines.h"
%include "Pet.h"
%include "Player.h"
// %include "SocialMgr.h"
// %include "Totem.h"
%include "VehicleDefines.h"
%include "Vehicle.h"
%include "Transport.h"
// %include "ObjectPosSelector.h"
// %include "UpdateData.h"
// %include "UpdateFieldFlags.h"
// %include "UpdateMask.h"

// Other
%include "Map.h"
%include "Spell.h"
// %include "ByteBuffer.h"
%include "WorldPacket.h"
%include "SpellAuraDefines.h"
%include "SpellAuraEffects.h"
%include "SpellAuras.h"
%include "Guild.h"
%include "Group.h"
%include "WorldSession.h"
%include "Chat.h"
%include "Config.h"
%include "DBCStore.h"
%include "ObjectAccessor.h"
%include "World.h"

// DB
%include "Field.h"
%include "DatabaseEnv.h"
%include "QueryResult.h"
%include "DatabaseWorkerPool.h"
%include "CharacterDatabase.h"
%include "LoginDatabase.h"
%include "WorldDatabase.h"
