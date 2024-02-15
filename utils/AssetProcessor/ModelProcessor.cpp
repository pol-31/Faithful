#include "ModelProcessor.h"

#include <iostream>

#include "TextureProcessor.h"

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include <cstdio>
#include <string>

ModelProcessor::ModelProcessor(
    bool encode,
    const std::filesystem::path& asset_destination,
    const std::filesystem::path& user_asset_root_dir,
    TextureProcessor* texture_processor)
    : texture_processor_(texture_processor),
      asset_destination_(asset_destination),
      user_asset_root_dir_(user_asset_root_dir),
      encode_(encode) {
  FindLastTextureId();
  loader_.SetPreserveImageChannels(true); //  4-channel loading
}

// TODO: use exceptions instead of bool return values

void ModelProcessor::Process(std::filesystem::path model_path) {
  cur_model_path_ = model_path;
  std::cerr << cur_model_path_ << std::endl;
  auto suffix = model_path.lexically_relative(user_asset_root_dir_);
  auto destination = asset_destination_ / suffix;
  destination.replace_extension("gltf");
  Read();
  if (encode_) {
    CompressTextures();
  } else {
    DecompressTextures();
  }
  if (Write(destination.string())) {
    return;
  }
  if (encode_) {
    if (OptimizeModel(destination.string())) {
      return;
    }
  }
}

bool ModelProcessor::Read() {
  bool ret = loader_.LoadASCIIFromFile(&model_, &error_string_,
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
  bool ret = loader_.WriteGltfSceneToFile(&model_, destination,
                                         false, false, true, false);
  if (!ret) {
    std::cerr << "Failed to write GLTF file: " << destination << std::endl;
    return false;
  }

  std::cout << "GLTF file successfully saved to: " << destination << std::endl;
  return true;
}

void ModelProcessor::CompressTextures() {
  std::string out_texture_name;
//  TODO: out_texture_name.reserve(7);
  int cur_id;
  for (int i = 0; i < model_.images.size(); ++i) {
    tinygltf::Image& image = model_.images[i];
    std::cout << image.uri << std::endl;
    if (!image.uri.empty()) {
      cur_id = MarkImageAsAProcessed(image.uri);
    } else {
      cur_id = ++last_texture_id_;
    }
    out_texture_name += std::to_string(cur_id);
    auto category = DeduceTextureCategory(i);
    if (category == AssetCategory::kTextureRG) {
      out_texture_name += "_rrrg";
    }
    out_texture_name += ".astc";

    auto image_data = std::make_unique<uint8_t>(image.image.size());
    int width = image.width;
    int height = image.height;
    std::move(image.image.begin(), image.image.end(), image_data.get());
    image.image.clear();

    std::cerr << cur_model_path_.string() << "--" << std::endl;
//    std::filesystem::path texture_path = "/home/pavlo/Desktop/from/damaged_helmet/untitled.gltf";//cur_model_path_.string();
//    texture_path = texture_path.parent_path();
//    std::cerr << texture_path.string() << "--" << std::endl;
//    texture_path /= out_texture_name;
    texture_processor_->Process("/home/pavlo/Desktop/to/damaged_helmet/1.png", std::move(image_data),
                                width, height, category);

    image.uri = out_texture_name;
    image.mimeType.clear();
    image.name.clear();
  }
}

AssetCategory ModelProcessor::DeduceTextureCategory(int image_id) {
  auto category = AssetCategory::kTextureLdr;
  for (const auto& material : model_.materials) {
    if (image_id == material.normalTexture.index ||
        image_id == material.pbrMetallicRoughness.baseColorTexture.index ||
        image_id == material.pbrMetallicRoughness.metallicRoughnessTexture.index ||
        image_id == material.occlusionTexture.index) {
      category = AssetCategory::kTextureRG;
      // can't be Hdr (according to gltf 2.0)
    }
  }
  return category;
}

int ModelProcessor::MarkImageAsAProcessed(const std::string& image_path) {
  for (const auto& img : processed_images_) {
    if (img.path == image_path) {
      return img.id;
    }
  }
  int new_id = ++last_texture_id_;
  processed_images_.push_back({image_path, new_id});
  return new_id;
}

void ModelProcessor::DecompressTextures() {
  /// as an input we have model ONLY with uri of images
  // rapidjson requires std::FILE
  std::FILE* fp = fopen(cur_model_path_.c_str(), "rb");
  char buffer[65536]; // TODO: may have sense take less size
  rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
  rapidjson::Document doc;
  doc.ParseStream(is);
  fclose(fp);

  if (!doc.IsObject()) {
    std::cerr << "ModelProcessor::DecompressTextures Invalid GLTF file format"
              << std::endl;
  }
  rapidjson::Value& images = doc["images"];
  if (!images.IsArray()) {
    std::cerr << "ModelProcessor::DecompressTextures No images found in GLTF file"
              << std::endl;
  }

  for (rapidjson::SizeType i = 0; i < images.Size(); ++i) {
    rapidjson::Value& image = images[i];
    if (image.HasMember("uri") && image["uri"].IsString()) {
      std::string uri = image["uri"].GetString();
      size_t dotPos = uri.find_last_of(".");
      if (dotPos != std::string::npos) {
        /// textures decoded to png
        uri.replace(dotPos, uri.length() - dotPos, ".png");
        rapidjson::Value newUri;
        newUri.SetString(uri.c_str(), static_cast<rapidjson::SizeType>(uri.length()),
                         doc.GetAllocator());
        image["uri"] = newUri;
      }
    }
  }

  auto json_destination = asset_destination_ /
                     cur_model_path_.lexically_relative(user_asset_root_dir_);
  FILE* fpOut = fopen(json_destination.c_str(), "wb");
  rapidjson::FileWriteStream os(fpOut, buffer, sizeof(buffer));
  rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
  doc.Accept(writer);
  fclose(fpOut);

  auto bin_file = cur_model_path_.replace_extension("bin");
  if (std::filesystem::exists(bin_file)) {
    auto bin_destination = asset_destination_ /
                           bin_file.lexically_relative(user_asset_root_dir_);
    std::filesystem::rename(bin_file, bin_destination);
  }

  // models textures are already processed by AssetProcessor
}

void ModelProcessor::FindLastTextureId() {
  for (const auto& image : std::filesystem::directory_iterator(
           asset_destination_ / "../models_textures")) {
    int number = std::stoi(image.path().stem());
    if (last_texture_id_ < number) {
      last_texture_id_ = number;
    }
  }
}

// in-place gltfpack
bool ModelProcessor::OptimizeModel(const std::string& destination) {
  std::string command;
  command.reserve(std::strlen(FAITHFUL_GLTFPACK_PATH) +
                  destination.size() * 2 + 12); // 12 for flags, whitespaces
  command = FAITHFUL_GLTFPACK_PATH;
  command += " -tr -i "; // no changes for textures
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
