#ifndef FAITHFUL_MODEL_H
#define FAITHFUL_MODEL_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../../Logger.h"
#include "Object.h"

#include <map>
#include <set>
#include <vector>
#include <forward_list>

#include "LoadPathManager.h"

namespace faithful {

// TODO: track loaded Models

class ModelLoader;
class Mesh;
struct UnprocessedMesh;
class Material;
class SkinnedMesh;
class SkinlessModelImpl;
class ModelSupervisor;

struct AiStringComparator;
struct AiNodeComparator;

namespace details {
namespace model {
/// for reusing we should track each loaded models
class ModelManager : public LoadPathManager<unsigned int> {
 protected:
  friend class faithful::ModelSupervisor;
  friend class faithful::ModelLoader;
  static GLuint Register(const char* path);
};

}  // namespace model
}  // namespace details

/**
 * each model has N meshes and for each we need separate vao, vbo, ebo and ubo
 * (for animated) So we can create all needed buffer at once: if we have 30
 * meshes we creating 30 id's for each mesh and store just offset for each type
 * of buffer in Model_class alongside with total amount In Mesh_class we also
 * storing offset to each type of buffer but related to parent Model_class
 * (reducing memory usage)
 * */

struct Bone {
  glm::mat4 offset_matrix;
  std::size_t id;
};

// TODO: each animation has all transforation matrices to all bones (for each
// animation frame)
//         so then we just using UBO to transfer all of them
// TODO: add __name__ which may be found by id
struct Animation {
  struct AnimationData {
    utility::Span<glm::mat4> matrices;
    float time;
  };
  utility::Span<AnimationData> data;
  std::size_t id;
};

struct VertexBoneInfo {
  struct BoneWeightInfo {
    std::size_t id = 0;
    float weight = 0;
  };
  void AddBone(std::size_t bone_id, float weight) {
    if (weight == 0)
      return;
    BoneWeightInfo buf, new_bone;
    new_bone = {bone_id, weight};
    for (int i = 0; i < 4; ++i) {
      if (bone_info[i].id == bone_id) {
        bone_info[i].weight = std::max(bone_id, bone_info[i].id);
        return;
      }
      if (new_bone.weight > bone_info[i].weight) {
        buf = bone_info[i];
        bone_info[i] = new_bone;
        new_bone = buf;
      }
    }
  }
  BoneWeightInfo bone_info[4];
};

// TODO: have sense replace "std::size_t" by just int / unsigned_int

/// 1 mesh - 1 Material (e.g. can't be 2 albedo textures for 1 Mesh)
/// IF there is no animations, we not tracking even bones (NO BONES/UBO LOGIC
/// without animations) max bones - 200 TODO: max bones 200?

// TODO: "skinned" --> "animated" (?)
// TODO: read about friend (private/protected/public specifier)
// TODO: i forgot...

// TODO: Model("../path"); --> Model calls ModelCreator, which
//    create either SkinnedModelImpl either SkinlessModelImpl, store it in
//    std::vector<Skinned, ubo_> or std::vector<Skinless> and returns ID-s & ptr
//    to transform mat
//  Then when we call Model::RunAnimation(global_id, local_id, anim_num, bool
//  repeat),
//    it sends signal to ModelCreator which search it in std::vector<Skinned,
//    ubo>
/// --> firstly try to find in animation_list_ that is in the most cases smaller
/// (good!)
/// --> find animations by local_id, find ubo by local_id, if exist --> to
/// animation_list_;

// TODO: optimization with 1 Mesh

class ModelLoader {
 public:
  ModelLoader();

  std::size_t BoneIdByName(const aiString& name);

  void InitMeshIndices(const aiMesh* mesh, std::vector<unsigned int>& indices);

  Material InitMeshMaterials(const aiScene* scene, const aiMesh* mesh);

  MultimeshObject3DImpl* LoadSkinless(const aiScene* scene);
  SkinnedObject3DImpl* LoadSkinned(const aiScene* scene);

  void InitCategory();

  void ProcessNode(const aiNode* node, const aiScene* scene);

  Mesh* ProcessSkinlessMesh(const UnprocessedMesh* mesh);
  SkinnedMesh* ProcessSkinnedMesh(const UnprocessedMesh* mesh);

  AnimationNode* GetInterpolatedAnimationKey(const aiAnimation* animation,
                                             const aiNode* node);

  const aiNodeAnim* GetNodeAnimation(const aiAnimation* animation,
                                     const aiString& node_name);

  bool NodeImportanceCheck(const aiNode* node, const aiScene* scene);

  GLuint LoadTexture(aiMaterial* mat, aiTextureType type);

  void set_path(const char* path) {
    path_ = path;
  }

 protected:
  // TODO: do we need "protected"?
  friend class Mesh;
  friend class Model;

  GLuint cur_vao_ = 0;
  GLuint cur_vbo_ = 0;
  GLuint cur_ebo_ = 0;
  unsigned int meshes_cur_id_ = 0;
  std::string path_;
  mutable utility::Span<UnprocessedMesh>
      unprocessed_meshes_;  // TODO: not_mutable
  std::set<const aiNode*, AiNodeComparator>* important_nodes_ = nullptr;

  ObjectRenderCategory category_ = ObjectRenderCategory::kDefault;

  utility::Span<AnimationNode*> animation_nodes_;
  mutable utility::Span<Mesh*> processed_meshes_;  // TODO: not_mutable

  std::map<aiString, Bone, AiStringComparator>* bones_ = nullptr;
  glm::mat4 global_inverse_transform_ = glm::mat4(1.0f);
};

class ModelSupervisor {
 public:
  ModelSupervisor() = default;
  std::tuple<glm::mat4*, unsigned int, unsigned int> Load(
      const char* path, bool static_load = false);

  // TODO: sort models by "shader_level" (?)
  void Draw(ObjectRenderPhase phase) const;
  void RunAnimation(unsigned int global_id, unsigned int local_id,
                    unsigned int anim_id, bool repeat);
  void Update(double framerate);

 private:
  Assimp::Importer importer_;
  /// we could store skinned elements in map, BUT
  /// animation running is used not as frequently as
  /// drawing of updating, so contiguous access matches better
  // TODO: std::forward_list || map || vector?
  std::vector<MultimeshObject3DImpl*> skinless_objects_;
  std::vector<SkinnedObject3DImpl*> skinned_objects_;
  ModelLoader loader_;
};

/*class Model : public Object3D {
 public:
  Model(const char* path) {
    std::tie(transform_, global_id_, local_id_) =
SimurghManager::get_scene()->AddModel(path);
  }
  // TODO: void RunAnimation(std::size_t num) {}
};*/

// TODO: aiString don't have comparisons, iterators.....
struct AiStringComparator {
  bool operator()(const aiString& arg1, const aiString& arg2) const noexcept {
    if (arg1 == arg2)
      return false;
    if (arg1.length != arg2.length)
      return (arg1.length < arg2.length);
    for (int i = 0; i < arg1.length; ++i) {
      if (arg1.data[i] != arg2.data[i])
        return (arg1.data[i] < arg2.data[i]);
    }
    return true;
  }
};

struct AiNodeComparator {
  bool operator()(const aiNode* node1, const aiNode* node2) const {
    return node1 < node2;
  }
};

glm::mat4 AssimpMatrixToGlm(const aiMatrix4x4& src);

}  // namespace faithful

#endif  // FAITHFUL_MODEL_H
