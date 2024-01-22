#ifndef FAITHFUL_TEXTURE_H
#define FAITHFUL_TEXTURE_H

#include <glad/gl.h>

#include "Image.h"

#include <filesystem>
#include <iostream> // todo: replace by Logger
#include <string>

namespace faithful {

/* We need manager with info about how relevant OpenGL_img_id is, so
 * when need to load new texture then just reuse them.
 *
 * BUT we update relevancy only if we need new texture
 * AND how we do this:
 * assume we are at point (0;0), then with regard to:
 * - time
 * - position
 * - act (part of the game)
 * Then we get list of relevant textures for current position
 * Then iterating through active textures and if it in list - save, mark as useless otherwise
 *
 * How we know where we need new texture:
 * - time
 * - position (divide all map to regions --> like bucket sort)
 * (cache included)
 *
 * */

/* How loading occurs:
 * @params: OpenGL id = cur_id, int mode = 1d / 2d
 * 1) delete previous data from cur_id (if needed)
 * 2) ifdef FAITHFUL_OPENGL_SUPPORT_ASTC
 * 3) read file, parse header
 * 4) decompress (if astc not supported)
 * 5) load to cur_id
 * */

class TextureManager {
 protected:
  friend class Texture;
  // friend class FaithfulCoreEngine;

  TextureManager() = default;

  bool Init() {
    if (initialized_) {
      return true;
    }
    // we have previously defined __num__ of textures
    // call glGenTextures for __num__ textures
    initialized_ = true;
    return true;
  }

  int Acquire() {
    // Is it blocking ?
  }

  void Release(int id) {
    // check all assets for relevancy,
    // give id of irrelevant texture
  }

 private:
  bool initialized_ = false;
  // pool of id
};

class Texture : public Image {
 public:
  enum class Type {
    kDiffuse,
    kSpecular,
    kNormal,
    kHeight
  };

  Texture(std::nullptr_t) {
    std::cerr << "Invalid TextureManager" << std::endl;
    abort();
  }

  Texture(TextureManager* manager) : manager_(manager) {
  }

  void Activate() {
    active_ = true;
    opengl_id_ = manager_->Acquire();
  }

  void Deactivate() {
    active_ = false;
    manager_->Release(opengl_id_);
    opengl_id_ = -1;
  }

 private:
  TextureManager* manager_ = nullptr;

  std::string file_path_;
  int global_id_;
  int opengl_id_;
  bool active_ = false;
  // properties (ldr/hdr/nmap deduce only once, reuse after)
};


}  // namespace faithful

#endif  // FAITHFUL_TEXTURE_H
