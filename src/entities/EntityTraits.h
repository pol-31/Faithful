#ifndef FAITHFUL_SRC_ENTITIES_ENTITYTRAITS_H
#define FAITHFUL_SRC_ENTITIES_ENTITYTRAITS_H

namespace faithful {


// TODO: do we need this file?

enum class EntityType {
  kAmbient,
  kBoss,
  kEnemy,
  kFurniture,
  kLoot,
  kNpc,
  kObstacle
};

// TODO: maybe add some other traits like hp/damage/speed/resistance/etc...

}  // namespace faithful

#endif  // FAITHFUL_SRC_ENTITIES_ENTITYTRAITS_H
