#ifndef FAITHFUL_SRC_EXECUTORS_AUDIOCONTEXT_H_
#define FAITHFUL_SRC_EXECUTORS_AUDIOCONTEXT_H_

#include <array>

#include <AL/al.h>
#include <AL/alc.h>

#include "../../config/Loader.h"
#include "../../utils/Function.h"

#include <queue> // TODO: do we need to replace it?

namespace faithful {

class Music;
class Sound;

namespace details {

// TODO: refactor limitation to caching
class AudioContext {
 public:
  AudioContext();
  ~AudioContext();

  void Play(Sound& sound);
  void Play(Music& music);

  // TODO 1: refactor to call by id
  // TODO 2: if already set -> smooth transmission
  void SetBackground(Music* music);

  void Update();

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
  void DeInitContext();

  void InitOpenALBuffersAndSources();
  void DeInitOpenALBuffersAndSources();

  /// return id in sound_sources_ / music_sources_, NOT directly OpenAL id <--- todo: to documentation
  int GetAvailableSoundSourceId();
  int GetAvailableMusicSourceId();

  void BackgroundSmoothTransition();

  void UpdateMusicStream(MusicSourceData& music_data);

  bool CheckOggOvErrors(int code, int buffer_id);
  bool CheckOggOvLoopErrors(int code);

  std::queue<folly::Function<void()>> task_queue_;

  ALCcontext* openal_context_;
  ALCdevice* openal_device_;

  std::array<SoundSourceData, faithful::config::kOpenalSoundNum> sound_sources_;

  /// music_sources_[0] <- main background music
  /// (where BackgroundSmoothTransition works)
  std::array<MusicSourceData, faithful::config::kOpenalMusicNum> music_sources_;

  faithful::Music* next_background_music_ = nullptr;

  float background_gain_ = 1.0f;
  float background_gain_step_ = 0.0f; // for smooth transition between two streams
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_EXECUTORS_AUDIOCONTEXT_H_
