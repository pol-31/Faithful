#include "TextureProcessor.h"

#include <iostream>
#include <fstream>
#include <filesystem>

#include "astcenc.h"
// #define STB_IMAGE_IMPLEMENTATION // TODO: already implemented in ModelProc...
#include "stb_image.h"
// #define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "../../config/AssetFormats.h"
#include "../../config/Paths.h"

#include "AssetCategory.h"
#include "AssetLoadingThreadPool.h"

// TODO: memory allocation (+ STBI_MALLOC/etc...)

// TODO: astc-enc - "We recommend using the latest stable
//                      release tag for production development."

// TODO; hdr/nmap test

// TODO: extra case: given .astc or .ktx

TextureProcessor::TextureProcessor(
    bool encode, const std::filesystem::path& asset_destination,
    AssetLoadingThreadPool* thread_pool)
    : encode_(encode),
      asset_destination_(asset_destination),
      thread_pool_(thread_pool),
      worker_thread_count_(thread_pool->get_thread_count()) {
}

TextureProcessor::~TextureProcessor() {
  if (context_ldr_)
    astcenc_context_free(context_ldr_);
  if (context_hdr_)
    astcenc_context_free(context_hdr_);
  if (context_nmap_)
    astcenc_context_free(context_nmap_);
}

bool TextureProcessor::SwitchToLdr() {
  if (context_hdr_)
    astcenc_context_free(context_hdr_);
  if (context_nmap_)
    astcenc_context_free(context_nmap_);
  if (!context_ldr_)
    return InitContextLdr();
  return true;
}
bool TextureProcessor::SwitchToHdr() {
  if (context_ldr_)
    astcenc_context_free(context_ldr_);
  if (context_nmap_)
    astcenc_context_free(context_nmap_);
  if (!context_hdr_)
    return InitContextHdr();
  return true;
}
bool TextureProcessor::SwitchToNmap() {
  if (context_ldr_)
    astcenc_context_free(context_ldr_);
  if (context_hdr_)
    astcenc_context_free(context_hdr_);
  if (!context_nmap_)
    return InitContextNmap();
  return true;
}

bool TextureProcessor::InitContextLdr() {
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
      astcenc_context_alloc(&config, worker_thread_count_ + 1, &context_ldr_);
  if (status != ASTCENC_SUCCESS) {
    std::cout << "Error: astc-enc ldr codec context alloc failed: "
              << astcenc_get_error_string(status) << std::endl;
    return false;
  }
  std::cout << "---------" << std::endl;
  return true;
}
bool TextureProcessor::InitContextHdr() {
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
      astcenc_context_alloc(&config, worker_thread_count_ + 1, &context_hdr_);
  if (status != ASTCENC_SUCCESS) {
    std::cout << "Error: astc-enc hdr codec context alloc failed: "
              << astcenc_get_error_string(status) << std::endl;
    return false;
  }
  return true;
}
bool TextureProcessor::InitContextNmap() {
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
      astcenc_context_alloc(&config, worker_thread_count_ + 1, &context_nmap_);
  if (status != ASTCENC_SUCCESS) {
    std::cout << "Error: astc-enc nmap codec context alloc failed: "
              << astcenc_get_error_string(status) << std::endl;
    return false;
  }
  return true;
}

void TextureProcessor::Process(const std::filesystem::path& path,
                               const std::filesystem::path& path_suffix,
                               AssetCategory category) {
  if (encode_) {
    Encode(path, path_suffix, category);
  } else {
    Decode(path, path_suffix, category);
  }
}

