#include "TextureProcessor.h"

#include <iostream>
#include <fstream>
#include <filesystem>

#include <stb_image.h>
#include <stb_image_write.h>

#include "../../config/AssetFormats.h"

#include "AssetCategory.h"
#include "AssetLoadingThreadPool.h"

// TODO: memory allocation (+ STBI_MALLOC/etc...)

TextureProcessor::TextureProcessor(
    bool encode,
    const std::filesystem::path& asset_destination,
    const std::filesystem::path& user_asset_root_dir,
    AssetLoadingThreadPool* thread_pool)
    : asset_destination_(asset_destination),
      user_asset_root_dir_(user_asset_root_dir),
      thread_pool_(thread_pool),
      encode_(encode) {}

TextureProcessor::~TextureProcessor() {
  if (context_ldr_)
    astcenc_context_free(context_ldr_);
  if (context_hdr_)
    astcenc_context_free(context_hdr_);
  if (context_rg_)
    astcenc_context_free(context_rg_);
}

bool TextureProcessor::SwitchToLdr() {
  if (context_hdr_)
    astcenc_context_free(context_hdr_);
  if (context_rg_)
    astcenc_context_free(context_rg_);
  if (!context_ldr_)
    return InitContextLdr();
  return true;
}
bool TextureProcessor::SwitchToHdr() {
  if (context_ldr_)
    astcenc_context_free(context_ldr_);
  if (context_rg_)
    astcenc_context_free(context_rg_);
  if (!context_hdr_)
    return InitContextHdr();
  return true;
}
bool TextureProcessor::SwitchToRG() {
  if (context_ldr_)
    astcenc_context_free(context_ldr_);
  if (context_hdr_)
    astcenc_context_free(context_hdr_);
  if (!context_rg_)
    return InitContextRG();
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
    std::cerr << "Error: astc-enc ldr codec config init failed: "
              << astcenc_get_error_string(status) << std::endl;
    return false;
  }

  status =
      astcenc_context_alloc(&config, thread_pool_->GetThreadNumber(),
                            &context_ldr_);
  if (status != ASTCENC_SUCCESS) {
    std::cerr << "Error: astc-enc ldr codec context alloc failed: "
              << astcenc_get_error_string(status) << std::endl;
    return false;
  }
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
    std::cerr << "Error: astc-enc hdr codec config init failed: "
              << astcenc_get_error_string(status) << std::endl;
    return false;
  }

  status =
      astcenc_context_alloc(&config, thread_pool_->GetThreadNumber(),
                            &context_hdr_);
  if (status != ASTCENC_SUCCESS) {
    std::cerr << "Error: astc-enc hdr codec context alloc failed: "
              << astcenc_get_error_string(status) << std::endl;
    return false;
  }
  return true;
}
bool TextureProcessor::InitContextRG() {
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
    std::cerr << "Error: astc-enc rg codec config init failed: "
              << astcenc_get_error_string(status) << std::endl;
    return false;
  }

  status =
      astcenc_context_alloc(&config, thread_pool_->GetThreadNumber(),
                            &context_rg_);
  if (status != ASTCENC_SUCCESS) {
    std::cerr << "Error: astc-enc rg codec context alloc failed: "
              << astcenc_get_error_string(status) << std::endl;
    return false;
  }
  return true;
}

void TextureProcessor::Process(const std::filesystem::path& texture_path,
                               AssetCategory category) {
  if (encode_) {
    Encode(texture_path, category);
  } else {
    Decode(texture_path, category);
  }
}


void TextureProcessor::Process(const std::filesystem::path& dest_path,
                               std::unique_ptr<uint8_t> image_data,
                               int width, int height,
                               AssetCategory category) {
  if (encode_) {
    Encode(dest_path, std::move(image_data), width, height, category);
  } else {
    Decode(dest_path, std::move(image_data), width, height, category);
  }
}

