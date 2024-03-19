#include "TextureProcessor.h"

#include <iostream>
#include <fstream>
#include <filesystem>

#include <stb_image.h>
#include <stb_image_write.h>

#include "../../config/AssetFormats.h"

#include "AssetCategory.h"
#include "AssetLoadingThreadPool.h"

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
  if (context_ldr_) {
    astcenc_context_free(context_ldr_);
  }
  if (context_hdr_) {
    astcenc_context_free(context_hdr_);
  }
  if (context_rg_) {
    astcenc_context_free(context_rg_);
  }
}

bool TextureProcessor::SwitchToLdr() {
  if (context_hdr_) {
    astcenc_context_free(context_hdr_);
    context_hdr_= nullptr;
  }
  if (context_rg_) {
    astcenc_context_free(context_rg_);
    context_rg_= nullptr;
  }
  if (!context_ldr_) {
    return InitContextLdr();
  }
  return true;
}
bool TextureProcessor::SwitchToHdr() {
  if (context_ldr_) {
    astcenc_context_free(context_ldr_);
    context_ldr_= nullptr;
  }
  if (context_rg_) {
    astcenc_context_free(context_rg_);
    context_rg_= nullptr;
  }
  if (!context_hdr_) {
    return InitContextHdr();
  }
  return true;
}
bool TextureProcessor::SwitchToRG() {
  if (context_ldr_) {
    astcenc_context_free(context_ldr_);
    context_ldr_= nullptr;
  }
  if (context_hdr_) {
    astcenc_context_free(context_hdr_);
    context_hdr_= nullptr;
  }
  if (!context_rg_) {
    return InitContextRG();
  }
  return true;
}

bool TextureProcessor::InitContextLdr() {
  using namespace faithful;
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
    std::cerr << "Error: astc-enc ldr codec config init failed: "
              << astcenc_get_error_string(status) << std::endl;
    return false;
  }

  status = astcenc_context_alloc(
      &config, thread_pool_->GetThreadNumber(), &context_ldr_);
  if (status != ASTCENC_SUCCESS) {
    std::cerr << "Error: astc-enc ldr codec context alloc failed: "
              << astcenc_get_error_string(status) << std::endl;
    return false;
  }
  return true;
}
bool TextureProcessor::InitContextHdr() {
  using namespace faithful;
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
    std::cerr << "Error: astc-enc hdr codec config init failed: "
              << astcenc_get_error_string(status) << std::endl;
    return false;
  }

  status = astcenc_context_alloc(
      &config, thread_pool_->GetThreadNumber(), &context_hdr_);
  if (status != ASTCENC_SUCCESS) {
    std::cerr << "Error: astc-enc hdr codec context alloc failed: "
              << astcenc_get_error_string(status) << std::endl;
    return false;
  }
  return true;
}
bool TextureProcessor::InitContextRG() {
  using namespace faithful;
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
    std::cerr << "Error: astc-enc rg codec config init failed: "
              << astcenc_get_error_string(status) << std::endl;
    return false;
  }

  status = astcenc_context_alloc(
      &config, thread_pool_->GetThreadNumber(), &context_rg_);
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
                               std::unique_ptr<uint8_t[]> image_data,
                               int width, int height,
                               AssetCategory category) {
  if (encode_) {
    Encode(dest_path, std::move(image_data), width, height, category);
  } else {
    std::cerr
        << "TextureProcessor::Process as a decoder with nested data NOT EXIST"
        << std::endl;
    std::terminate();
  }
}

bool TextureProcessor::Encode(const std::filesystem::path& texture_path,
                              AssetCategory category) {
  using namespace faithful;
  int image_x, image_y, image_c;
  // force 4 component (astc codec requires it)
  auto image_data = (uint8_t*)stbi_load(
      texture_path.string().c_str(), &image_x, &image_y, &image_c, 4);
  if (!image_data) {
    std::cerr << "Error: stb_image texture loading failed: " << texture_path
              << std::endl;
    return false;
  }
  std::unique_ptr<uint8_t[]> image_data_ptr(image_data);

  /// extension replaced inside the ProvideEncodeContextAndFilename()
  auto dest_path = asset_destination_ /
                   texture_path.lexically_relative(user_asset_root_dir_);
  // structure binding can't be used with capturing (context used in lambda further)
  std::pair<astcenc_context*, std::string> res =
      ProvideEncodeContextAndFilename(dest_path, category);
  astcenc_context* context = res.first;
  std::string out_filename = res.second;

  int comp_len = CalculateCompLen(image_x, image_y);
  auto comp_data = std::make_unique<uint8_t[]>(comp_len);

  astcenc_image image;
  image.dim_x = image_x;
  image.dim_y = image_y;
  image.dim_z = 1;
  if (category != AssetCategory::kTextureHdr) {
    image.data_type = static_cast<astcenc_type>(config::kTexLdrDataType);
  } else {
    image.data_type = static_cast<astcenc_type>(config::kTexHdrDataType);
  }
  image.data = reinterpret_cast<void**>(&image_data);

  // no need to make it atomic, only "fail"-thread write
  bool encode_success = true;
  thread_pool_->Execute([=, comp_data_get = comp_data.get(),
                         &image, &encode_success](int thread_id) {
    astcenc_error status = astcenc_compress_image(
        context, &image, &config::kTexCompSwizzle,
        comp_data_get, comp_len, thread_id);
    if (status != ASTCENC_SUCCESS) {
      encode_success = false;
    }
  });

  if (!encode_success) {
    std::cerr << "Error: astc-enc texture compression failed" << std::endl;
    return false;
  }

  return WriteEncodedData(out_filename, image_x, image_y,
                          comp_len, comp_data.get());
}

