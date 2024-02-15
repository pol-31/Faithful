#ifndef FAITHFUL_CONFIG_ENTITIES_H_
#define FAITHFUL_CONFIG_ENTITIES_H_

namespace faithful {
namespace config {

// maximum number of entities that can appear simultaneously on the screen
namespace max_instances {
inline constexpr int kAmbient = 20;
inline constexpr int kBoss = 2;
inline constexpr int kEnemy = 20;
inline constexpr int kFurniture = 30;
inline constexpr int kLoot = 10;
inline constexpr int kNPC = 5;
inline constexpr int kObstacle = 30;
} // max_instances

// maximum number of unique entity types that can be loaded simultaneously in memory
namespace max_types {
inline constexpr int kAmbient = 10;
inline constexpr int kBoss = 2;
inline constexpr int kEnemy = 6;
inline constexpr int kFurniture = 2;
inline constexpr int kLoot = 10;
inline constexpr int kNPC = 3;
inline constexpr int kObstacle = 10;
} // max_types

} // config
} // faithful

#endif  // FAITHFUL_CONFIG_ENTITIES_H_
