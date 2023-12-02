#include "src/Engine.h"
#include "src/entities/Objects_3d.h"
#include "src/entities/Objects_2d.h"

#include <thread>
#include "utils/Executors/ThreadPools.h"
#include "src/entities/Model.h"

#include "src/entities/Mesh.h"

void SetDebugKeys(faithful::KeyboardInputHandler* key_handler) {
  using namespace faithful;
  key_handler->Bind(Key::Escape, []() {
    SimurghManager::get_window()->Close();
  });
  key_handler->Bind(Key::F, []() {
    SimurghManager::get_window()->FullscreenOn();
  });
  key_handler->Bind(Key::X, []() {
    SimurghManager::get_window()->FullscreenOff();
  });
  key_handler->Bind(Key::_1, []() {
    SimurghManager::SwitchSceneRequest(2);
  });
  key_handler->Bind(Key::_2, []() {
    SimurghManager::SwitchSceneRequest(3);
  });
  key_handler->Bind(Key::_3, []() {
    SimurghManager::SwitchSceneRequest(1);
  });
}


/* TODO 0: consider how our Game Engine should looks like <<prog_1>>
 *   what we need: floor(tesselation) adjustament, water, object positioning.
 *   Not even scene switching (implemented by code). For floor we just load/create/
 *   /rename height map and store them into binaries. Model changing, sounds, cubemaps,
 *   textures, materials - not concern of this program
 *
 * TODO 1: rewrite logging system (DONE)
 * TODO 2: delete _Runtime_Loading_, use only static updating <<prog_2>> ! <-- cur work
 * TODO 3: Models, Object, Object2D, Object3D --> delete.......
 * TODO 4: rewrite input handlers:
 *   we don't need them anymore, so just delete.
 *   Instead we need to add few _completed_pack_ of input keys
 *   with keys actions/control_btn-s, but taking
 *   into consideration priority and order of keys (most important to begin,
 *   less - to end; sometimes after action discard other checks)
 * TODO 5: rewrite Cursor.h and Camera.h:
 *   we need __exact__ number and types of camera/cursor with
 *   __exact__ textures/parameters (almost all)
 *
 *
 *
 *
 * */




#include <iostream>
#include <AL/al.h>
#include <AL/alc.h>
#include <sndfile.h>

void checkError() {
  ALenum error = alGetError();
  if (error != AL_NO_ERROR) {
    std::cerr << "OpenAL error: " << alGetString(error) << std::endl;
    exit(EXIT_FAILURE);
  }
}


