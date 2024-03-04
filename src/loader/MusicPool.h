#ifndef FAITHFUL_SRC_LOADER_MUSICPOOL_H_
#define FAITHFUL_SRC_LOADER_MUSICPOOL_H_

#include <AL/al.h>
#include <vorbis/vorbisfile.h>

#include "IAssetPool.h"
#include "../../config/Loader.h"
#include "assets_data/MusicData.h"

namespace faithful {
namespace details {
namespace assets {

std::size_t OggReadCallback(
    void* ptr, std::size_t size, std::size_t nmemb, void* datasource);
int OggSeekCallback(void *datasource, ogg_int64_t offset, int whence);
long OggTellCallback(void* datasource);

class MusicPool
    : public IAssetPool<MusicData, faithful::config::kMusicCacheSize> {
 public:
  using Base = IAssetPool<MusicData, faithful::config::kMusicCacheSize>;
  using DataType = typename Base::DataType;

  MusicPool() = default;

  /// not copyable
  MusicPool(const MusicPool&) = delete;
  MusicPool& operator=(const MusicPool&) = delete;

  /// movable
  MusicPool(MusicPool&&) = default;
  MusicPool& operator=(MusicPool&&) = default;

 private:
  DataType LoadImpl(typename Base::TrackedDataType& instance_info) override;

  static bool CheckStreamErrors(std::ifstream& stream);
  static void ResetStream(std::ifstream& stream, MusicData& music_data);
  static ov_callbacks GetOggCallbacks();
  static ALenum DeduceMusicFormat(const MusicData& music_data);
};

} // namespace assets
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_MUSICPOOL_H_