bool TextureProcessor::Encode(const std::filesystem::path& path,
                              const std::filesystem::path& path_suffix,
                              AssetCategory category) {
  using namespace faithful;  // for namespace faithful::config
  int image_x, image_y, image_c;
  // force 4 component (astc codec requires it)
  auto image_data =
      (uint8_t*)stbi_load(path.c_str(), &image_x, &image_y, &image_c, 4);
  if (!image_data) {
    std::cout << "Error: stb_image texture loading failed: " << path
              << std::endl;
    return false;
  }

  std::filesystem::path out_filename;
  astcenc_context* context;
  PrepareEncodeContextAndFilename(path_suffix, category, out_filename, context);

  int comp_len;
  uint8_t* comp_data;
  PrepareCompData(image_x, image_y, comp_len, comp_data);

  astcenc_image image;
  image.dim_x = image_x;
  image.dim_y = image_y;
  image.dim_z = 1;
  image.data_type = static_cast<astcenc_type>(config::tex_data_type);
  image.data = reinterpret_cast<void**>(&image_data);

  while (!thread_pool_->thread_tasks_mutex_.try_lock()) {
  }
  std::atomic_flag encode_success = ATOMIC_FLAG_INIT;
  encode_success.test_and_set();
  for (int i = 0; i < worker_thread_count_; ++i) {
    thread_pool_->threads_tasks_[i].first =
        std::move([=, &image, &encode_success]() {
          astcenc_error status = astcenc_compress_image(
              context, &image, &config::tex_comp_swizzle, comp_data, comp_len,
              i + 1);  // 0 for Main thread
          if (status != ASTCENC_SUCCESS)
            encode_success.clear();
        });
    thread_pool_->threads_tasks_[i].second = true;
  }
  thread_pool_->thread_tasks_mutex_.unlock();
  astcenc_error status = astcenc_compress_image(
      context, &image, &config::tex_comp_swizzle, comp_data, comp_len, 0);
  if (status != ASTCENC_SUCCESS) {
    encode_success.clear();
  }
  while (!thread_pool_->Completed()) {
  }
  if (!encode_success.test_and_set()) {
    std::cout << "Error: astc-enc texture compress failed" << std::endl;
    stbi_image_free(image_data);  // TODO: allocations...
    delete[] comp_data;
    return false;
  }
  thread_pool_->UpdateContext();

  if (!WriteEncodedData(out_filename, image_x, image_y, comp_len, comp_data)) {
    stbi_image_free(image_data);  // TODO: allocations...
    delete[] comp_data;
    return false;
  }

  stbi_image_free(image_data);  // TODO: allocations...
  delete[] comp_data;
  return true;
}

void TextureProcessor::PrepareCompData(int image_x, int image_y, int& comp_len,
                                       uint8_t*& comp_data) {
  using namespace faithful;  // for namespace faithful::config
  int block_count_x =
      (image_x + config::tex_comp_block_x - 1) / config::tex_comp_block_x;
  int block_count_y =
      (image_y + config::tex_comp_block_y - 1) / config::tex_comp_block_y;
  comp_len = block_count_x * block_count_y * 16;
  comp_data = new uint8_t[comp_len];  // TODO: allocations...
}

void TextureProcessor::PrepareEncodeContextAndFilename(
    const std::filesystem::path& relative_path, AssetCategory category,
    std::filesystem::path& out_filename, astcenc_context*& context) {
  out_filename = asset_destination_;  // TODO: allocations...
  out_filename /= relative_path;
  out_filename.remove_filename();
  std::filesystem::create_directories(out_filename);
  out_filename += relative_path.stem();

  switch (category) {
    case AssetCategory::kTextureLdr:
      SwitchToLdr();
      context = context_ldr_;
      out_filename += "_ldr.astc";
      break;
    case AssetCategory::kTextureHdr:
      SwitchToHdr();
      context = context_hdr_;
      out_filename += "_hdr.astc";
      break;
    case AssetCategory::kTextureNmap:
      SwitchToNmap();
      context = context_nmap_;
      out_filename += "_nmap.astc";
      break;
    default:
      break;
  }
  astcenc_compress_reset(context);
}

bool TextureProcessor::WriteEncodedData(const std::string& filename,
                                        unsigned int image_x,
                                        unsigned int image_y,
                                        int comp_data_size,
                                        const uint8_t* comp_data) {
  using namespace faithful;  // for namespace faithful::config
  std::ofstream out_file(filename, std::ios::binary);
  if (!out_file.is_open()) {
    std::cout << "Error: failed to create file for encoded data" << std::endl;
    return false;
  }
  AstcHeader header;
  header.magic[0] = 0x13;
  header.magic[1] = 0xAB;
  header.magic[2] = 0xA1;
  header.magic[3] = 0x5C;

  header.block_x = config::tex_comp_block_x;
  header.block_y = config::tex_comp_block_y;
  header.block_z = 1;

  header.dim_x[2] = static_cast<uint8_t>(image_x >> 16);
  header.dim_x[1] = static_cast<uint8_t>(image_x >> 8);
  header.dim_x[0] = static_cast<uint8_t>(image_x);

  header.dim_y[2] = static_cast<uint8_t>(image_y >> 16);
  header.dim_y[1] = static_cast<uint8_t>(image_y >> 8);
  header.dim_y[0] = static_cast<uint8_t>(image_y);

  header.dim_z[0] = 1;
  header.dim_z[1] = 0;
  header.dim_z[2] = 0;

  out_file.write(reinterpret_cast<const char*>(&header), sizeof(AstcHeader));
  out_file.write(reinterpret_cast<const char*>(comp_data), comp_data_size);
  return true;
}