bool TextureProcessor::Encode(const std::filesystem::path& dest_path,
                              std::unique_ptr<uint8_t[]> image_data,
                              int width, int height,
                              AssetCategory category) {
  using namespace faithful;
  // structure binding can't be used with capturing (used in lambda further)
  std::pair<astcenc_context*, std::string> res =
      ProvideEncodeContextAndFilename(dest_path, category);
  astcenc_context* context = res.first;
  auto out_filename = res.second;

  int comp_len = CalculateCompLen(width, height);
  auto comp_data = std::make_unique<uint8_t[]>(comp_len);

  astcenc_image image;
  image.dim_x = width;
  image.dim_y = height;
  image.dim_z = 1;
  if (category != AssetCategory::kTextureHdr) {
    image.data_type = static_cast<astcenc_type>(config::kTexLdrDataType);
  } else {
    image.data_type = static_cast<astcenc_type>(config::kTexHdrDataType);
  }

  /// need valid l-value pointer line further
  auto data_ptr = reinterpret_cast<void*>(image_data.get());
  image.data = reinterpret_cast<void**>(&data_ptr);

  // no need to make it atomic, only "fail"-thread write
  bool encode_success = true;
  thread_pool_->Execute([context, comp_len,
                         comp_data_get = comp_data.get(),
                         &image, &encode_success](int thread_id) {
    astcenc_error status = astcenc_compress_image(
        context, &image, &config::kTexCompSwizzle,
        comp_data_get, comp_len, thread_id);
    if (status != ASTCENC_SUCCESS) {
      encode_success = false;
    }
  });

  if (!encode_success) {
    std::cerr << "Error: astc-enc texture compression failed" << std::endl;
    return false;
  }

  return WriteEncodedData(out_filename, width, height,
                          comp_len, comp_data.get());
}

int TextureProcessor::CalculateCompLen(int image_x, int image_y) {
  using namespace faithful;
  int block_count_x =
      (image_x + config::kTexCompBlockX - 1) / config::kTexCompBlockX;
  int block_count_y =
      (image_y + config::kTexCompBlockY - 1) / config::kTexCompBlockY;
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
      break;
    case AssetCategory::kTextureHdr:
      SwitchToHdr();
      context = context_hdr_;
      /// if we don't have "_hdr" we should add it here
      /// texture still can have such category if some models
      /// will identify it as a normal map or MetallicRoughness
      if (DetectTexHdrSuffix(dest_path)) {
      } else {
        auto new_stem = dest_path.stem().string();
        new_stem += "_rrrg";
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
      } else {
        auto new_stem = dest_path.stem().string();
        new_stem += "_rrrg";
        new_filename.replace_filename(new_stem);
      }
      break;
    default:
      std::terminate(); // you shouldn't be there
  }
  new_filename.replace_extension("astc");

  astcenc_compress_reset(context);
  return {context, new_filename};
}

bool TextureProcessor::WriteEncodedData(std::string filename,
                                        unsigned int image_x,
                                        unsigned int image_y,
                                        int comp_data_size,
                                        const uint8_t* comp_data) {
  using namespace faithful;
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

  header.block_x = config::kTexCompBlockX;
  header.block_y = config::kTexCompBlockY;
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
  using namespace faithful;
  auto context = ProvideDecodeContext(category);

  int image_x, image_y, comp_len;
  std::unique_ptr<uint8_t[]> comp_data;
  if (!ReadAstcFile(texture_path.string(), image_x, image_y, comp_len, comp_data)) {
    return false;
  }
  auto image_data = std::make_unique<uint8_t[]>(image_x * image_y * 4);

  astcenc_image image;
  image.dim_x = image_x;
  image.dim_y = image_y;
  image.dim_z = 1;
  if (category != AssetCategory::kTextureHdr) {
    image.data_type = static_cast<astcenc_type>(config::kTexLdrDataType);
  } else {
    image.data_type = static_cast<astcenc_type>(config::kTexHdrDataType);
  }
  image.data = reinterpret_cast<void**>(&image_data);

  // no need to make it atomic, only "fail"-thread write
  bool decode_success = true;
  thread_pool_->Execute([=, comp_data_get = comp_data.get(),
                         &image, &decode_success](int thread_id) {
    astcenc_error status = astcenc_decompress_image(
        context, comp_data_get, comp_len, &image,
        &config::kTexCompSwizzle, thread_id);
    if (status != ASTCENC_SUCCESS) {
      decode_success = false;
    }
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
                          category, std::move(image_data));
}

bool TextureProcessor::WriteDecodedData(
    std::string filename, unsigned int image_x, unsigned int image_y,
    AssetCategory category, std::unique_ptr<uint8_t[]> image_data) {
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
                                    std::unique_ptr<uint8_t[]>& comp_data) {
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
  comp_data = std::make_unique<uint8_t[]>(comp_len);
  file.read(reinterpret_cast<char*>(comp_data.get()), comp_len);
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
