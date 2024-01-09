#ifndef FAITHFUL_ASSET_FORMATS_H
#define FAITHFUL_ASSET_FORMATS_H

#include <thread>
#include <array>

#include "../external/astc-encoder/Source/astcenc.h"

namespace faithful {
namespace config {

/// _____AssetDownloader_____

inline constexpr char asset_info_url[] =
    "https://raw.githubusercontent.com/pol-31/Faithful/main/config/asset_info.txt";

/// _____AssetProcessor_____

/// compression:

constexpr std::array<std::string_view, 4> audio_comp_formats = {
  ".mp3", ".flac", ".wav", ".ogg"
};
constexpr std::array<std::string_view, 12> tex_comp_formats = {
  ".bmp", ".jpeg", "jpg", ".png", ".tga", ".psd", ".ppm", ".pgm",
  ".exr", ".hdr",
  ".dds", ".ktx"
};
constexpr std::array<std::string_view, 2> model_comp_formats = {
  ".gltf", ".glb"
};

inline const int thread_max = std::thread::hardware_concurrency();

// if file size is less then threshold, we use only 1 thread,
// otherwise - all thread_max (for simplicity we're not taking
// thread_max/2 or thread_max/4, but it's better to do this) // TODO:
inline constexpr int audio_comp_thread_threshold = 0; // TODO: is it kb? (if yes, float --> int)
inline constexpr int tex_comp_thread_threshold = 2000; // TODO: is it kb? (if yes, float --> int)

/// decompression:
inline constexpr char audio_music_decomp_format[] = "ogg";
inline constexpr char audio_sound_decomp_format[] = "wav";
inline constexpr char tex_ldr_decomp_format[] = "png";
inline constexpr char tex_hdr_decomp_format[] = "hdr";
inline constexpr char tex_nmap_decomp_format[] = "png";
inline constexpr char models_decomp_format[] = "gltf";

/// audio compression
inline constexpr float audio_comp_quality = 0.4; // [0.1; 1]
inline constexpr int audio_comp_chunk_size = 1024 * 64;
inline constexpr int audio_decomp_frames_count = 4096;

/// models compression:
// TODO: Model parameters:
//  preprocess/postprocess flags (Assimp::Importer) + optimization algorithms

/// textures compression
inline constexpr int tex_comp_block_x = 6;
inline constexpr int tex_comp_block_y = 6;
inline constexpr int tex_comp_block_z = 1;

inline constexpr int tex_data_type = ASTCENC_TYPE_U8;
inline constexpr astcenc_profile tex_comp_profile_ldr = ASTCENC_PRF_LDR;
inline constexpr astcenc_profile tex_comp_profile_hdr = ASTCENC_PRF_HDR;
inline constexpr astcenc_swizzle tex_comp_swizzle {
  ASTCENC_SWZ_R, ASTCENC_SWZ_G, ASTCENC_SWZ_B, ASTCENC_SWZ_A
};

// not constexpr because ASTCENC_PRE_MEDIUM defined as a static const
inline const float tex_comp_quality = ASTCENC_PRE_MEDIUM;

} // config
} // faithful

#endif //FAITHFUL_ASSET_FORMATS_H
