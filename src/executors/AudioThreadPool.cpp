#include "AudioThreadPool.h"

#include "../../utils/Logger.h"

#include "../loader/Music.h"
#include "../loader/Sound.h"

namespace faithful {
namespace details {


AudioThreadPool::AudioThreadPool() {
  InitOpenALContext();
  if (!openal_initialized_) {
    std::cerr << "Can't create OpenAL context" << std::endl;
    std::terminate(); // TODO: replace by Logger::LogIF OR FAITHFUL_TERMINATE
  }
  InitOpenALBuffersAndSources();

  // TODO: somehow we need obtain ptr/ref to MusicManager/SoundManager ::data_
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
  }

  for (int i = 0; i < music_sources_.size(); ++i) {
    music_sources_[i].source_id = source_ids[i];
    for (int j = 0; j < faithful::config::openal_buffers_per_music; ++j) {
      music_sources_[i].buffers_id[j] = buffer_ids[i]; // TODO: need offset
    }
  }
  openal_initialized_ = true;

  // TODO:
  //    AL_CALL(alGenSources, 1, &audioData.source);
  //    AL_CALL(alSourcef, audioData.source, AL_PITCH, 1);
  //    AL_CALL(alSourcef, audioData.source, AL_GAIN, 0.5);//DEFAULT_GAIN);
  //    AL_CALL(alSource3f, audioData.source, AL_POSITION, 0, 0, 0);
  //    AL_CALL(alSource3f, audioData.source, AL_VELOCITY, 0, 0, 0);
  //    AL_CALL(alSourcei, audioData.source, AL_LOOPING, AL_FALSE);
  //    AL_CALL(alGenBuffers, buffers_size, &audioData.buffers[0]);
}


AudioThreadPool::~AudioThreadPool() {
  InitOpenALBuffersAndSources();
  DeinitOpenALContext();
}

void AudioThreadPool::DeinitOpenALContext() {
  if (openal_initialized_) {
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(openal_context_);
    alcCloseDevice(openal_device_);
    openal_initialized_ = false;
  }
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
    source_ids[i] = music_sources_[i].source_id;
    for (int j = 0; j < faithful::config::openal_buffers_per_music; ++j) {
      buffer_ids[i] = music_sources_[i].buffers_id[j]; // TODO: need offset
    }
  }

  AL_CALL(alDeleteSources, total_sources_num, source_ids);
  AL_CALL(alDeleteBuffers, total_buffers_num, buffer_ids);
}



void AudioThreadPool::Run() {
  if (state_ != State::kNotStarted) {
    return;
  }
  state_ = State::kRunning;
  while (state_ != State::kJoined &&
         state_ != State::kSuspended) {
      //TODO: sound tasks
      // task_queue_->Front();
      // task_queue_->Pop();
    for (auto& music : music_sources_) {
      UpdateMusicStream(music);
    }
  }
}


/* for these two we have some queues with length of openal_sound_num, openal_music_num
 * then when the queue is full (we didn't have enough time to process them all),
 * -> we just rewrite the older by newer
 * */
void AudioThreadPool::Play(Sound sound) {
  //
}
void AudioThreadPool::Play(Music music) {
  auto buffer_size = faithful::config::openal_buffers_size;
  auto buffers_num = faithful::config::openal_buffers_per_music;

  /*auto data = std::make_unique<char>(buffer_size);

  for(std::uint8_t i = 0; i < buffers_num; ++i) {
    int dataSoFar = 0;
    while(dataSoFar < buffer_size) {
      int result = ov_read(
          &audioData.oggVorbisFile, data.get() + dataSoFar,
          buffer_size - dataSoFar, 0, 2, 1, &audioData.oggCurrentSection);
      if(CheckOggOvErrors(result, i)) {
        std::cerr << "AudioThreadPool::Play(Music) error" << std::endl;
        break;
      }
      dataSoFar += result;
    }
    AL_CALL(alBufferData, audioData.buffers[i], audioData.format, data,
            dataSoFar, audioData.sampleRate);
  }*/


//  AL_CALL(alBufferData, audioData.buffers[i], audioData.format,
  //  data, dataSoFar, audioData.sample_rate_); < ----- for each buffer
//  AL_CALL(alSourceQueueBuffers, audioData.source, buffers_size, &audioData.buffers[0]);
}

void AudioThreadPool::SetBackground(faithful::Music music) {
  SmoothlyStop(music_sources_[0].source_id);
  music_sources_[0].data = music;
  // TODO: switch background_update_data
  SmoothlyStart(music_sources_[0].source_id);
}

