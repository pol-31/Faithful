#include "AudioThreadPool.h"

#include "../../utils/Logger.h"

#include "../loader/Music.h"
#include "../loader/Sound.h"

#include "queues/LifoBoundedMPSCBlockingQueue.h"

namespace faithful {
namespace details {

// TODO: alSourceRewind(source); for Play(Sound) to pleyback from the beginning

AudioThreadPool::AudioThreadPool(assets::MusicPool* music_manager,
                                 assets::SoundPool* sound_manager)
    : music_manager_(music_manager), sound_manager_(sound_manager) {
  task_queue_ = new queue::LifoBoundedMPSCBlockingQueue<Task>;
}
AudioThreadPool::~AudioThreadPool() {
  delete task_queue_;
}

void AudioThreadPool::Join() {
  state_ = State::kJoined;
  // TODO: Join() from main thread and Run() from AudioThread intersecting there
  //   need synchronization
  while (!task_queue_->Empty()) {
    (task_queue_->Front())();
  }
  for (auto& thread : threads_) {
    thread.join();
  }
}

void AudioThreadPool::InitOpenALContext() {
  if (openal_initialized_) {
    return;
  }
  ALCdevice* device = alcOpenDevice(nullptr);
  if (!device) {
    std::cerr << "Failed to initialize OpenAL device" << std::endl;
    return;
  }

  ALCcontext* context = alcCreateContext(device, nullptr);
  if (!context) {
    std::cerr << "Failed to create OpenAL context" << std::endl;
    alcCloseDevice(device);
    return;
  }
  /// we have only one OpenAL context TODO: static assert?
  alcMakeContextCurrent(context);
  std::cerr << "OpenAL contect initialized" << std::endl;
  openal_initialized_ = true;
}
void AudioThreadPool::DeinitOpenALContext() {
  if (openal_initialized_) {
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(openal_context_);
    alcCloseDevice(openal_device_);
    openal_initialized_ = false;
  }
}

void AudioThreadPool::InitOpenALBuffersAndSources() {
  int total_sources_num = sound_sources_.size() +
                          music_sources_.size();
  ALuint source_ids[total_sources_num];
  AL_CALL(alGenSources, total_sources_num, source_ids);

  int total_buffers_num = sound_sources_.size() +
                          faithful::config::openal_buffers_per_music *
                              music_sources_.size();
  ALuint buffer_ids[total_buffers_num];
  AL_CALL(alGenBuffers, total_buffers_num, buffer_ids);

  for (int i = 0; i < sound_sources_.size(); ++i) {
    sound_sources_[i].source_id = source_ids[i];
    sound_sources_[i].buffer_id = buffer_ids[i];
    /// pitch, gain, position, velocity, etc - by default
    AL_CALL(alSourcei, sound_sources_[i].source_id, AL_LOOPING, AL_FALSE);
  }

  for (int i = 0; i < music_sources_.size(); ++i) { // <-- todo; write to documentation (order of source/buffer)
    music_sources_[i].source_id = source_ids[i + sound_sources_.size()];
    for (int j = 0; j < faithful::config::openal_buffers_per_music; ++j) {
      music_sources_[i].buffers_id[j] = buffer_ids[i * j + j + sound_sources_.size()];
    }
    /// pitch, gain, position, velocity, etc - by default
    AL_CALL(alSourcei, music_sources_[i].source_id, AL_LOOPING, AL_TRUE);
  }
  openal_initialized_ = true;
}
void AudioThreadPool::DeinitOpenALBuffersAndSources() {
  int total_sources_num = sound_sources_.size() +
                          music_sources_.size();
  ALuint source_ids[total_sources_num];

  int total_buffers_num = sound_sources_.size() +
                          faithful::config::openal_buffers_per_music *
                              music_sources_.size();
  ALuint buffer_ids[total_buffers_num];

  for (int i = 0; i < sound_sources_.size(); ++i) {
    source_ids[i] = sound_sources_[i].source_id;
    buffer_ids[i] = sound_sources_[i].buffer_id;
  }

  for (int i = 0; i < music_sources_.size(); ++i) {
    source_ids[i + sound_sources_.size()] = music_sources_[i].source_id;
    for (int j = 0; j < faithful::config::openal_buffers_per_music; ++j) {
      buffer_ids[i * j + j + sound_sources_.size()] = music_sources_[i].buffers_id[j];
    }
  }

  AL_CALL(alDeleteSources, total_sources_num, source_ids);
  AL_CALL(alDeleteBuffers, total_buffers_num, buffer_ids);
}


void AudioThreadPool::Run() {
  if (state_ != State::kNotStarted) {
    return;
  }

  threads_[0] = std::thread([this]() {
    /// initialization
    InitOpenALContext();
    if (!openal_initialized_) {
      std::cerr << "Can't create OpenAL context" << std::endl;
      std::terminate(); // TODO: replace by Logger::LogIF OR FAITHFUL_TERMINATE
    }
    InitOpenALBuffersAndSources();

    /// main loop
    state_ = State::kRunning;
    while(state_ != State::kJoined &&
           state_ != State::kSuspended) {
      if (!task_queue_->Empty()) {
        (task_queue_->Front())();
      }
      for(auto& music : music_sources_) {
        UpdateMusicStream(music);
      }
      if (background_gain_step_ != 0) {
        BackgroundSmoothTransition();
      }
      ReleaseSources();
    }

    /// deinitialization
    DeinitOpenALBuffersAndSources();
    DeinitOpenALContext();
  });
}

// internally in thread, so AL_CALL safe
void AudioThreadPool::ReleaseSources() {
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
void AudioThreadPool::BackgroundSmoothTransition() {
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

int AudioThreadPool::GetAvailableSoundSourceId() {
  for (int i = 0; i < sound_sources_.size(); ++i) {
    if (!sound_sources_[i].busy) { // TODO: CAS? OR only one consumer ?
      sound_sources_[i].busy = true;
      return i;
    }
  }
  return -1;
}
int AudioThreadPool::GetAvailableMusicSourceId() {
  /// we're starting from 1 because 0 for background music (always busy)
  for (int i = 0; i < music_sources_.size(); ++i) {
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
void AudioThreadPool::Play(const Sound& sound) {
  int source_id = GetAvailableSoundSourceId();
  if (source_id == -1) {
    std::cerr << "can't get available id for sound" << std::endl;
    // FAITHFUL_DEBUG log warning
    return; // all busy, can skip <-- todo; write to documentation
  }

  int sound_opengl_id = sound.GetInternalId();
  task_queue_->Push([=]() {
    std::cout << "inside the task_queue_" << std::endl;
    auto& audio_info = sound_manager_->sound_heap_data_[sound_opengl_id];
    AL_CALL(alBufferData, sound_sources_[source_id].buffer_id,
            audio_info.format, audio_info.data.get(),
            audio_info.size, audio_info.sample_rate);
    // TODO: can we bind them inside the initialization?
    AL_CALL(alSourcei, sound_sources_[source_id].source_id, AL_BUFFER,
            sound_sources_[source_id].buffer_id);
    AL_CALL(alSourcePlay, sound_sources_[source_id].source_id);
    /// non-blocking, continue spinning in thread pool run-loop
  });
}
void AudioThreadPool::Play(const Music& music) {
  int source_id = GetAvailableMusicSourceId();
  if (source_id == -1) {
    std::cerr << "can't get available id for music" << std::endl;
    // FAITHFUL_DEBUG log warning
    return; // all busy, can skip <-- todo; write to documentation
  }

  auto buffer_size = faithful::config::openal_buffers_size;
  auto buffers_num = faithful::config::openal_buffers_per_music;
  int music_opengl_id = music.GetInternalId();

  task_queue_->Push([=]() {
    auto& audio_info = music_manager_->music_heap_data_[music_opengl_id];
    auto data = std::make_unique<char>(buffer_size);
    for (int i = 0; i < buffers_num; ++i) {
      int dataSoFar = 0;
      while (dataSoFar < buffer_size) {
        int result = ov_read(
            &audio_info.ogg_vorbis_file, data.get() + dataSoFar,
            buffer_size - dataSoFar, 0, 2, 1, &audio_info.ogg_cur_section);
        if (CheckOggOvErrors(result, i)) {
          std::cerr << "AudioThreadPool::Play(Music) error" << std::endl;
          break;
        }
        dataSoFar += result;
      }
      AL_CALL(alBufferData, music_sources_[source_id].buffers_id[i],
              audio_info.format, data.get(), dataSoFar, audio_info.sample_rate);
    }
    AL_CALL(alSourceQueueBuffers, source_id,
            faithful::config::openal_buffers_per_music,
            &music_sources_[source_id].buffers_id[0]);
    /// non-blocking, continue spinning in thread pool run-loop
  });
}

/// should be called only from one thread
void AudioThreadPool::SetBackground(faithful::Music* music) {
  next_background_music_ = music;
  background_gain_step_ = faithful::config::default_background_gain_step;
}

// internally in thread, so AL_CALL safe
void AudioThreadPool::UpdateMusicStream(MusicSourceData& music_data) {
  ALint buffersProcessed = 0;
  AL_CALL(alGetSourcei, music_data.source_id, AL_BUFFERS_PROCESSED, &buffersProcessed);
  if (buffersProcessed <= 0) {
    return;
  }

  auto& audio_info = music_manager_->music_heap_data_[music_data.data->GetInternalId()];

  while(buffersProcessed--) {
    ALuint buffer;
    AL_CALL(alSourceUnqueueBuffers, music_data.source_id, 1, &buffer);

    auto buffer_size = faithful::config::openal_buffers_size;
    auto data = std::make_unique<char>(buffer_size);
    std::memset(data.get(), 0, buffer_size);

    int sizeRead = 0;

    while(sizeRead < buffer_size) {
      int result = ov_read(
          &audio_info.ogg_vorbis_file, data.get() + sizeRead,
          buffer_size - sizeRead, 0, 2, 1,
          reinterpret_cast<int*>(audio_info.ogg_cur_section));
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
        if (CheckOggOvLoopErrors(ov_raw_seek(&audio_info.ogg_vorbis_file, 0))) {
          return;
        }
      }
      sizeRead += result;
    }
    ALsizei dataSizeToBuffer = sizeRead;

    if (dataSizeToBuffer > 0) {
      AL_CALL(alBufferData, buffer, audio_info.format, data.get(),
              dataSizeToBuffer, audio_info.sample_rate);
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
bool AudioThreadPool::CheckOggOvErrors(int code, int buffer_id) {
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
bool AudioThreadPool::CheckOggOvLoopErrors(int code) {
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
