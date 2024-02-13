#ifndef FAITHFUL_MODEL_H
#define FAITHFUL_MODEL_H

namespace faithful {

enum class EntityType {
  kBoss,
  kDestructibleObject,
  kEnemy,
  kNPC,
  kPeacefulCreature,
  kPickableItem,
  kTerrainObject
};

// TODO: maybe add some other traits like hp/damage/speed/resistance/etc...

}  // namespace faithful

#endif  // FAITHFUL_MODEL_H
