#include "Engine.h"
#include "io/InputHandler.h"
#include "../utils/executors/ThreadPools.h"
#include "loader/ShaderProgram.h"
#include "environment/CubeMap.h"
#include "loader/Sprite.h"
#include "entities/Objects_2d.h"

#include "entities/Model.h"

namespace faithful {


Window* CurrentWindow() {
  return SimurghManager::get_window();
}

glm::ivec2 CurrentResolution() {
  return SimurghManager::get_window()->MonitorResolution();
}

RenderThreadPool* CurrentRenderThreadPool() {
  return SimurghManager::get_thraed_pool_manager()->get_render_thread_pool();
}
LoadThreadPool* CurrentLoadThreadPool() {
  return SimurghManager::get_thraed_pool_manager()->get_load_thread_pool();
}
SoundThreadPool* CurrentSoundThreadPool() {
  return SimurghManager::get_thraed_pool_manager()->get_sound_thread_pool();
}
ObjectThreadPool* CurrentObjectThreadPool() {
  return SimurghManager::get_thraed_pool_manager()->get_object_thread_pool();
}


SimurghManager::SimurghManager() {
  if (window_ != nullptr) return;
  Init();
}

void SimurghManager::Init() {
  logger_ = CreateLogger();
  thread_pool_manager_ = new ThreadPoolManager;

  logger_->Log(LogType::kInfo, "useful information1");
  logger_->Log(LogType::kInfo, "useful information2");
  logger_->Log(LogType::kInfo, "useful information3");
  logger_->Log(LogType::kInfo, "useful information4");
  logger_->Log(LogType::kInfo, "useful information5");
  logger_->Log(LogType::kInfo, "useful information6");
  logger_->Log(LogType::kInfo, "useful information7");
  logger_->Log(LogType::kInfo, "useful information8");

  thread_pool_manager_->get_render_thread_pool()->Config([=](){
    logger_->LogIf(faithful::LogType::kFatal, !glfwInit(), "Unable to init GLFW");

    SimurghManager::set_window(new Window());
    logger_->LogIf(faithful::LogType::kFatal, !gladLoaderLoadGL(),
                   "Unable to init GLAD");


    utility::DefaultShaderProgram::Init();
    DefaultCursor::Init();
    DefaultTextures::Init();
    DefaultSprites::Init();
    DefaultCubeMap::Init();

    SimurghManager::set_scene(new Scene3D());
    SimurghManager::get_scene()->MakeActive();

    Cube cube1;
    Cube cube2;
    Cube cube3;
    Cube cube4;

    auto skeleton = new Model("../assets/models/tree.obj");
//    auto skeleton = new Model("../resources/models/Skeleton.fbx");
    //Model skeleton("/home/pavlo/Downloads/6e48z1kc7r40-bugatti/bugatti/bugatti.obj");
//    skeleton->ScaleOn(0.001f, 0.001f, 0.001f);
    //skeleton->RunAnimation(0, true);

    /*Model rotated_skeleton("../resources/models/Skeleton.fbx");
    rotated_skeleton.ScaleOn(0.001f, 0.001f, 0.001f);
    rotated_skeleton.TranslateTo(0.0f, 1.0f, 0.0f);
    rotated_skeleton.RotateOn(45.0f, 1.0f, 0.0f, 0.0f);
*/
    // TODO: Scale(float size); --> not dublicating

    Model archer("/home/pavlo/Downloads/Longbow/Erika Archer.dae");
    archer.ScaleOn(0.001f, 0.001f, 0.001f);
    archer.TranslateTo(1.0f, 1.0f, 0.0f);

    scene_->get_camera()->get_keyboard_handler()->Bind(Key::Up, [=]() {
      skeleton->TranslateOn(0.0f, 10.0f, 0.0f);
      std::cout << skeleton->get_position().y << std::endl;
    })    ;
    scene_->get_camera()->get_keyboard_handler()->Bind(Key::Down, [=]() {
      skeleton->TranslateOn(0.0f, -10.2f, 0.0f);
    })    ;
    scene_->get_camera()->get_keyboard_handler()->Bind(Key::Left, [=]() {
      skeleton->TranslateOn(10.2f, 0.0f, 0.0f);
    })    ;
    scene_->get_camera()->get_keyboard_handler()->Bind(Key::Right, [=]() {
      skeleton->TranslateOn(-10.2f, .2f, 0.0f);
    });
    /*scene_->get_camera()->get_keyboard_handler()->Bind(Key::Space, [=]() {
      skeleton->RunAnimation(0, false);
    });*/ // lagging a lot
  });

  StartProcessing();

  while (!initialized_) {}
}

void SimurghManager::StartProcessing() {
  framerate_.last_ = glfwGetTime();
  UpdateFramerate();

  thread_pool_manager_->get_render_thread_pool()->Config([=]() {
    glClearColor(0.3f, 0.3f, 0.8f, 1.0f); // TODO: to class_Scene
    glfwSwapInterval(1); // Vsync - window property

    //auto model1 = new ModelLoader("/home/pavlo/Downloads/Longbow/Erika Archer.dae", true);
    //auto model6 = new Model("../resources/objects/43-obj/obj/Wolf_One_obj.obj", true);
    //auto model2 = new ModelLoader("../resources/objects/Skeleton.fbx", true);
    //model1->TranslateTo(0.0f, 3.0f, 0.0f);
    //model2->TranslateTo(-3.0f, 3.0f, 0.0f);
    //model1->ScaleTo(0.01f, 0.01f, 0.01f);
    //model2->ScaleTo(0.01f, 0.01f, 0.01f);
    //model6->ScaleTo(0.1f, 0.05f, 0.05f);
    //auto model2 = new Model("../resources/objects/cube_obj/cottage_obj.obj", true);
    //model2->TranslateTo(0.0f, 3.0f, 0.0f);
//    model2->ScaleTo(0.0001f, 0.0001f, 0.0001f);

//    auto model1 = new Model("../resources/objects/cube_obj/cottege1.obj", true);
//    model1->TranslateTo(0.0f, 3.0f, 0.0f);
    //auto model4 = new Model("../resources/objects/backpack/backpack.obj", true);
    //model4->TranslateTo(0.0f, 4.0f, 0.0f);
    //auto model3 = new Model("../resources/objects/cube_obj/ak.obj", true);
    //model3->TranslateTo(2.0f, 1.0f, 0.0f);
  });

  thread_pool_manager_->get_render_thread_pool()->AttachRenderLoop([&]() {
    UpdateFramerate();

    faithful::SimurghManager::get_scene()->ProcessInput(
      faithful::SimurghManager::get_window());
    faithful::SimurghManager::get_scene()->ProcessDrawing();

    glfwSwapBuffers(faithful::SimurghManager::get_window()->Glfw());
    glfwPollEvents();
  });

  thread_pool_manager_->get_render_thread_pool()->Run();
}

// TODO: ~SimurghManager() { glfwTerminate(); }


void SimurghManager::ProcessOpenGLFunctions() {}

// TODO?: conversion from double to float
//      (hmmmmmmm, does it make sense to keep framerate in double...)
void SimurghManager::UpdateFramerate() {
  double current_frame = glfwGetTime();
  framerate_.delta_ = current_frame - framerate_.last_;
  framerate_.last_ = current_frame;
}

// TODO: implement scene switching (may be assign ID for each scene)
void SimurghManager::SwitchSceneRequest(int scene_id) {
  Scene* target_scene = SceneManager::SceneById(scene_id);
  if (target_scene != nullptr) set_scene(target_scene);
}

void SimurghManager::set_camera(Camera* camera) {
  scene_->set_camera(camera);
}
void SimurghManager::set_scene(Scene* scene) {
  scene_ = scene;
  scene_->MakeActive();
}

Camera* SimurghManager::get_camera() {
  return scene_->get_camera();
}
Scene* SimurghManager::get_scene() {
  return scene_;
}

Window* SimurghManager::window_ = nullptr;
Scene* SimurghManager::scene_ = nullptr;
ThreadPoolManager* SimurghManager::thread_pool_manager_ = nullptr;
Logger* SimurghManager::logger_ = nullptr;
SimurghManager::FrameRate SimurghManager::framerate_ = {0, 0};
bool SimurghManager::initialized_ = false;

} // namespace faithful
