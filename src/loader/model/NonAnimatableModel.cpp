#include "NonAnimatableModel.h"

#include <glm/gtc/type_ptr.hpp>

namespace faithful {

void NonAnimatableModel::DrawMeshes(
    const std::vector<glm::mat4>& transforms,
    ShaderProgram& shader_program, tinygltf::Mesh &mesh) {
  for (const glm::mat4& transform : transforms) {
    for (size_t i = 0; i < mesh.primitives.size(); ++i) {
      tinygltf::Primitive primitive = mesh.primitives[i];
      tinygltf::Accessor indexAccessor = data_->model.accessors[primitive.indices];

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                   data_->vbo_and_ebos.at(indexAccessor.bufferView));

      glDrawElements(primitive.mode, indexAccessor.count,
                     indexAccessor.componentType,
                     reinterpret_cast<void*>(indexAccessor.byteOffset));

      shader_program.SetUniformMat4v("transform", 1, GL_FALSE,
                                     glm::value_ptr(transform));
    }
  }
}

void NonAnimatableModel::DrawModelNodes(
    const std::vector<glm::mat4>& transforms,
    ShaderProgram& shader_program, tinygltf::Node &node) {
  if ((node.mesh >= 0) && (node.mesh < data_->model.meshes.size())) {
    DrawMeshes(transforms, shader_program, data_->model.meshes[node.mesh]);
  }
  for (size_t i = 0; i < node.children.size(); i++) {
    DrawModelNodes(transforms, shader_program, data_->model.nodes[node.children[i]]);
  }
}

void NonAnimatableModel::Draw(
    const std::vector<glm::mat4>& transforms, ShaderProgram& shader_program) {
  glBindVertexArray(data_->vao);

  const tinygltf::Scene &scene = data_->model.scenes[data_->model.defaultScene];
  for (size_t i = 0; i < scene.nodes.size(); ++i) {
    DrawModelNodes(transforms, shader_program, data_->model.nodes[scene.nodes[i]]);
  }

  shader_program.Bind();

  shader_program.SetUniform("tex_albedo", 0);
  shader_program.SetUniform("tex_emission", 1);
  shader_program.SetUniform("tex_metal_rough", 2);
  shader_program.SetUniform("tex_normal", 3);
  shader_program.SetUniform("tex_occlusion", 4);
  glActiveTexture(GL_TEXTURE0);
  data_->material.albedo.Bind();
  glActiveTexture(GL_TEXTURE1);
  data_->material.emission.Bind();
  glActiveTexture(GL_TEXTURE2);
  data_->material.metal_rough.Bind();
  glActiveTexture(GL_TEXTURE3);
  data_->material.normal.Bind();
  glActiveTexture(GL_TEXTURE4);
  data_->material.occlusion.Bind();

  glBindVertexArray(0);
}

}  // namespace faithful
