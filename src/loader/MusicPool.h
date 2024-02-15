#ifndef FAITHFUL_SRC_LOADER_MUSICPOOL_H_
#define FAITHFUL_SRC_LOADER_MUSICPOOL_H_

#include <array>
#include <iostream> // todo: replace by Logger
#include <string>

#include <AL/al.h>
#include <vorbis/vorbisfile.h>

#include "../config/Loader.h"
#include "../../utils/Logger.h"
#include "IAssetPool.h"
#include "AssetInstanceInfo.h"

#include "AudioData.h"

namespace faithful {
namespace details {

class AudioThreadPool;

namespace assets {

std::size_t OggReadCallback(void* ptr, std::size_t size,
                            std::size_t nmemb, void* datasource);
int OggSeekCallback(void *datasource, ogg_int64_t offset, int whence);
long OggTellCallback(void* datasource);

/// should be only 1 instance for the entire program
class MusicPool : public IAssetPool<faithful::config::max_active_music_num> {
 public:
  using Base = IAssetPool<faithful::config::max_active_music_num>;

  MusicPool();
  ~MusicPool() = default;

  /// not copyable
  MusicPool(const MusicPool&) = delete;
  MusicPool& operator=(const MusicPool&) = delete;

  /// movable
  MusicPool(MusicPool&&) = default;
  MusicPool& operator=(MusicPool&&) = default;

  // TODO: Is it blocking ? <<-- add thread-safety
  AssetInstanceInfo Load(std::string&& music_path);

 protected:
  friend class faithful::details::AudioThreadPool;
  std::array<MusicData, faithful::config::max_active_sound_num> music_heap_data_;

 private:
  bool LoadMusicData(int music_id);

  static bool CheckStreamErrors(std::ifstream& stream);
  static void ResetStream(std::ifstream& stream, MusicData& music_data);
  static ov_callbacks GetOggCallbacks();
  static ALenum DeduceMusicFormat(const MusicData& music_data);

  using Base::active_instances_;
  using Base::free_instances_;
  using Base::default_id_;
};

}  // namespace assets
}  // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_MUSICPOOL_H_
