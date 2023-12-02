#include "Model.h"

#include <assimp/postprocess.h>

#include <glm/gtc/quaternion.hpp>

#include "../../Logger.h"
#include "Mesh.h"
#include "AnimationNode.h"

#define AI_CONFIG_PP_RVC_FLAGS aiComponent_COLORS | \
                               aiComponent_LIGHTS | \
                               aiComponent_CAMERAS

#define AI_CONFIG_PP_LBW_MAX_WEIGHTS 4

namespace faithful {

/// returns: transformation matrix, global_id, local_id
std::tuple<glm::mat4*, unsigned int, unsigned int>
    ModelSupervisor::Load(const char* path, bool static_load) {
  unsigned int* found_model = details::model::ModelManager::Find(path);
  if (found_model) {
    std::cout << " MODEL (global_id_) REUSED" << std::endl;
    for (auto i : skinless_objects_) {
      if (i->get_global_id() == *found_model)
        return i->CreateInstance();
    }
    for (auto i : skinned_objects_) {
      if (i->get_global_id() == *found_model)
        return i->CreateInstance();
    }
    // unreachable here: object always exist (managed by Dev)
  } else {
    /*
     TODO 4: glEnable(GL_CULLING)
     TODO 6: Assimp logging? (aiProcess_ValudateDataStructure / aiGetErrorString())
             {scene & material(texture) loading}
     TODO 9: bug no_response
     TODO 10: bug no 3d_object renderings
     */

    /// flags for oprimising were emited because we acces/iterate node_tree only
    ///     once for transfer data info vao/vbo/ebo/etc...

    std::cout << "START " << glfwGetTime() << std::flush;

    const aiScene *scene = importer_.ReadFile(
      path,
      aiProcess_Triangulate |
      aiProcess_GenUVCoords |
      aiProcess_GenSmoothNormals |
      aiProcess_CalcTangentSpace |
      aiProcess_FlipUVs |
      aiProcess_LimitBoneWeights |
      aiProcess_RemoveComponent |
      aiProcess_FindInvalidData
      // TODO: aiProcess_JoinIdenticalVertices is senseless because of aiProcess_GenSmoothNormals
    );

    std::cout << "\nEND " << glfwGetTime() << std::flush << "\n";

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
      std::cout /*<< Log(LogType::kFatal)*/ << "ERROR::ASSIMP:: " << importer_.GetErrorString();
      return {nullptr, 0, 0};
    }

    std::size_t meshes_num = scene->mNumMeshes;
    if (meshes_num == 0) return {nullptr, 0, 0}; // no work

    std::size_t path_size = 0;
    for (auto i = path; *i != '\0'; ++i) {
      ++path_size;
    }
    utility::Span<char> path_copy(path_size);
    std::memcpy(path_copy.get_data(), path, path_size);

    loader_.set_path(path);

    unsigned int global_id = 0;
    if (scene->mNumAnimations == 0) {
      skinless_objects_.push_back(loader_.LoadSkinless(scene));
      global_id = skinless_objects_.back()->get_global_id();
      details::model::ModelManager::loaded_files_->insert({path_copy, global_id});
      importer_.FreeScene();
      return skinless_objects_.back()->CreateInstance();
    } else {
      skinned_objects_.push_back(loader_.LoadSkinned(scene));
      global_id = skinned_objects_.back()->get_global_id();
      details::model::ModelManager::loaded_files_->insert({path_copy, global_id});
      importer_.FreeScene();
      return skinned_objects_.back()->CreateInstance();
    }
  }
}

// TODO: (rel to GetInterpolatedAnimationKey()) we couldn't skip nodes with
//      neither bone nor animation, because it can have more than 1 child and
//      those children should be "attached" not to parent but grandparent (then
//      grandparent has to previously find all such nodes to create
//      utility::Span with enough space)

