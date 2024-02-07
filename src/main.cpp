#include <iostream>

#define FAITHFUL_DEBUG // todo: -> CMake
#define AL_LIBTYPE_STATIC // todo: -> CMake

#include <stdlib.h>
#include <mimalloc-override.h>
/// We don't need to #include "mimalloc-new-delete.h"
/// as it has already been overridden by libmimalloc.a

#include <cmath>
#include <iostream>


#include "executors/AudioThreadPool.h"
#include "loader/Sound.h"
#include "loader/Music.h"
#include "loader/Shader.h"

//TODO:
// 1) init OpenGL context inside the RenderThreadPool
// 3) render loop inside the RenderThreadPool
// 2) folly::Future + combinators ---> Texture2D.h
// 4) textures + tinygltf
// 5) run tinygltf example
// BONUS: Music.h


// class Main thread (a.k. RenderThreadPool): GLFW(input handling), OpenGL

// class AudioThreadPool: sound processing / music streaming

// WorkerThreadPool: movement/animation/collision_computation (what cell are we in)
//                   loading (Passive/Active)
// * class StaticWorkerThreadPool  -- default threads for object processing
//       -- most depends on std::thread::hardware_concurrency()
// * class DynamicWorkerThreadPool -- extra threads in __Loading__ case

// if someone doesn't have any task it can steal it from other (see
// GO_threading) OR HELP him

// TODO: do we need to restrict amount of opened simultaneous fd (file_descriptors)

// States: NormalMode, IntensiveLoadingMode

void TestAudioThreadPool();

int MainGameSceneSetup() {

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

}

#include "executors/RenderThreadPool.h"

int main() {
  faithful::details::RenderThreadPool render_tp_;
  // render_tp_.Run();
  //TestAudioThreadPool();
  if (MainGameSceneSetup()) {
    std::cerr << "Unable to InitRenderContext" << std::endl;
  }
  return 0;
}

void TestAudioThreadPool() {
  faithful::details::AudioThreadPool audio_thread;

  faithful::details::audio::SoundManager sound_manager;
  faithful::details::audio::MusicManager music_manager;

  faithful::Music music = music_manager.Load("/home/pavlo/Desktop/faithful_assets/audio/Pantera - Hard Lines Sunken Cheeks ( 160kbps ).ogg");

  faithful::Sound sound = sound_manager.Load("/home/pavlo/Desktop/ahem_x.wav");
  faithful::Sound sound1 = sound_manager.Load("/home/pavlo/Downloads/air_raid.wav");
  faithful::Sound sound2 = sound_manager.Load("/home/pavlo/Downloads/arrow_x.wav");
  faithful::Sound sound3 = sound_manager.Load("/home/pavlo/Downloads/airplane_chime_x.wav");

  audio_thread.Play(sound);
  audio_thread.Run();
  audio_thread.Play(music);

  audio_thread.Play(sound1);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::thread{[&](){
    for (int i = 0; i < 10; ++i) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      audio_thread.Play(sound);
    }
  }}.detach();
  std::thread{[&](){
    for (int i = 0; i < 10; ++i) {
      std::this_thread::sleep_for(std::chrono::seconds(2));
      audio_thread.Play(sound2);
    }
  }}.detach();

  audio_thread.Play(sound2);
  std::cerr << "Hello, World?" << std::endl;
  std::cout << "Sleep 60 sec" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(60));
}