bool TextureProcessor::Encode(const std::filesystem::path& texture_path,
                              AssetCategory category) {
  using namespace faithful;  // for namespace faithful::config
  int image_x, image_y, image_c;
  // force 4 component (astc codec requires it)
  auto image_data =
      (uint8_t*)stbi_load(texture_path.string().c_str(),
                          &image_x, &image_y, &image_c, 4);
  if (!image_data) {
    std::cerr << "Error: stb_image texture loading failed: " << texture_path
              << std::endl;
    return false;
  }
  std::shared_ptr<uint8_t> image_data_ptr(
      image_data, [](uint8_t* ptr) {
        delete[] ptr;
      });

  /// extension replaced inside the ProvideEncodeContextAndFilename()
  auto dest_path = asset_destination_ /
                   texture_path.lexically_relative(user_asset_root_dir_);
  // structure binding can't be used with capturing (context used in lambda further)
  std::pair<astcenc_context*, std::string> res =
      ProvideEncodeContextAndFilename(dest_path, category);
  astcenc_context* context = res.first;
  std::string out_filename = res.second;

  std::cout << "DEST PATH: " << dest_path << std::endl;
  std::cout << "OUT PATH: " << out_filename << std::endl;

  int comp_len = CalculateCompLen(image_x, image_y);
  auto comp_data = std::make_shared<uint8_t>(comp_len);

  astcenc_image image;
  image.dim_x = image_x;
  image.dim_y = image_y;
  image.dim_z = 1;
  image.data_type = static_cast<astcenc_type>(config::tex_data_type);

  image.data = reinterpret_cast<void**>(&image_data);
//  image.data = reinterpret_cast<void**>(image_data_ptr.get());

  // no need to make it atomic, only "fail"-thread write
  bool encode_success = true;
  thread_pool_->Execute([=, &image, &encode_success](int thread_id) {
    astcenc_error status = astcenc_compress_image(
        context, &image, &config::tex_comp_swizzle,
        comp_data.get(), comp_len, thread_id);
    if (status != ASTCENC_SUCCESS)
      encode_success = false;
  });

  if (!encode_success) {
    std::cerr << "Error: astc-enc texture compression failed" << std::endl;
    return false;
  }

  return WriteEncodedData(out_filename, image_x, image_y,
                          comp_len, comp_data.get());
}

bool TextureProcessor::Encode(const std::filesystem::path& dest_path,
                              std::unique_ptr<uint8_t> image_data,
                              int width, int height,
                              AssetCategory category) {

  using namespace faithful;  // for namespace faithful::config

  // structure binding can't be used with capturing (used in lambda further)
  std::pair<astcenc_context*, std::string> res =
      ProvideEncodeContextAndFilename(dest_path, category);
  astcenc_context* context = res.first;
  auto out_filename = res.second;

  int comp_len = CalculateCompLen(width, height);
  auto comp_data = std::make_shared<uint8_t>(comp_len);

  astcenc_image image;
  image.dim_x = width;
  image.dim_y = height;
  image.dim_z = 1;
  image.data_type = static_cast<astcenc_type>(config::tex_data_type);

  //  image.data = reinterpret_cast<void**>(&image_data);
  image.data = reinterpret_cast<void**>(*image_data.get());

  // no need to make it atomic, only "fail"-thread write
  bool encode_success = true;
  thread_pool_->Execute([=, &image, &encode_success](int thread_id) {
    astcenc_error status = astcenc_compress_image(
        context, &image, &config::tex_comp_swizzle,
        comp_data.get(), comp_len, thread_id);
    if (status != ASTCENC_SUCCESS)
      encode_success = false;
  });

  if (!encode_success) {
    std::cerr << "Error: astc-enc texture compression failed" << std::endl;
    return false;
  }

  return WriteEncodedData(out_filename, width, height,
                          comp_len, comp_data.get());
}

int TextureProcessor::CalculateCompLen(int image_x, int image_y) {
  using namespace faithful;  // for namespace faithful::config
  int block_count_x =
      (image_x + config::tex_comp_block_x - 1) / config::tex_comp_block_x;
  int block_count_y =
      (image_y + config::tex_comp_block_y - 1) / config::tex_comp_block_y;
  return block_count_x * block_count_y * 16;
}


std::pair<astcenc_context*, std::string>
    TextureProcessor::ProvideEncodeContextAndFilename(
    const std::filesystem::path& dest_path, AssetCategory category) {
  std::cout << "__" << dest_path.string() << std::endl;
  std::filesystem::create_directories(dest_path.parent_path());
  astcenc_context* context;
  std::filesystem::path new_filename = dest_path;
  switch (category) {
    case AssetCategory::kTextureLdr:
      SwitchToLdr();
      context = context_ldr_;
      new_filename.replace_extension("astc");
      break;
    case AssetCategory::kTextureHdr:
      SwitchToHdr();
      context = context_hdr_;
      /// if we don't have "_hdr" we should add it here
      /// texture still can have such category if some models
      /// will identify it as a normal map or MetallicRoughness
      if (DetectTexHdrSuffix(dest_path)) {
        new_filename.replace_extension("astc");
      } else {
        auto new_stem = dest_path.stem().string();
        new_stem += "_rrrg.astc";
        new_filename.replace_filename(new_stem);
      }
      break;
    case AssetCategory::kTextureRG:
      SwitchToRG();
      context = context_rg_;
      /// if we don't have "_rrrg" we should add it here
      /// texture still can have such category if some models
      /// will identify it as a normal map or MetallicRoughness
      if (DetectTexRGSuffix(dest_path)) {
        new_filename.replace_extension("astc");
      } else {
        auto new_stem = dest_path.stem().string();
        new_stem += "_rrrg.astc";
        new_filename.replace_filename(new_stem);
      }
      break;
    default:
      break;
  }

  astcenc_compress_reset(context);
  return {context, new_filename};
}