/// should be called for each animation
AnimationNode* ModelLoader::GetInterpolatedAnimationKey(const aiAnimation* animation,
                                                        const aiNode* node) {
  aiString node_name(node->mName.data);
  glm::mat4 node_transform(AssimpMatrixToGlm(node->mTransformation));
  const aiNodeAnim* ai_node_anim = GetNodeAnimation(animation, node_name);
  bool has_animation = static_cast<bool>(ai_node_anim);
  auto founded_bone = bones_->find(node_name);
  bool is_bone = founded_bone != bones_->end();
  /**
   * if has_anim & is_bone --> default;
   * if !has_anim & is_bone --> bone with animations_.get_size() == 0
   * if has_anim & !is_bone --> dummy-bone with animations_
   * if !has_anim & !is_bone --> dummy-bone with animations_.get_size() == 0
   * */
  AnimationNode* anim_node = nullptr;
  std::size_t bone_id = 0;
  glm::mat4 bone_offset_matrix = glm::mat4(1.0f);
  if (has_animation && is_bone) {
    bone_id = BoneIdByName(node_name);
    bone_offset_matrix = founded_bone->second.offset_matrix;
  } else if (!(has_animation || is_bone)) {
    bone_id = bones_->size() - 1; // dummy bone (not used)
    bone_offset_matrix = glm::mat4(1.0f);
  } else if (has_animation) {
    bone_id = bones_->size() - 1; // dummy bone (not used)
    bone_offset_matrix = glm::mat4(1.0f);
  } else /** is_bone == true */ {
    bone_id = BoneIdByName(node_name);
    bone_offset_matrix = founded_bone->second.offset_matrix;
  }

  int children_num = 0;
  for (int i = 0 ; i < node->mNumChildren ; ++i) {
//    if (important_nodes_->find(&*node->mChildren[i]) != important_nodes_->end())
      ++children_num;
  }
  anim_node = new AnimationNode(ai_node_anim, bone_offset_matrix, node_transform, bone_id, children_num);
  std::size_t cur_pos = 0;
  for (int i = 0 ; i < node->mNumChildren ; ++i) {
//    if (important_nodes_->find(&*node->mChildren[i]) != important_nodes_->end()) {
    anim_node->InsertNode(cur_pos++, GetInterpolatedAnimationKey(animation, node->mChildren[i]));
//    }
  }
  return anim_node;
}

bool ModelLoader::NodeImportanceCheck(const aiNode* node, const aiScene* scene) {
  bool important = false;
  for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
    important |= (scene->mMeshes[node->mMeshes[i]]->mNumBones != 0);
  }
  for (unsigned int i = 0; i < node->mNumChildren; ++i) {
    important |= NodeImportanceCheck(node->mChildren[i], scene);
  }
  if (important) {
    important_nodes_->insert(&*node);
    //std::cout << "Important +1" << std::endl;
  } else {
    //std::cout << "NOT important" << std::endl;
  }
  return important;
}

const aiNodeAnim* ModelLoader::GetNodeAnimation(const aiAnimation* animation, const aiString& node_name) {
  for (uint i = 0 ; i < animation->mNumChannels ; i++) {
    const aiNodeAnim* node_anim = animation->mChannels[i];
    // TODO: give each node ID so now we could compare by id
    //    OR compare by pointer
    if (node_anim->mNodeName == node_name) {
      return node_anim;
    }
  }
  return nullptr;
}


MultimeshObject3DImpl* ModelLoader::LoadSkinless(const aiScene* scene) {
  std::size_t meshes_num = scene->mNumMeshes;
  processed_meshes_ = utility::Span<Mesh*>(meshes_num);
  unprocessed_meshes_ = utility::Span<UnprocessedMesh>(meshes_num);
  meshes_cur_id_ = 0;
  ProcessNode(scene->mRootNode, scene);
  InitCategory();

  auto new_object = new MultimeshObject3DImpl;
  new_object->Configurate1(meshes_num);

  cur_vao_ = new_object->get_vao();
  cur_vbo_ = new_object->get_vbo();
  cur_ebo_ = new_object->get_ebo();

  for (const UnprocessedMesh& m : unprocessed_meshes_)
    processed_meshes_[m.id] = ProcessSkinlessMesh(&m);

  new_object->Configurate2(std::move(processed_meshes_), category_);
  return new_object;
}

ModelLoader::ModelLoader() {
  bones_ = new std::map<aiString, Bone, AiStringComparator>;
  important_nodes_ = new std::set<const aiNode*, AiNodeComparator>;
}

