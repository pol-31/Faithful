#include <iostream>

#define FAITHFUL_DEBUG // todo: -> CMake
#define AL_LIBTYPE_STATIC // todo: -> CMake

//#include <stdlib.h>
//#include <mimalloc-override.h>
/// We don't need to #include "mimalloc-new-delete.h"
/// as it has already been overridden by libmimalloc.a

// TODO: do we need to restrict amount of opened simultaneous fd (file_descriptors)

// TODO: fix all #pragma & class names, cause we have some troubles now

// TODO 4: glEnable(GL_CULLING) enable/disable

// TODO: loader/ModelPool.h

// TODO: make all constructors light-weight (add Init() at "heavy" initializing)

#include "executors/ExecutionEnvironment.h"

int main() {
  faithful::details::ExecutionEnvironment execution_environment;
  std::cout << "__start__" << std::endl;

  execution_environment.Run();

  std::cout << "__end__" << std::endl;

//  faithful::details::RenderThreadPool render_tp_;
  // render_tp_.Run();
  //TestAudioThreadPool();
//  if (MainGameSceneSetup()) {
//    std::cerr << "Unable to InitRenderContext" << std::endl;
//  }

  return 0;
}
/*

void TestAudioThreadPool() {
  faithful::details::assets::SoundPool sound_manager;
  faithful::details::assets::MusicPool music_manager;
  faithful::details::AudioThreadPool audio_thread(&music_manager, &sound_manager);


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
*/

// tinygltf changes list:
// preferences:
//   solely RapidJSON, cpp14
// remove: audio, lights, cameras, PositionalEmitter
//   extensions, extras
//   no DRACO, no ANDROID, no STB_IMAGE_WRITE
//   remove jsonhpp: C++ JSON library.
//   UpdateImageObject <- we handle it only by url property
//   "todo" and "ifdef 0"
//  LoadImageData - req_comp set to 4
// TODO: tinygltf lods - level of detail nodes MSFT_lod



// TODO: inside the game: TINYGLTF_NO_EXTERNAL_IMAGE
// TODO: inside the game: TINYGLTF_USE_RAPIDJSON_CRTALLOCATOR
//                    (in AssetProcessor we have only one json at a time)





/**TODO-list:
 *  - STB_IMAGE_WRITE inside the tinygltf.h
 *  - Total Managers Initialization in main.h (or not in main.h)
 *  - folly::Function, folly::Future & combinators implementation --> TextureProcessing
 *  - model linkage optimization (utility for linking texture/json directly to src code
 *     - for models/textures/gui which will be used throughout the game - like configurations)
 *  - Font?
 *  - ModelManager
 *  - Animation processing, interpolation
 *  - fir music streaming
 * */








