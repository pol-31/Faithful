#include "ModelProcessor.h"

#include <iostream>

#include "../../config/Paths.h"

bool TinygltfLoadTextureStub(tinygltf::Image *image, const int image_idx,
                             std::string *err, std::string *warn, int req_width,
                             int req_height, const unsigned char *bytes,
                             int size, void *user_data) {
  (void)image, (void)image_idx, (void)err, (void)warn, (void)req_width,
      (void)req_height, (void)bytes, (void)size, (void)user_data;
  /// main logic provided by ModelProcessor::DecompressTextures()
  return true;
}

ModelProcessor::ModelProcessor(
    TextureProcessor& texture_processor,
    ReplaceRequest& replace_request)
    : texture_processor_(texture_processor),
      replace_request_(replace_request) {
  /// force 4-channel loading, mandatory for astc
  loader_.SetPreserveImageChannels(true);
  /// while decompression we load images on our own because of ".astc" extension,
  /// which is not supported by GLTF 2.0 spec
  loader_.SetImageWriter(nullptr, nullptr);
}

void ModelProcessor::SetDestinationDirectory(
    const std::filesystem::path& path) {
  models_destination_path_ = path / "models";
  std::filesystem::create_directories(models_destination_path_);
}

// TODO: exceptions

void ModelProcessor::Encode(const std::filesystem::path& path) {
  std::string out_filename =
      (models_destination_path_ / path.filename().
                                  replace_extension(".gltf")).string();
  if (std::filesystem::exists(out_filename)) {
    std::string request{out_filename};
    request += "\nalready exist. Do you want to replace it?";
    if (!replace_request_(std::move(request))) {
      return;
    }
  }
  processed_images_.clear();
  cur_model_path_ = path;
  loader_.SetImageLoader(&tinygltf::LoadImageData, nullptr);
  Read();
  CompressTextures();
  Write(out_filename);
  OptimizeModel(out_filename);
}
void ModelProcessor::Decode(const std::filesystem::path& path) {
  /// extension already ".astc"
  std::string out_filename =
      (models_destination_path_ / path.filename()).string();
  if (std::filesystem::exists(out_filename)) {
    std::string request{out_filename};
    request += "\nalready exist. Do you want to replace it?";
    if (!replace_request_(std::move(request))) {
      return;
    }
  }
  processed_images_.clear();
  cur_model_path_ = path;
  loader_.SetImageLoader(TinygltfLoadTextureStub, nullptr);
  Read();
  DecompressTextures();
  Write(out_filename);
}

bool ModelProcessor::Read() {
  model_ = std::make_unique<tinygltf::Model>();
  bool ret = loader_.LoadASCIIFromFile(model_.get(), &error_string_,
                                       &warning_string_, cur_model_path_);
  if (!warning_string_.empty()) {
    std::cerr << "Warning: " << warning_string_ << std::endl;
  }
  if (!error_string_.empty()) {
    std::cerr << "Error: " << error_string_ << std::endl;
    return false;
  }
  if (!ret) {
    std::cerr << "Failed to load GLTF file: " << cur_model_path_ << std::endl;
    return false;
  }
  return true;
}

bool ModelProcessor::Write(const std::string& destination) {
  bool ret = loader_.WriteGltfSceneToFile(model_.get(), destination,
                                         false, false, true, false);
  if (!ret) {
    std::cerr << "Failed to write GLTF file: " << destination << std::endl;
    return false;
  }

  std::cout << "GLTF file successfully saved to: " << destination << std::endl;
  return true;
}

void ModelProcessor::CompressTextures() {
  for (std::size_t i = 0; i < model_->images.size(); ++i) {
    tinygltf::Image& image = model_->images[i];
    if (!image.uri.empty()) {
      processed_images_.push_back((cur_model_path_ / image.uri).lexically_normal());
    }
    auto model_texture_config = DeduceEncodeTextureCategory(i);

    // forced 4 channels (see ctor)
    int total_len = image.width * image.height * 4;
    auto image_data = std::make_unique<uint8_t[]>(total_len);
    std::move(image.image.begin(), image.image.end(), image_data.get());
    image.image.clear();

    /// relative path (i.e. in the same directory)
    image.uri = model_texture_config.out_path.filename().string();
    image.mimeType.clear();
    image.name.clear();

    texture_processor_.Encode(
        model_texture_config.out_path, std::move(image_data),
        image.width, image.height, model_texture_config.category);
  }
}