int main() {

  std::string str = "ttt";
  faithful::ConsoleLogger logger;
  logger.Log(faithful::LogType::kInfo, str);
  // TODO: glfwInitAllocator();
  using namespace faithful;
  SimurghManager engine;

  // Initialize OpenAL
  ALCdevice* device = alcOpenDevice(nullptr);
  if (!device) {
    std::cerr << "Failed to open OpenAL device." << std::endl;
    return EXIT_FAILURE;
  }

  ALCcontext* context = alcCreateContext(device, nullptr);
  if (!context) {
    std::cerr << "Failed to create OpenAL context." << std::endl;
    alcCloseDevice(device);
    return EXIT_FAILURE;
  }

  alcMakeContextCurrent(context);

  // Load WAV file
  SF_INFO fileInfo;
  SNDFILE* sndfile = sf_open("test.wav", SFM_READ, &fileInfo);
  if (!sndfile) {
    std::cerr << "Failed to open WAV file." << std::endl;
    alcDestroyContext(context);
    alcCloseDevice(device);
    return EXIT_FAILURE;
  }

  // Read WAV data
  ALsizei dataSize = static_cast<ALsizei>(fileInfo.frames) * fileInfo.channels * sizeof(short);
  short* bufferData = new short[dataSize];
  sf_read_short(sndfile, bufferData, dataSize);
  sf_close(sndfile);

  // Create OpenAL buffer
  ALuint buffer;
  alGenBuffers(1, &buffer);
  alBufferData(buffer, (fileInfo.channels == 2) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16,
               bufferData, dataSize, fileInfo.samplerate);

  // Create OpenAL source
  ALuint source;
  alGenSources(1, &source);
  alSourcei(source, AL_BUFFER, buffer);

  // Play the sound
  alSourcePlay(source);

  // Wait for the sound to finish
  ALint state;
  do {
    alGetSourcei(source, AL_SOURCE_STATE, &state);
  } while (state == AL_PLAYING);

  // Clean up
  alDeleteSources(1, &source);
  alDeleteBuffers(1, &buffer);
  alcDestroyContext(context);
  alcCloseDevice(device);

  delete[] bufferData;

  return EXIT_SUCCESS;







/*
  auto game_scene = new Scene3D(SceneMode::Shooter);
  auto menu_scene = new Scene2D(SceneMode::Platformer);

  SetDebugKeys(SimurghManager::get_camera()->get_keyboard_handler());
  SetDebugKeys(menu_scene->get_camera()->get_keyboard_handler());
  SetDebugKeys(game_scene->get_camera()->get_keyboard_handler());

  SimurghManager::set_scene(menu_scene);

  // Scene #2


  Hud character1;
  character1.ScaleTo(0.1f, 0.1f);
  //character.RotateTo(5.0f);

  character1.check_collision_ = true;
//  Rectangle cubea3d0;
//  cubea3d0.TranslateTo(0.7f, 0.7f);
//  cubea3d0.ScaleTo(0.3f, 0.3f);
//  cubea3d0.RotateTo(45.0f);
//  Triangle cubea3d1;
//  cubea3d1.TranslateTo(0.0f, 0.7f);
//  cubea3d1.ScaleTo(0.3f, 0.3f);
  Circle cubea3f2;
  //cubea3f2.TranslateTo(-0.7f, 0.7f);
  cubea3f2.ScaleTo(0.3f, 0.3f);
//  cubea3f2.RotateTo(90.0f);
  Circle cubea3fc2;
  cubea3fc2.TranslateTo(-0.2f, 0.3f);
  cubea3fc2.ScaleTo(0.3f, 0.3f);
//  cubea3fc2.RotateTo(90.0f);

  SpriteObject cubesa3d0(1.7f, 1.6f, -10.0f);
  SpriteObject csubea3d1(2.7f, 2.7f, -10.0f);
  SpriteObject cusbea3f2(3.7f, 3.8f, -10.0f);

  Cube cube30(0.0f, 1.0f, -10.0f);
  Cube cube31(1.0f, 1.0f, -10.0f);
  Cube cube32(2.0f, 1.0f, -10.0f);
  Cube cube33(3.0f, 1.0f, -10.0f);
  Cube cube34(4.0f, 1.0f, -10.0f);
  Cube cube35(5.0f, 1.0f, -10.0f);
  Cube cube36(6.0f, 1.0f, -10.0f);



  SimurghManager::get_scene()->get_camera()->get_keyboard_handler()->Bind(Key::Up, [&]() {
    character1.TranslateOn(0.00f, 0.01f);
  });
  SimurghManager::get_scene()->get_camera()->get_keyboard_handler()->Bind(Key::Down, [&]() {
    character1.TranslateOn(0.00f, -0.01f);
  });
  SimurghManager::get_scene()->get_camera()->get_keyboard_handler()->Bind(Key::Left, [&]() {
    character1.TranslateOn(-0.01f, 0.00f);
  });
  SimurghManager::get_scene()->get_camera()->get_keyboard_handler()->Bind(Key::Right, [&]() {
    character1.TranslateOn(0.01f, 0.00f);
  });

  // Scene #1

  // game
  SimurghManager::set_scene(game_scene);
*//*
  Model model2("../resources/objects/backpack/backpack.obj");
  model2.TranslateTo(3.0f, 3.0f, 0.0f);
  Model model3("../resources/objects/backpack/backpack.obj");
  model3.TranslateTo(4.0f, 3.0f, 0.0f);
  Model model4("../resources/objects/backpack/backpack.obj");
  model4.TranslateTo(5.0f, 3.0f, 0.0f);
  Model model5("../resources/objects/backpack/backpack.obj");
  model5.TranslateTo(6.0f, 3.0f, 0.0f);
  Model model6("../resources/objects/backpack/backpack.obj");
  model6.TranslateTo(7.0f, 3.0f, 0.0f);
  Model model7("../resources/objects/backpack/backpack.obj");
  model7.TranslateTo(8.0f, 3.0f, 0.0f);*//*
*//*

  auto model1 = new Model("/home/pavlo/Downloads/Longbow/Erika Archer.dae", true);
  model1->TranslateTo(0.0f, 4.0f, 0.0f);
  model1->ScaleTo(0.001f, 0.001f, 0.001f);
*//*

  Cube cube54(1.0f, 0.0f, -3.0f);
  cube54.ScaleTo(1.0f, 2.0f, 1.0f);
  Cube cube55(4.0f, 0.0f, -5.0f);
  cube55.ScaleTo(1.0f, 1.0f, 8.0f);
  Cube cube56(3.0f, 1.0f, 3.0f);
  cube56.ScaleTo(0.5f, 5.0f, 1.0f);
  Cube cube57(4.0f, 0.0f, 8.0f);
  cube57.ScaleTo(0.3f, 0.3f, 0.3f);
  Cube cube58(-4.0f, 2.0f, -5.0f);
  cube58.ScaleTo(3.0f, 4.0f, 3.0f);

  Cube cube3e(0.0f, 0.0f, 0.0f);

  SpriteObject cubcesa3d0(1.7f, 1.6f, -10.0f);
  SpriteObject csucbea3d1(2.7f, 2.7f, -10.0f);
  SpriteObject cuxsbea3f2(3.7f, 3.8f, -10.0f);


  Hud cubea3dd0;
  cubea3dd0.TranslateTo(0.7f, 0.6f)
            .ScaleTo(0.1f, 0.1f);
  Hud cubea3dd1;
  cubea3dd1.TranslateTo(0.7f, 0.7f)
            .ScaleTo(0.1f, 0.1f);
  Hud cubea3df2;
  cubea3df2.TranslateTo(0.7f, 0.8f)
            .ScaleTo(0.1f, 0.1f);

  simurgh::SimurghManager::SwitchSceneRequest(1);

  // Scene #3

  Cube cube11(1.0f, 1.0f, -10.0f);
  Cube cube12(2.0f, 1.0f, -10.0f);
  Cube cube13(2.0f, 4.0f, -10.0f);
  Cube cube4e(0.0f, 0.0f, 0.0f);

  Cube cube1(+5.0f, +5.0f, -5.0f);
  Cube cube2(-5.0f, +5.0f, -5.0f);
  Cube cube3(+5.0f, -5.0f, -5.0f);
  Cube cube4(-5.0f, -5.0f, -5.0f);
  Cube cube5(+5.0f, +5.0f, +5.0f);
  Cube cube6(-5.0f, +5.0f, +5.0f);
  Cube cube7(+5.0f, -5.0f, +5.0f);
  Cube cube0(-5.0f, -5.0f, +5.0f);
  Cube cube1e(0.0f, 0.0f, 0.0f);
  SpriteObject cubesaq3d0(1.7f, 1.6f, -10.0f);
  SpriteObject csubeaq3d1(2.7f, 2.7f, -10.0f);
  SpriteObject cusbeaq3f2(3.7f, 3.8f, -10.0f);


  Hud cubea3da0;
  cubea3da0.TranslateTo(0.9f, 0.6f);
  cubea3da0.ScaleTo(0.1f, 0.1f);
  cubea3da0.RotateTo(90.0f);
//  Hud cubead3da0;
//  cubead3da0.TranslateTo(0.7f, 0.6f);
//  cubead3da0.ScaleTo(0.1f, 0.1f);
//  cubead3da0.RotateTo(70.0f);
  Hud cubea3dda0;
  cubea3dda0.TranslateTo(0.3f, 0.3f);
  cubea3dda0.ScaleTo(0.2f, 0.2f);
  cubea3dda0.RotateTo(180.0f);
  Hud cubxea3da0;
  cubxea3da0.TranslateTo(-0.3f, 0.6f);
  cubxea3da0.ScaleTo(0.1f, 0.1f);
  cubxea3da0.RotateTo(270.0f);
  //Hud cubea3da1;
//  cubea3da1.TranslateTo(0.1f, 0.7f);
//  cubea3da1.ScaleTo(0.1f, 0.1f);
//  cubea3da1.RotateTo(25.0f);
  Hud cubea3af2;
  cubea3af2.TranslateTo(-0.9f, 0.8f);
  cubea3af2.ScaleTo(0.1f, 0.1f);
  cubea3af2.RotateTo(360.0f);

  Hud character;
  character.ScaleTo(0.1f, 0.1f);
  //character.RotateTo(5.0f);

  character.check_collision_ = true;

  SimurghManager::get_scene()->get_camera()->get_keyboard_handler()->Bind(Key::Up, [&]() {
    character.TranslateOn(0.00f, 0.01f);
  });
  SimurghManager::get_scene()->get_camera()->get_keyboard_handler()->Bind(Key::Down, [&]() {
    character.TranslateOn(0.00f, -0.01f);
  });
  SimurghManager::get_scene()->get_camera()->get_keyboard_handler()->Bind(Key::Left, [&]() {
    character.TranslateOn(-0.01f, 0.00f);
  });
  SimurghManager::get_scene()->get_camera()->get_keyboard_handler()->Bind(Key::Right, [&]() {
    character.TranslateOn(0.01f, 0.00f);
  });*/

  std::this_thread::sleep_for(std::chrono::seconds(10000));
  return 0;
}

