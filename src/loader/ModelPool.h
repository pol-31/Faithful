#ifndef FAITHFUL_SRC_LOADER_MODELPOOL_H_
#define FAITHFUL_SRC_LOADER_MODELPOOL_H_

#include <string>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

#include "../common/OpenGlContextAwareBase.h"
#include "IAssetPool.h"
#include "../../../config/Loader.h"

#include "../entities/Ambient.h"
#include "../entities/Boss.h"
#include "../entities/Enemy.h"
#include "../entities/Furniture.h"
#include "../entities/Loot.h"
#include "../entities/Npc.h"
#include "../entities/Obstacle.h"

#include "assets_data/ModelData.h"

#define TINYGLTF_USE_RAPIDJSON_CRTALLOCATOR // TODO:

namespace faithful {
namespace details {
namespace assets {

class TexturePool;
class SoundPool;

/// Wrapper for all model types, serves as:
///  - a connection point between user interface & implementation
///  - a storage for data (other classes take models refs / copies from there)
/// private inheritance from IAssetPool - we don't want Load(std::string),
/// but other mechanic still useful
class ModelPool:
    private IAssetPool<ModelData, faithful::config::kModelCacheSize>,
    public OpenGlContextAwareBase {
 public:
  using Base = IAssetPool<ModelData, faithful::config::kModelCacheSize>;
  using DataType = typename Base::DataType;

  ModelPool() = delete;
  ModelPool(SoundPool* sound_pool, TexturePool* texture_pool);

  /// not copyable
  ModelPool(const ModelPool&) = delete;
  ModelPool& operator=(const ModelPool&) = delete;

  /// not movable (because of mutex)
  ModelPool(ModelPool&&) = delete;
  ModelPool& operator=(ModelPool&&) = delete;

  std::shared_ptr<ModelData> Load(int model_id);

  /// returns true if there was executed a task,
  /// otherwise false, what means all have been already processed
  /// this is important for LoadingManager to understand phase of loading:
  /// normal or stress
  bool Assist();

  void ClearModelCache();
  void ClearModelInactive();

 private:
  struct ModelFileInfo {
    std::string name;
    std::string path;
    std::vector<std::string> sound_paths;
    char type;
    int id;
  };

  class ProcessingContext {
   public:
    tinygltf::Model model;
    std::shared_ptr<ModelData> model_data;
    ModelFileInfo* model_info;

    void Process();

   private:
    bool LoadModelFile(std::string filename);

    /// collecting vertex data for computing bounding shapes
    std::vector<glm::vec3> CollectModelVertices();

    /// generating of vao / vbo, should be called from opengl_context
    void BindMesh(std::map<int, GLuint>& vbos,
                  tinygltf::Mesh &mesh);

    void BindModelNodes(std::map<int, GLuint>& vbos,
                        tinygltf::Node &node);

    std::pair<GLuint, std::map<int, GLuint>> BindModel();
  };

  DataType LoadImpl(typename Base::TrackedDataType& instance_info) override {
    return instance_info.data;
  }

  void Init();

  void LoadTextures(tinygltf::Model& model, Material& material);

  SoundPool* sound_pool_;
  TexturePool* texture_pool_;

  // TODO: std::unique_ptr<char[]> collision_buffer_;

  /// "all" related to all from Faithful/assets/main/ with extension .gltf
  /// initially sorted (at Init())
  std::vector<ModelFileInfo> all_models_;

  std::vector<Ambient> ambient_models_;
  std::vector<Boss> boss_models_;
  std::vector<Enemy> enemy_models_;
  std::vector<Furniture> furniture_models_;
  std::vector<Loot> loot_models_;
  std::vector<Npc> npc_models_;
  std::vector<Obstacle> obstacle_models_;

  std::mutex mutex_processing_tasks_;
  std::vector<std::unique_ptr<ProcessingContext>> processing_tasks_;
};

/// we're using only "url" property, but
/// load inside the LoadTextures() with texture_pool_
bool LoadImageData(tinygltf::Image *image, const int image_idx, std::string *err,
                   std::string *warn, int req_width, int req_height,
                   const unsigned char *bytes, int size, void *user_data) {
  (void)image;
  (void)image_idx;
  (void)err;
  (void)warn;
  (void)req_width;
  (void)req_height;
  (void)bytes;
  (void)size;
  (void)user_data;
  return true;
}

} // namespace assets
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_MODELPOOL_H_
