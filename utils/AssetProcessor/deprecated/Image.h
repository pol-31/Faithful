#ifndef FAITHFUL_IMAGE_H
#define FAITHFUL_IMAGE_H

#include <map>
#include <set>

#define GLFW_INCLUDE_NONE // for arbitrary OpenGL functions including order
#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <glm/glm.hpp>

#include "../../Span.h"
#include "LoadPathManager.h"

namespace faithful {

// TODO: anisotropic filtering
// TODO: rewrite texture_ops with OpenCL (gpu-programming)

class ImageIdComparator;
class Image;
class DefaultImage;
class Texture;
class CubeMap;


enum class ImageType {
  kTexture,
  kSprite
};

// TODO:
// TODO: comments (COMMENTS !!!!!!!!!! <<<--------------------------------------------------)
// TODO:

namespace details {
namespace image {

/// for reusing we should track each loaded image
class ImageManager : public LoadPathManager<Image*> {
 protected:
  friend class faithful::Image;
  static GLuint Register(const char *path, faithful::ImageType type);
  /// loaded_files_ find by path; stock_images_ find by id.
  static std::set<Image*, ImageIdComparator>* stock_images_;
};

} // namespace image
} // namespace details

enum class ImageFilter {
  kNearest,
  kLinear
};

class Image {
 protected:
  friend class details::image::ImageManager;
  friend class DefaultTextures;
  friend class DefaultSprites;
  friend class ImageIdComparator;
  friend class DefaultCubeMap;

  Image() {}
  Image(const Image& tex);
  Image(Image&& tex);

  Image& operator=(const Image& tex);
  Image& operator=(Image&& tex);

  Image(const char* path, ImageType type);

  ~Image();


  static GLuint InitLoad(const char* path, ImageType type);

  static Image* Load(const char* path);

  static void DefaultTextureConfig(GLuint id);
  static void DefaultSpriteConfig(GLuint id);
  void DefaultCubemapConfig();

  void DefaultConfig();

  static GLuint GenNewId();
  static void LoadImageToOpenGL(Image* img);
  static void DeleteImageOpenGL(GLuint id);

  /// modifiers

  void ApplyModification(bool RenderThread = false);

  void MinFilter(ImageFilter filter);
  void MagFilter(ImageFilter filter);
  void MipMapLevel(std::size_t level, const Image& img);
  void MipMapLevel(std::size_t level, const char* path);

  /// getters/setters

  bool Modified() const {
    return !data_.Empty();
  }

  ImageType get_type() const {
    return type_;
  }

  void set_type(ImageType type) {
    type_ = type;
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

  int get_channels() const {
    return channels_;
  }
  void set_channels(int channels) {
    channels_ = channels;
  }

  GLenum get_color_model() const {
    return color_model_;
  }
  void set_color_model(GLenum colorModel) {
    color_model_ = colorModel;
  }

  int width_ = 0;
  int height_ = 0;
  int channels_ = 0;
  GLenum color_model_;

  ImageType type_ = ImageType::kTexture;

  // if (data.Empty()) - id the same as is image and we don't need copy
  // BUT if we want to change image (resize/rotate/custom__glTexImage), then we making a copy
  utility::Span<unsigned char> data_;

  // used in derived classes: Texture, Sprite...
  GLuint id_ = 0;
};

struct ImageIdComparator {
  bool operator()(const Image* img1, const Image* img2) const {
    return img1->get_id() < img2->get_id();
  }
};


} // namespace faithful

#endif // FAITHFUL_IMAGE_H
