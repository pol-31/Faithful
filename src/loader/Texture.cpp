#include "Texture.h"

#define GLFW_INCLUDE_NONE // for arbitrary OpenGL functions including order
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "../../utils/Executors/ThreadPoolCall.h"
#include "Sprite.h"

namespace faithful {

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
/// DEPRECATED & MOVED TO Faithful/utils/AssetProcessor
///                      (or .../AssetPreprocessor)
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
/*

Texture::Texture(const Sprite& sprite) : Image(sprite) {}

void Texture::WrapS(WrapType wrap_type) {
  ApplyModification();

  switch (wrap_type) {
    case WrapType::kClampToBorder:
      SingleSemiDeferredCall{}.Do([=]() {
        glBindTexture(GL_TEXTURE_2D, id_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
      });
      break;
    case WrapType::kClampToEdge:
      SingleSemiDeferredCall{}.Do([=]() {
        glBindTexture(GL_TEXTURE_2D, id_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      });
      break;
    case WrapType::kMirroredRepeat:
      SingleSemiDeferredCall{}.Do([=]() {
        glBindTexture(GL_TEXTURE_2D, id_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
      });
      break;
    case WrapType::kRepeat:
      SingleSemiDeferredCall{}.Do([=]() {
        glBindTexture(GL_TEXTURE_2D, id_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      });
      break;
  }
}
void Texture::WrapT(WrapType wrap_type) {
  ApplyModification();

  switch (wrap_type) {
    case WrapType::kClampToBorder:
      SingleSemiDeferredCall{}.Do([=]() {
        glBindTexture(GL_TEXTURE_2D, id_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
      });
      break;
    case WrapType::kClampToEdge:
      SingleSemiDeferredCall{}.Do([=]() {
        glBindTexture(GL_TEXTURE_2D, id_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      });
      break;
    case WrapType::kMirroredRepeat:
      SingleSemiDeferredCall{}.Do([=]() {
        glBindTexture(GL_TEXTURE_2D, id_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
      });
      break;
    case WrapType::kRepeat:
      SingleSemiDeferredCall{}.Do([=]() {
        glBindTexture(GL_TEXTURE_2D, id_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      });
      break;
  }
}
void Texture::WrapR(WrapType wrap_type) {
  ApplyModification();

  switch (wrap_type) {
    case WrapType::kClampToBorder:
      SingleSemiDeferredCall{}.Do([=]() {
        glBindTexture(GL_TEXTURE_2D, id_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
      });
      break;
    case WrapType::kClampToEdge:
      SingleSemiDeferredCall{}.Do([=]() {
        glBindTexture(GL_TEXTURE_2D, id_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
      });
      break;
    case WrapType::kMirroredRepeat:
      SingleSemiDeferredCall{}.Do([=]() {
        glBindTexture(GL_TEXTURE_2D, id_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
      });
      break;
    case WrapType::kRepeat:
      SingleSemiDeferredCall{}.Do([=]() {
        glBindTexture(GL_TEXTURE_2D, id_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
      });
      break;
  }
}

void DefaultTextures::Init() {
  if (initialized_) return;
  ids_.fill(0);
  for (const auto& entry : std::filesystem::directory_iterator(dir_path_)) {
    if (entry.is_regular_file()) {
      std::cout << entry.path() << std::endl;
      ids_[++ids_pos_] = (Image::InitLoad(entry.path().c_str(), ImageType::kTexture));
    }
  }
  initialized_ = true;
}

bool DefaultTextures::initialized_ = false;
uint8_t DefaultTextures::ids_pos_ = 0;
std::array<GLuint, sizeof(uint8_t)> DefaultTextures::ids_ = std::array<GLuint, sizeof(uint8_t)>();
// precaution: path relative to cmake-build-debug
std::filesystem::path DefaultTextures::dir_path_ = "../assets/textures";
*/

} // namespace faithful
