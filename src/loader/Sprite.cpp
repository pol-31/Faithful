#include "Sprite.h"

#include "Texture.h"

namespace faithful {


///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
/// DEPRECATED & MOVED TO Faithful/utils/AssetProcessor
///                      (or .../AssetPreprocessor)
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

/*

Sprite::Sprite(const Texture& tex) : Image(tex) {}


void DefaultSprites::Init() {
  if (initialized_) return;
  ids_.fill(0);
  for (const auto& entry : std::filesystem::directory_iterator(dir_path_)) {
    if (entry.is_regular_file()) {
      std::cout << entry.path() << std::endl;
      ids_[++ids_pos_] = Image::InitLoad(entry.path().c_str(), ImageType::kSprite);
    }
  }
  initialized_ = true;
}

bool DefaultSprites::initialized_ = false;
uint8_t DefaultSprites::ids_pos_ = 0;
std::array<GLuint, sizeof(uint8_t)> DefaultSprites::ids_ = std::array<GLuint, sizeof(uint8_t)>();
// precaution: path relative to cmake-build-debug
std::filesystem::path DefaultSprites::dir_path_ = "../assets/textures/sprites";

*/

} // namespace faithful
