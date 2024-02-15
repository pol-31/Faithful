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
inline constexpr int max_active_texture2d_num = 20;
inline constexpr int max_active_texture1d_num = 5;
inline constexpr int max_active_shader_program_num = 5;

inline constexpr int threads_per_texture = 4; // max / 2 || max -> +1 at static loading

inline constexpr int shader_starting_buffer_size = 256;


inline constexpr int max_simultaneously_animated_models = 20; // related to instance
inline constexpr int max_active_model_types = 40; // related to class


// see src/loader/AudioThreadPool.h/.cpp
inline constexpr float default_background_gain_step = 0.005f;

} // config
} // faithful

#endif  // FAITHFUL_CONFIG_LOADER_H_
