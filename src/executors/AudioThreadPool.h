#ifndef FAITHFUL_SRC_EXECUTORS_AUDIOTHREADPOOL_H_
#define FAITHFUL_SRC_EXECUTORS_AUDIOTHREADPOOL_H_

#include "Executor.h"

#include <array>
#include <cstring>
#include <fstream>
#include <map>
#include <string>
#include <type_traits>
#include <iostream> // todo: replace

#include <AL/al.h>
#include <AL/alc.h>
#include <ogg/ogg.h>

#include "../../config/Loader.h"

namespace faithful {

class Music;
class Sound;

namespace details {

namespace assets {

class MusicPool;
class SoundPool;

} // namespace details

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

  AudioThreadPool() = delete;

  AudioThreadPool(assets::MusicPool* music_manager,
                  assets::SoundPool* sound_manager);

  ~AudioThreadPool();

  void Play(const Sound& sound);
  void Play(const Music& music);

  void SetBackground(Music* music); // if already set -> smooth transmission

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
    std::array<ALuint, faithful::config::openal_buffers_per_music> buffers_id;
    faithful::Music* data;
    ALuint source_id;
    bool busy = false;
  };

  void ReleaseSources(); // TODO: rename

  void InitOpenALContext();
  void DeinitOpenALContext();

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

  std::array<SoundSourceData, faithful::config::openal_sound_num> sound_sources_;

  /// music_sources_[0] <- main background music
  /// (where BackgroundSmoothTransition works)
  std::array<MusicSourceData, faithful::config::openal_music_num> music_sources_;

  faithful::Music* next_background_music_ = nullptr;

  details::assets::MusicPool* music_manager_ = nullptr;
  details::assets::SoundPool* sound_manager_ = nullptr;

  float background_gain_ = 1.0f;
  float background_gain_step_ = 0.0f; // for smooth transition between two streams

  bool openal_initialized_ = false;
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_EXECUTORS_AUDIOTHREADPOOL_H_
