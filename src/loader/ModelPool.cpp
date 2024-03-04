#include "ModelPool.h"

#include <algorithm>
#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <tiny_gltf.h>

#include "TexturePool.h"
#include "SoundPool.h"

#include "../../config/Paths.h"

namespace faithful {
namespace details {
namespace assets {

ModelPool::ModelPool(SoundPool* sound_pool, TexturePool* texture_pool)
    : sound_pool_(sound_pool), texture_pool_(texture_pool) {
  Init();
}

void ModelPool::Init() {
  std::ofstream assets_info_file(FAITHFUL_ASSET_INFO_FILE);
  // id;name;type;sounds_id,sound_id;path;
  // example of data:
  // 1;Bear;a;/path1,/path2,/path3,/path4;/home/pavlo/Desktop/assets/Bear.gltf;
  // 2;ear;a;/path1,/path2,/path3;/home/pavlo/Desktop/assets/ear.gltf;
  std::ifstream file(FAITHFUL_ASSET_INFO_FILE);
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << FAITHFUL_ASSET_INFO_FILE << std::endl;
    return;
  }

  std::string line;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    ModelFileInfo info;

    std::string field;
    std::getline(iss, field, ';');
    info.id = std::stoi(field);

    std::getline(iss, info.name, ';');

    std::getline(iss, field, ';');

    iss >> info.type;

    std::getline(iss, field, ';');
    std::istringstream soundPathsStream(field);
    while (std::getline(soundPathsStream, field, ',')) {
      info.sound_paths.push_back(field);
    }

    std::getline(iss, info.path, ';');

