#include "Texture.h"

#include <fstream>
#include <iostream> // todo: replace by Logger

#include <glad/glad.h>

#include "../config/AssetFormats.h"


namespace faithful {

TextureManager::TextureManager() {
  // TODO:
  //  1) call glGenTextures for __num__ textures
  //  2) load 1 default texture
  //  3) init free_instances_ with indices: 1,2,3,....,max_active_texture_num

#ifndef FAITHFUL_OPENGL_SUPPORT_ASTC
  InitContextLdr();
  InitContextHdr();
  InitContextNmap();
#endif

//  TODO: setup configs for __all__ textures
//  GLuint textureID;
//  glGenTextures(1, &textureID);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


TextureManager::~TextureManager() {
  if (context_ldr_)
    astcenc_context_free(context_ldr_);
  if (context_hdr_)
    astcenc_context_free(context_hdr_);
  if (context_nmap_)
    astcenc_context_free(context_nmap_);

  // TODO: delete all OpenGL textures:
  //  glDeleteTextures(1, &id);
}

// TODO: Is it blocking ? <<-- add thread-safety
int TextureManager::Load(std::string&& texture_path) {
  for (auto& t : active_instances_) {
    if (*t.path == texture_path) {
      ++t.counter;
      return t.opengl_id;
    }
  }
  if (!CleanInactive()) {
    std::cerr
        << "Can't load texture: reserve more place for textures; "
        << max_active_texture_num << " is not enough;\n"
        << "failure for: " << texture_path << std::endl;
    return default_texture_id_;
  }
  int id = free_instances_.Back();
  free_instances_.PopBack();
  auto& instance = active_instances_[id];
  ++instance.counter;
  *instance.path = std::move(texture_path);
  LoadTextureData(id);
  return id;
}

#ifndef FAITHFUL_OPENGL_SUPPORT_ASTC
bool TextureManager::InitContextLdr() {
  using namespace faithful;  // for namespace faithful::config
  astcenc_config config;
  config.block_x = config::tex_comp_block_x;
  config.block_y = config::tex_comp_block_y;
  config.block_z = config::tex_comp_block_z;
  config.profile = config::tex_comp_profile_ldr;

  astcenc_error status = astcenc_config_init(
      config::tex_comp_profile_ldr, config::tex_comp_block_x,
      config::tex_comp_block_y, config::tex_comp_block_z,
      config::tex_comp_quality, 0, &config);
  if (status != ASTCENC_SUCCESS) {
    std::cout << "Error: astc-enc ldr codec config init failed: "
              << astcenc_get_error_string(status) << std::endl;
    return false;
  }

  status =
      astcenc_context_alloc(&config, threads_per_texture, &context_ldr_);
  if (status != ASTCENC_SUCCESS) {
    std::cout << "Error: astc-enc ldr codec context alloc failed: "
              << astcenc_get_error_string(status) << std::endl;
    return false;
  }
  return true;
}
bool TextureManager::InitContextHdr() {
  using namespace faithful;  // for namespace faithful::config
  astcenc_config config;
  config.block_x = config::tex_comp_block_x;
  config.block_y = config::tex_comp_block_y;
  config.block_z = config::tex_comp_block_z;
  config.profile = config::tex_comp_profile_hdr;

  astcenc_error status = astcenc_config_init(
      config::tex_comp_profile_hdr, config::tex_comp_block_x,
      config::tex_comp_block_y, config::tex_comp_block_z,
      config::tex_comp_quality, 0, &config);
  if (status != ASTCENC_SUCCESS) {
    std::cout << "Error: astc-enc hdr codec config init failed: "
              << astcenc_get_error_string(status) << std::endl;
    return false;
  }

  status =
      astcenc_context_alloc(&config, threads_per_texture, &context_hdr_);
  if (status != ASTCENC_SUCCESS) {
    std::cout << "Error: astc-enc hdr codec context alloc failed: "
              << astcenc_get_error_string(status) << std::endl;
    return false;
  }
  return true;
}
bool TextureManager::InitContextNmap() {
  using namespace faithful;  // for namespace faithful::config
  astcenc_config config;
  config.block_x = config::tex_comp_block_x;
  config.block_y = config::tex_comp_block_y;
  config.block_z = config::tex_comp_block_z;
  config.profile = config::tex_comp_profile_ldr;

  unsigned int flags{0};
  flags |= ASTCENC_FLG_MAP_NORMAL;

  config.flags |= flags;

  astcenc_error status = astcenc_config_init(
      config::tex_comp_profile_ldr, config::tex_comp_block_x,
      config::tex_comp_block_y, config::tex_comp_block_z,
      config::tex_comp_quality, flags, &config);
  if (status != ASTCENC_SUCCESS) {
    std::cout << "Error: astc-enc nmap codec config init failed: "
              << astcenc_get_error_string(status) << std::endl;
    return false;
  }

  status =
      astcenc_context_alloc(&config, threads_per_texture, &context_nmap_);
  if (status != ASTCENC_SUCCESS) {
    std::cout << "Error: astc-enc nmap codec context alloc failed: "
              << astcenc_get_error_string(status) << std::endl;
    return false;
  }
  return true;
}
#endif

void TextureManager::LoadTextureData(int active_instance_id) {
  auto& instance = active_instances_[active_instance_id];
  /// assets/textures contain only 6x6x1 astc, so there's no need
  /// to check is for "ASTC" and block size.
  /// but we still need to get texture resolution
  std::ifstream texture_file(*instance.path);
  AstcHeader header;
  texture_file.read(reinterpret_cast<char*>(&header), sizeof(AstcHeader));
  int width = header.dim_x[0] | header.dim_x[1] << 8 | header.dim_x[2] << 16;
  int height = header.dim_y[0] | header.dim_y[1] << 8 | header.dim_y[2] << 16;
  /// .astc always has 4 channels
  int texture_data_size = width * height * 4;
  auto texture_data = std::make_unique<uint8_t>(texture_data_size);
  texture_file.read(reinterpret_cast<char*>(texture_data.get()),
                    texture_data_size);

  glBindTexture(GL_TEXTURE_2D, instance.opengl_id);
#ifdef FAITHFUL_OPENGL_SUPPORT_ASTC
    glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_ASTC_4x4_KHR,
                 width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data.get());
    glGenerateMipmap(GL_TEXTURE_2D);
#else
  TextureCategory category = DeduceTextureCategory(*instance.path);
  astcenc_context* context = PrepareContext(category);
  auto decomp_data = DecompressAstcTexture(std::move(texture_data),
                                           width, height, context);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_UNSIGNED_BYTE,
               width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, decomp_data.get());
#endif
  glGenerateMipmap(GL_TEXTURE_2D);
}


