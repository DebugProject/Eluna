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
    #include "Config.h"

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

// Ignored
// Not working
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
%ignore Position::GetPosition(float &x, float &y) const;
%ignore Position::GetPosition(float &x, float &y, float &z) const;
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
// Fix GetPosition() // Others might need this too!
%apply float& OUTPUT { float &x };
%apply float& OUTPUT { float &y };
%apply float& OUTPUT { float &z };
%apply float& OUTPUT { float &o };


%rename(GetSelection) Player::GetSelectedUnit() const;


// %import DBCStructure.i

// src/server/game/Entities
%include "../src/server/shared/Utilities/EventProcessor.h"
%include "../src/server/game/Entities/Object/ObjectDefines.h"
%include "../src/server/game/Entities/Object/Object.h"
%template(CorpseGridObject) GridObject<Corpse>;
%template(PlayerGridObject) GridObject<Player>;
%template(CreatureGridObject) GridObject<Creature>;
%template(GameObjectGridObject) GridObject<GameObject>;
%template(DynamicObjectGridObject) GridObject<DynamicObject>;
%include "../src/server/game/Entities/Object/Updates/UpdateFields.h"
%include "../src/server/game/Entities/Unit/Unit.h"
%include "../src/server/game/Entities/Item/ItemEnchantmentMgr.h"
%include "../src/server/game/Entities/Item/ItemPrototype.h"
%include "../src/server/game/Entities/Item/Item.h"
%include "../src/server/game/Entities/Item/Container/Bag.h"
%include "../src/server/game/Entities/Corpse/Corpse.h"
%include "../src/server/game/Entities/Creature/Creature.h"
%include "../src/server/game/Entities/Creature/CreatureGroups.h"
%include "../src/server/game/Entities/Creature/TemporarySummon.h"
%include "../src/server/game/Entities/Creature/GossipDef.h"
%include "../src/server/game/Entities/DynamicObject/DynamicObject.h"
%include "../src/server/game/Entities/GameObject/GameObject.h"
%include "../src/server/game/Entities/Pet/PetDefines.h"
%include "../src/server/game/Entities/Pet/Pet.h"
%include "../src/server/game/Entities/Player/Player.h"
%include "../src/server/game/Entities/Vehicle/VehicleDefines.h"
%include "../src/server/game/Entities/Vehicle/Vehicle.h"
%include "../src/server/game/Entities/Transport/Transport.h"
// %include "SocialMgr.h"
// %include "Totem.h"
// %include "ObjectPosSelector.h"
// %include "UpdateData.h"
// %include "UpdateFieldFlags.h"
// %include "UpdateMask.h"

// Other
// %include "ByteBuffer.h"
%include "../src/server/shared/Packets/WorldPacket.h"
%include "../src/server/shared/Configuration/Config.h"
%include "../src/server/shared/DataStores/DBCStore.h"
%include "../src/server/game/Maps/Map.h"
%include "../src/server/game/Spells/Spell.h"
%include "../src/server/game/Spells/Auras/SpellAuraDefines.h"
%include "../src/server/game/Spells/Auras/SpellAuraEffects.h"
%include "../src/server/game/Spells/Auras/SpellAuras.h"
%include "../src/server/game/Guilds/Guild.h"
%include "../src/server/game/Groups/Group.h"
%include "../src/server/game/Server/WorldSession.h"
%include "../src/server/game/Chat/Chat.h"
%include "../src/server/game/Globals/ObjectAccessor.h"
%include "../src/server/game/World/World.h"

// DB
%include "../src/server/shared/Database/Field.h"
%include "../src/server/shared/Database/DatabaseEnv.h"
%include "../src/server/shared/Database/QueryResult.h"
%include "../src/server/shared/Database/DatabaseWorkerPool.h"
%include "../src/server/shared/Database/Implementation/CharacterDatabase.h"
%include "../src/server/shared/Database/Implementation/LoginDatabase.h"
%include "../src/server/shared/Database/Implementation/WorldDatabase.h"

// Singletons
// NOTE! Comment the singleton out from the wrapped header
%constant World* World = sWorld;
%constant ConfigMgr* ConfigMgr = sConfigMgr;
%constant ObjectAccessor* ObjectAccessor = sObjectAccessor;