bool TextureProcessor::WriteEncodedData(std::string filename,
                                        unsigned int image_x,
                                        unsigned int image_y,
                                        int comp_data_size,
                                        const uint8_t* comp_data) {
  using namespace faithful;  // for namespace faithful::config
  std::ofstream out_file(filename, std::ios::binary);
  if (!out_file.is_open()) {
    std::cerr << "Error: failed to create file for encoded data" << std::endl;
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

bool TextureProcessor::Decode(const std::filesystem::path& texture_path,
                              AssetCategory category) {
  using namespace faithful;  // for namespace faithful::config
  auto context = ProvideDecodeContext(category);

  int image_x, image_y, comp_len;
  uint8_t* comp_data; // protected with std::shared_ptr going forward
  if (!ReadAstcFile(texture_path.string(), image_x, image_y, comp_len, comp_data)) {
    return false;
  }
  auto image_data = std::make_shared<uint8_t>(image_x * image_y * 4);
  std::shared_ptr<uint8_t> comp_data_ptr(
       comp_data, [](uint8_t* ptr) {
        delete[] ptr;
      });

  astcenc_image image;
  image.dim_x = image_x;
  image.dim_y = image_y;
  image.dim_z = 1;
  image.data_type = static_cast<astcenc_type>(config::tex_data_type);
  image.data = reinterpret_cast<void**>(&image_data);

  // no need to make it atomic, only "fail"-thread write
  bool decode_success = true;
  thread_pool_->Execute([=, &image, &decode_success](int thread_id) {
    astcenc_error status = astcenc_decompress_image(
        context, comp_data, comp_len, &image,
        &config::tex_comp_swizzle, thread_id);
    if (status != ASTCENC_SUCCESS)
      decode_success = false;
  });

  if (!decode_success) {
    std::cerr << "Error: astc-enc texture decompression failed" << std::endl;
    return false;
  }

  /// extension replaced inside the ProvideEncodeContextAndFilename()
  auto dest_path = asset_destination_ /
                   texture_path.lexically_relative(user_asset_root_dir_);
  std::filesystem::create_directories(dest_path.parent_path());
  switch (category) {
    case AssetCategory::kTextureLdr:
      [[fallthrough]];
    case AssetCategory::kTextureRG:
      dest_path.replace_extension("png");
      break;
    case AssetCategory::kTextureHdr:
      dest_path.replace_extension("hdr");
      {
        /// all compressed hdr files have suffix "_hdr"
        /// if not - data wasn't been encoded by AssetProcessor
        auto new_stem = dest_path.stem().string();
        new_stem[new_stem.size() - 4] = '.'; // suffix_hdr -> .hdr
        new_stem += ".hdr";
        dest_path.replace_filename(new_stem);
      }
      break;
    default:
      std::cerr << "TextureProcessor::Decode incorrect category" << std::endl;
      std::terminate();
  }
  return WriteDecodedData(dest_path.string(), image_x, image_y,
                   category, image_data);
}

bool TextureProcessor::Decode(const std::filesystem::path& dest_path,
                              std::unique_ptr<uint8_t> image_data,
                              int width, int height,
                              AssetCategory category) {
  std::cerr << "TextureProcessor::Decode from memory NOT IMPLEMENTED" << std::endl;
  return false;
}

bool TextureProcessor::WriteDecodedData(
    std::string filename, unsigned int image_x, unsigned int image_y,
    AssetCategory category, std::shared_ptr<uint8_t> image_data) {
  if (category != AssetCategory::kTextureHdr) {
    if (stbi_write_png(filename.c_str(), image_x, image_y, 4,
                       image_data.get(), 4 * image_x)) {
      std::cerr << "Error: stb_image_write failed to save texture" << std::endl;
      return false;
    }
  } else {
    if (stbi_write_hdr(filename.c_str(), image_x, image_y, 4,
                       reinterpret_cast<const float*>(image_data.get()))) {
      std::cerr << "Error: stb_image_write failed to save texture" << std::endl;
      return false;
    }
  }
  return true;
}

bool TextureProcessor::ReadAstcFile(const std::string& path, int& width,
                                    int& height, int& comp_len,
                                    uint8_t*& comp_data) {
  std::ifstream file(path, std::ios::binary);
  if (!file.is_open()) {
    std::cerr << "Error: texture loading failed: " << path << std::endl;
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

  comp_len = CalculateCompLen(width, height);
  comp_data = new uint8_t[comp_len];
  file.read(reinterpret_cast<char*>(comp_data), comp_len);
  return true;
}

astcenc_context* TextureProcessor::ProvideDecodeContext(
    AssetCategory category) {
  switch (category) {
    case AssetCategory::kTextureLdr:
      SwitchToLdr();
      astcenc_decompress_reset(context_ldr_);
      return context_ldr_;
    case AssetCategory::kTextureHdr:
      SwitchToHdr();
      astcenc_decompress_reset(context_hdr_);
      return context_hdr_;
    case AssetCategory::kTextureRG:
      SwitchToRG();
      astcenc_decompress_reset(context_rg_);
      return context_rg_;
    default:
      return nullptr;
  }
}