std::unique_ptr<uint8_t> TextureManager::DecompressAstcTexture(
    std::unique_ptr<uint8_t> tex_data, int tex_width,
    int tex_height, astcenc_context* context) {
  using namespace faithful;  // for namespace faithful::config
  int block_count_x = (tex_width + config::tex_comp_block_x - 1)
                      / config::tex_comp_block_x;
  int block_count_y = (tex_height + config::tex_comp_block_y - 1)
                      / config::tex_comp_block_y;
  int comp_len = block_count_x * block_count_y * 16;
  auto comp_data = std::make_unique<uint8_t>(comp_len);

  astcenc_image image;
  image.dim_x = tex_width;
  image.dim_y = tex_height;
  image.dim_z = 1;
  image.data_type = static_cast<astcenc_type>(config::tex_data_type);
  image.data = reinterpret_cast<void**>(tex_data.get());

//  TODO: there need asynchronous programming --> combinators & executors
//  bool decode_fail = true;
//  for (std::size_t i = 0; i < thread_pool_->threads_tasks_.size(); ++i) {
//    thread_pool_->threads_tasks_[i].first =
//        [=, &image, &decode_fail]() {
//          decode_fail |= astcenc_decompress_image(
//              context, comp_data, comp_len,
//              &image, &config::tex_comp_swizzle, i);
//        };
//  }
}

astcenc_context* TextureManager::PrepareContext(TextureManager::TextureCategory category) {
  astcenc_context* context;
  switch (category) {
    case TextureCategory::kLdr:
      context = context_ldr_;
      break;
    case TextureCategory::kHdr:
      context = context_hdr_;
      break;
    case TextureCategory::kNmap:
      context = context_nmap_;
      break;
  }
  astcenc_decompress_reset(context);
  return context;
}

TextureManager::TextureCategory TextureManager::DeduceTextureCategory(
    const std::filesystem::path& filename) {
  if (DetectHdr(filename)) {
    return TextureCategory::kHdr;
  } else if (DetectNmap(filename)) {
    return TextureCategory::kNmap;
  } else {
    return TextureCategory::kLdr;
  }
}
bool TextureManager::DetectHdr(const std::filesystem::path& filename) {
  const std::string& stem_string(filename.stem().string());
  // len of "_hdr" + at least 1 char for actual name
  if (stem_string.length() < 5) {
    return false;
  }
  size_t suffixPos = stem_string.length() - 4;
  return stem_string.compare(suffixPos, std::string::npos, "_hdr") == 0;
}
bool TextureManager::DetectNmap(const std::filesystem::path& filename) {
  const std::string& stem_string(filename.stem().string());
  // len of "_nmap" + at least 1 char for actual name
  if (stem_string.length() < 6) {
    return false;
  }
  size_t suffixPos = stem_string.length() - 5;
  return stem_string.compare(suffixPos, std::string::npos, "_nmap") == 0;
}

void Texture::Bind(GLenum target) {
  glBindTexture(target, opengl_id_);
}


}  // namespace faithful
