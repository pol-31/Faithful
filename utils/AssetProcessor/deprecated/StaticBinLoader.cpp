#include "StaticBinLoader.h"

#include <cstdint>

namespace faithful {
namespace static_load {

void ReadTrivialModel(std::ifstream&& ifstream) {
  /**
   * uint32 | idx_num
   * uint32 x 5 | tex_ids
   * -----------------------------------------
   * uint32 x 3 | position (cast from float)
   * uint32 x 2 | tex_coord (cast from float)
   * uint32 x 3 | normal (cast from float)
   * uint32 x 3 | tangent (cast from float)
   * uint32 x 3 | bitangent (cast from float)
   * */
  uint32_t uint_mesh_data[6];
  ifstream.read(reinterpret_cast<char*>(uint_mesh_data),
                sizeof(uint_mesh_data));

#if __SIZEOF_FLOAT__ == 4
  float float_mesh_data[14];
  ifstream.read(reinterpret_cast<char*>(float_mesh_data),
                sizeof(float_mesh_data));
#else
  int min_copy_size;
#if __SIZEOF_FLOAT__ < 4
  min_copy_size = sizeof(float);
#else  // stupid case, but why not...
  min_copy_size = sizeof(uint32_t);
#endif
  uint32_t buf[14];
  float float_mesh_data[14];
  ifstream.read(reinterpret_cast<char*>(buf), sizeof(buf));
  for (int i = 0; i < 14; ++i)
    std::memcpy(&float_mesh_data[i], &buf[i], min_copy_size);
#endif
  // TODO: create & configure TrivialObject3D
  // TODO: we also need to store vao, vbo, ebo, ubo(?) indices
  //       < in _save_ we also store higher used ubo, ebo, vao,
  //          so then just allocate all of them at once >
  //        also __last_global_id__
}
void ReadMultimeshModel(std::ifstream&& ifstream) {
  /*if (mesh_num < 5) {
    // 2 threadpool calls
  } else if (mesh_num < 20) {
    // 4 threadpool calls
  } else {
    // 7(for all) threadpool calls
    // TODO: add parallelism
  }*/
}
void ReadSkinnedModel(std::ifstream&& ifstream) {
  /*if (mesh_num < 5) {
    // 1 ofstream & threadpool call
  } else if (mesh_num < 20) {
    // 2 ofstreams & threadpool calls
  } else {
    // 4 ofstreams & threadpool calls
    // TODO: add parallelism
  }*/
  // TODO: read global_inverse_transform
  // TODO: read animation metadata
  // TODO: process & parallelize animation_tree-s
}

void ReadBinModel(std::filesystem::path path) {
  std::cout << path << std::endl;
  std::ifstream read_stream(path, std::ios::binary);
  if (!read_stream.is_open()) {
    std::cerr << "Failed to open .bin file"
              << std::endl;  // TODO: replace by Logger
  }
  // TODO: metadata
  /**
   * uint16 | model_id, amount of 32768 is enough
   * uint16 | some_extra_info
   * uint16 | mesh_num=N, amount of 16384 is enough
   * uint16 | bone num
   */
  uint32_t metadata[2];
  read_stream.read(reinterpret_cast<char*>(metadata), sizeof(metadata));
  // narrowing from uint16 to int, so it's Ok
  int mesh_num = metadata[1] >> 16;
  int bone_num = metadata[1] & (1U >> 16);
  if (mesh_num == 1) {
    ReadTrivialModel(std::move(read_stream));
  } else if (bone_num == 0) {
    ReadMultimeshModel(std::move(read_stream));
  } else {
    ReadSkinnedModel(std::move(read_stream));
  }
}

void ReadModelsFromDir(const char* dir_path) {
  // TODO: notify SimurghManager about __loading_phase__
  for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
    // TODO: parallelism (each .bin separatedly into LoadThreadPool)
    if (entry.is_regular_file())
      ReadBinModel(entry.path());
  }
}

void WriteModelsFromDir(const char* dir_path) {
}
void WriteBinModel(
    std::filesystem::path path) {  // TODO: Model_class instance as a parameter
  // open as "rewrite" mode
  /*std::ofstream file("test.txt", std::ios::binary);

  if (!file.is_open()) {
    std::cerr << "Failed to open the file." << std::endl;
  }

  int metaInfo[4] = {1, 2, 3, 4}; // Replace with your metadata values
  file.write(reinterpret_cast<char*>(metaInfo), sizeof(metaInfo));
   */
}

void WriteTrivialModel(std::ifstream&& ifstream) {
}
void WriteMultimeshModel(std::ifstream&& ifstream) {
}
void WritekinnedModel(std::ifstream&& ifstream) {
}

static int skinless_mesh_size = 20 * sizeof(uint32_t);
static int skinned_mesh_size = 44 * sizeof(uint32_t);
static int metadata_size = 4 * sizeof(uint16_t);

}  // namespace static_load
}  // namespace faithful
