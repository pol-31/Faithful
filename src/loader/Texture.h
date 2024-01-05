#ifndef FAITHFUL_TEXTURE_H
#define FAITHFUL_TEXTURE_H

#define GLFW_INCLUDE_NONE  // for arbitrary OpenGL functions including order
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "Image.h"

#include <array>
#include <filesystem>

namespace faithful {

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
/// DEPRECATED & MOVED TO Faithful/utils/AssetProcessor
///                      (or .../AssetPreprocessor)
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

/*
class Sprite;

class Texture : public Image {
 public:
  enum class Type {
    kDiffuse,
    kSpecular,
    kNormal,
    kHeight
  };

  enum class WrapType {
    kClampToEdge,
    kClampToBorder,
    kMirroredRepeat,
    kRepeat
  };

  Texture() = default;
  Texture(const Texture& tex) : Image(tex) {}
  Texture(Texture&& tex) : Image() {
    id_ = tex.get_id();
    type_ = tex.get_type();
    data_ = std::move(tex.get_data());
    width_ = tex.get_width();
    height_ = tex.get_height();
    channels_ = tex.get_channels();
    color_model_ = tex.get_color_model();

    tex.set_data(nullptr);
  }

  Texture& operator=(const Texture& tex) {
    Image::operator=(tex);
  }
  Texture& operator=(Texture&& tex) {
    Image::operator=(std::move(tex));
  }

  Texture(const char* path)
      : Image(path, ImageType::kTexture) {}
  Texture(const Sprite& sprite);

  static Texture* InitLoad(const char* path) {
    auto texture = new Texture();
    texture->set_id(Image::InitLoad(path, ImageType::kTexture));
  }

  /// modifiers

  void WrapS(WrapType type);
  void WrapT(WrapType type);
  void WrapR(WrapType type);

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

class DefaultTextures {
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

#endif  // FAITHFUL_TEXTURE_H
