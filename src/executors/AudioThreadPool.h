#ifndef FAITHFUL_SRC_EXECUTORS_AUDIOTHREADPOOL_H_
#define FAITHFUL_SRC_EXECUTORS_AUDIOTHREADPOOL_H_

#include <array>

#include <AL/al.h>
#include <AL/alc.h>

#include "IExecutor.h"

#include "../../config/Loader.h"

namespace faithful {

class Music;
class Sound;

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
class AudioThreadPool : public IStaticExecutor<1> {
 public:
  using Base = IStaticExecutor<1>;

  AudioThreadPool();

  ~AudioThreadPool();

  void Play(Sound& sound);
  void Play(Music& music);

  // TODO 1: refactor to call by id
  // TODO 2: if already set -> smooth transmission
  void SetBackground(Music* music);

  void Run() override;
  void Join() override;

 private:
  /// Sound: .wav
  struct SoundSourceData {
    ALuint buffer_id;
    ALuint source_id;
    bool busy = false;
  };

  /// Music: .ogg
  struct MusicSourceData {
    std::array<ALuint, faithful::config::kOpenalBuffersPerMusic> buffers_id;
    faithful::Music* data;
    ALuint source_id;
    bool busy = false;
  };

  void ReleaseSources(); // TODO: rename

  void InitContext();
  void DeinitContext();

  void InitOpenALBuffersAndSources();
  void DeinitOpenALBuffersAndSources();

  /// return id in sound_sources_ / music_sources_, NOT directly OpenAL id <--- todo: to documentation
  int GetAvailableSoundSourceId();
  int GetAvailableMusicSourceId();

  void BackgroundSmoothTransition();

  void UpdateMusicStream(MusicSourceData& music_data);

  bool CheckOggOvErrors(int code, int buffer_id);
  bool CheckOggOvLoopErrors(int code);

  ALCcontext* openal_context_;
  ALCdevice* openal_device_;

  std::array<SoundSourceData, faithful::config::kOpenalSoundNum> sound_sources_;

  /// music_sources_[0] <- main background music
  /// (where BackgroundSmoothTransition works)
  std::array<MusicSourceData, faithful::config::kOpenalMusicNum> music_sources_;

  faithful::Music* next_background_music_ = nullptr;

  float background_gain_ = 1.0f;
  float background_gain_step_ = 0.0f; // for smooth transition between two streams

  bool openal_initialized_ = false;
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_EXECUTORS_AUDIOTHREADPOOL_H_
