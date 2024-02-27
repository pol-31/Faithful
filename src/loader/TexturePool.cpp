#include "TexturePool.h"

#include <fstream>
#include <iostream> // todo: replace by Logger

#include <glad/glad.h>

#include "../../config/AssetFormats.h"

#include "../executors/DisplayInteractionThreadPool.h"

namespace faithful {
namespace details {
namespace assets {

TexturePool::TexturePool() {
//#ifndef FAITHFUL_OPENGL_SUPPORT_ASTC
  InitContextLdr();
  InitContextHdr();
  InitContextNmap();
//#endif
}

TexturePool::~TexturePool() {
  if (context_ldr_) {
    astcenc_context_free(context_ldr_);
  }
  if (context_hdr_) {
    astcenc_context_free(context_hdr_);
  }
  if (context_nmap_) {
    astcenc_context_free(context_nmap_);
  }

  /* UpdateManager see not how many tasks but to "circle" around the player:
   * R1 - visible screen
   * R2 - nearby visible - visible + 8 (in tiles total 3x3=9 tiles)
   * R3 - should be processed but necessary
   * R4 - if all R3 loaded we can lazy with 1 thread load from there
   *
   * SO:
   *  - if there's something not loaded in R1-R2 - it's full stress loading (+ dynam tp)
   *  - if R3 - 2 threads
   *  - if R4 - 1 thread - 2 contexts
   *
   * BY DEFAULT:
   * - 1 context for 2 threads - ldr
   * - 1 context for 2 threads - nmap
   * IF HDR:
   * - + 1 context for 2 threads - hdr
   * IF R1-R2:
   * - +1 context for 8 threads - ldr
   * - +1 context for 8 threads - nmap
   * */



  // TODO: in id reuse firstly glDeleteTexture of previous

  // TODO: how to handle failure
  glDeleteTextures(active_instances_.size(), opengl_ids);
}

void TexturePool::SetOpenGlContext(DisplayInteractionThreadPool* opengl_context) {
  opengl_context_ = opengl_context;
}

void TexturePool::Assist() {
  if (!processing_tasks_.empty()) {
    std::lock_guard lock(mutex_processing_tasks_);
    if (!processing_tasks_.empty()) {
      for (auto& task : processing_tasks_) {
        if (task.free_thread_slots != 0) {
          --task.free_thread_slots;
          task.working_threads_left.fetch_add(1);
          task.decoding_function(task.free_thread_slots);
          return;
        }
      }
    }
  }
}


TexturePool::DataType TexturePool::LoadImpl(TrackedDataType& instance_info) {
  /// assets/textures contain only 6x6x2 astc, so there's no need
  /// to check is for "ASTC" and block size.
  /// but we still need to get texture resolution
  std::ifstream texture_file(instance_info.path);
  AstcHeader header;
  texture_file.read(reinterpret_cast<char*>(&header), sizeof(AstcHeader));
  int width = header.dim_x[0] | header.dim_x[1] << 8 | header.dim_x[2] << 16;
  int height = header.dim_y[0] | header.dim_y[1] << 8 | header.dim_y[2] << 16;
  /// .astc always has 4 channels
  int texture_data_size = width * height * 4;
  auto texture_data = std::make_unique<uint8_t[]>(texture_data_size);
  texture_file.read(reinterpret_cast<char*>(texture_data.get()),
                    texture_data_size);
  // TODO: GL_COMPRESSED_RGBA_ASTC_6x6_KHR +
  //  + normal_map z-coord reconstruction in shader
  DecompressAstcTexture(instance_info, std::move(texture_data), width, height);

  instance_info.data->opengl_context = opengl_context_;

  return instance_info.data;
}

//#ifndef FAITHFUL_OPENGL_SUPPORT_ASTC
bool TexturePool::InitContextLdr() {
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

  status = astcenc_context_alloc(&config, config::threads_per_texture,
                                 &context_ldr_);
  if (status != ASTCENC_SUCCESS) {
    std::cout << "Error: astc-enc ldr codec context alloc failed: "
              << astcenc_get_error_string(status) << std::endl;
    return false;
  }
  return true;
}
bool TexturePool::InitContextHdr() {
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

  status = astcenc_context_alloc(&config, config::threads_per_texture,
                                 &context_hdr_);
  if (status != ASTCENC_SUCCESS) {
    std::cout << "Error: astc-enc hdr codec context alloc failed: "
              << astcenc_get_error_string(status) << std::endl;
    return false;
  }
  return true;
}
bool TexturePool::InitContextNmap() {
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

  status = astcenc_context_alloc(&config, config::threads_per_texture,
                                 &context_nmap_);
  if (status != ASTCENC_SUCCESS) {
    std::cout << "Error: astc-enc nmap codec context alloc failed: "
              << astcenc_get_error_string(status) << std::endl;
    return false;
  }
  return true;
}
//#endif

void TexturePool::DecompressAstcTexture(
    TrackedDataType& instance_info,
    std::unique_ptr<uint8_t[]> data,
    int width, int height) {
  using namespace faithful;  // for namespace faithful::config
  int block_count_x = (width + config::tex_comp_block_x - 1)
                      / config::tex_comp_block_x;
  int block_count_y = (height + config::tex_comp_block_y - 1)
                      / config::tex_comp_block_y;
  int comp_len = block_count_x * block_count_y * 16;

  TextureCategory category = DeduceTextureCategory(instance_info.path);

  if (category == TextureCategory::kHdr) {
    comp_len *= 4; // not u8 but float32
  }
  auto comp_data = std::make_unique<uint8_t[]>(comp_len);

  astcenc_image image;
  image.dim_x = width;
  image.dim_y = height;
  image.dim_z = 1;

  if (category == TextureCategory::kHdr) {
    image.data_type = static_cast<astcenc_type>(config::tex_hdr_data_type);
  } else {
    image.data_type = static_cast<astcenc_type>(config::tex_ldr_data_type);
  }
  image.data = reinterpret_cast<void**>(data.get());

  astcenc_context* context = PrepareContext(category);

  ProcessingContext processing_context;
  processing_context.data = instance_info.data;
  processing_context.context = context;
  processing_context.compressed_data_length = comp_len;
  processing_context.compressed_data = std::move(comp_data);
  processing_context.image = std::move(image);
  processing_context.swizzle = config::tex_comp_swizzle;
  processing_context.decoding_function = [](int thread_id) {
    auto decode_fail = astcenc_decompress_image(
        context, comp_data.get(), comp_len,
        &image, &config::tex_comp_swizzle, thread_id);

    if (decode_fail != ASTCENC_SUCCESS) {
      // set processing_context.success = false
      return;
    }

    // if (working_threads_left != 1) {

    // TODO: if (other.done) ...
    //  render thread pool push:
    opengl_context_->Put([]{
      glGenTextures(1, &instance_info.data->id);
      glBindTexture(GL_TEXTURE_2D, instance_info.data->id);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      if (category == TextureCategory::kHdr) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height,
                     0, GL_RGB, GL_FLOAT, data.get());
      } else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_UNSIGNED_BYTE,
                     width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.get());
        glGenerateMipmap(GL_TEXTURE_2D);
      }
      // TODO: if !GL_CALL or !DecompressAstcTexture()
      instance_info.data->ready = true;
    });
  };

  processing_context.context = ;
  processing_context.context = ;

  processing_tasks_.push_back(processing_context);
}

astcenc_context* TexturePool::PrepareContext(
    TexturePool::TextureCategory category) {
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

TexturePool::TextureCategory TexturePool::DeduceTextureCategory(
    const std::string& filename) {
  if (DetectHdr(filename)) {
    return TextureCategory::kHdr;
  } else if (DetectNmap(filename)) {
    return TextureCategory::kNmap;
  } else {
    return TextureCategory::kLdr;
  }
}
bool TexturePool::DetectHdr(const std::string& filename) {
  // len of "_hdr" + at least 1 char for actual name
  if (filename.length() < 5) {
    return false;
  }
  size_t suffixPos = filename.length() - 4;
  return filename.compare(suffixPos, std::string::npos, "_hdr") == 0;
}
bool TexturePool::DetectNmap(const std::string& filename) {
  // len of "_nmap" + at least 1 char for actual name
  if (filename.length() < 6) {
    return false;
  }
  size_t suffixPos = filename.length() - 5;
  return filename.compare(suffixPos, std::string::npos, "_nmap") == 0; // TODO: or _rrrg ?
}

} // namespace assets
} // namespace details
} // namespace faithful
