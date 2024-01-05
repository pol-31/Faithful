#include "Objects_2d.h"

#include "../loader/Sprite.h"

namespace faithful {

/// ___________DEPRECATED_____________
/// __________________________________
/// __________________________________
/// __________________________________

/*

void InitObjects2D() {
  DefaultRectangleObject::Init();
  DefaultTriangleObject::Init();
  DefaultCircleObject::Init();
}

void Rectangle::DefaultInit() {
  vao_ = DefaultRectangleObject::vao_;
  vbo_ = DefaultRectangleObject::vbo_;
  image_id_ = DefaultRectangleObject::texture_id_;
  shader_program_ = DefaultRectangleObject::shader_program_;
  vertex_num_ = DefaultRectangleObject::vertex_num_;
  vertex_data_ = new VertexData[vertex_num_];
  std::memcpy(static_cast<void*>(vertex_data_),
              static_cast<void*>(DefaultRectangleObject::vertex_data_),
              sizeof(VertexData) * vertex_num_);
  start_vertex_positions_ = DefaultRectangleObject::start_vertex_position_;
}
void Triangle::DefaultInit() {
  vao_ = DefaultTriangleObject::vao_;
  vbo_ = DefaultTriangleObject::vbo_;
  image_id_ = DefaultTriangleObject::texture_id_;
  shader_program_ = DefaultTriangleObject::shader_program_;
  vertex_num_ = DefaultTriangleObject::vertex_num_;
  vertex_data_ = new VertexData[vertex_num_];
  std::memcpy(static_cast<void*>(vertex_data_),
              static_cast<void*>(DefaultTriangleObject::vertex_data_),
              sizeof(VertexData) * vertex_num_);
  start_vertex_positions_ = DefaultTriangleObject::start_vertex_position_;
}
void Circle::DefaultInit() {
  vao_ = DefaultCircleObject::vao_;
  vbo_ = DefaultCircleObject::vbo_;
  image_id_ = DefaultCircleObject::texture_id_;
  shader_program_ = DefaultCircleObject::shader_program_;
  vertex_num_ = DefaultCircleObject::vertex_num_;
  vertex_data_ = new VertexData[vertex_num_];
  std::memcpy(static_cast<void*>(vertex_data_),
              static_cast<void*>(DefaultCircleObject::vertex_data_),
              sizeof(VertexData) * vertex_num_);
  start_vertex_positions_ = DefaultCircleObject::start_vertex_position_;
}

void DefaultRectangleObject::Init() {
  if (initialized_) return;

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);

  DefaultTextures::Init();
  texture_id_ = DefaultSprites::ids_[1];

  utility::DefaultShaderProgram::Init();
  shader_program_ = utility::DefaultShaderProgram::object2d_;

  start_vertex_position_[0] = {-0.5f, -0.5f};
  start_vertex_position_[1] = {-0.5f, 0.5f};
  start_vertex_position_[2] = {0.5f, 0.5f};
  start_vertex_position_[3] = {0.5f, -0.5f};

  vertex_data_[0].point = {-0.5f, -0.5f};
  vertex_data_[1].point = {-0.5f, 0.5f};
  vertex_data_[2].point = {0.5f, 0.5f};
  vertex_data_[3].point = {0.5f, -0.5f};

  vertex_data_[0].text_coord = {0.0f, 0.0f};
  vertex_data_[1].text_coord = {0.0f, 1.0f};
  vertex_data_[2].text_coord = {1.0f, 1.0f};
  vertex_data_[3].text_coord = {1.0f, 0.0f};

  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, vertex_num_ *
sizeof(Object2D__not_complete::VertexData), vertex_data_, GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
sizeof(Object2D__not_complete::VertexData), (void *) 0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
sizeof(Object2D__not_complete::VertexData), (void *)
(sizeof(Object2D__not_complete::Point2D))); glEnableVertexAttribArray(1);

  initialized_ = true;
}

void DefaultTriangleObject::Init() {
  if (initialized_) return;

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);

  DefaultTextures::Init();
  texture_id_ = DefaultSprites::ids_[1];

  utility::DefaultShaderProgram::Init();
  shader_program_ = utility::DefaultShaderProgram::object2d_;

  start_vertex_position_[0] = {-0.5f, -0.5f};
  start_vertex_position_[1] = {0.0f, 0.5f};
  start_vertex_position_[2] = {0.5f, -0.5f};

  vertex_data_[0].point = {-0.5f, -0.5f};
  vertex_data_[1].point = {0.0f, 0.5f};
  vertex_data_[2].point = {0.5f, -0.5f};

  vertex_data_[0].text_coord = {0.0f, 0.0f};
  vertex_data_[1].text_coord = {0.5f, 1.0f};
  vertex_data_[2].text_coord = {1.0f, 0.0f};

  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, vertex_num_ *
sizeof(Object2D__not_complete::VertexData), vertex_data_, GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
sizeof(Object2D__not_complete::VertexData), (void *) 0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
sizeof(Object2D__not_complete::VertexData), (void *)
(sizeof(Object2D__not_complete::Point2D))); glEnableVertexAttribArray(1);

  initialized_ = true;
}

void DefaultCircleObject::Init() {
  if (initialized_) return;

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);

  DefaultTextures::Init();
  texture_id_ = DefaultSprites::ids_[1];

  utility::DefaultShaderProgram::Init();
  shader_program_ = utility::DefaultShaderProgram::object2d_;

  start_vertex_position_[0] = {-0.5f, 0.0f};
  start_vertex_position_[1] = {-0.45f, 0.2f};
  start_vertex_position_[2] = {-0.35f, 0.35f};
  start_vertex_position_[3] = {-0.2f, 0.45f};
  start_vertex_position_[4] = {0.0f, 0.5f};
  start_vertex_position_[5] = {0.2f, 0.45f};
  start_vertex_position_[6] = {0.35f, 0.35f};
  start_vertex_position_[7] = {0.45f, 0.2f};
  start_vertex_position_[8] = {0.5f, 0.0f};
  start_vertex_position_[9] = {0.45f, -0.2f};
  start_vertex_position_[10] = {0.35f, -0.35f};
  start_vertex_position_[11] = {0.2f, -0.45f};
  start_vertex_position_[12] = {0.0f, -0.5f};
  start_vertex_position_[13] = {-0.2f, -0.45f};
  start_vertex_position_[14] = {-0.35f, -0.35f};
  start_vertex_position_[15] = {-0.45f, -0.2f};

  vertex_data_[0].point = {-0.5f, 0.0f};
  vertex_data_[1].point = {-0.45f, 0.2f};
  vertex_data_[2].point = {-0.35f, 0.35f};
  vertex_data_[3].point = {-0.2f, 0.45f};
  vertex_data_[4].point = {0.0f, 0.5f};
  vertex_data_[5].point = {0.2f, 0.45f};
  vertex_data_[6].point = {0.35f, 0.35f};
  vertex_data_[7].point = {0.45f, 0.2f};
  vertex_data_[8].point = {0.5f, 0.0f};
  vertex_data_[9].point = {0.45f, -0.2f};
  vertex_data_[10].point = {0.35f, -0.35f};
  vertex_data_[11].point = {0.2f, -0.45f};
  vertex_data_[12].point = {0.0f, -0.5f};
  vertex_data_[13].point = {-0.2f, -0.45f};
  vertex_data_[14].point = {-0.35f, -0.35f};
  vertex_data_[15].point = {-0.45f, -0.2f};

  vertex_data_[0].text_coord = {0.0f, 0.5f};
  vertex_data_[1].text_coord = {0.2f, 0.7f};
  vertex_data_[2].text_coord = {0.35f, 0.85f};
  vertex_data_[3].text_coord = {0.45f, 0.95f};
  vertex_data_[4].text_coord = {0.5f, 1.0f};
  vertex_data_[5].text_coord = {0.55f, 0.95f};
  vertex_data_[6].text_coord = {0.65f, 0.85f};
  vertex_data_[7].text_coord = {0.8f, 0.7f};
  vertex_data_[8].text_coord = {1.0f, 0.5f};
  vertex_data_[9].text_coord = {0.8f, 0.3f};
  vertex_data_[10].text_coord = {0.65f, 0.15f};
  vertex_data_[11].text_coord = {0.55f, 0.05f};
  vertex_data_[12].text_coord = {0.5f, 0.0f};
  vertex_data_[13].text_coord = {0.45f, 0.05f};
  vertex_data_[14].text_coord = {0.35f, 0.15f};
  vertex_data_[15].text_coord = {0.2f, 0.3f};

  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, vertex_num_ *
sizeof(Object2D__not_complete::VertexData), vertex_data_, GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
sizeof(Object2D__not_complete::VertexData), (void *) 0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
sizeof(Object2D__not_complete::VertexData), (void *)
(sizeof(Object2D__not_complete::Point2D))); glEnableVertexAttribArray(1);

  initialized_ = true;
}


GLuint DefaultRectangleObject::vao_ = 0;
GLuint DefaultRectangleObject::vbo_ = 0;
GLuint DefaultRectangleObject::texture_id_ = 0;
utility::ShaderProgram *DefaultRectangleObject::shader_program_ = nullptr;
bool DefaultRectangleObject::initialized_ = false;
Object2D__not_complete::VertexData* DefaultRectangleObject::vertex_data_ =
  new Object2D__not_complete::VertexData[4];
std::size_t DefaultRectangleObject::vertex_num_ = 4;
Object2D__not_complete::Point2D* DefaultRectangleObject::start_vertex_position_
= new Object2D__not_complete::Point2D[4];

GLuint DefaultTriangleObject::vao_ = 0;
GLuint DefaultTriangleObject::vbo_ = 0;
GLuint DefaultTriangleObject::texture_id_ = 0;
utility::ShaderProgram *DefaultTriangleObject::shader_program_ = nullptr;
bool DefaultTriangleObject::initialized_ = false;
Object2D__not_complete::VertexData* DefaultTriangleObject::vertex_data_ =
  new Object2D__not_complete::VertexData[3];
std::size_t DefaultTriangleObject::vertex_num_ = 3;
Object2D__not_complete::Point2D* DefaultTriangleObject::start_vertex_position_ =
new Object2D__not_complete::Point2D[3];

GLuint DefaultCircleObject::vao_ = 0;
GLuint DefaultCircleObject::vbo_ = 0;
GLuint DefaultCircleObject::texture_id_ = 0;
utility::ShaderProgram *DefaultCircleObject::shader_program_ = nullptr;
bool DefaultCircleObject::initialized_ = false;
Object2D__not_complete::VertexData* DefaultCircleObject::vertex_data_ =
  new Object2D__not_complete::VertexData[16];
std::size_t DefaultCircleObject::vertex_num_ = 16;
Object2D__not_complete::Point2D* DefaultCircleObject::start_vertex_position_ =
new Object2D__not_complete::Point2D[16];
*/

}  // namespace faithful
