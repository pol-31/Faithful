#ifndef FAITHFUL_ASSETSINFO_H
#define FAITHFUL_ASSETSINFO_H

#include <filesystem>

#include "AssetCategory.h"
#include "AssetLoadingThreadPool.h"

#include "../../config/Paths.h"

// how to distinguish nmap and ldr - only by gltf model processing
// if texture used in at least one model it's should be u8 ldr (see gltf format info)
// if texture used in at least one model it's decompressed to assets/model_textures

class AssetsAnalyzer {
 public:
  struct AssetInfo {
    AssetInfo(std::string file_path, AssetCategory file_category)
        : source_path(file_path),
          category(file_category) {
    }
    std::filesystem::path source_path;
    AssetCategory category;
  };


  AssetsAnalyzer(
      bool encode,
      const std::filesystem::path asset_destination,
      const std::filesystem::path user_asset_root_dir,
      bool force = false)
      : asset_destination_(asset_destination),
        user_asset_root_dir_(user_asset_root_dir),
        encode_(encode),
        force_(force) {}

  void CollectAssetsInfo(const std::filesystem::path& src_path);


  // should be called after Handle...Duplicates
  const std::vector<AssetInfo>& GetAudioToProcess() const {
    return audio_to_process_;
  }
  std::vector<AssetInfo>& GetModelsToProcess() {
    return models_to_process_;
  }
  const std::vector<AssetInfo>& GetLdrTexturesToProcess() const {
    return ldr_textures_to_process_;
  }
  const std::vector<AssetInfo>& GetHdrTexturesToProcess() const {
    return hdr_textures_to_process_;
  }
  const std::vector<AssetInfo>& GetRGTexturesToProcess() const {
    return rg_textures_to_process_;
  }

  // should be called before Get...ToProcess
  void HandleAudioDuplicates();
  void HandleModelDuplicates();
  void HandleTextureDuplicates();

  /** Prevents rewriting existing "active" assets by newer. Called
   * inside the CollectAssetsInfo(), but may be called additionally
   * */
  void LoadActiveAssets(
      const std::filesystem::path& path = FAITHFUL_ASSET_PATH);

  /// removes _hdr, _rrrg suffixes that serves to distinguish
  /// them in compressed (.astc) format
  static void RemoveTextureSuffix(std::string& path);

 private:
  void AnalyzePath(const std::filesystem::path& path);
  void AnalyzeDir(const std::filesystem::path& filename,
                  const std::filesystem::path& root_dir);

  bool ReplaceAssetIORequest(const std::filesystem::path& asset_path);

  /// for each path (no matter "processed" or "unprocessed")
  /// removes extension and suffixes for "fair" comparison
  void HandleDuplicatesImpl(const std::vector<AssetInfo>& processed,
                             std::vector<AssetInfo>& unprocessed);

  std::vector<AssetInfo> audio_to_process_;
  std::vector<AssetInfo> models_to_process_;
  std::vector<AssetInfo> ldr_textures_to_process_;
  std::vector<AssetInfo> hdr_textures_to_process_;
  std::vector<AssetInfo> rg_textures_to_process_;

  /// WITHOUT EXTENSION
  std::vector<AssetInfo> audio_processed_;
  std::vector<AssetInfo> models_processed_;
  std::vector<AssetInfo> ldr_textures_processed_;
  std::vector<AssetInfo> hdr_textures_processed_;
  std::vector<AssetInfo> rg_textures_processed_;

  // for AssetCategory::kUnknown
  std::vector<AssetInfo> assets_to_process_;

  std::filesystem::path asset_destination_;

  std::filesystem::path user_asset_root_dir_;

  bool force_;  // always replace files with identical names

  // options if filename the same but path or size has been changed
  bool all_replace_ = false;
  bool all_keep_ = false;

  bool encode_;
};

#endif  // FAITHFUL_ASSETSINFO_H
