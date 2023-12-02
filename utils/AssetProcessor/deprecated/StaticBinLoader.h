#ifndef FAITHFUL_STATICBINLOADER_H
#define FAITHFUL_STATICBINLOADER_H

#include <cstdint>
#include <filesystem>
#include <fstream>

#include <iostream> // temporarily

//#include "../../config/config_paths.h"
#include "AnimationNode.h"

namespace faithful {
namespace static_load {

/** What data we want to store:
 * Features:
 * 3d-obj(Model) features
 *   all images (textures, sprites) ids in certain order
 *       (then sequentially init by OpenGL runtime functions)
 *   shaders
 *   sounds
 * Instances:
 *   basic configurations
 *   per-scene data:
 *     camera & handlers, cursor features
 *     3d-obj with binded transform, shaders, sounds, materials
 *     2d-obj with binded transform, shaders, sounds, materials
 * */

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
///////////////////////   Model features   /////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

/** SERIALIZATION (1 .bin for 1 model)
 * ______________________________[--------Metadata--------]
 * uint16 | model_id, amount of 32768 is enough
 * uint16 | some_extra_info
 * uint16 | mesh_num=N, amount of 16384 is enough
 * uint16 | bone num
 * ______________________________[--------Mesh_data-------]
 * // size of mesh depends on is_animated and have only two diff values
 * <optimization 1: T1 threads depends on mesh_num, if N=1 -> TrivialModel>
 * uint32[N] | idx_num
 * (uint32 x 5)[N] | tex_ids
 * (uint32 x 3)[N] | position (cast from float)
 * (uint32 x 2)[N] | tex_coord (cast from float)
 * (uint32 x 3)[N] | normal (cast from float)
 * (uint32 x 3)[N] | tangent (cast from float)
 * (uint32 x 3)[N] | bitangent (cast from float)
 * ____if_animated____
 * (uint32 x 4)[N] | bone_weights (cast from float)
 * (uint32 x 4)[N] | bone_ids
 * uint32 x 16 | global_inverse_transform_mat (cast from float)
 * ______________________________[-----Animation_data-----]
 * uint8  | animations_num=M --> 128 different animations is enough
 * <optimization 2: T2 threads depends on animation_num
 *             (1 thread - 1/few tree)
 *             (1 tree - 1 thread (exclusive access))>
 * uint32 * M | animation sizes (in bytes)
 *    [M]
 *     uint16 | children_num
 *     uint32 x 16 | node_transform (cast from float)
 *     uint32 x 16 | bone_transform (cast from float)
 *     uint16 | bone_id_
 *     uint32 | offset_rotation_and_position_
 *     uint32 | total_keys_num=K
 *     uint32[K] | keys_data (cast from float)
 *     ... other nodes with the same structure
 * Animation_tree stored in DFS (pre-order)
 * */


// TODO: need incapsulation (ReadTrivialModel, ReadBinModel, etc...)


// TODO (further development): concurrently parse animation-tree
//        (for now 1 tree can't be parsed concurrently)


static std::filesystem::path default_dir_path_ = "";//SIMURGH_MODELS_BIN_PATH;

uint32_t CastFloatToUint32(float float_value) {
  uint32_t uint_value;
  std::memcpy(&uint_value, &float_value, sizeof(float));
  return uint_value;
}

float CastUint32ToFloat(uint32_t uint_value) {
  float float_value;
  std::memcpy(&float_value, &uint_value, sizeof(float));
  return float_value;
}


void ReadModelsFromDir(const char* dir_path = default_dir_path_.c_str());

void ReadBinModel(std::filesystem::path path);

void ReadTrivialModel(std::ifstream&& ifstream);
void ReadMultimeshModel(std::ifstream&& ifstream);
void ReadSkinnedModel(std::ifstream&& ifstream);


void WriteModelsFromDir(const char* dir_path = default_dir_path_.c_str());

void WriteBinModel(std::filesystem::path path);

void WriteTrivialModel(std::ifstream&& ifstream);
void WriteMultimeshModel(std::ifstream&& ifstream);
void WritekinnedModel(std::ifstream&& ifstream);



////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
/////////////////////////  Model instances  ////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////



// SaveSession --> saving of all scenes






/**TOTAL _SAVE_ info:
 * last global_id_
 * last global vao, vbo, ebo, ubo
 * */

/* TRIVIAL3D
 *
  // unsigned int last_local_id_ = instances_num;

  unsigned int global_id_ = 0;
  uint32_t | vao_
  uint32_t | vbo_
  uint32_t | ebo_

  instance info:
  bool bit, uin32_t(if exist) | shader_program_
  uint32_t | scene_id_
  uint32_t | instance_num=N
   [N]
     transform
     material
     scene_id_ // TODO:.. hmm.......

  struct InstanceInfo {
    glm::mat4* transform;
    Material material;
  };

  std::map<unsigned int, InstanceInfo> drawable_instances_;
  std::map<unsigned int, InstanceInfo> undrawable_instances_;
  */

// TODO: the same for 2d-models, textures, sprites, sounds

} // namespace static_load
} // namespace faithful

#endif // FAITHFUL_STATICBINLOADER_H
