#ifndef FAITHFUL_CONFIG_LOADER_H_
#define FAITHFUL_CONFIG_LOADER_H_

namespace faithful {
namespace config {

inline constexpr int openal_buffers_per_music = 4;
inline constexpr int openal_buffers_size = 65536;

// TODO: explain why 6 total
inline constexpr int openal_sound_num = 4;  // at least 1
inline constexpr int openal_music_num = 2;  // at least 1

inline constexpr int max_active_music_num = 2;
inline constexpr int max_active_sound_num = 4;
inline constexpr int max_active_textures_num = 10;

} // config
} // faithful

#endif  // FAITHFUL_CONFIG_LOADER_H_
