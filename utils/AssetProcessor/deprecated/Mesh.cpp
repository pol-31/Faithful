#include "Mesh.h"

#include "Model.h"
#include "../../Executors/ThreadPoolCall.h"

namespace faithful {

Mesh::Mesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices,
           Material material, GLuint vao, GLuint vbo, GLuint ebo) {
  material_ = material;
  Setup(std::move(vertices), std::move(indices), vao, vbo, ebo);
  /*
  if (!static_load) {
    ImmediateCall{}.Do([=, vertices = std::move(vertices), indices =
  std::move(indices)]() mutable { this->Setup(vertices, indices, vao, vbo, ebo);
    });
  } else {
    Setup(std::move(vertices), std::move(indices), vao, vbo, ebo);
  }*/
}

// vertices/indices as r-value
void Mesh::Setup(std::vector<Vertex> vertices,
                 std::vector<unsigned int> indices, GLuint vao, GLuint vbo,
                 GLuint ebo) {
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
               vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, tex_coords));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, normal));

  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, tangent));

  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, bitangent));

  glBindVertexArray(0);
  index_num_ = indices.size();
}

SkinnedMesh::SkinnedMesh(std::vector<SkinnedVertex>&& vertices,
                         std::vector<unsigned int>&& indices, Material material,
                         GLuint vao, GLuint vbo, GLuint ebo) {
  material_ = material;
  Setup(std::move(vertices), std::move(indices), vao, vbo, ebo);
  /*if (!static_load) {
    ImmediateCall{}.Do([=, vertices = std::move(vertices), indices =
  std::move(indices)]() mutable { this->Setup(vertices, indices, vao, vbo, ebo);
    });
  } else {
  }*/
}

// vertices/indices as r-value
void SkinnedMesh::Setup(std::vector<SkinnedVertex> vertices,
                        std::vector<unsigned int> indices, GLuint vao,
                        GLuint vbo, GLuint ebo) {
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SkinnedVertex),
               vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex),
                        (void*)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex),
                        (void*)offsetof(SkinnedVertex, tex_coords));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex),
                        (void*)offsetof(SkinnedVertex, normal));

  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex),
                        (void*)offsetof(SkinnedVertex, tangent));

  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex),
                        (void*)offsetof(SkinnedVertex, bitangent));

  glEnableVertexAttribArray(5);
  glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex),
                        (void*)offsetof(SkinnedVertex, bone_weights));

  glEnableVertexAttribArray(6);
  glVertexAttribIPointer(6, 4, GL_UNSIGNED_INT, sizeof(SkinnedVertex),
                         (void*)offsetof(SkinnedVertex, bone_ids));

  glBindVertexArray(0);
  index_num_ = indices.size();
}

}  // namespace faithful