SkinnedObject3DImpl* ModelLoader::LoadSkinned(const aiScene* scene) {
  meshes_cur_id_ = 0;
  bones_->clear();
  important_nodes_->clear();
  std::size_t meshes_num = scene->mNumMeshes;
  processed_meshes_ = utility::Span<Mesh*>(meshes_num);
  unprocessed_meshes_ = utility::Span<UnprocessedMesh>(meshes_num);

  ProcessNode(scene->mRootNode, scene);
  InitCategory();
  global_inverse_transform_ = glm::inverse(AssimpMatrixToGlm(scene->mRootNode->mTransformation));

  NodeImportanceCheck(scene->mRootNode, scene);

  auto new_object = new SkinnedObject3DImpl;
  new_object->Configurate1(meshes_num);

  cur_vao_ = new_object->get_vao();
  cur_vbo_ = new_object->get_vbo();
  cur_ebo_ = new_object->get_ebo();

  for (const UnprocessedMesh& m : unprocessed_meshes_)
    processed_meshes_[m.id] = ProcessSkinnedMesh(&m);

  if (bones_->empty()) {
    std::cout << "There's no bones" << std::endl;
    // TODO: need optimization
  }
  /// dummy bone for nodes which don't have nodes but have animations and bones in children nodes
  bones_->insert({aiString{}, {glm::mat4(1.0f), bones_->size()}}); // TODO: emplace?
  assert(bones_->size() < 200); // TODO: max amount of bones = 200

  animation_nodes_ = utility::Span<AnimationNode*>(scene->mNumAnimations);
  // TODO: may be parallelized
  for (int i = 0; i < scene->mNumAnimations; ++i)
    animation_nodes_[i] = GetInterpolatedAnimationKey(scene->mAnimations[i], scene->mRootNode);

  new_object->Configurate2(std::move(processed_meshes_), category_, bones_->size(),
                           std::move(animation_nodes_), global_inverse_transform_);
  return new_object;
}

/// may be: default; with_height; with_height_parallax
void ModelLoader::InitCategory() {
  bool has_height_map = false;
  for (auto& m : unprocessed_meshes_) {
    if ((m.material.tex_normal_ != 0) && (m.material.tex_height_ != 0)) {
      category_ = ObjectRenderCategory::kHeightParallaxMap;
      return;
    } else if (m.material.tex_normal_ != 0) {
      has_height_map = true;
    }
  }
  if (has_height_map)
    category_ = ObjectRenderCategory::kHeightMap;
}

void ModelLoader::ProcessNode(const aiNode *node, const aiScene *scene) {
  for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
    const aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    Material mesh_material = InitMeshMaterials(scene, mesh);
    unprocessed_meshes_[meshes_cur_id_] = {mesh, mesh_material, meshes_cur_id_};
    ++meshes_cur_id_;
  }
  for (unsigned int i = 0; i < node->mNumChildren; ++i) {
    ProcessNode(node->mChildren[i], scene);
  }
}

/* TODO:
 *  rewrite ProcessSkinlessMesh, ProcessSkinnedMesh
 *  making vertices EBO-friendly
 *  IDEA:
 *  // Load the mesh with normal generation
 *  mesh = aiImportFile("your_model.obj", aiProcess_GenNormals);
 *  // Perform manual vertex duplication if needed
 *  manualVertexDuplication(mesh);
 *  // Create EBO
 *  createElementBufferObject(mesh);
 *  // Render the mesh
 *  renderMesh(mesh);
 *
 *  EXAMPLE:
#include <algorithm>
#include <unordered_map>
#include <vector>

// Structure to represent a vertex with position and normal
struct Vertex {
    float x, y, z;
    float nx, ny, nz;
};

// Custom hash function for unordered_map to check vertex equality
struct VertexHash {
    std::size_t operator()(const Vertex& v) const {
        return std::hash<float>()(v.x) ^ std::hash<float>()(v.y) ^ std::hash<float>()(v.z);
    }
};

void manualVertexDuplication(aiMesh* mesh) {
    std::unordered_map<Vertex, unsigned int, VertexHash> vertexMap;
    std::vector<Vertex> duplicatedVertices;
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        aiFace& face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            unsigned int originalIndex = face.mIndices[j];
            Vertex originalVertex = {
                mesh->mVertices[originalIndex].x,
                mesh->mVertices[originalIndex].y,
                mesh->mVertices[originalIndex].z,
                mesh->mNormals[originalIndex].x,
                mesh->mNormals[originalIndex].y,
                mesh->mNormals[originalIndex].z
            };
            // Check if the vertex is already in the map
            auto it = vertexMap.find(originalVertex);
            if (it == vertexMap.end()) {
                unsigned int newIndex = static_cast<unsigned int>(duplicatedVertices.size());
                vertexMap[originalVertex] = newIndex;
                duplicatedVertices.push_back(originalVertex);
            }
        }
    }
    mesh->mNumVertices = static_cast<unsigned int>(duplicatedVertices.size());
    delete[] mesh->mVertices;
    delete[] mesh->mNormals;
    mesh->mVertices = new aiVector3D[mesh->mNumVertices];
    mesh->mNormals = new aiVector3D[mesh->mNumVertices];

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        mesh->mVertices[i] = aiVector3D(duplicatedVertices[i].x, duplicatedVertices[i].y, duplicatedVertices[i].z);
        mesh->mNormals[i] = aiVector3D(duplicatedVertices[i].nx, duplicatedVertices[i].ny, duplicatedVertices[i].nz);
    }
}

 *
 *
 *
 * */



