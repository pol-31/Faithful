#ifndef FAITHFUL_SRC_LOADER_AUDIO_H_
#define FAITHFUL_SRC_LOADER_AUDIO_H_

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream> // todo: replace by Logger
#include <map>
#include <string>
#include <vector>

#include "../config/AssetFormats.h"

#include "IAsset.h"

#include <vorbis/vorbisfile.h>

#include "AudioData.h"

#include "../../utils/Logger.h"


namespace faithful {

namespace details {
namespace audio {


std::size_t OggReadCallback(void* ptr, std::size_t size,
                            std::size_t nmemb, void* datasource) {
  auto audio = reinterpret_cast<MusicData*>(datasource);
  ALsizei length = size * nmemb;

  if(audio->size_consumed + length > audio->size) {
    length = audio->size - audio->size_consumed;
  }

  auto& music_fstream = audio->fstream;

  if(!music_fstream.is_open()) {
    music_fstream.open(audio->filename, std::ios::binary);
    if(!music_fstream.is_open()) {
      std::cerr << "Error: Can't open streaming file: "
                << audio->filename << std::endl;
      return 0;
    }
  }

  auto buffer = std::make_unique<char>(length);


  music_fstream.clear();
  music_fstream.seekg(audio->size_consumed);
  if(!music_fstream.read(buffer.get(), length)) {
    if (music_fstream.eof()) {
      music_fstream.clear();
    } else if(music_fstream.fail()) {
      std::cerr << "Error libogg: fail / bad stream" << audio->filename
                << std::endl;
      music_fstream.clear();
      return 0;
    }
  }
  audio->size_consumed = audio->size_consumed + length;

  std::memcpy(ptr, buffer.get(), length);

  music_fstream.clear();

  return length;
}

int OggSeekCallback(void *datasource, ogg_int64_t offset, int whence) {
  auto audio = reinterpret_cast<MusicData*>(datasource);

  if(whence == SEEK_CUR) {
    audio->size_consumed = audio->size_consumed + offset;
  } else if(whence == SEEK_END) {
    audio->size_consumed = audio->size - offset;
  } else if(whence == SEEK_SET) {
    audio->size_consumed = offset;
  } else {
    return -1;
  }

  if(audio->size_consumed < 0) {
    audio->size_consumed = 0;
    return -1;
  }

  if(audio->size_consumed > audio->size) {
    audio->size_consumed = audio->size;
    return -1;
  }

  return 0;
}

long OggTellCallback(void* datasource) {
  auto audio = reinterpret_cast<MusicData*>(datasource);
  return audio->size_consumed;
}



/// should be only 1 instance for the entire program
template <int max_active_music> // 2 by default
class MusicManager
    : public faithful::details::IAssetManager<max_active_music> {
 public:
  using Base = faithful::details::IAssetManager<max_active_music>;
  using InstanceInfo = details::InstanceInfo;

  MusicManager() {
    for (int i = 0; i < max_active_music; ++i) {
      active_instances_[i] = i;
      free_instances_[i] = i;
      data_[i] = {}; // TODO: optimize num of allocations
    }
  }

  ~MusicManager() {
    for (auto& i : active_instances_) {
      if (i.ref_counter.Active()) {
        std::cerr
            << "MusicManager deleted, but ref_counter is still active for "
            << i.opengl_id << std::endl;
        // TODO: terminate?
      }
      delete i.ref_counter;
    }
  }

  /// not copyable
  MusicManager(const MusicManager&) = delete;
  MusicManager& operator=(const MusicManager&) = delete;

  /// movable
  MusicManager(MusicManager&&) = default;
  MusicManager& operator=(MusicManager&&) = default;

  // TODO: Is it blocking ? <<-- add thread-safety
  InstanceInfo Load(std::string&& music_path) {
    auto [sound_id, active_instances_id, is_new_id] = Base::AcquireId(music_path); // structure binding
    if (sound_id == 0) {
      return {"", nullptr, default_music_id_};
    } else if (!is_new_id) {
      return {music_path, active_instances_[active_instances_id], sound_id};
    }
    if (!LoadMusicData(sound_id, music_path)) {
      return {"", nullptr, default_music_id_};
    }
    return {music_path, active_instances_[active_instances_id], sound_id};
  }

 private:
  bool LoadMusicData(int music_id, const std::string& music_path) {
    auto found_element = data_.find(music_id);
    if (found_element == data_.end()) {
      std::cerr << "MusicManager::data_ error: can't find id in LoadMusicData()"
                << std::endl;
      return false;
    }

    auto& found_data = found_element->second;
    auto& music_stream = *&found_data.fstream;

    found_data.filename = music_path;
    music_stream.open(music_path, std::ios::binary);
    if(!music_stream.is_open()) {
      std::cerr << "MusicManager::LoadMusicData error: couldn't open file"
                << std::endl;
      return false;
    }

    ResetStream(music_stream, found_data);

    if(ov_open_callbacks(reinterpret_cast<void*>(&found_data),
                          &found_data.ogg_vorbis_file, nullptr, -1,
                          GetOggCallbacks()) < 0) {
      std::cerr << "ERROR: Could not ov_open_callbacks" << std::endl;
      return false;
    }

    vorbis_info* vorbis_info = ov_info(&found_data.ogg_vorbis_file, -1);

    found_data.channels = vorbis_info->channels;
    found_data.bits_per_sample = 16;
    found_data.sample_rate = vorbis_info->rate;

    if(!CheckStreamErrors(music_stream)) {
      return false;
    }

    ALenum format = DeduceMusicFormat(found_data);
    if (format == AL_NONE) {
      return false;
    } else {
      found_data.format = format;
    }
    return true;
  }

  bool CheckStreamErrors(std::ifstream& stream) {
    if(stream.eof()) {
      std::cerr << "ERROR: Already reached EOF without loading data" << std::endl;
    } else if(stream.fail()) {
      std::cerr << "ERROR: Fail bit set" << std::endl;
    } else if(!stream) {
      std::cerr << "ERROR: file is false" << std::endl;
    } else {
      return false;
    }
    return true;
  }

  void ResetStream(std::ifstream& stream, MusicData& music_data) {
    stream.seekg(0, std::ios_base::beg);
    stream.ignore(std::numeric_limits<std::streamsize>::max());
    music_data.size = stream.gcount();
    stream.clear();
    stream.seekg(0,std::ios_base::beg);
    music_data.size_consumed = 0;
  }

  ov_callbacks GetOggCallbacks() {
    ov_callbacks ogg_callbacks;
    ogg_callbacks.read_func = OggReadCallback;
    ogg_callbacks.close_func = nullptr; // closing on our own
    ogg_callbacks.seek_func = OggSeekCallback;
    ogg_callbacks.tell_func = OggTellCallback;
    return ogg_callbacks;
  }

  ALenum DeduceMusicFormat(const MusicData& music_data) {
    int channels = music_data.channels;
    int bits_per_sample = music_data.bits_per_sample;

    if(channels == 1 && bits_per_sample == 8) {
      return AL_FORMAT_MONO8;
    } else if(channels == 1 && bits_per_sample == 16) {
      return AL_FORMAT_MONO16;
    } else if(channels == 2 && bits_per_sample == 8) {
      return AL_FORMAT_STEREO8;
    } else if(channels == 2 && bits_per_sample == 16) {
      return AL_FORMAT_STEREO16;
    } else {
      std::cerr
          << "MusicManager::DeduceMusicFormat error: unable to deduce format"
          << std::endl;
      return AL_NONE;
    }
  }

  std::map<int, MusicData> data_;

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

  // TODO: add protected section; friend AudioThreadPool; protected MusicData
 private:
  using Base::opengl_id_;
};

} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_AUDIO_H_
