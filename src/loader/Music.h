#ifndef FAITHFUL_SRC_LOADER_AUDIO_H_
#define FAITHFUL_SRC_LOADER_AUDIO_H_

#include <array>
#include <iostream> // todo: replace by Logger
#include <string>

#include <vorbis/vorbisfile.h>

#include "../config/Loader.h"
#include "../../utils/Logger.h"
#include "IAsset.h"


namespace faithful {

namespace details {

struct MusicData;

namespace audio {

extern std::array<MusicData, faithful::config::max_active_music_num> music_heap_data_;

std::size_t OggReadCallback(void* ptr, std::size_t size,
                            std::size_t nmemb, void* datasource);
int OggSeekCallback(void *datasource, ogg_int64_t offset, int whence);
long OggTellCallback(void* datasource);

/// should be only 1 instance for the entire program
class MusicManager
    : public faithful::details::IAssetManager<faithful::config::max_active_music_num> {
 public:
  using Base = faithful::details::IAssetManager<faithful::config::max_active_music_num>;
  using InstanceInfo = details::InstanceInfo;

  MusicManager();
  ~MusicManager();

  /// not copyable
  MusicManager(const MusicManager&) = delete;
  MusicManager& operator=(const MusicManager&) = delete;

  /// movable
  MusicManager(MusicManager&&) = default;
  MusicManager& operator=(MusicManager&&) = default;

  // TODO: Is it blocking ? <<-- add thread-safety
  InstanceInfo Load(std::string&& music_path);

 private:
  bool LoadMusicData(int music_id, const std::string& music_path);

  static bool CheckStreamErrors(std::ifstream& stream);
  static void ResetStream(std::ifstream& stream, MusicData& music_data);
  static ov_callbacks GetOggCallbacks();
  static ALenum DeduceMusicFormat(const MusicData& music_data);

  using Base::active_instances_;
  using Base::free_instances_;
  int default_music_id_ = 0;  // adjust
};

}  // namespace audio
}  // namespace details

class Music : public details::IAsset {
 public:
  using Base = details::IAsset;
  using Base::Base;
  using Base::operator=;

 private:
  using Base::opengl_id_;
};

} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_AUDIO_H_
