#ifndef FAITHFUL_CONFIG_IO_H_
#define FAITHFUL_CONFIG_IO_H_

#include "../external/glm/glm/glm.hpp"

namespace faithful {
namespace config {

inline constexpr char window_title[] = "Faithful";
inline constexpr int max_monitors = 5;
inline constexpr int max_monitor_modes = 10;

/// Camera
inline constexpr float camera_fov = 0.7854; // 45 degrees
inline constexpr float camera_pitch = -45.0f;
inline constexpr float camera_yaw = 0.0f;

inline constexpr float camera_perspective_near = 0.01f;
inline constexpr float camera_perspective_far = 1000.0f;

inline constexpr glm::vec3 camera_dir_world_up{0.0f, 1.0f, 0.0f};

inline constexpr glm::vec3 camera_dir_position{0.0f, 5.0f, -5.0f};

// TODO: add fov



} // config
} // faithful

#endif  // FAITHFUL_CONFIG_IO_H_
