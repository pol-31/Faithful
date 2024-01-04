#include "CubeMap.h"

#include "../../utils/Executors/ThreadPoolCall.h"
#include "../../external/stb_image/stb_image.h"

namespace faithful {

/*

CubeMap::CubeMap(const CubeMap& other) {}
CubeMap::CubeMap(CubeMap&& other) {}

CubeMap& CubeMap::operator=(const CubeMap& other) {}
CubeMap& CubeMap::operator=(CubeMap&& other) {}
*/

CubeMap::CubeMap(const char* path) {}
CubeMap::CubeMap(
  const char* path1, const char* path2, const char* path3,
  const char* path4, const char* path5, const char* path6) {}

CubeMap::CubeMap(const Texture& tex) {}
CubeMap::CubeMap(
  const Texture& tex1, const Texture& tex2, const Texture& tex3,
  const Texture& tex4, const Texture& tex5, const Texture& tex6) {}

CubeMap::CubeMap(const Sprite& sp) {}
CubeMap::CubeMap(
  const Sprite& sp1, const Sprite& sp2, const Sprite& sp3,
  const Sprite& sp4, const Sprite& sp5, const Sprite& sp6) {}

CubeMap::CubeMap(glm::vec4 rgba) {}

void CubeMap::Draw(glm::mat4 view, glm::mat4 projection) const {
  glDepthMask(GL_FALSE);

  glDepthFunc(GL_LEQUAL);

  shader_program_->Use();

  shader_program_->AdjustMat4v("view", 1, GL_FALSE,
                               glm::value_ptr(glm::mat4(glm::mat3(view))));
  shader_program_->AdjustMat4v("projection", 1, GL_FALSE,
                               glm::value_ptr(projection));

  glBindVertexArray(vao_);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glDepthMask(GL_TRUE);

  glDepthFunc(GL_LESS);
}


CubeMap::~CubeMap() {
  // TODO: delete buffers
}

void DefaultCubeMap::Init() {
  if (initialized_) return;
  initialized_ = true;

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);

  int width, height, channels, color_model;

  stbi_set_flip_vertically_on_load(false);
  unsigned char *data = stbi_load(face_right_path_, &width,
                                  &height, &channels, 0);

  if (channels == 1)
    color_model = GL_RED;
  else if (channels == 3)
    color_model = GL_RGB;
  else if (channels == 4)
    color_model = GL_RGBA;

  glGenTextures(1, &texture_id_);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_);

  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 0, 0, color_model,
               width, height, 0, color_model, GL_UNSIGNED_BYTE, data);
  unsigned char *data1 = stbi_load(face_left_path_, &width, &height,
                                   &channels, 0);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, color_model,
               width, height, 0, color_model, GL_UNSIGNED_BYTE, data1);
  unsigned char *data2 = stbi_load(face_up_path_, &width, &height,
                                   &channels, 0);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, color_model,
               width, height, 0, color_model, GL_UNSIGNED_BYTE, data2);
  unsigned char *data3 = stbi_load(face_down_path_, &width, &height,
                                   &channels, 0);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, color_model,
               width, height, 0, color_model, GL_UNSIGNED_BYTE, data3);
  unsigned char *data4 = stbi_load(face_front_path_, &width, &height,
                                   &channels, 0);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, color_model,
               width, height, 0, color_model, GL_UNSIGNED_BYTE, data4);
  unsigned char *data5 = stbi_load(face_back_path_, &width, &height,
                                   &channels, 0);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, color_model,
               width, height, 0, color_model, GL_UNSIGNED_BYTE, data5);

  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  utility::DefaultShaderProgram::Init();
  shader_program_ = utility::DefaultShaderProgram::cubemap_;

  GLfloat vertices[] = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
  };

  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                        3 * sizeof(GLfloat), (void *) 0);
  glEnableVertexAttribArray(0);

}

GLuint DefaultCubeMap::vao_ = 0;
GLuint DefaultCubeMap::vbo_ = 0;
GLuint DefaultCubeMap::texture_id_ = 0;
utility::ShaderProgram* DefaultCubeMap::shader_program_ = nullptr;
bool DefaultCubeMap::initialized_ = false;


char* DefaultCubeMap::face_up_path_ =
  "../resources/default_cubemap/top.jpg";
char* DefaultCubeMap::face_down_path_ =
  "../resources/default_cubemap/bottom.jpg";
char* DefaultCubeMap::face_left_path_ =
  "../resources/default_cubemap/left.jpg";
char* DefaultCubeMap::face_right_path_ =
  "../resources/default_cubemap/right.jpg";
char* DefaultCubeMap::face_front_path_ =
  "../resources/default_cubemap/front.jpg";
char* DefaultCubeMap::face_back_path_ =
  "../resources/default_cubemap/back.jpg";

} // namespace faithful
