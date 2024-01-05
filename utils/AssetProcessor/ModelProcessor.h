#ifndef ASSETPROCESSOR_MODELPROCESSOR_H
#define ASSETPROCESSOR_MODELPROCESSOR_H

#include <filesystem>
#include <string>

#include "document.h"

#include "../../config/Paths.h"

class AssetsAnalyzer;
enum class AssetCategory;

// TODO: default value FAITHFUL_TEMP_DIR, which should be
//   created&deleted only by AssetProcessor_class

class ModelProcessor {
 public:
  ModelProcessor(
      bool encode, AssetsAnalyzer* assets_analyzer,
      const std::filesystem::path& user_asset_root_dir,
      const std::filesystem::path& asset_destination,
      const std::filesystem::path& temp_dir = FAITHFUL_ASSET_TEMP_TEXTURES_PATH)
      : encode_(encode),
        assets_analyzer_(assets_analyzer),
        user_asset_root_dir_(user_asset_root_dir),
        asset_destination_(asset_destination),
        extracted_textures_dir_(temp_dir) {
  }

  void Process(const std::filesystem::path& model_path,
               const std::filesystem::path& path_suffix);

  void EncodeGlbModel(const std::filesystem::path& model_path,
                      const std::filesystem::path& path_suffix);
  void DecodeGlbModel(const std::filesystem::path& model_path,
                      const std::filesystem::path& path_suffix);

  void EncodeGltfModel(const std::filesystem::path& model_path,
                       const std::filesystem::path& path_suffix);
  void DecodeGltfModel(const std::filesystem::path& model_path,
                       const std::filesystem::path& path_suffix);

  void ExtractBase64ImageData(const std::string& uri,
                              const std::filesystem::path& path_suffix,
                              const std::filesystem::path& out_filename,
                              AssetCategory category);

  void ExtractBufferImageData(int buffer_view_id,
                              const std::string& out_filename);

  void ExtractExternalImageData(const std::string& uri,
                                const std::filesystem::path& path_suffix,
                                bool dir_created);

  void EncodeGltfModelTextures(const std::filesystem::path& path_suffix,
                               rapidjson::Value& images,
                               rapidjson::Document& document);

 private:
  AssetsAnalyzer* assets_analyzer_;
  /// for temporary images and their further encoding
  std::filesystem::path extracted_textures_dir_;

  std::filesystem::path asset_destination_;

  /// useful when model has external texture to understand
  /// has it been processed before
  std::filesystem::path user_asset_root_dir_;

  bool encode_;
};

#endif  // ASSETPROCESSOR_MODELPROCESSOR_H
