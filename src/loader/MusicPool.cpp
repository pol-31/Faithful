#include "MusicPool.h"

#include <cstring>
#include <fstream>
#include <iostream> // todo: replace by Logger
#include <limits>
#include <string>

#include <AL/alext.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

namespace faithful {
namespace details {
namespace assets {

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

  auto moreData = std::make_unique<char[]>(length);

  music_fstream.clear();
  music_fstream.seekg(audio->size_consumed);
  if(!music_fstream.read(&moreData[0], length)) {
    if (music_fstream.eof()) {
      music_fstream.clear();
    } else if (music_fstream.fail() || music_fstream.bad()) {
      std::cerr << "Error libogg: fail / bad stream" << audio->filename
                << std::endl;
      music_fstream.clear();
      return 0;
    }
  }
  audio->size_consumed += length;

  std::memcpy(ptr, moreData.get(), length);

  music_fstream.clear();
  return length;
}

int OggSeekCallback(void *datasource, ogg_int64_t offset, int whence) {
  auto audio = reinterpret_cast<MusicData*>(datasource);

  if(whence == SEEK_CUR) {
    audio->size_consumed += offset;
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

MusicPool::DataType MusicPool::LoadImpl(
    typename Base::TrackedDataType& instance_info) {
  auto& music_stream = instance_info.data->fstream;

  instance_info.data->filename = instance_info.path;
  music_stream.open(instance_info.data->filename, std::ios::binary);
  if(!music_stream.is_open()) {
    std::cerr << "MusicManager::LoadMusicData error: couldn't open file"
              << std::endl;
    std::terminate();
  }

  ResetStream(music_stream, *instance_info.data);

  if(ov_open_callbacks(reinterpret_cast<void*>(&*instance_info.data), // not sure &*
                        &instance_info.data->ogg_vorbis_file, nullptr, -1,
                        GetOggCallbacks()) < 0) {
    std::cerr << "ERROR: Could not ov_open_callbacks" << std::endl;
    std::terminate();
  }

  vorbis_info* vorbis_info = ov_info(&instance_info.data->ogg_vorbis_file, -1);

  instance_info.data->channels = vorbis_info->channels;
  instance_info.data->bits_per_sample = 16;
  instance_info.data->sample_rate = vorbis_info->rate;

  if(CheckStreamErrors(music_stream)) {
    std::terminate();
  }

  ALenum format = DeduceMusicFormat(*instance_info.data);
  if (format == AL_NONE) {
    std::terminate();
  } else {
    instance_info.data->format = format;
  }
  return instance_info.data;
}

bool MusicPool::CheckStreamErrors(std::ifstream& stream) {
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

void MusicPool::ResetStream(std::ifstream& stream, MusicData& music_data) {
  stream.seekg(0, std::ios_base::beg);
  stream.ignore(std::numeric_limits<std::streamsize>::max());
  music_data.size = stream.gcount();
  stream.clear();
  stream.seekg(0,std::ios_base::beg);
  music_data.size_consumed = 0;
}

ov_callbacks MusicPool::GetOggCallbacks() {
  ov_callbacks ogg_callbacks;
  ogg_callbacks.read_func = OggReadCallback;
  ogg_callbacks.close_func = nullptr; // closing on our own
  ogg_callbacks.seek_func = OggSeekCallback;
  ogg_callbacks.tell_func = OggTellCallback;
  return ogg_callbacks;
}

ALenum MusicPool::DeduceMusicFormat(const MusicData& music_data) {
  int channels = music_data.channels;
  int bits_per_sample = music_data.bits_per_sample;

  // Vorbis format use float natively, so load as
  // float to avoid clipping when possible
  bool float_ext_supported = false;
  if (alIsExtensionPresent("AL_EXT_float32")) {
    float_ext_supported = true;
  }

  if (float_ext_supported) {
    if (bits_per_sample == 8) {
      if (channels == 1) {
        return AL_FORMAT_MONO_FLOAT32;
      } else if (channels == 2) {
        return AL_FORMAT_STEREO_FLOAT32;
      }
    } else if (bits_per_sample == 16) {
      if (channels == 1) {
        return AL_FORMAT_MONO_FLOAT32;
      } else if (channels == 2) {
        return AL_FORMAT_STEREO_FLOAT32;
      }
    }
  } else {
    if (bits_per_sample == 8) {
      if (channels == 1) {
        return AL_FORMAT_MONO8;
      } else if (channels == 2) {
        return AL_FORMAT_STEREO8;
      }
    } else if (bits_per_sample == 16) {
      if (channels == 1) {
        return AL_FORMAT_MONO16;
      } else if (channels == 2) {
        return AL_FORMAT_STEREO16;
      }
    }
  }

  std::cerr
      << "MusicManager::DeduceMusicFormat error: unable to deduce format"
      << std::endl;
  return AL_NONE;
}

} // namespace assets
} // namespace details
} // namespace faithful
