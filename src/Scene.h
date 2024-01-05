#ifndef FAITHFUL_SCENE_H
#define FAITHFUL_SCENE_H

#include <set>

#include "entities/Object.h"
#include "entities/Model.h"
#include "entities/Objects_3d.h";

namespace faithful {

class Camera;
class Cursor;
class Window;
class Cubemap;

enum class SceneMode {
  Shooter,
  Rpg,
  Strategy,
  Platformer
};

/**
 * Each Scene has its own _lists_
 *   (but not _objects_ - they may be used in few scenes):
 * drawable_list_;
 * collisionable_list_;
 * update_list_.
 * */
class Scene {
 public:
  // see Scene(Dummy, int id) below
  struct Dummy {};

  Scene();
  Scene(const Scene& other) = delete;
  Scene(Scene&& other) = delete;
  Scene& operator=(const Scene& other) = delete;
  Scene& operator=(Scene&& other) = delete;

  ~Scene();

  // means NO-tracking by SceneManager; all fields set to nullptr
  // used in SceneManager for creating of temp-instance _scene_ and
  // call std::set::find(_scene_)
  Scene(Dummy, int id)
      : id_(id) {
  }

  int Id() const {
    return id_;
  }
  virtual void ProcessDrawing() {
  }
  void ProcessInput(Window* window);
  /// animations
  /// rigid body
  /// physics
  // TODO: void ProcessUpdateObj();
  // TODO: create class UpdateManager;
  // TODO: create class Updatable - user has to just inherit from it
  //              and override Update() method.
  //  ____We also want class UpdateableUbiqutous and then just put lambdas into
  //  it

  // TODO: std::forward_list<const SingleObjectImpl__not_complete*>*
  // update_list_ = nullptr;

  /// __subscribers__-like system
  // void ProcessCollisions();
  // void ProcessAudio();

  Camera* get_camera() const {
    return camera_;
  }
  Cursor* get_cursor() const {
    return cursor_;
  }
  // RenderSequence* get_draw_list() const { return draw_list_; }

  void set_camera(Camera* camera) {
    camera_ = camera;
  }
  void set_cursor(Cursor* cursor) {
    cursor_ = cursor;
  }

  void MakeActive();

  void set_floor(unsigned int local_floor_id) {
  }  // TODO:
  void set_background(unsigned int local_background_id) {
  }  // TODO:

  virtual std::pair<int, int> AddFloor() {
  }
  virtual std::pair<int, int> AddBackgtound() {
  }

  virtual std::tuple<glm::mat3*, int, int> AddTrivial2D(
      unsigned int global_id) {
    auto found_creator = trivial2d_creators_.find(global_id);
    if (found_creator == trivial2d_creators_.end())
      return {nullptr, 0, 0};
    return found_creator->second->CreateInstance();
  }
  virtual std::tuple<glm::mat4*, int, int> AddTrivial3D() {
    return {nullptr, 0, 0};
  }
  virtual std::tuple<glm::mat4*, unsigned int, unsigned int> AddModel(
      const char* path) {
    return {nullptr, 0, 0};
  }
  virtual void RunAnimation(unsigned int global_id, unsigned int local_id,
                            unsigned int anim_id, bool repeat) {
  }

 protected:
  Camera* camera_ = nullptr;
  Cursor* cursor_ = nullptr;
  std::map<int /*global_obj_id*/, TrivialObject2DImpl*> trivial2d_creators_;

 private:
  static int global_id_;
  int id_ = -1;
  // TODO: std::forward_list<const SingleObjectImpl__not_complete*>*
  // collision_list_= nullptr;
  // TODO: DrawManager + shader programs from ShaderProgram.h/cpp (static)
};

class Scene2D : public Scene {
 public:
  Scene2D();
  Scene2D(SceneMode mode);
  ~Scene2D();
  void ProcessDrawing() override;

  std::pair<int, int> AddFloor() override {
  }  // TODO:
  std::pair<int, int> AddBackgtound() override {
  }  // TODO:

 private:
  void InitStrategy();
  void InitPlatformer();

  //  Background2D* background_ = nullptr;
  //  Floor2D* floor_ = nullptr;
};
class Scene3D : public Scene {
 public:
  Scene3D();
  Scene3D(SceneMode mode);
  ~Scene3D();
  void ProcessDrawing() override;

  std::pair<int, int> AddFloor() override {
  }  // TODO:
  std::pair<int, int> AddBackgtound() override {
  }  // TODO:

  std::tuple<glm::mat4*, int, int> AddTrivial3D() override {
    return cube_creator_.CreateInstance();
  }
  std::tuple<glm::mat4*, unsigned int, unsigned int> AddModel(
      const char* path) override {
    return model_supervisor_.Load(path);
  }
  void RunAnimation(unsigned int global_id, unsigned int local_id,
                    unsigned int anim_id, bool repeat) override {
    model_supervisor_.RunAnimation(global_id, local_id, anim_id, repeat);
  }

 private:
  void InitShooter();
  void InitRpg();

  // TODO: class Background3D fits more
  //  CubeMap* cubemap_ = nullptr;
  //  Floor3D* floor_ = nullptr;
  // TODO:_____________________ NOT map BUT explicitly CubeCreator,
  // ModelCreator, etc...
  CubeCreator cube_creator_;
  ModelSupervisor model_supervisor_;
};

struct ScenePointerComparator {
  bool operator()(const Scene* scene1, const Scene* scene2) const {
    return scene1->Id() < scene2->Id();
  }
};

// holds ptr to each Scene and gives them by scene.id_
class SceneManager {
 public:
  static Scene* SceneById(int id);

 protected:
  friend class Scene;
  static void StartTracking(Scene* scene);
  static void StopTracking(Scene* scene);

 private:
 public:
  static std::set<Scene*, ScenePointerComparator>* scenes_;
};

}  // namespace faithful

#endif  // FAITHFUL_SCENE_H
