#ifndef FAITHFUL_CUBEMAP_H
#define FAITHFUL_CUBEMAP_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../loader/Image.h"
#include "../loader/Texture.h"
#include "../loader/Sprite.h"
#include "../loader/ShaderProgram.h"

namespace faithful {

// TODO: SkyDome

class DefaultCubeMap {
 public:
  static void Init();

  static GLuint vao_;
  static GLuint vbo_;
  static GLuint texture_id_;
  static utility::ShaderProgram* shader_program_;

  static char* face_up_path_;
  static char* face_down_path_;
  static char* face_left_path_;
  static char* face_right_path_;
  static char* face_front_path_;
  static char* face_back_path_;

 private:
  static bool initialized_;
};

class CubeMap {
 public:
  enum class Face {
    kUp,
    kDown,
    kLeft,
    kRight,
    kFront,
    kBack,
    kAll,
    kSides
  };

  CubeMap() {
    DefaultInit();
  }
  CubeMap(const CubeMap& other) {
    DefaultInit();
  }
  CubeMap(CubeMap&& other) {
    DefaultInit();
  }

  CubeMap& operator=(const CubeMap& other) {
    DefaultInit();
  }
  CubeMap& operator=(CubeMap&& other) {
    DefaultInit();
  }

  CubeMap(const char* path);
  CubeMap(const char* path1, const char* path2, const char* path3,
          const char* path4, const char* path5, const char* path6);

  CubeMap(const Texture& tex);
  CubeMap(const Texture& tex1, const Texture& tex2, const Texture& tex3,
          const Texture& tex4, const Texture& tex5, const Texture& tex6);

  CubeMap(const Sprite& sp);
  CubeMap(const Sprite& sp1, const Sprite& sp2, const Sprite& sp3,
          const Sprite& sp4, const Sprite& sp5, const Sprite& sp6);

  CubeMap(glm::vec4 color);
  CubeMap(glm::vec4 color1, glm::vec4 color2, glm::vec4 color3,
          glm::vec4 color4, glm::vec4 color5, glm::vec4 color6);

  void SetFace(Face face_type, const char* path);
  void SetFace(Face face_type, const Sprite& sp);
  void SetFace(Face face_type, const Texture& tex);
  void setFace(Face face_type, glm::vec4 color);

  void LoadImageToOpenGL(GLuint face_type, Image* img);

  void Draw(glm::mat4 view, glm::mat4 projection) const;

  ~CubeMap();

 private:
  void DefaultInit(/* bool add_texture */) {

    DefaultCubeMap::Init();
    vao_ = DefaultCubeMap::vao_;
    vbo_ = DefaultCubeMap::vbo_;
    texture_id_ = DefaultCubeMap::texture_id_;
    shader_program_ = DefaultCubeMap::shader_program_;
  }
  // TODO: Image default __full_white_texture__

  void SetFaceImpl(Face face_type) {
    switch (face_type) {
      case Face::kUp:
        break;
      case Face::kDown:
        break;
      case Face::kLeft:
        break;
      case Face::kRight:
        break;
      case Face::kFront:
        break;
      case Face::kBack:
        break;
      case Face::kAll:
        break;
      case Face::kSides:
        break;
    }
  }

  // Image class members: data_, width_, height_,
  //   channels_, color_model_ are not using
  GLuint vao_ = 0;
  GLuint vbo_ = 0;
  GLuint texture_id_ = 0;
  utility::ShaderProgram *shader_program_ = nullptr;
};

} // namespace faithful

#endif // FAITHFUL_CUBEMAP_H
