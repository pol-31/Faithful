#ifndef FAITHFUL_SRC_EXECUTORS_AUDIOTHREADPOOL_H_
#define FAITHFUL_SRC_EXECUTORS_AUDIOTHREADPOOL_H_

#include "Executor.h"

#include <array>
#include <string>
#include <type_traits>
#include <iostream> // todo: replace
#include <fstream>
#include <cstring>

#include <AL/al.h>
#include <AL/alc.h>
#include <ogg/ogg.h>

#include "queues/LifoBoundedMPSCBlockingQueue.h"

#include "../../config/Loader.h"

#include "../src/loader/AudioData.h"

namespace faithful {
class Sound;
class Music;
namespace details {

/** AudioThreadPool purpose:
 * - encapsulate OpenAL
 * - create & handle 6 sources:
 *   - play background music
 *   - play extra background effect#1 (e.g. weather)
 *   - play extra background effect#2 (e.g. someone speak)
 *   - 3 for sounds
 *
 * */

/// "1" - because we need only ONE openAL context
class AudioThreadPool : public StaticExecutor<1> {
 public:
  using Base = StaticExecutor<1>;

  AudioThreadPool();
  ~AudioThreadPool();

  void Play(Sound sound);
  void Play(Music music);

  void SetBackground(Music music); // if already set -> smooth transmission

  void Run() override;

 private:
  /// Sound: .wav
  struct SoundSourceData {
    faithful::Sound* data;
    ALuint buffer_id;
    ALuint source_id;
    bool busy;
  };

  struct MusicSourceData {
    std::array<ALuint, faithful::config::openal_buffers_per_music> buffers_id;
    faithful::Music* data;
    ALuint source_id;
    bool busy;
  };

  void InitOpenALContext();
  void DeinitOpenALContext();

  void InitOpenALBuffersAndSources();
  void DeinitOpenALBuffersAndSources();

  void UpdateMusicStream(MusicSourceData& music_data);

  void SmoothlyStart(ALuint source);
  void SmoothlyStop(ALuint source);

  bool CheckOggOvErrors(int code, int buffer_id);

  ALCcontext* openal_context_;
  ALCdevice* openal_device_;

  std::array<SoundSourceData, faithful::config::openal_sound_num> sound_sources_;
  std::array<MusicSourceData, faithful::config::openal_music_num> music_sources_;

  bool openal_initialized_ = false;
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_EXECUTORS_AUDIOTHREADPOOL_H_
