#ifndef FAITHFUL_SRC_LOADER_SOUND_H_
#define FAITHFUL_SRC_LOADER_SOUND_H_

#include "AssetBase.h"
#include "assets_data/SoundData.h"

namespace faithful {

class Sound
    : public details::assets::AssetBase<details::assets::SoundData> {
 public:
  using Base = details::assets::AssetBase<details::assets::SoundData>;
  using Base::Base;
  using Base::operator=;

  void Play();

  const std::string& GetFilename() const {
    return data_->filename;
  }
  int GetChannels() const {
    return data_->channels;
  }
  int GetSampleRate() const {
    return data_->sample_rate;
  }
  int GetBitsPerSample() const {
    return data_->bits_per_sample;
  }
  ALsizei GetSize() const {
    return data_->size;
  }
  ALenum GetFormat() const {
    return data_->format;
  }
  const std::unique_ptr<char>& GetData() const {
    return data_->data;
  }

 private:
  using Base::data_;
};

} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_SOUND_H_