bool TextureProcessor::Decode(const std::filesystem::path& path,
                              const std::filesystem::path& path_suffix,
                              AssetCategory category) {
  using namespace faithful;  // for namespace faithful::config
  std::string out_filename;
  astcenc_context* context;
  PrepareDecodeContextAndFilename(path, category, out_filename, context);

  int image_x, image_y, comp_len;
  uint8_t* comp_data;
  if (!ReadAstcFile(path, image_x, image_y, comp_len, comp_data)) {
    return false;
  }
  uint8_t* image_data = new uint8_t[image_x * image_y * 4];

  astcenc_image image;
  image.dim_x = image_x;
  image.dim_y = image_y;
  image.dim_z = 1;
  image.data_type = static_cast<astcenc_type>(config::tex_data_type);
  image.data = reinterpret_cast<void**>(&image_data);

  while (!thread_pool_->thread_tasks_mutex_.try_lock()) {
  }
  bool decode_fail = true;
  for (int i = 0; i < thread_pool_->threads_tasks_.size(); ++i) {
    thread_pool_->threads_tasks_[i].first =
        std::move([=, &image, &decode_fail]() {
          decode_fail |= astcenc_decompress_image(
              context, comp_data, comp_len, &image, &config::tex_comp_swizzle,
              i + 1);  // 0 for Main thread
        });
    thread_pool_->threads_tasks_[i].second = true;
  }
  thread_pool_->thread_tasks_mutex_.unlock();

  decode_fail |= astcenc_decompress_image(context, comp_data, comp_len, &image,
                                          &config::tex_comp_swizzle, 0);
  while (!thread_pool_->Completed()) {
  }
  if (decode_fail) {
    std::cout << "Error: astc-enc texture decompress failed" << std::endl;
    stbi_image_free(image_data);  // TODO: allocations...
    delete[] comp_data;
    return false;
  }

  thread_pool_->UpdateContext();

  std::filesystem::path out_texture_path =
      asset_destination_ / path_suffix / out_filename;
  if (category != AssetCategory::kTextureHdr) {
    if (stbi_write_png(out_texture_path.c_str(), image_x, image_y, 4,
                       image_data, 4 * image_x)) {
      std::cout << "Error: stb_image_write failed to save texture" << std::endl;
      stbi_image_free(image_data);  // TODO: allocations...
      delete[] comp_data;
      return false;
    }
  } else {
    if (stbi_write_hdr(out_texture_path.c_str(), image_x, image_y, 4,
                       reinterpret_cast<const float*>(image_data))) {
      std::cout << "Error: stb_image_write failed to save texture" << std::endl;
      stbi_image_free(image_data);  // TODO: allocations...
      delete[] comp_data;
      return false;
    }
  }

  stbi_image_free(image_data);
  delete[] comp_data;
  return true;
}

bool TextureProcessor::ReadAstcFile(const std::string& path, int& width,
                                    int& height, int& comp_len,
                                    uint8_t*& comp_data) {
  std::ifstream file(path, std::ios::binary);
  if (!file.is_open()) {
    std::cout << "Error: texture loading failed: " << path << std::endl;
    return false;
  }
  AstcHeader header;
  file.read(reinterpret_cast<char*>(&header), sizeof(AstcHeader));

  if (header.magic[0] != 0x13 || header.magic[1] != 0xAB ||
      header.magic[2] != 0xA1 || header.magic[3] != 0x5C) {
    std::cerr << "Error: invalid ASTC file (metadata): " << path << std::endl;
    return false;
  }

  if ((header.dim_z[0] | header.dim_z[1] << 8 | header.dim_z[2]) != 1) {
    std::cerr << "Error: only 2d textures supported: " << path << std::endl;
    return false;
  }

  // we don't care about block sizes, because currently all textures compressed
  // with the same configs (see Faithful/config/AssetFormats.h)
  width = header.dim_x[0] | header.dim_x[1] << 8 | header.dim_x[2] << 16;
  height = header.dim_y[0] | header.dim_y[1] << 8 | header.dim_y[2] << 16;

  PrepareCompData(width, height, comp_len, comp_data);
  file.read(reinterpret_cast<char*>(comp_data), comp_len);
  return true;
}

void TextureProcessor::PrepareDecodeContextAndFilename(
    const std::filesystem::path& path, AssetCategory category,
    std::string& out_filename, astcenc_context* context) {
  std::string extension;
  switch (category) {
    case AssetCategory::kTextureLdr:
      extension = faithful::config::tex_ldr_decomp_format;
      SwitchToLdr();
      context = context_ldr_;
      break;
    case AssetCategory::kTextureHdr:
      extension = faithful::config::tex_hdr_decomp_format;
      SwitchToHdr();
      context = context_hdr_;
      break;
    case AssetCategory::kTextureNmap:
      extension = faithful::config::tex_nmap_decomp_format;
      SwitchToNmap();
      context = context_nmap_;
      break;
    default:
      break;
  }
  astcenc_decompress_reset(context);

  /// as an input we have something like this:
  /// test_hdr.astc
  std::string buffer = path.filename().string();
  int name_end_pos = buffer.rfind('_');

  out_filename.reserve(name_end_pos + 1 + 5);  // + 5 for format
  out_filename = buffer.substr(0, name_end_pos);
  out_filename += ".";
  out_filename += extension;
}