Mesh* ModelLoader::ProcessSkinlessMesh(const UnprocessedMesh *mesh) {
  const aiMesh* ai_mesh = mesh->mesh;
  // TODO: NOT std::vector, BUT utility::Span
  std::vector<Mesh::Vertex> vertices;
  vertices.reserve(ai_mesh->mNumVertices);
  std::vector<unsigned int> indices;

  for (unsigned int i = 0; i < ai_mesh->mNumVertices; ++i) {
    Mesh::Vertex vertex;
    vertex.position = {ai_mesh->mVertices[i].x, ai_mesh->mVertices[i].y, ai_mesh->mVertices[i].z};
    //vertex.normal = {ai_mesh->mNormals[i].x, ai_mesh->mNormals[i].y, ai_mesh->mNormals[i].z};
    //vertex.tangent = {ai_mesh->mTangents[i].x, ai_mesh->mTangents[i].y, ai_mesh->mTangents[i].z};
    //vertex.bitangent = {ai_mesh->mBitangents[i].x, ai_mesh->mBitangents[i].y, ai_mesh->mBitangents[i].z};
    if (ai_mesh->mTextureCoords[0]) {
      vertex.tex_coords = {ai_mesh->mTextureCoords[0][i].x, ai_mesh->mTextureCoords[0][i].y};
    } else {
      vertex.tex_coords = {0.0f, 0.0f};
    }
    vertices.push_back(vertex);
  }
  InitMeshIndices(ai_mesh, indices);

  return new Mesh(std::move(vertices), std::move(indices), mesh->material,
                  cur_vao_+mesh->id, cur_vbo_+mesh->id, cur_ebo_+mesh->id);
}

SkinnedMesh* ModelLoader::ProcessSkinnedMesh(const UnprocessedMesh *mesh) {
  const aiMesh* ai_mesh = mesh->mesh;
  // TODO: NOT std::vector, BUT utility::Span
  std::vector<SkinnedMesh::SkinnedVertex> vertices;
  vertices.reserve(ai_mesh->mNumVertices);
  std::vector<unsigned int> indices;
  std::vector<VertexBoneInfo> vertex_bone_info(ai_mesh->mNumVertices);

  for (int i = 0; i < ai_mesh->mNumBones; ++i) {
    aiBone* bone = ai_mesh->mBones[i];
    bones_->insert({bone->mName, {AssimpMatrixToGlm(bone->mOffsetMatrix), bones_->size()}}); // TODO: emplace?
    // TODO: bones_->try_emplace(bone->mName, AssimpMatrixToGlm(bone->mOffsetMatrix), bones_->size());
    assert(bones_->size() < 200); // TODO: max amount of bones = 200
    for (int j = 0; j < bone->mNumWeights; ++j)
      vertex_bone_info[bone->mWeights[j].mVertexId].AddBone(
        BoneIdByName(bone->mName), bone->mWeights[j].mWeight);
  }

  for (unsigned int i = 0; i < ai_mesh->mNumVertices; ++i) {
    SkinnedMesh::SkinnedVertex vertex;
    vertex.position = {ai_mesh->mVertices[i].x, ai_mesh->mVertices[i].y, ai_mesh->mVertices[i].z};
    vertex.normal = {ai_mesh->mNormals[i].x, ai_mesh->mNormals[i].y, ai_mesh->mNormals[i].z};
    //vertex.tangent = {ai_mesh->mTangents[i].x, ai_mesh->mTangents[i].y, ai_mesh->mTangents[i].z};
    //vertex.bitangent = {ai_mesh->mBitangents[i].x, ai_mesh->mBitangents[i].y, ai_mesh->mBitangents[i].z};
    for (int j = 0; j < 4; ++j) {
      vertex.bone_ids[j] = vertex_bone_info[i].bone_info[j].id;
      vertex.bone_weights[j] = vertex_bone_info[i].bone_info[j].weight;
    }
    if (ai_mesh->mTextureCoords[0]) {
      // TODO: 1) compare with nullptr;
      //       2) other way to check existing of teture coords for model
      vertex.tex_coords = {ai_mesh->mTextureCoords[0][i].x, ai_mesh->mTextureCoords[0][i].y};
    } else {
      vertex.tex_coords = {0.0f, 0.0f};
    }
    vertices.push_back(vertex);
  }
  InitMeshIndices(ai_mesh, indices);
  return new SkinnedMesh(std::move(vertices), std::move(indices), mesh->material,
                         cur_vao_+mesh->id, cur_vbo_+mesh->id, cur_ebo_+mesh->id);
}

