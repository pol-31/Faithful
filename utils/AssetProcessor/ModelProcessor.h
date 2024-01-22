#ifndef ASSETPROCESSOR_MODELPROCESSOR_H
#define ASSETPROCESSOR_MODELPROCESSOR_H

#include <filesystem>
#include <string>

#include <rapidjson/document.h>

#include "../../config/Paths.h"

class AssetsAnalyzer;
enum class AssetCategory;


// 1 model <-> 1 class instance

class ModelProcessor {
 public:
  ModelProcessor(
      bool encode, AssetsAnalyzer* assets_analyzer,
      const std::filesystem::path& user_asset_root_dir,
      const std::filesystem::path& asset_destination,
      const std::filesystem::path& temp_dir = FAITHFUL_ASSET_TEMP_TEXTURES_PATH)
      : assets_analyzer_(assets_analyzer),
        asset_destination_(asset_destination),
        user_asset_root_dir_(user_asset_root_dir),
        encode_(encode) {
  }

  void Process(const std::filesystem::path& model_path,
               const std::filesystem::path& path_suffix);

  bool ConvertGlbToGltf(
      const std::filesystem::path& model_path,
      const std::filesystem::path& path_suffix);

  bool ExtractBuffers();
  bool ExtractTextures();

  std::string DecodeBase64(const std::string& base64_data);

  bool ExtractGltfModelTextures(const std::filesystem::path& model_path);

  bool EncodeGltfModelTextures(const std::filesystem::path& model_path);
  bool DecodeTextures(const std::filesystem::path& model_path,
                      const std::filesystem::path& path_suffix);

  bool OptimizeModel(const std::string& model_path);

  bool SaveModel(const rapidjson::Document& document,
                 const std::string& model_path);
  bool ReadGltfModel(const std::string& model_path);

 private:
  AssetsAnalyzer* assets_analyzer_;

  std::filesystem::path asset_destination_;

  /// useful when model has external texture to understand
  /// has it been processed before
  std::filesystem::path user_asset_root_dir_;

  rapidjson::Document document;
  bool encode_;
};

#endif  // ASSETPROCESSOR_MODELPROCESSOR_H
