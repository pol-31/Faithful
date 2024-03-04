#include "TexturePool.h"

#include <fstream>
#include <iostream> // todo: replace by Logger

#include <glad/glad.h>

#include "../../config/AssetFormats.h"

#include "../executors/DisplayInteractionThreadPool.h"

// TODO: ASTCENC_FLG_DECOMPRESS_ONLY

namespace faithful {
namespace details {
namespace assets {

TexturePool::TexturePool() {
  InitContextLdr();
  InitContextHdr();
  InitContextNmap();
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
}

bool TexturePool::Assist() {
  if (processing_tasks_.empty()) {
    return false;
  }
  std::unique_lock lock(mutex_processing_tasks_);
  if (processing_tasks_.empty()) {
    return false;
  }
  for (auto it = processing_tasks_.begin(); it != processing_tasks_.end();) {
    if (it->get()->completed) {
      /// Since we have already acquired the mutex and
      /// are iterating through all elements, let's maximize our efforts
      /// - let's clean up completed tasks
      processing_tasks_.erase(it++);
    } else if (it->get()->free_thread_slots != 0) {
      --(it->get()->free_thread_slots);
      it->get()->working_threads_left += 1;
      /// release lock and start processing
      lock.unlock();
      /// assist only once, we don't want to hang out there all the time
      it->get()->Decompress(it->get()->free_thread_slots);
      return true;
    }
  }
}

TexturePool::DataType TexturePool::LoadImpl(TrackedDataType& instance_info) {
  instance_info.data->opengl_context = opengl_context_;
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

  int block_count_x = (width + config::kTexCompBlockX - 1)
                      / config::kTexCompBlockX;
  int block_count_y = (height + config::kTexCompBlockY - 1)
                      / config::kTexCompBlockY;
  int comp_len = block_count_x * block_count_y * 16;

  astcenc_image image;
  image.dim_x = width;
  image.dim_y = height;
  image.dim_z = 1;

  TextureCategory category = DeduceTextureCategory(instance_info.path);
  if (category == TextureCategory::kHdr) {
    comp_len *= 4; // float32 instead of u8
    texture_data_size *= 4;
    image.data_type = static_cast<astcenc_type>(config::kTexHdrDataType);
  } else {
    image.data_type = static_cast<astcenc_type>(config::kTexLdrDataType);
  }

  auto texture_data = std::make_unique<uint8_t[]>(texture_data_size);
  texture_file.read(reinterpret_cast<char*>(texture_data.get()),
                    texture_data_size);
  image.data = reinterpret_cast<void**>(texture_data.get());

  astcenc_context* context = PrepareContext(category);

  auto processing_context = std::make_unique<ProcessingContext>();
  processing_context->data = instance_info.data;
  processing_context->context = context;
  processing_context->category = category;
  processing_context->compressed_data_length = comp_len;
  processing_context->compressed_data = std::make_unique<uint8_t[]>(comp_len);
  processing_context->image = std::move(image);
  processing_context->swizzle = config::kTexCompSwizzle;

  switch (category) {
    case TextureCategory::kLdr:
      processing_context->free_thread_slots = max_thread_per_ldr_;
      break;
    case TextureCategory::kNmap:
      processing_context->free_thread_slots = max_thread_per_nmap_;
      break;
    case TextureCategory::kHdr:
      processing_context->free_thread_slots = max_thread_per_hdr_;
      break;
  }
  std::lock_guard lock(mutex_processing_tasks_);
  processing_tasks_.push_back(std::move(processing_context));
  return instance_info.data;
}

/* TODO:
 *  textures loading is the slower part of assets loading, so
 *  there we've separated actual loading and tracking,
 *  so when we have object Texture by Load() we still need to Assist()
 *  to process it
 * */


bool TexturePool::InitContextLdr() {
  using namespace faithful;  // for namespace faithful::config
  astcenc_config config;
  config.block_x = config::kTexCompBlockX;
  config.block_y = config::kTexCompBlockY;
  config.block_z = config::kTexCompBlockZ;
  config.profile = config::kTexCompProfileLdr;

  astcenc_error status = astcenc_config_init(
      config::kTexCompProfileLdr, config::kTexCompBlockX,
      config::kTexCompBlockY, config::kTexCompBlockZ,
      config::kTexCompQuality, 0, &config);
  if (status != ASTCENC_SUCCESS) {
    std::cout << "Error: astc-enc ldr codec config init failed: "
              << astcenc_get_error_string(status) << std::endl;
    return false;
  }

  status = astcenc_context_alloc(&config, max_thread_per_hdr_,
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
  config.block_x = config::kTexCompBlockX;
  config.block_y = config::kTexCompBlockY;
  config.block_z = config::kTexCompBlockZ;
  config.profile = config::kTexCompProfileHdr;

  astcenc_error status = astcenc_config_init(
      config::kTexCompProfileHdr, config::kTexCompBlockX,
      config::kTexCompBlockY, config::kTexCompBlockZ,
      config::kTexCompQuality, 0, &config);
  if (status != ASTCENC_SUCCESS) {
    std::cout << "Error: astc-enc hdr codec config init failed: "
              << astcenc_get_error_string(status) << std::endl;
    return false;
  }

  status = astcenc_context_alloc(&config, max_thread_per_hdr_,
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
  config.block_x = config::kTexCompBlockX;
  config.block_y = config::kTexCompBlockY;
  config.block_z = config::kTexCompBlockZ;
  config.profile = config::kTexCompProfileLdr;

  unsigned int flags{0};
  flags |= ASTCENC_FLG_MAP_NORMAL;

  config.flags |= flags;

  astcenc_error status = astcenc_config_init(
      config::kTexCompProfileLdr, config::kTexCompBlockX,
      config::kTexCompBlockY, config::kTexCompBlockZ,
      config::kTexCompQuality, flags, &config);
  if (status != ASTCENC_SUCCESS) {
    std::cout << "Error: astc-enc nmap codec config init failed: "
              << astcenc_get_error_string(status) << std::endl;
    return false;
  }

  status = astcenc_context_alloc(&config, max_thread_per_nmap_,
                                 &context_nmap_);
  if (status != ASTCENC_SUCCESS) {
    std::cout << "Error: astc-enc nmap codec context alloc failed: "
              << astcenc_get_error_string(status) << std::endl;
    return false;
  }
  return true;
}

void TexturePool::ProcessingContext::MakeComplete() {
  /// removing this task from TexturePool::processing_tasks_
  /// performed inside the Assist()

  if (!success) {
    std::cerr << "TexturePool threads can't decompress texture" << std::endl;
    return;
  }

  /// pass by copy, because of std::shared_ptr class member
  data->opengl_context->Put([=] { // capturing by copy - data is std::shared_ptr
    glGenTextures(1, &data->id);
    glBindTexture(GL_TEXTURE_2D, data->id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if (category == TextureCategory::kHdr) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, image.dim_x, image.dim_y,
                   0, GL_RGB, GL_FLOAT, data.get());
    } else {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_UNSIGNED_BYTE,
                   image.dim_x, image.dim_y, 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, data.get());
      glGenerateMipmap(GL_TEXTURE_2D);
    }
    data->ready = true;
    // TODO: handle OpenGL call failure?
  });
  completed = true;
}

void TexturePool::ProcessingContext::Decompress(int thread_id) {
  auto decode_fail = astcenc_decompress_image(
      context, compressed_data.get(), compressed_data_length,
      &image, &swizzle, thread_id);
  if (decode_fail != ASTCENC_SUCCESS) {
    success = false;
  }
  if (working_threads_left.fetch_sub(1) == 1) { // if other done
    MakeComplete();
  }
}

astcenc_context* TexturePool::PrepareContext(
    TexturePool::TextureCategory category) {
  astcenc_context* context;
  switch (category) {
    case TextureCategory::kLdr:
      context = context_ldr_;
      break;
    case TextureCategory::kNmap:
      context = context_nmap_;
      break;
    case TextureCategory::kHdr:
      context = context_hdr_;
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
  // len of "_rrrg" + at least 1 char for actual name
  if (filename.length() < 6) {
    return false;
  }
  size_t suffixPos = filename.length() - 5;
  return filename.compare(suffixPos, std::string::npos, "_rrrg") == 0;
}

} // namespace assets
} // namespace details
} // namespace faithful
