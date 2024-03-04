---
##Ambient
Describes peaceful creatures such as animals that have no interactions but can be killed for resources.
---
##Boss
Describes boss characters with special mechanics, HP, and unique appearances.
---
##Enemy
Represents hostile entities that attack the player when within range and move around otherwise.
---
##Furniture
Represents objects like fences, barrels, bushes, dishes, and jugs that can be destroyed by the player.
___
##Loot
Describes items that can be picked up and added to the player's inventory.
---
##NPC
Represents non-player characters that have health points (HP) and can interact with the player (talk, trade, fight).
---
##Obstacle
Describes objects like trees, buildings, ruins, and monuments that are part of the terrain, cannot be destroyed, have collision, etc.
---

Naming:
- we call *Handler OR just * if it holds only one instance (Player, Terrain, Sky)
   but it also related to Vegetation and Liquid cause basically
   they have 1 instance (map of grass / map of liquid == all 'bool' arrays)
- we call *Pool if it holds static num (> 1) of instances (Enemy, TerrainObject, PhenomenonField)
- we call *Managers if it aware of object's types and specified on one exact function
  (DrawManager - Draw(), CollisionManager - CheckCollision(), UpdateManger - Update())