//void foo() {
//  /// GAME SCENE
//  //... camera, input handlers ...//
//  Cube cube1;
//  Cube cube2;
//  Model player(path);
//  Model enemy1();
//  Model enemy2();
//  //... their positions ...//
//  Hud hp(position, size);
//  Hud ammo(position, size);
//
//  /// MENU/SETTINGS SCENE
//  //... camera, input handlers ...//
//  //... background framebuffer, transparency ...//
//
//  Hud general_layout();
//  TableLayout table(position(x, y), size(5 x 5 cells));
//  Button exit_btn(no_position, size, fn); // if there is no position - it's not drawable
//  Button resume_btn(no_position, size, fn);
//  table[4][4].Add(exit_btn);
//  table[0][4].Add(resume_btn);
//
//
//}


// TODO: are cursor related to Camera obj

// TODO: CollisionManager -> 2 list: for 2d and 3d

// TODO: DefaultShader, DefaultSprite, Default, default.... hell
// TODO: Shaders madness
// TODO: Object.h - rotations

// TODO 3: collisions
// TODO 4: HUD __panel__ (pixel::hud_element)


// TODO 10: optimize ShaderProgram creating (DrawingProcessing), cause we create shader program for each object
// TODO 11: reuse GeometryObjects (we creating each copy from beginning by now)

// TODO 16: dirs TEST for further google testing
