#ifndef FAITHFUL_MESH_H
#define FAITHFUL_MESH_H

#define GLFW_INCLUDE_NONE  // for arbitrary OpenGL functions including order
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/scene.h>

#include <string>
#include <vector>

#include "Image.h"

namespace faithful {

class ModelLoader;

struct Material {
  GLuint tex_albedo_ = 1;     // TOOD: set on default (grey?)
  GLuint tex_roughness_ = 0;  // TOOD: set on default (mid)
  GLuint tex_metallic_ = 0;   // TOOD: set on default (mid)
  GLuint tex_normal_ = 0;
  GLuint tex_height_ = 0;
  GLuint padding1_for_further_tex_ = 0;  // AO?
  GLuint padding2_for_further_tex_ = 0;  // Emission?
};

struct UnprocessedMesh {
  const aiMesh* mesh;
  Material material;
  unsigned int id;
};

class Mesh {
 public:
  struct Vertex {
    glm::vec3 position;
    glm::vec2 tex_coords;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
  };

  Mesh() = default;
  Mesh(const Mesh&) = default;
  Mesh(Mesh&&) = default;
  Mesh& operator=(const Mesh&) = default;
  Mesh& operator=(Mesh&&) = default;

  // indices and vertices as r-value Span<T>
  Mesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices,
       Material material, GLuint vao, GLuint vbo, GLuint ebo);

  // vertices/indices as r-value
  void Setup(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
             GLuint vao, GLuint vbo, GLuint ebo);

  int get_index_num() {
    return index_num_;
  }
  Material get_material() {
    return material_;
  }

 protected:
  Material material_;
  int index_num_ = 0;
};

class SkinnedMesh : public Mesh {
 public:
  struct SkinnedVertex {
    glm::vec3 position;
    glm::vec2 tex_coords;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    glm::vec4 bone_weights;
    glm::ivec4 bone_ids;
  };

  SkinnedMesh() = default;
  SkinnedMesh(const SkinnedMesh&) = default;
  SkinnedMesh(SkinnedMesh&&) = default;
  SkinnedMesh& operator=(const SkinnedMesh&) = default;
  SkinnedMesh& operator=(SkinnedMesh&&) = default;

  // indices and vertices as r-value Span<T>
  SkinnedMesh(std::vector<SkinnedVertex>&& vertices,
              std::vector<unsigned int>&& indices, Material material,
              GLuint vao, GLuint vbo, GLuint ebo);

  // vertices/indices as r-value
  void Setup(std::vector<SkinnedVertex> vertices,
             std::vector<unsigned int> indices, GLuint vao, GLuint vbo,
             GLuint ebo);
};

}  // namespace faithful

#endif  // FAITHFUL_MESH_H
