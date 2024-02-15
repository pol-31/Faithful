#include "Texture1DPool.h"

#include <glad/glad.h>

namespace faithful {
namespace details {
namespace assets {

Texture1DPool::Texture1DPool() {
  GLuint opengl_ids[faithful::config::max_active_texture1d_num];
  glGenTextures(faithful::config::max_active_texture1d_num, opengl_ids);
  for (int i = 0; i < faithful::config::max_active_texture1d_num; ++i) {
    active_instances_[i].internal_id = opengl_ids[i];
    glBindTexture(GL_TEXTURE_1D, opengl_ids[i]);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }
  // TODO: load 1 default texture
}

Texture1DPool::~Texture1DPool() {
  GLuint opengl_ids[faithful::config::max_active_texture1d_num];
  for (int i = 0; i < faithful::config::max_active_texture1d_num; ++i) {
    opengl_ids[i] = active_instances_[i].internal_id;
  }
  glDeleteTextures(active_instances_.size(), opengl_ids);
}

AssetInstanceInfo Texture1DPool::Load(std::size_t size, float* data, int channels) {
  auto [texture_id, ref_counter, is_new_id] = Base::AcquireId("");
  if (texture_id == -1) {
    return {"", nullptr, default_id_, default_id_};
  } else if (!is_new_id) {
    return {"", ref_counter, active_instances_[texture_id].internal_id, texture_id};
  }

  GLenum channel_type;
  switch (channels) {
    case 1:
      channel_type = GL_RED;
      break;
    case 2:
      channel_type = GL_RG;
      break;
    case 3:
      channel_type = GL_RGB;
      break;
    case 4:
      channel_type = GL_RGBA;
      break;
    default:
      // log error
      channel_type = GL_RED;
  }
  glBindTexture(GL_TEXTURE_1D, active_instances_[texture_id].internal_id);
  glTexImage1D(GL_TEXTURE_1D, 0, GL_UNSIGNED_BYTE,
               size, 1, channel_type, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_1D);
  return {"", ref_counter, active_instances_[texture_id].internal_id, texture_id};
}

} // namespace assets
} // namespace details
} // namespace faithful
