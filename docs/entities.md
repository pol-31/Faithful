##TerrainObject
Describes objects like trees, buildings, ruins, and monuments that are part of the terrain, cannot be destroyed, have collision, etc.
---
##DestructibleObject
Represents objects like fences, barrels, bushes, dishes, and jugs that can be destroyed by the player.
---
##PeacefulCreature
Describes peaceful creatures such as animals that have no interactions but can be killed for resources.
___
##NPC
Represents non-player characters that have health points (HP) and can interact with the player (talk, trade, fight).
---
##Enemy
Represents hostile entities that attack the player when within range and move around otherwise.
---
##Boss
Describes boss characters with special mechanics, HP, and unique appearances.
---
##PickableItem
Describes items that can be picked up and added to the player's inventory.
---

### We preserve number of each in config/Entities.h

We have this inside the stc/entities/EntityTraits:
enum class EntityType {
kBoss,
kDestructibleObject,
kEnemy,
kNPC,
kPeacefulCreature,
kPickableItem,
kTerrainObject
};

And this inside the config/Entities.h:

inline constexpr int kBoss = 2;
inline constexpr int kDestructibleObject = 30;
inline constexpr int kEnemy = 20;
inline constexpr int kNPC = 5;
inline constexpr int kPeacefulCreature = 20;
inline constexpr int kPickableItem = 10;
inline constexpr int kTerrainObject = 30;

inline constexpr int kBoss = 2;
inline constexpr int kDestructibleObject = 2;
inline constexpr int kEnemy = 6;
inline constexpr int kNPC = 3;
inline constexpr int kPeacefulCreature = 10;
inline constexpr int kPickableItem = 10;
inline constexpr int kTerrainObject = 10;

They are used inside the ModelManager, CollisionManager, DrawableManager, UpdatableManager, etc...

Naming:
- we call *Handler OR just * if it holds only one instance (Player, Terrain, Sky)
   but it also related to Vegetation and Liquid cause basically
   they have 1 instance (map of grass / map of liquid == all 'bool' arrays)
- we call *Pool if it holds static num (> 1) of instances (Enemy, TerrainObject, PhenomenonField)
- we call *Managers if it aware of object's types and specified on one exact function
  (DrawManager - Draw(), CollisionManager - CheckCollision(), UpdateManger - Update())
