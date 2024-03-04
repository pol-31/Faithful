#ifndef FAITHFUL_CONFIG_LOADER_H_
#define FAITHFUL_CONFIG_LOADER_H_

#include <thread>

namespace faithful {
namespace config {

// TODO: deduce std::thread::hardware_concurrency() in CMake,
//   (we can't use it there because it's not constexpr)
inline constexpr int kTotalThreadsNum = 8;

/// "-2" because of DisplayInteractionThreadPool (main thread)
/// and AudioThreadPool (1 thread for each)
/// and "-1" because GameLogicThreadPool has lead_thread_
/// NOT TO CHANGE (other values not supported by now)
inline constexpr int kTotalGameLogicThreads = kTotalThreadsNum - 2 - 1;

inline constexpr int kOpenalBuffersPerMusic = 4;
inline constexpr int kOpenalBuffersSize = 65536;

inline constexpr int kThreadsPerTexture = 4; // max / 2 || max -> +1 at static loading

inline constexpr int kShaderStartingBufferSize = 256;

// see src/loader/AudioThreadPool.h/.cpp
inline constexpr float kDefaultBackgroundGainStep = 0.005f;

// TODO: explain why 6 total
inline constexpr int kOpenalSoundNum = 4;
inline constexpr int kOpenalMusicNum = 2;

inline constexpr int kMusicCacheSize = 2;
inline constexpr int kSoundCacheSize = 4;
inline constexpr int kTextureCacheSize = 20;
inline constexpr int kModelCacheSize = 10;

inline constexpr int kMapStressLoadingAssersNumThreshold = 4;

} // config
} // faithful

#endif  // FAITHFUL_CONFIG_LOADER_H_
