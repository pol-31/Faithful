#include "AudioThreadPool.h"

#include "../../utils/Logger.h"

#include "../loader/Music.h"
#include "../loader/Sound.h"

namespace faithful {
namespace details {

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
  //
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

bool AudioThreadPool::InitOpenALContext() {
  ALCdevice* device = alcOpenDevice(nullptr);
  if (!device) {
    std::cerr << "Failed to initialize OpenAL device" << std::endl;
    return false;
  }

  ALCcontext* context = alcCreateContext(device, nullptr);
  if (!context) {
    std::cerr << "Failed to create OpenAL context" << std::endl;
    alcCloseDevice(device);
    return false;
  }

  /// we have only one OpenAL context
  alcMakeContextCurrent(context);

  /// generate sources & buffers
  int total_sources_num = faithful::config::openal_sound_num +
                          faithful::config::openal_music_num;
  ALuint source_ids[total_sources_num];
  alGenSources(total_sources_num, source_ids);

  int total_buffers_num = faithful::config::openal_sound_num +
                          faithful::config::openal_buffers_per_music *
                              faithful::config::openal_music_num;
  ALuint buffer_ids[total_buffers_num];
  alGenBuffers(total_buffers_num, buffer_ids);

  for (int i = 0; i < faithful::config::openal_sound_num; ++i) {
    sound_sources_[i].source_id = source_ids[i];
    sound_sources_[i].buffer_id = buffer_ids[i];
  }

  for (int i = 0; i < faithful::config::openal_sound_num; ++i) {
    music_sources_[i].source_id = source_ids[i];
    for (int j = 0; j < faithful::config::openal_buffers_per_music; ++j) {
      music_sources_[i].buffers_id[j] = buffer_ids[i]; // TODO: need offset
    }
  }
  return true;
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
      }
      sizeRead += result;
    }
    dataSizeToBuffer = sizeRead;

    if(dataSizeToBuffer > 0) {
      AL_CALL(alBufferData, buffer, music_data.data.format, data, dataSizeToBuffer, music_data.data.sampleRate);
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


} // namespace details
} // namespace faithful
