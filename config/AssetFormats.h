#ifndef FAITHFUL_ASSET_FORMATS_H
#define FAITHFUL_ASSET_FORMATS_H

#include <thread>
#include <array>

#include "../external/astc-encoder/Source/astcenc.h"

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

constexpr std::array<std::string_view, 4> kAudioCompFormats = {
  ".mp3", ".flac", ".wav", ".ogg"
};
constexpr std::array<std::string_view, 12> kTexCompFormats = {
  ".bmp", ".jpeg", "jpg", ".png", ".tga", ".psd", ".ppm", ".pgm",
  ".exr", ".hdr",
  ".dds", ".ktx"
};
constexpr std::array<std::string_view, 2> kModelCompFormats = {
  ".gltf", ".glb"
};

inline const int kThreadMax = std::thread::hardware_concurrency();

// if file size is less then threshold, we use only 1 thread,
// otherwise - all thread_max (for simplicity we're not taking
// thread_max/2 or thread_max/4, but it's better to do this) // TODO:
inline constexpr int kAudioCompThreadThreshold = 0; // TODO: is it kb? (if yes, float --> int)
inline constexpr int kTexCompThreadThreshold = 2000; // TODO: is it kb? (if yes, float --> int)

/// audio compression
inline constexpr float kAudioCompQuality = 0.1; // [0.1; 1]
inline constexpr int kAudioCompBufferSize = 4096000;
inline constexpr int kAudioCompChunkSize = 4096;

/// models compression:
// TODO: Model parameters: (there should be gltfpack flags - quantization?)

/// textures compression
inline constexpr int kTexCompBlockX = 6;
inline constexpr int kTexCompBlockY = 6;
inline constexpr int kTexCompBlockZ = 1;

inline constexpr int kTexLdrDataType = ASTCENC_TYPE_U8;
inline constexpr int kTexHdrDataType = ASTCENC_TYPE_F32;
inline constexpr astcenc_profile kTexCompProfileLdr = ASTCENC_PRF_LDR;
inline constexpr astcenc_profile kTexCompProfileHdr = ASTCENC_PRF_HDR;
inline constexpr astcenc_swizzle kTexCompSwizzle{
  ASTCENC_SWZ_R, ASTCENC_SWZ_G, ASTCENC_SWZ_B, ASTCENC_SWZ_A
};

// not constexpr because ASTCENC_PRE_MEDIUM defined as a static const
inline const float kTexCompQuality = ASTCENC_PRE_MEDIUM;

} // config
} // faithful

#endif //FAITHFUL_ASSET_FORMATS_H