void ModelLoader::InitMeshIndices(const aiMesh *mesh, std::vector<unsigned int>& indices) {
  for (int i = 0; i < mesh->mNumFaces; ++i) {
    const aiFace& face = mesh->mFaces[i];
    for (int j = 0; j < face.mNumIndices; ++j)
      indices.push_back(face.mIndices[j]);
  }
}
// TODO: int / unsigned int / std::size_t

/// Currently there's no aiTextureType_AMBIENT_OCCLUSION
Material ModelLoader::InitMeshMaterials(const aiScene *scene, const aiMesh *mesh) {
  aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
  Material mesh_material;
  // only first texture of certain type from aiMaterial
  mesh_material.tex_albedo_ = LoadTexture(material, aiTextureType_BASE_COLOR);
  if (mesh_material.tex_albedo_ == 0)
    mesh_material.tex_albedo_ = LoadTexture(material, aiTextureType_DIFFUSE);

  mesh_material.tex_metallic_ = LoadTexture(material, aiTextureType_METALNESS);
  if (mesh_material.tex_metallic_ == 0)
    mesh_material.tex_metallic_ = LoadTexture(material, aiTextureType_SPECULAR);

  mesh_material.tex_roughness_ = LoadTexture(material, aiTextureType_DIFFUSE_ROUGHNESS);
  if (mesh_material.tex_roughness_ == 0)
    mesh_material.tex_roughness_ = LoadTexture(material, aiTextureType_SPECULAR);

  mesh_material.tex_normal_ = LoadTexture(material, aiTextureType_NORMALS);
  mesh_material.tex_height_ = LoadTexture(material, aiTextureType_HEIGHT);

  return mesh_material;
}

GLuint ModelLoader::LoadTexture(aiMaterial *mat, aiTextureType type) {
  aiString str;
  if (mat->GetTexture(type, 0, &str) == AI_FAILURE) {
    return 0;
  }
  //std::cout << "type " << type << std::endl;
  std::string rel_path(path_);
  rel_path = rel_path.substr(0, rel_path.find_last_of('/'));
  rel_path += '/';
  rel_path += str.C_Str();

  GLuint texture_id;
  if (true /*static_load_*/) {
    texture_id = Texture::InitLoad(rel_path.c_str())->get_id();
  } else {
    texture_id = Texture(rel_path.c_str()).get_id();
  }
  return texture_id;
}

std::size_t ModelLoader::BoneIdByName(const aiString& name) {
  // always found (managed by Dev)
  return bones_->find(name)->second.id;
}

// TODO: to namespace simurgh::utility
glm::mat4 AssimpMatrixToGlm(const aiMatrix4x4& aiMatrix) {
  glm::mat4 glmMatrix;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      glmMatrix[i][j] = aiMatrix[j][i];
    }
  }
  return glmMatrix;
}

void ModelSupervisor::Draw(ObjectRenderPhase phase) const {
  for (auto obj : skinless_objects_)
    obj->Draw(phase);
  for (auto obj : skinned_objects_)
    obj->Draw(phase);
}
void ModelSupervisor::RunAnimation(unsigned int global_id,
                                   unsigned int local_id,
                                   unsigned int anim_id,
                                   bool repeat) {
  for (auto obj : skinned_objects_) {
    if (obj->get_global_id() == global_id)
      obj->RunAnimation(local_id, anim_id, repeat);
  }
}

void ModelSupervisor::Update(double framerate) {
  for (auto obj : skinned_objects_)
    obj->Update(framerate);
}

} // namespace faithful
