#include "Scene.h"
#include "environment/CubeMap.h"
#include "environment/Environment.h"

#include "Engine.h"
#include "common/CollisionManager.h"

namespace faithful {

// TODO: glClearColor()

Scene::Scene() {
  id_ = ++global_id_;
  SceneManager::StartTracking(this);
  // TODO: create default 2Dshapes (square, circle, rectangle, etc...)
}

Scene2D::Scene2D() {
  // collision_list_ = new std::forward_list<const
  // SingleObjectImpl__not_complete*>;
  InitStrategy();
  DefaultCursor::Init();
  // TODO: create default 2Dshapes (square, circle, rectangle, etc...)
}

Scene3D::Scene3D() {
  // collision_list_ = new std::forward_list<const
  // SingleObjectImpl__not_complete*>;
  InitShooter();
  DefaultCursor::Init();
  // TODO: create default 2Dshapes (square, circle, rectangle, etc...)
}

Scene2D::Scene2D(SceneMode mode) {
  // collision_list_ = new std::forward_list<const
  // SingleObjectImpl__not_complete*>;

  DefaultCursor::Init();
  switch (mode) {
    case SceneMode::Strategy:
      InitStrategy();
      break;
    case SceneMode::Platformer:
      InitPlatformer();
      break;
    default:
      std::cout << "Wrong scene mode (need 2D, not 3D)" << std::endl;
      // TODO: logger
  }
  // TODO: create default 2Dshapes (square, circle, rectangle, etc...)
}

Scene3D::Scene3D(SceneMode mode) {
  // collision_list_ = new std::forward_list<const
  // SingleObjectImpl__not_complete*>;

  DefaultCursor::Init();
  switch (mode) {
    case SceneMode::Shooter:
      InitShooter();
      break;
    case SceneMode::Rpg:
      InitRpg();
      break;
    default:
      std::cout << "Wrong scene mode (need 3D, not 2D)" << std::endl;
      // TODO: logger
  }
  // TODO: create default 2Dshapes (square, circle, rectangle, etc...)
}

Scene::~Scene() {
  // draw_list_ == nullptr means its dummy object
  if (id_ == -1)
    return;
  SceneManager::StopTracking(this);
  delete camera_;
  delete cursor_;
  // delete collision_list_;
}

Scene2D::~Scene2D() {
  //  delete background_;
  //  delete floor_;
}

Scene3D::~Scene3D() {
  //  delete cubemap_;
  //  delete floor_;
}

void Scene::MakeActive() {
  // CollisionManager::BindCollisionList(collision_list_);
  camera_->MakeActive();
  cursor_->MakeActive();
}

void Scene2D::ProcessDrawing() {
  if (camera_ == nullptr)
    return;
  // TODO:
  //       DrawManager::Process2dDrawing(camera_->CreateViewMatrix(),
  //       camera_->CreateProjectionMatrix());

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_STENCIL_TEST);

  glClear(GL_COLOR_BUFFER_BIT);

  for (auto obj : trivial2d_creators_)
    obj.second->Draw(ObjectRenderPhase::kGeometry);  // kDefault
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

// TODO 1: create&config default geom shapes (cube, rectangle, etc...)

// TODO 2: view, proj, create "static" shader programs and use them in
// ObjectImpl-like_classes
// TODO 3: adjust view, proj matrices and update them each frame

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

void Scene3D::ProcessDrawing() {
  if (camera_ == nullptr)
    return;
  // TODO:
  //       DrawManager::Process3dDrawing(camera_->CreateViewMatrix(),
  //       camera_->CreateProjectionMatrix());

  /* We draw in that order:
   *    hud (+ stencil buffer - depth buffer)
   *    default 3d/2d objects (+ stencil buffer)
   *      + off-screen rendering;
   *      + post-processing.
   * */

  glm::mat4 view = camera_->CreateViewMatrix();
  glBindBuffer(GL_UNIFORM_BUFFER, utility::DefaultShaderProgram::ubo_matrices_);
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
                  glm::value_ptr(view));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);

  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glStencilMask(0xFF);

  glClear(GL_STENCIL_BUFFER_BIT);

  // z-fighting for stencil when too close to screen
  //    BUT it doesn't matter because this stencil only for optimization
  for (auto obj : trivial2d_creators_)
    obj.second->Draw(ObjectRenderPhase::kGeometry);  // kStepcil

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
  glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
  glStencilMask(0x00);
  cube_creator_.Draw(ObjectRenderPhase::kGeometry);
  // for (auto obj : trivial3d_creators_)
  //   obj.second->Draw(ObjectRenderPhase::kGeometry); // kDefault;
  model_supervisor_.Update(SimurghManager::get_framerate());
  model_supervisor_.Draw(ObjectRenderPhase::kGeometry);  // kDefault

  // if (current_draw_list_->cubemap_ != nullptr)
  //   current_draw_list_->cubemap_->Draw(view, projection);

  glDisable(GL_STENCIL_TEST);
  glDisable(GL_DEPTH_TEST);

  for (auto obj : trivial2d_creators_)
    obj.second->Draw(ObjectRenderPhase::kGeometry);  // kDefault

  // TODO: + offscreen rendering
  // TODO: + post-processing
}

void Scene::ProcessInput(faithful::Window* window) {
  if (camera_ == nullptr)
    return;
  camera_->ProcessInput(window);
}

// TODO: to solve problems with "drawing on wrong scene"
//       we always switching on new scene firstly

void Scene3D::InitShooter() {
  camera_ = new Camera(SceneMode::Shooter);
  cursor_ = DefaultCursor::no_cursor_;
  // cubemap_ = new CubeMap;
  // draw_list_->set_cubemap(cubemap_);

  // auto floor3d = new Floor3D(Floor3D::DefaultType::Grass);
  // draw_list_->Add(floor3d);
  // floor_ = floor3d;
}
void Scene3D::InitRpg() {
  camera_ = new Camera(SceneMode::Rpg);
  cursor_ = DefaultCursor::no_cursor_;
  // cubemap_ = new CubeMap;
  // draw_list_->set_cubemap(cubemap_);

  // auto floor3d = new Floor3D(Floor3D::DefaultType::Grass);
  // draw_list_->Add(floor3d);
  // floor_ = floor3d;
}
void Scene2D::InitStrategy() {
  camera_ = new Camera(SceneMode::Strategy);
  cursor_ = DefaultCursor::cursor_;

  // auto background = new Wallpaper(Wallpaper::DefaultType::Map);
  // draw_list_->Add(background, -1);
}
void Scene2D::InitPlatformer() {
  camera_ = new Camera(SceneMode::Platformer);
  cursor_ = DefaultCursor::no_cursor_;
  // auto floor2d = new Floor2D(Floor2D::DefaultType::Grass);
  // draw_list_->Add(floor2d, -1);
  ////collision_list_->push_front(floor2d);
  // floor_ = floor2d;

  // auto background = new Wallpaper(Wallpaper::DefaultType::Twilight);
  // draw_list_->Add(background, -1);
}

void SceneManager::StartTracking(Scene* scene) {
  scenes_->insert(scene);
}

void SceneManager::StopTracking(Scene* scene) {
  scenes_->erase(scene);
}

Scene* SceneManager::SceneById(int id) {
  Scene dummy_scene(Scene::Dummy{}, id);
  auto target = scenes_->find(&dummy_scene);
  if (target == scenes_->end())
    return nullptr;

  return *target;
}

int Scene::global_id_ = 0;
std::set<Scene*, ScenePointerComparator>* SceneManager::scenes_ =
    new std::set<Scene*, ScenePointerComparator>;

}  // namespace faithful
