#ifndef FAITHFUL_ASSETSINFO_H
#define FAITHFUL_ASSETSINFO_H

#include <filesystem>

#include "AssetLoadingThreadPool.h"

#include "ReplaceRequest.h"

/// Collects full paths of all assets, that should be processed.
/// All assets withing the same category should have distinct names,
/// otherwise user gets warning message, but program still valid.
/// supported asset formats provided inside the Faithful/config/AssetFormats.h
class AssetsAnalyzer {
 public:
  AssetsAnalyzer() = delete;
  AssetsAnalyzer(const std::filesystem::path& path, bool encode);

  AssetsAnalyzer(const AssetsAnalyzer&) = default;
  AssetsAnalyzer& operator=(const AssetsAnalyzer&) = default;

  AssetsAnalyzer(AssetsAnalyzer&) = default;
  AssetsAnalyzer& operator=(AssetsAnalyzer&&) = default;

  const std::vector<std::filesystem::path>& GetAudioToProcess() const {
    return audio_to_process_;
  }
  const std::vector<std::filesystem::path>& GetModelsToProcess() const {
    return models_to_process_;
  }
  const std::vector<std::filesystem::path>& GetTexturesToProcess() const {
    return textures_to_process_;
  }

 private:
  enum class AssetCategory {
    kAudio,
    kModel,
    kTexture,
    kUnknown
  };

  void SortEntries();

  void AnalyzePath(const std::filesystem::path& path);
  void AnalyzeDir(const std::filesystem::path& path);

  void AddEntry(const std::filesystem::path& path);
  void AddEntryImpl(const std::filesystem::path& new_asset,
                    std::vector<std::filesystem::path>& assets);

  AssetCategory DeduceAssetCategory(const std::filesystem::path& path);

  std::vector<std::filesystem::path> audio_to_process_;
  std::vector<std::filesystem::path> models_to_process_;
  std::vector<std::filesystem::path> textures_to_process_;

  bool encode_;
};

#endif  // FAITHFUL_ASSETSINFO_H