void ModelProcessor::DecompressTextures() {
  for (std::size_t i = 0; i < model_->images.size(); ++i) {
    tinygltf::Image& image = model_->images[i];
    auto texture_path = (cur_model_path_.parent_path() / image.uri);
    processed_images_.push_back(texture_path.string());

    auto model_texture_config = DeduceDecodeTextureCategory(
        texture_path.stem().string());

    image.uri = std::filesystem::path(image.uri)
                    .replace_extension(".png").string();
    std::cout << "Load from " << texture_path << " to " << model_texture_config.out_path << std::endl;
    texture_processor_.Decode(
        texture_path, model_texture_config.out_path,
        model_texture_config.category);
  }
}

// TODO: rename
ModelProcessor::ModelTextureConfig ModelProcessor::DeduceEncodeTextureCategory(
    int model_image_id) {
  auto out_path = (models_destination_path_ / cur_model_path_.stem()).string();
  TextureProcessor::TextureCategory category;
  /// our models consist only of one material, so we use it
  if (model_image_id == model_->materials[0].pbrMetallicRoughness
                            .metallicRoughnessTexture.index) {
    out_path += "_met_rough.astc";
    category = TextureProcessor::TextureCategory::kLdrRg;
  } else if (model_image_id == model_->materials[0].normalTexture.index) {
    out_path += "_normal.astc";
    category = TextureProcessor::TextureCategory::kLdrRgNmap;
  } else if (model_image_id == model_->materials[0].occlusionTexture.index) {
    out_path += "_occlusion.astc";
    category = TextureProcessor::TextureCategory::kLdrR;
  } else if (model_image_id == model_->materials[0].emissiveTexture.index) {
    out_path += "_emissive.astc";
    category = TextureProcessor::TextureCategory::kLdrRgb;
  } else { // albedo (material.pbrMetallicRoughness.baseColorTexture.index)
    out_path += "_albedo.astc";
    category = TextureProcessor::TextureCategory::kLdrRgba;
  }
  return {out_path, category};
}

ModelProcessor::ModelTextureConfig ModelProcessor::DeduceDecodeTextureCategory(
    std::string_view path) {
  auto out_path = (models_destination_path_ / path).replace_extension(".png")
                      .string();
  TextureProcessor::TextureCategory category;
  if (path.ends_with("_met_rough")) {
    category = TextureProcessor::TextureCategory::kLdrRg;
  } else if (path.ends_with("_normal")) {
    category = TextureProcessor::TextureCategory::kLdrRgNmap;
  } else if (path.ends_with("_occlusion")) {
    category = TextureProcessor::TextureCategory::kLdrR;
  } else if (path.ends_with("_emissive")) {
    category = TextureProcessor::TextureCategory::kLdrRgb;
  } else { // probably for "_albedo"
    category = TextureProcessor::TextureCategory::kLdrRgba;
  }
  return {out_path, category};
}

// TODO: constexpr std::string?
bool ModelProcessor::OptimizeModel(const std::string& destination) {
  std::string command;
  command.reserve(std::strlen(FAITHFUL_GLTFPACK_PATH) +
                  destination.size() * 2 + 17); // 17 for flags, whitespaces
  command = FAITHFUL_GLTFPACK_PATH;
  command += " -tr -noq -i "; // no changes for textures
  command += destination;
  command += " -o ";
  command += destination;
  if (std::system(command.c_str()) != 0) {
    std::cerr << "Error: unable to optimize model" << std::endl;
    return false;
  }
  std::cout << "Optimized" << std::endl;
  return true;
}
