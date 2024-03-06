#include "AudioContext.h"

#include "../../utils/Logger.h"

#include "../loader/MusicPool.h"
#include "../loader/Music.h"
#include "../loader/SoundPool.h"
#include "../loader/Sound.h"

namespace faithful {
namespace details {

// TODO: alSourceRewind(source); for Play(Sound) to pleyback from the beginning

AudioContext::AudioContext() {
  InitContext();
  InitOpenALBuffersAndSources();
}

AudioContext::~AudioContext() {
  DeInitOpenALBuffersAndSources();
  DeInitContext();
}

void AudioContext::Update() {
  if (!task_queue_.empty()) {
    (task_queue_.front())();
  }
  for(auto& music : music_sources_) {
    UpdateMusicStream(music);
  }
  if (background_gain_step_ != 0) {
    BackgroundSmoothTransition();
  }
  ReleaseSources();
}

void AudioContext::InitContext() {
  ALCdevice* device = alcOpenDevice(nullptr);
  if (!device) {
    std::cerr << "Failed to initialize OpenAL device" << std::endl;
    std::terminate();
  }

  ALCcontext* context = alcCreateContext(device, nullptr);
  if (!context) {
    std::cerr << "Failed to create OpenAL context" << std::endl;
    alcCloseDevice(device);
    std::terminate();
  }
  alcMakeContextCurrent(context);
  std::cerr << "OpenAL contect initialized" << std::endl;
}

void AudioContext::DeInitContext() {
  alcMakeContextCurrent(nullptr);
  alcDestroyContext(openal_context_);
  alcCloseDevice(openal_device_);
}

void AudioContext::InitOpenALBuffersAndSources() {
  int total_sources_num = sound_sources_.size() +
                          music_sources_.size();
  std::vector<ALuint> sources_ids(total_sources_num);
  AL_CALL(alGenSources, total_sources_num, sources_ids.data());

  int total_buffers_num = sound_sources_.size() +
                          faithful::config::kOpenalBuffersPerMusic *
                              music_sources_.size();
  std::vector<ALuint> buffers_ids(total_buffers_num);
  AL_CALL(alGenBuffers, total_buffers_num, buffers_ids.data());

  for (std::size_t i = 0; i < sound_sources_.size(); ++i) {
    sound_sources_[i].source_id = sources_ids[i];
    sound_sources_[i].buffer_id = buffers_ids[i];
    /// pitch, gain, position, velocity, etc - by default
    AL_CALL(alSourcei, sound_sources_[i].source_id, AL_LOOPING, AL_FALSE);
  }

  for (std::size_t i = 0; i < music_sources_.size(); ++i) { // <-- todo; write to documentation (order of source/buffer)
    music_sources_[i].source_id = sources_ids[i + sound_sources_.size()];
    for (int j = 0; j < faithful::config::kOpenalBuffersPerMusic; ++j) {
      music_sources_[i].buffers_id[j] = buffers_ids[i * j + j + sound_sources_.size()];
    }
    /// pitch, gain, position, velocity, etc - by default
    AL_CALL(alSourcei, music_sources_[i].source_id, AL_LOOPING, AL_TRUE);
  }
}
void AudioContext::DeInitOpenALBuffersAndSources() {
  int total_sources_num = sound_sources_.size() +
                          music_sources_.size();
  std::vector<ALuint> sources_ids(total_sources_num);

  int total_buffers_num = sound_sources_.size() +
                          faithful::config::kOpenalBuffersPerMusic *
                              music_sources_.size();
  std::vector<ALuint> buffers_ids(total_buffers_num);

  for (std::size_t i = 0; i < sound_sources_.size(); ++i) {
    sources_ids[i] = sound_sources_[i].source_id;
    buffers_ids[i] = sound_sources_[i].buffer_id;
  }

  for (std::size_t i = 0; i < music_sources_.size(); ++i) {
    sources_ids[i + sound_sources_.size()] = music_sources_[i].source_id;
    for (int j = 0; j < faithful::config::kOpenalBuffersPerMusic; ++j) {
      buffers_ids[i * j + j + sound_sources_.size()] = music_sources_[i].buffers_id[j];
    }
  }

  AL_CALL(alDeleteSources, total_sources_num, sources_ids.data());
  AL_CALL(alDeleteBuffers, total_buffers_num, buffers_ids.data());
}

// internally in thread, so AL_CALL safe
void AudioContext::ReleaseSources() {
  // TODO: possible SIMD optimization
  ALint state;
  for (auto& source : sound_sources_) {
    AL_CALL(alGetSourcei, source.source_id, AL_SOURCE_STATE, &state);
    if(state != AL_PLAYING) {
      AL_CALL(alSourceStop, source.source_id);
      source.busy = false;
    }
  }
  for (auto& source : music_sources_) {
    AL_CALL(alGetSourcei, source.source_id, AL_SOURCE_STATE, &state);
    if(state != AL_PLAYING) {
      AL_CALL(alSourceStop, source.source_id);
      source.busy = false;
    }
  }
}

// internally in thread, so AL_CALL safe
void AudioContext::BackgroundSmoothTransition() {
  background_gain_ -= background_gain_step_;
  AL_CALL(alSourcei, music_sources_[0].source_id, AL_GAIN, background_gain_);
  if (background_gain_ == 0) {
    if (next_background_music_) {
      music_sources_[0].data = next_background_music_;
      next_background_music_ = nullptr;
      background_gain_step_ *= -1;
    }
  } else if (background_gain_ == 1) {
    background_gain_step_ = 0;
  }
}

int AudioContext::GetAvailableSoundSourceId() {
  for (std::size_t i = 0; i < sound_sources_.size(); ++i) {
    if (!sound_sources_[i].busy) { // TODO: CAS? OR only one consumer ?
      sound_sources_[i].busy = true;
      return i;
    }
  }
  return -1;
}
int AudioContext::GetAvailableMusicSourceId() {
  /// we're starting from 1 because 0 for background music (always busy)
  for (std::size_t i = 0; i < music_sources_.size(); ++i) {
    if (!music_sources_[i].busy) { // TODO: CAS? OR only one consumer ?
      music_sources_[i].busy = true;
      return i;
    }
  }
  return -1;
}

/* for these two we have some queues with length of openal_sound_num, openal_music_num
 * then when the queue is full (we didn't have enough time to process them all),
 * -> we just rewrite the older by newer
 * */
void AudioContext::Play(Sound& sound) {
  int source_id = GetAvailableSoundSourceId();
  if (source_id == -1) {
    std::cerr << "can't get available id for sound" << std::endl;
    // FAITHFUL_DEBUG log warning
    return; // all busy, can skip <-- todo; write to documentation
  }
  task_queue_.push([=, this]() {
    std::cout << "inside the task_queue_" << std::endl;
    AL_CALL(alBufferData, sound_sources_[source_id].buffer_id,
            sound.GetFormat(), sound.GetData().get(), sound.GetSize(),
            sound.GetSampleRate());
    // TODO: can we bind them inside the initialization?
    AL_CALL(alSourcei, sound_sources_[source_id].source_id, AL_BUFFER,
            sound_sources_[source_id].buffer_id);
    AL_CALL(alSourcePlay, sound_sources_[source_id].source_id);
    /// non-blocking, continue spinning in thread pool run-loop
  });
}
void AudioContext::Play(Music& music) {
  int source_id = GetAvailableMusicSourceId();
  if (source_id == -1) {
    std::cerr << "can't get available id for music" << std::endl;
    // FAITHFUL_DEBUG log warning
    return; // all busy, can skip <-- todo; write to documentation
  }

  auto buffer_size = faithful::config::kOpenalBuffersSize;
  auto buffers_num = faithful::config::kOpenalBuffersPerMusic;

    auto data = std::make_unique<char>(buffer_size);
  task_queue_.push([buffer_size, buffers_num, this, source_id, &music] {
    auto data = std::make_unique<char>(buffer_size);
    for (int i = 0; i < buffers_num; ++i) {
      int dataSoFar = 0;
      while (dataSoFar < buffer_size) {
        int result = ov_read(&music.GetOggVorbisFile(),
                             data.get() + dataSoFar, buffer_size - dataSoFar, 0,
                             2, 1, &music.GetOggCurSection());
        if (CheckOggOvErrors(result, i)) {
          std::cerr << "AudioThreadPool::Play(Music) error" << std::endl;
          break;
        }
        dataSoFar += result;
      }
      AL_CALL(alBufferData, music_sources_[source_id].buffers_id[i],
              music.GetFormat(), data.get(), dataSoFar, music.GetSampleRate());
    }
    AL_CALL(alSourceQueueBuffers, source_id,
            faithful::config::kOpenalBuffersPerMusic,
            &music_sources_[source_id].buffers_id[0]);
    /// non-blocking, continue spinning in thread pool run-loop
  });
}

/// should be called only from one thread
void AudioContext::SetBackground(faithful::Music* music) {
  next_background_music_ = music;
  background_gain_step_ = faithful::config::kDefaultBackgroundGainStep;
}

// internally in thread, so AL_CALL safe
void AudioContext::UpdateMusicStream(MusicSourceData& music_data) {
  ALint buffersProcessed = 0;
  AL_CALL(alGetSourcei, music_data.source_id, AL_BUFFERS_PROCESSED, &buffersProcessed);
  if (buffersProcessed <= 0) {
    return;
  }

  while(buffersProcessed--) {
    ALuint buffer;
    AL_CALL(alSourceUnqueueBuffers, music_data.source_id, 1, &buffer);

    auto buffer_size = faithful::config::kOpenalBuffersSize;
    auto data = std::make_unique<char>(buffer_size);
    std::memset(data.get(), 0, buffer_size);

    int sizeRead = 0;

    while(sizeRead < buffer_size) {
      int result = ov_read(
          &music_data.data->GetOggVorbisFile(), data.get() + sizeRead,
          buffer_size - sizeRead, 0, 2, 1,
          &music_data.data->GetOggCurSection());
      if (result == OV_HOLE) {
        std::cerr << "ERROR: OV_HOLE found in update of buffer " << std::endl;
        break;
      } else if (result == OV_EBADLINK) {
        std::cerr << "ERROR: OV_EBADLINK found in update of buffer " << std::endl;
        break;
      } else if (result == OV_EINVAL) {
        std::cerr << "ERROR: OV_EINVAL found in update of buffer " << std::endl;
        break;
      } else if (result == 0) {
        if (CheckOggOvLoopErrors(ov_raw_seek(
                &music_data.data->GetOggVorbisFile(), 0))) {
          return;
        }
      }
      sizeRead += result;
    }
    ALsizei dataSizeToBuffer = sizeRead;

    if (dataSizeToBuffer > 0) {
      AL_CALL(alBufferData, buffer, music_data.data->GetFormat(), data.get(),
              dataSizeToBuffer, music_data.data->GetSampleRate());
      AL_CALL(alSourceQueueBuffers, music_data.source_id, 1, &buffer);
    }

    if (dataSizeToBuffer < buffer_size) {
      std::cout << "Data missing" << std::endl;
    }

    ALint state;
    AL_CALL(alGetSourcei, music_data.source_id, AL_SOURCE_STATE, &state);
    if (state != AL_PLAYING) {
      AL_CALL(alSourceStop, music_data.source_id);
      AL_CALL(alSourcePlay, music_data.source_id);
    }
  }
}


// internally in thread, so AL_CALL safe
bool AudioContext::CheckOggOvErrors(int code, int buffer_id) {
  if (code == OV_HOLE) {
    std::cerr << "ERROR: OV_HOLE found in initial read of buffer "
              << buffer_id << std::endl;
  } else if (code == OV_EBADLINK) {
    std::cerr << "ERROR: OV_EBADLINK found in initial read of buffer "
              << buffer_id << std::endl;
  } else if (code == OV_EINVAL) {
    std::cerr << "ERROR: OV_EINVAL found in initial read of buffer "
              << buffer_id << std::endl;
  } else if (code == 0) {
    std::cerr << "ERROR: EOF found in initial read of buffer " <<
        buffer_id << std::endl;
  } else {
    return false;
  }
  return true;
}

// internally in thread, so AL_CALL safe
bool AudioContext::CheckOggOvLoopErrors(int code) {
  if (code == OV_ENOSEEK) {
    std::cerr << "ERROR: OV_ENOSEEK found when trying to loop" << std::endl;
  } else if (code == OV_EINVAL) {
    std::cerr << "ERROR: OV_EINVAL found when trying to loop" << std::endl;
  } else if (code == OV_EREAD) {
    std::cerr << "ERROR: OV_EREAD found when trying to loop" << std::endl;
  } else if (code == OV_EFAULT) {
    std::cerr << "ERROR: OV_EFAULT found when trying to loop" << std::endl;
  } else if (code == OV_EOF) {
    std::cerr << "ERROR: OV_EOF found when trying to loop" << std::endl;
  } else if (code == OV_EBADLINK) {
    std::cerr << "ERROR: OV_EBADLINK found when trying to loop" << std::endl;
  }

  if (code != 0) {
    std::cerr << "ERROR: Unknown error in ov_raw_seek" << std::endl;
    return true;
  } else {
    // TODO: FAITHFUL_DEBUG log warning
    return false; // error not so critical, so we can ignore it
  }
}


} // namespace details
} // namespace faithful
