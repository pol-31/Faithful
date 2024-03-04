#ifndef FAITHFUL_CONFIG_IO_H_
#define FAITHFUL_CONFIG_IO_H_

#include "../external/glm/glm/glm.hpp"

namespace faithful {
namespace config {

inline constexpr char kWindowTitle[] = "Faithful";
inline constexpr int kMaxMonitors = 10; // enough?)

/// Camera
inline constexpr float kCameraFov = 0.7854; // 45 degrees
inline constexpr float kCameraPitch = -45.0f;
inline constexpr float kCameraYaw = 0.0f;

inline constexpr float kCameraPerspectiveNear = 0.01f;
inline constexpr float kCameraPerspectiveFar = 1000.0f;

inline constexpr glm::vec3 kCameraDirWorldUp{0.0f, 1.0f, 0.0f};

inline constexpr glm::vec3 kCameraDirPosition{0.0f, 5.0f, -5.0f};

} // config
} // faithful

#endif  // FAITHFUL_CONFIG_IO_H_