void AudioThreadPool::SmoothlyStart(ALuint source) {
  AL_CALL(alSourcePlay, source); // todo smoothly
  // looping
}
void AudioThreadPool::SmoothlyStop(ALuint source) {
  AL_CALL(alSourceStop, source); // todo smoothly
}

void AudioThreadPool::UpdateMusicStream(MusicSourceData& music_data) {
  ALint buffersProcessed = 0;
  AL_CALL(alGetSourcei, music_data.source_id, AL_BUFFERS_PROCESSED, &buffersProcessed);
  if(buffersProcessed <= 0) {
    return;
  }
  while(buffersProcessed--) {
    ALuint buffer;
    AL_CALL(alSourceUnqueueBuffers, music_data.source_id, 1, &buffer);

    auto buffer_size = faithful::config::openal_buffers_size;
    auto data = std::make_unique<char>(buffer_size);
    std::memset(data.get(), 0, buffer_size);

    ALsizei dataSizeToBuffer = 0;
    std::int32_t sizeRead = 0;

    while(sizeRead < buffer_size) {
      std::int32_t result = ov_read(
          &music_data.data.oggVorbisFile, data.get() + sizeRead,
          buffer_size - sizeRead, 0, 2, 1,
          reinterpret_cast<int*>(music_data.data.oggCurrentSection));
      if(result == OV_HOLE) {
        std::cerr << "ERROR: OV_HOLE found in update of buffer " << std::endl;
        break;
      } else if(result == OV_EBADLINK) {
        std::cerr << "ERROR: OV_EBADLINK found in update of buffer " << std::endl;
        break;
      } else if(result == OV_EINVAL) {
        std::cerr << "ERROR: OV_EINVAL found in update of buffer " << std::endl;
        break;
      } else if(result == 0) {
        std::int32_t seekResult = ov_raw_seek(&music_data.data.oggVorbisFile, 0);
        if(seekResult == OV_ENOSEEK)
          std::cerr << "ERROR: OV_ENOSEEK found when trying to loop" << std::endl;
        else if(seekResult == OV_EINVAL)
          std::cerr << "ERROR: OV_EINVAL found when trying to loop" << std::endl;
        else if(seekResult == OV_EREAD)
          std::cerr << "ERROR: OV_EREAD found when trying to loop" << std::endl;
        else if(seekResult == OV_EFAULT)
          std::cerr << "ERROR: OV_EFAULT found when trying to loop" << std::endl;
        else if(seekResult == OV_EOF)
          std::cerr << "ERROR: OV_EOF found when trying to loop" << std::endl;
        else if(seekResult == OV_EBADLINK)
          std::cerr << "ERROR: OV_EBADLINK found when trying to loop" << std::endl;

        if(seekResult != 0) {
          std::cerr << "ERROR: Unknown error in ov_raw_seek" << std::endl;
          return;
        }

        // TODO: CheckOggOvErrors(result, i);
      }
      sizeRead += result;
    }
    dataSizeToBuffer = sizeRead;

    if(dataSizeToBuffer > 0) {
      AL_CALL(alBufferData, buffer, music_data.data.format, data, dataSizeToBuffer, music_data.data.sample_rate_);
      AL_CALL(alSourceQueueBuffers, music_data.source_id, 1, &buffer);
    }

    if(dataSizeToBuffer < buffer_size) {
      std::cout << "Data missing" << std::endl;
    }

    ALint state;
    AL_CALL(alGetSourcei, music_data.source_id, AL_SOURCE_STATE, &state);
    if(state != AL_PLAYING) {
      AL_CALL(alSourceStop, music_data.source_id);
      AL_CALL(alSourcePlay, music_data.source_id);
    }
  }
}


bool AudioThreadPool::CheckOggOvErrors(int code, int buffer_id) {
  if(code == OV_HOLE) {
    std::cerr << "ERROR: OV_HOLE found in initial read of buffer "
              << buffer_id << std::endl;
  } else if(code == OV_EBADLINK) {
    std::cerr << "ERROR: OV_EBADLINK found in initial read of buffer "
              << buffer_id << std::endl;
  } else if(code == OV_EINVAL) {
    std::cerr << "ERROR: OV_EINVAL found in initial read of buffer "
              << buffer_id << std::endl;
  } else if(code == 0) {
    std::cerr << "ERROR: EOF found in initial read of buffer " <<
        buffer_id << std::endl;
  } else {
    return true;
  }
  return false;
}


} // namespace details
} // namespace faithful
