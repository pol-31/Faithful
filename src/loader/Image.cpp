
#include <iostream>
#define GLFW_INCLUDE_NONE // for arbitrary OpenGL functions including order
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "../../external/stb_image/stb_image.h"

#include "Image.h"
#include "../../utils/Span.h"
#include "../../utils/Allocator.h"

#include "../../utils/Executors/ThreadPoolCall.h"

#include <thread>

namespace faithful {

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
/// DEPRECATED & MOVED TO Faithful/utils/AssetProcessor
///                      (or .../AssetPreprocessor)
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
/*
namespace details {
namespace image {

GLuint ImageManager::Register(const char *path, ImageType type) {
  Image* img = Image::Load(path);
  GLuint id = Image::GenNewId();
  img->set_id(id);
  img->set_type(type);

  std::size_t path_size = 0;
  for (auto i = path; *i != '\0'; ++i) {
    ++path_size;
  }

  utility::Span<char> path_copy(path_size);
  std::memcpy(path_copy.get_data(), path, path_size);


  if (type == ImageType::kTexture) {
    details::image::ImageManager::loaded_files_->insert({path_copy, img});
    details::image::ImageManager::stock_images_->insert(img);
  } else {
    auto temp_img = new Image;
    GLuint temp_id;
    glGenTextures(1, &temp_id);
    glBindTexture(GL_TEXTURE_2D, temp_id);
    temp_img->set_data(img->get_data().MakeCopy());
    temp_img->set_type(ImageType::kTexture);
    temp_img->set_id(temp_id);
    details::image::ImageManager::loaded_files_->insert({path_copy,
                                                         temp_img});
    details::image::ImageManager::stock_images_->insert(temp_img);
    temp_img->DefaultConfig();
    Image::LoadImageToOpenGL(temp_img);
  }

  img->DefaultConfig();
  Image::LoadImageToOpenGL(img);
  return id;
}

std::set<Image*, ImageIdComparator>* ImageManager::stock_images_ =
  new std::set<Image*, ImageIdComparator>;

} // namespace image
} // namespace details

Image::Image(const Image& img) {
  // if data_ is non-modified, then data_ become also nullptr
  // otherwise: data copying
  data_ = img.get_data().MakeCopy();

  if (img.Modified()) {
    id_ = Image::GenNewId();
    DefaultConfig();
    Image::LoadImageToOpenGL(this);
  } else {
    id_ = img.get_id();
  }
  type_ = img.get_type();
  width_ = img.get_width();
  height_ = img.get_height();
  channels_ = img.get_channels();
  color_model_ = img.get_color_model();
}
Image::Image(Image&& img) {
  id_ = img.get_id();
  type_ = img.get_type();
  data_ = std::move(img.get_data());
  width_ = img.get_width();
  height_ = img.get_height();
  channels_ = img.get_channels();
  color_model_ = img.get_color_model();

  img.set_data(nullptr);
}

Image& Image::operator=(const Image& img) {
  // if data_ is non-modified, then data_ become also nullptr
  // otherwise: data copying
  data_ = img.get_data().MakeCopy();

  if (img.Modified()) {
    DeleteImageOpenGL(id_);
    id_ = Image::GenNewId();
    DefaultConfig();
    Image::LoadImageToOpenGL(this);
  } else {
    id_ = img.get_id();
  }
  type_ = img.get_type();
  width_ = img.get_width();
  height_ = img.get_height();
  channels_ = img.get_channels();
  color_model_ = img.get_color_model();
}
Image& Image::operator=(Image&& img) {
  if (img.Modified()) {
    DeleteImageOpenGL(id_);
  }
  id_ = img.get_id();
  type_ = img.get_type();
  data_ = std::move(img.get_data());
  width_ = img.get_width();
  height_ = img.get_height();
  channels_ = img.get_channels();
  color_model_ = img.get_color_model();

  img.set_data(nullptr);
}


GLuint Image::GenNewId() {
  GLuint id = ImmediateCall{}.Do([=](){
    GLuint new_id;
    glGenTextures(1, &new_id);
    return new_id;
  });
  return id;
}

void Image::LoadImageToOpenGL(Image* img) {
  auto copy_data = img->get_data().MakeCopy();
  SingleSemiDeferredCall{}.Do([=](){
    glBindTexture(GL_TEXTURE_2D, img->get_id());
    glTexImage2D(GL_TEXTURE_2D, 0, img->get_color_model(),
                 img->get_width(), img->get_height(), 0,
                 img->get_color_model(), GL_UNSIGNED_BYTE, copy_data.get_data());
    glGenerateMipmap(GL_TEXTURE_2D);
  });
}

Image::Image(const char* path, ImageType type) {
  if (path == nullptr) return;
  auto found_image = details::image::ImageManager::Find(path);
  if (found_image != nullptr) {
    auto reused_image = *found_image;
    if (type_ != reused_image->get_type()) {
      type_ = reused_image->get_type();
      DefaultConfig();
    }
    id_ = reused_image->get_id();
    data_ = nullptr;
    width_ = reused_image->get_width();
    height_ = reused_image->get_height();
    channels_ = reused_image->get_channels();
    color_model_ = reused_image->get_color_model();
    return;
  }
  type_ = type;

  /// between Find() and Register() other threads may intervene
  /// therefore we'll get redundant image copy in loaded_files_, BUt
  /// overhear emitted from blocking by CAS/spinlock/mutex not_worth_it

  id_ = details::image::ImageManager::Register(path, type);
}

Image* Image::Load(const char* path) {
  auto img = new Image();
  int width, height, channels, color_model;
  //stbi_set_flip_vertically_on_load(true);
  unsigned char *data = stbi_load(path, &width, &height, &channels, 0);
  if (data) {
    if (channels == 1)
      color_model = GL_RED;
    else if (channels == 3)
      color_model = GL_RGB;
    else if (channels == 4)
      color_model = GL_RGBA;
  } else {
    std::cout << "texture load error at: " << path << std::endl;
  }
  img->set_id(0);
  img->set_data({static_cast<std::size_t>(width * height * channels), data});
  img->set_width(width);
  img->set_height(height);
  img->set_channels(channels);
  img->set_color_model(color_model);

  return img;
}

Image::~Image() {
  if (Modified()) {
    details::image::ImageManager::stock_images_->erase(this);
    DeleteImageOpenGL(id_);
  }
}

void Image::DeleteImageOpenGL(GLuint id) {
  SingleSemiDeferredCall{}.Do([=](){
    glDeleteTextures(1, &id);
  });
}


void Image::DefaultConfig() {
  switch (type_) {
    case ImageType::kTexture:
      SingleSemiDeferredCall{}.Do([=](){ DefaultTextureConfig(id_); });
      break;
    case ImageType::kSprite:
      SingleSemiDeferredCall{}.Do([=](){ DefaultSpriteConfig(id_); });
      break;
  }
}

void Image::DefaultTextureConfig(GLuint id) {
  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Image::DefaultSpriteConfig(GLuint id) {
  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Image::ApplyModification(bool RenderThread) {
  Image dummy;
  dummy.set_id(id_);
  auto original_image =
    *(details::image::ImageManager::stock_images_->find(&dummy));
  data_ = (*original_image).get_data().MakeCopy();

  if (RenderThread) {
    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);
  } else {
    id_ = Image::GenNewId();
  }
  details::image::ImageManager::stock_images_->insert(this);
  Image::LoadImageToOpenGL(this);
}

void Image::MinFilter(ImageFilter filter) {
  ApplyModification();

  if (filter == ImageFilter::kNearest) {
    SingleSemiDeferredCall{}.Do([=]() {
      glBindTexture(GL_TEXTURE_2D, id_);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    });
  } else {
    SingleSemiDeferredCall{}.Do([=]() {
      glBindTexture(GL_TEXTURE_2D, id_);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    });
  }
}
void Image::MagFilter(ImageFilter filter) {
  ApplyModification();

  if (filter == ImageFilter::kNearest) {
    SingleSemiDeferredCall{}.Do([=]() {
      glBindTexture(GL_TEXTURE_2D, id_);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    });
  } else {
    SingleSemiDeferredCall{}.Do([=]() {
      glBindTexture(GL_TEXTURE_2D, id_);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    });
  }
}
void Image::MipMapLevel(std::size_t level, const Image& img) {
  ApplyModification();

  if (level == 0) return;
  auto data_copy = img.get_data().MakeCopy();
  SingleSemiDeferredCall{}.Do([=]() {
    glBindTexture(GL_TEXTURE_2D, id_);
    glTexImage2D(GL_TEXTURE_2D, level, img.get_color_model(),
                 img.get_width(), img.get_height(), 0,
                 img.get_color_model(), GL_UNSIGNED_BYTE, data_copy.get_data());
  });
}
void Image::MipMapLevel(std::size_t level, const char* path) {
  ApplyModification();

  if (level == 0) return;
  Image* img = Image::Load(path);
  SingleSemiDeferredCall{}.Do([=]() {
    glBindTexture(GL_TEXTURE_2D, id_);
    glTexImage2D(GL_TEXTURE_2D, level, img->get_color_model(),
                 img->get_width(), img->get_height(), 0,
                 img->get_color_model(), GL_UNSIGNED_BYTE, img->get_data().get_data());
  });
}

// initialized before main engine part inside the _Render_thread_
GLuint Image::InitLoad(const char* path, ImageType type) {
  std::size_t path_size = 0;
  for (auto i = path; *i != '\0'; ++i) {
    ++path_size;
  }

  GLuint id;
  auto reused_image = details::image::ImageManager::Find(path);
  if (reused_image != nullptr) {
    if ((*reused_image)->get_type() == type) {
      id = (*reused_image)->get_id();
      return id;
    } // else type is founded kSprite (below) or unfounded _type_
  }
  Image* target_img;

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  if (reused_image != nullptr) {
    target_img->set_data((*reused_image)->get_data().MakeCopy());
    DefaultSpriteConfig(id);
  } else {
    target_img = Image::Load(path);
    target_img->set_id(id);

    utility::Span<char> path_copy(path_size);
    std::memcpy(path_copy.get_data(), path, path_size);
    if (type == ImageType::kTexture) {
      details::image::ImageManager::loaded_files_->insert({path_copy,
                                                           target_img});
      details::image::ImageManager::stock_images_->insert(target_img);
    } else {
      auto temp_img = new Image;
      GLuint temp_id;
      glGenTextures(1, &temp_id);
      glBindTexture(GL_TEXTURE_2D, temp_id);
      temp_img->set_data(target_img->get_data().MakeCopy());
      DefaultTextureConfig(temp_id);
    }

    if (type == ImageType::kTexture) {
      DefaultTextureConfig(id);
    } else {
      DefaultSpriteConfig(id);
    }
  }
  auto data_copy = target_img->get_data().MakeCopy();
  glTexImage2D(GL_TEXTURE_2D, 0, target_img->get_color_model(),
               target_img->get_width(), target_img->get_height(), 0,
               target_img->get_color_model(), GL_UNSIGNED_BYTE,
               data_copy.get_data());
  glGenerateMipmap(GL_TEXTURE_2D);

  return id;
}*/

} // namespace faithful
