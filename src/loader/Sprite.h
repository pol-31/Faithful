#ifndef FAITHFUL_SPRITE_H
#define FAITHFUL_SPRITE_H

#include <glm/glm.hpp>
#define GLFW_INCLUDE_NONE  // for arbitrary OpenGL functions including order
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "Image.h"
#include "../entities/Object.h"

#include <filesystem>

namespace faithful {

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
/// DEPRECATED & MOVED TO Faithful/utils/AssetProcessor
///                      (or .../AssetPreprocessor)
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
/*


class Texture;

class Sprite : public Image {//, public Object {
 public:
  enum class Rotation {
    CW,
    CCW,
    Flip,
    Mirrowed,
    CW_mirrowed,
    CCW_mirrowed,
    Flip_mirrowed
  };

  Sprite() = default;
  Sprite(const Sprite& sprite) : Image(sprite) {}
  Sprite(Sprite&& sprite) : Image(std::move(sprite)) {}

  Sprite& operator=(const Sprite& sprite) {
    Image::operator=(sprite);
  }
  Sprite& operator=(Sprite&& sprite) {
    Image::operator=(std::move(sprite));
  }

  Sprite(const char* path) : Image(path, ImageType::kTexture) {}
  Sprite(const Texture& tex);

  /// modifiers

  // TODO: Crop()                 | Sprite feature
  // TODO: glTexSubimage2D        | Sprite feature
  // TODO: Rotate()               | Sprite feature
  // TODO: as image: +opacity     | Sprite feature
  // TODO: coloring               | Sprite feature

  // there are: Sprite | 2Dobject

  void MinFilter(ImageFilter filter) {
    Image::MinFilter(filter);
  }
  void MagFilter(ImageFilter filter) {
    Image::MagFilter(filter);
  }
  void MipMapLevel(std::size_t level, const Image& img) {
    Image::MipMapLevel(level, img);
  }
  void MipMapLevel(std::size_t level, const char* path) {
    Image::MipMapLevel(level, path);
  }

  /// getters/setters

  bool Modified() const {
    return !data_.Empty();
  }

  GLuint get_id() const {
    return id_;
  }
  void set_id(GLuint id) {
    id_ = id;
  }

  utility::Span<unsigned char> get_data() const {
    return data_;
  }
  void set_data(utility::Span<unsigned char> data) {
    if (data == nullptr) {
      data_ = nullptr;
    } else data_ = data;
  }

  int get_width() const {
    return width_;
  }
  void set_width(int width) {
    width_ = width;
  }

  int get_height() const {
    return height_;
  }
  void set_height(int height) {
    height_ = height;
  }

  GLenum get_color_model() const {
    return color_model_;
  }
  void set_color_model(GLenum colorModel) {
    color_model_ = colorModel;
  }

  int get_channels() const {
    return channels_;
  }
  void set_channels(int channels) {
    channels_ = channels;
  }
};


class DefaultSprites {
 public:
  static void Init();
  static std::array<GLuint, sizeof(uint8_t)> ids_;
 private:
  static uint8_t ids_pos_;
  static bool initialized_;
  static std::filesystem::path dir_path_;
};
*/

}  // namespace faithful

#endif  // FAITHFUL_SPRITE_H
