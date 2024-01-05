#include "Objects_3d.h"

#include <glad/gl.h>

#include "../loader/Texture.h"
#include "../Engine.h"

namespace faithful {

/// ___________DEPRECATED_____________
/// __________________________________
/// __________________________________
/// __________________________________

/* DEPRECATED
 *
 * just delete it BUT instead of it create classes Cube, Sphere,
 * etc... which describes __global_id__
 *
 * for init-convenience: Cube cube1; Sphere sphere1;
 *         (not "Model" but internally is it)
 *
 * */

CubeCreator::CubeCreator() {
  Configurate1(36);

  GLfloat vertices[] = {
      0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.5f,  -0.5f, 0.5f,  0.0f, 0.0f,
      0.5f,  -0.5f, -0.5f, 1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
      0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f,

      -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, -0.5f, 0.5f,  0.5f,  0.0f, 1.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,

      0.5f,  0.5f,  0.5f,  0.0f, 1.0f, -0.5f, 0.5f,  0.5f,  1.0f, 1.0f,
      -0.5f, 0.5f,  -0.5f, 1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
      0.5f,  0.5f,  -0.5f, 0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 0.0f,

      0.5f,  -0.5f, 0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  0.0f, 1.0f,
      0.5f,  -0.5f, -0.5f, 0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,

      0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 1.0f,
      -0.5f, -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
      -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, -0.5f, 0.5f,  1.0f, 0.0f,

      0.5f,  0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 0.0f, 1.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
  };

  unsigned int indices[36];
  for (int i = 0; i < 36; ++i) indices[i] = i;

  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * sizeof(unsigned int),
               indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                        (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                        (void*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);
}

Cube::Cube() {
  std::tie(transform_, global_id_, local_id_) =
      SimurghManager::get_scene()->AddTrivial3D();
}

Model::Model(const char* path) {
  std::tie(transform_, global_id_, local_id_) =
      SimurghManager::get_scene()->AddModel(path);
}

void Model::RunAnimation(unsigned int anim_id, bool repeat) {
  SimurghManager::get_scene()->RunAnimation(global_id_, local_id_, anim_id,
                                            repeat);
}

Sprite3dCreator::Sprite3dCreator() {
  Configurate1(6);
  GLfloat vertices[] = {-0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,
                        0.5f,  0.0f,  1.0f,  0.5f, 0.5f, 0.5f,  1.0f,
                        1.0f,  0.5f,  -0.5f, 0.5f, 1.0f, 0.0f};
  unsigned int indices[] = {0, 1, 2, 0, 3, 2};

  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * sizeof(unsigned int),
               indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                        (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                        (void*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);
}

}  // namespace faithful
