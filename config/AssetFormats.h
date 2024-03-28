#ifndef FAITHFUL_ASSET_FORMATS_H
#define FAITHFUL_ASSET_FORMATS_H

#include <thread>
#include <array>
#include <string_view>

#include <astcenc.h>

// TODO: constinit?

namespace faithful {
namespace config {

/// _____AssetDownloader_____
inline constexpr char kAssetInfoUrl[] =
    "https://raw.githubusercontent.com/pol-31/Faithful/main/config/faithful_assets_info.txt";

/// _____AssetDownloader_____
inline constexpr char kDefaultZipName[] = "faithful_assets.zip";
inline constexpr char kDefaultInfoName[] = "faithful_assets_info.txt";
inline constexpr char kDefaultTempDirName[] = "AssetPack_temp";
inline constexpr char kDefaultCurlCliCommand[] = "curl -f -o";
inline constexpr char kDefaultWgetCommand[] = "wget -O";
inline constexpr char kDefaultInvokeWebrequestCommand[] =
    "powershell.exe -Command \"Invoke-WebRequest";
inline constexpr char kDefaultUrl[] =
    "https://drive.usercontent.google.com/download?id=1zFzeoB8Su-4yt9bqMOgJ7uCRbKMlP2DT&export=download&authuser=0&confirm=t&uuid=9bc19c5b-d6af-4ea1-87f4-a0c26a6701fd&at=APZUnTWJGjDQQGFN2pYOQosBzPL3:1705005036841";
inline constexpr int kDefaultUrlRedirectionsCount = 0;

/// _____AssetProcessor_____

/// compression:
// for more information see faithful/external/stb/stb_image.h
constexpr std::array<std::string_view, 10> kTexCompFormats = {
    ".bmp", ".hdr", ".HDR", ".jpeg", "jpg",
    ".pgm", ".png", ".ppm", ".psd", ".tga"
};

constexpr std::array<std::string_view, 2> kModelCompFormats = {
    ".glb", ".gltf"
};

// TODO: need to deduce std::thread::hardware_concurrency in CMake
inline const int kMaxHardwareThread = std::thread::hardware_concurrency();

/// textures compression
inline constexpr int kTexCompBlockX = 4;
inline constexpr int kTexCompBlockY = 4;
inline constexpr int kTexCompBlockZ = 1;

inline constexpr astcenc_type kTexLdrDataType = ASTCENC_TYPE_U8;
inline constexpr astcenc_type kTexHdrDataType = ASTCENC_TYPE_F32;

/// Wow, this is the first time I've used designated initialization.
/// Otherwise, I would need to add 15 '0' and 'nullptr' values
inline constexpr astcenc_config kTextureConfigLdr{
    .profile = ASTCENC_PRF_LDR, .flags = 0,
    .block_x = kTexCompBlockX, .block_y = kTexCompBlockY,
    .block_z = kTexCompBlockZ
};
inline constexpr astcenc_config kTextureConfigHdr{
    .profile = ASTCENC_PRF_HDR, .flags = 0,
    .block_x = kTexCompBlockX, .block_y = kTexCompBlockY,
    .block_z = kTexCompBlockZ
};
inline constexpr astcenc_config kTextureConfigLdrNormal{
    .profile = ASTCENC_PRF_LDR, .flags = ASTCENC_FLG_MAP_NORMAL,
    .block_x = kTexCompBlockX, .block_y = kTexCompBlockY,
    .block_z = kTexCompBlockZ
};
inline constexpr astcenc_config kTextureConfigLdrAlphaPerceptual{
    .profile = ASTCENC_PRF_LDR,
    .flags = ASTCENC_FLG_USE_ALPHA_WEIGHT | ASTCENC_FLG_USE_PERCEPTUAL,
    .block_x = kTexCompBlockX, .block_y = kTexCompBlockY,
    .block_z = kTexCompBlockZ
};

inline constexpr astcenc_swizzle kTextureSwizzleRgba{
  ASTCENC_SWZ_R, ASTCENC_SWZ_G, ASTCENC_SWZ_B, ASTCENC_SWZ_A
};
inline constexpr astcenc_swizzle kTextureSwizzleRgb1{
  ASTCENC_SWZ_R, ASTCENC_SWZ_G, ASTCENC_SWZ_B, ASTCENC_SWZ_1
};
inline constexpr astcenc_swizzle kTextureSwizzleGggb{
  ASTCENC_SWZ_G, ASTCENC_SWZ_G, ASTCENC_SWZ_G, ASTCENC_SWZ_B
};
inline constexpr astcenc_swizzle kTextureSwizzleRrrg{
  ASTCENC_SWZ_R, ASTCENC_SWZ_R, ASTCENC_SWZ_R, ASTCENC_SWZ_G
};
inline constexpr astcenc_swizzle kTextureSwizzleRrr1{
  ASTCENC_SWZ_R, ASTCENC_SWZ_R, ASTCENC_SWZ_R, ASTCENC_SWZ_1
};
inline constexpr astcenc_swizzle kTextureSwizzle0ra1{
  ASTCENC_SWZ_0, ASTCENC_SWZ_R, ASTCENC_SWZ_A, ASTCENC_SWZ_1
};
inline constexpr astcenc_swizzle kTextureSwizzleRaz1{
  ASTCENC_SWZ_R, ASTCENC_SWZ_A, ASTCENC_SWZ_Z, ASTCENC_SWZ_1
};

// not constexpr because ASTCENC_PRE_MEDIUM defined as a static const
inline const float kTexCompQuality = ASTCENC_PRE_MEDIUM;

} // config
} // faithful

#endif //FAITHFUL_ASSET_FORMATS_H
