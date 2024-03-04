all should implement Update / Draw ?
they all should be inherited from CreatureModel, FurnitureModel, ImmovableModel


/// T A C D (transform, animation, collision, draw):
/// unique_destructible_objects_
/// examples: table, chair, vase, window
/// class only for loading & storing (not for drawing or animation)

/// T A C D S U (transform, animation, collision, draw, sound, update):
/// unique_bosses_, unique_enemies_, unique_npc_, unique_peaceful_creature_
/// class only for loading & storing (not for drawing or animation)

/// T C D (transform, collision, draw):
/// unique_pickable_item_, unique_terrain_object_
/// examples: walls, big stone, loot
/// class only for loading & storing (not for drawing or animation)