    all_models_.push_back(info);
  }
}

void ModelPool::ClearModelCache() {
  Base::ClearCache();
}

void ModelPool::ClearModelInactive() {
  Base::ClearInactive();
}

std::shared_ptr<ModelData> ModelPool::Load(int model_id) {
  ModelFileInfo dummy;
  dummy.id = model_id;
  auto it = std::lower_bound(
      all_models_.begin(), all_models_.end(), dummy,
      [](const ModelFileInfo& info1, const ModelFileInfo& info2) {
        return info1.id < info2.id;
      });
  if (it == all_models_.end() || it->id != dummy.id) {
    std::cerr << "ModelPool::Load incorrect id" << std::endl;
    std::terminate();
  }

  tinygltf::Model model;
  auto model_data = Base::Load(it->path);
  model_data->global_model_id = it->id;
  model_data->opengl_context = opengl_context_;

  for (const auto& path : it->sound_paths) {
    model_data->sounds.push_back(sound_pool_->Load(path));
  }

  LoadTextures(model, model_data->material);

  switch (it->type) {
    case 'a': // EntityType::kAmbient
      ambient_models_.push_back(model_data);
      break;
    case 'b': // EntityType::kBoss
      boss_models_.push_back(model_data);
      break;
    case 'e': // EntityType::kEnemy
      enemy_models_.push_back(model_data);
      break;
    case 'f': // EntityType::kFurniture
      furniture_models_.push_back(model_data);
      break;
    case 'l': // EntityType::kLoot
      loot_models_.push_back(model_data);
      break;
    case 'n': // EntityType::kNpc
      npc_models_.push_back(model_data);
      break;
    case 'o': // EntityType::kObstacle
      obstacle_models_.push_back(model_data);
      break;
    default:
      loot_models_.push_back(model_data);
      std::cerr << "incorrect model type, treating it as a loot" << std::endl;
  }

  auto processing_context = std::make_unique<ProcessingContext>();
  processing_context->model = std::move(model);
  processing_context->model_data = model_data;
  processing_context->model_info = &*it;

  std::lock_guard lock(mutex_processing_tasks_);
  processing_tasks_.push_back(std::move(processing_context));
  return {model_data};
}

bool ModelPool::Assist() {
  if (processing_tasks_.empty()) {
    return false;
  }
  std::unique_lock lock(mutex_processing_tasks_);
  if (processing_tasks_.empty()) {
    return false;
  }
  for (auto& task : processing_tasks_) {
    /// release lock and start processing
    lock.unlock();
    /// assist only once, we don't want to hang out there all the time
    task->Process();
    return true;
  }
}

void ModelPool::ProcessingContext::Process() {
  LoadModelFile(model_info->path);
  auto vertices = CollectModelVertices();
  model_data->collision_area.ComputeCollisionArea(vertices);

  model_data->model = std::move(model);

  model_data->opengl_context->Put([this]{
    auto vao_and_vbos = BindModel();
    model_data->vao = vao_and_vbos.first;
    model_data->vbo_and_ebos = vao_and_vbos.second;
    model_data->ready = true;
    /// delete all data from all tinygltf::Model buffers, because
    /// they already transmitted to GPU by glBufferData. We don't
    /// delete all model because we still need hierarchy for drawing
    //TODO:
    // for (auto& buffer : model_data->model.buffers) {
    //   buffer.data.clear();
    // }
  });
}

void ModelPool::LoadTextures(tinygltf::Model& model, Material& material) {
  auto albedo_tex = model.materials[0].pbrMetallicRoughness.baseColorTexture.index; // or diffuseTexture
  auto emission_tex = model.materials[0].emissiveTexture.index;
  auto metal_rough = model.materials[0].pbrMetallicRoughness.metallicRoughnessTexture.index;
  auto normal_tex = model.materials[0].normalTexture.index;
  auto occlusion_tex = model.materials[0].occlusionTexture.index;

  if (albedo_tex != -1) {
    auto& albedo_texture = model.textures[albedo_tex];
    auto image_index = albedo_texture.source;
    if (image_index != -1) {
      auto& image_uri = model.images[image_index].uri;
      std::cout << "Albedo texture URI: " << image_uri << std::endl;
      material.albedo = Texture(texture_pool_->Load(image_uri));
    } else {
      std::cout << "There's no occlusion texture" << std::endl;
    }
  } else {
    std::cout << "There's no occlusion texture" << std::endl;
  }

  if (emission_tex != -1) {
    auto& emission_texture = model.textures[emission_tex];
    auto image_index = emission_texture.source;
    if (image_index != -1) {
      auto& image_uri = model.images[image_index].uri;
      std::cout << "Emission texture URI: " << image_uri << std::endl;
      material.emission = Texture(texture_pool_->Load(image_uri));
    } else {
      std::cout << "There's no occlusion texture" << std::endl;
    }
  } else {
    std::cout << "There's no occlusion texture" << std::endl;
  }

  if (metal_rough != -1) {
    auto& metallic_roughness_texture = model.textures[metal_rough];
    auto image_index = metallic_roughness_texture.source;
    if (image_index != -1) {
      auto& image_uri = model.images[image_index].uri;
      std::cout << "Metallic-roughness texture URI: " << image_uri << std::endl;
      material.metal_rough = Texture(texture_pool_->Load(image_uri));
    }
  } else {
    std::cout << "There's no occlusion texture" << std::endl;
  }

  if (normal_tex != -1) {
    auto& normal_texture = model.textures[normal_tex];
    auto image_index = normal_texture.source;
    if (image_index != -1) {
      auto& image_uri = model.images[image_index].uri;
      std::cout << "Normal texture URI: " << image_uri << std::endl;
      material.normal = Texture(texture_pool_->Load(image_uri));
    } else {
      std::cout << "There's no occlusion texture" << std::endl;
    }
  } else {
    std::cout << "There's no occlusion texture" << std::endl;
  }

  if (occlusion_tex != -1) {
    auto& occlusion_texture = model.textures[occlusion_tex];
    auto image_index = occlusion_texture.source;
    if (image_index != -1) {
      auto& image_uri = model.images[image_index].uri;
      std::cout << "Occlusion texture URI: " << image_uri << std::endl;
      material.occlusion = Texture(texture_pool_->Load(image_uri));
    } else {
      std::cout << "There's no occlusion texture" << std::endl;
    }
  } else {
    std::cout << "There's no occlusion texture" << std::endl;
  }
}

std::vector<glm::vec3> ModelPool::ProcessingContext::CollectModelVertices() {
  std::vector<glm::vec3> vertices;
  for (size_t i = 0; i < model.bufferViews.size(); ++i) {
    const tinygltf::BufferView &bufferView = model.bufferViews[i];
    if (bufferView.target == GL_ARRAY_BUFFER) {
      const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
      glm::vec3 value;
      for (size_t j = 0; j < bufferView.byteLength; j += sizeof(glm::vec3)) {
        std::memcpy(&value, &buffer.data[j], sizeof(glm::vec3));
        vertices.push_back(value);
      }
    }
  }
  return vertices;
}

bool ModelPool::ProcessingContext::LoadModelFile(std::string filename) {
  std::string err;
  std::string warn;

  tinygltf::TinyGLTF loader;
  loader.SetImageLoader(LoadImageData, nullptr);
  bool res = loader.LoadASCIIFromFile(
      &model, &err, &warn, filename);
  if (!warn.empty()) {
    std::cout << "WARN: " << warn << std::endl;
  }

  if (!err.empty()) {
    std::cout << "ERR: " << err << std::endl;
  }

  if (!res) {
    std::cout << "Failed to load glTF: " << filename << std::endl;
  } else {
    std::cout << "Loaded glTF: " << filename << std::endl;
  }

  return res;
}

void ModelPool::ProcessingContext::BindMesh(
    std::map<int, GLuint>& vbos, tinygltf::Mesh &mesh) {
  for (size_t i = 0; i < model.bufferViews.size(); ++i) {
    const tinygltf::BufferView &bufferView = model.bufferViews[i];
    if (bufferView.target == 0) {
      /*Unsupported bufferView (from spec2.0 readme):
       * https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
       * ... drawArrays function should be used with a count equal to
       * the count property of the accessors referenced by the
       * attributes property (they are all equal for a given primitive)
       */
      std::cout << "WARN: bufferView.target is zero" << std::endl;
      continue;
    }

    const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];

    GLuint vbo;
    glGenBuffers(1, &vbo);
    vbos[i] = vbo;
    glBindBuffer(bufferView.target, vbo);
    glBufferData(bufferView.target, bufferView.byteLength,
                 &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
  }

  for (size_t i = 0; i < mesh.primitives.size(); ++i) {
    tinygltf::Primitive primitive = mesh.primitives[i];
    tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

    for (auto &attrib : primitive.attributes) {
      tinygltf::Accessor accessor = model.accessors[attrib.second];
      int byteStride =
          accessor.ByteStride(model.bufferViews[accessor.bufferView]);
      glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

      int size = 1;
      if (accessor.type != TINYGLTF_TYPE_SCALAR) {
        size = accessor.type;
      }

      int vaa = -1;
      if (attrib.first.compare("POSITION") == 0) {
        vaa = 0;
      }
      if (attrib.first.compare("NORMAL") == 0) {
        vaa = 1;
      }
      if (attrib.first.compare("TEXCOORD_0") == 0) {
        vaa = 2;
      }
      if (attrib.first.compare("JOINTS_0") == 0) {
        vaa = 3;
      }
      if (attrib.first.compare("WEIGHTS_0") == 0) {
        vaa = 4;
      }
      if (vaa > -1) {
        glEnableVertexAttribArray(vaa);
        glVertexAttribPointer(vaa, size, accessor.componentType,
                              accessor.normalized ? GL_TRUE : GL_FALSE,
                              byteStride, reinterpret_cast<void*>(accessor.byteOffset));
      } else
        std::cout << "vaa missing: " << attrib.first << std::endl;
    }
  }
}

void ModelPool::ProcessingContext::BindModelNodes(
    std::map<int, GLuint>& vbos, tinygltf::Node &node) {
  if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
    BindMesh(vbos, model.meshes[node.mesh]);
  }

  for (size_t i = 0; i < node.children.size(); i++) {
    assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
    BindModelNodes(vbos, model.nodes[node.children[i]]);
  }
}

std::pair<GLuint, std::map<int, GLuint>> ModelPool::ProcessingContext::BindModel() {
  std::map<int, GLuint> vbos;
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  const tinygltf::Scene &scene = model.scenes[model.defaultScene];
  for (size_t i = 0; i < scene.nodes.size(); ++i) {
    assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
    BindModelNodes(vbos, model.nodes[scene.nodes[i]]);
  }

  glBindVertexArray(0);
  // cleanup vbos but do not delete index buffers yet
  for (auto it = vbos.cbegin(); it != vbos.cend();) {
    tinygltf::BufferView bufferView = model.bufferViews[it->first];
    if (bufferView.target != GL_ELEMENT_ARRAY_BUFFER) {
      glDeleteBuffers(1, &vbos[it->first]);
      vbos.erase(it++);
    } else {
      ++it;
    }
  }
  return {vao, vbos};
}

} // namespace assets
} // namespace details
} // namespace faithful
