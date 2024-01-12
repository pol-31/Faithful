#ifndef FAITHFUL_ASSETSINFO_H
#define FAITHFUL_ASSETSINFO_H

#include <filesystem>

#include "AssetCategory.h"
#include "AssetLoadingThreadPool.h"
#include "AssetInfo.h"

#include "../../config/Paths.h"

class AssetsAnalyzer {
 public:
  AssetsAnalyzer(AssetLoadingThreadPool& thread_pool, bool force = false)
      : thread_pool_(thread_pool),
        force_(force) {
  }

  void AddEntry(AssetInfo&& new_asset);

  void GatherEncodeThreadInfo(
      const std::filesystem::path& root_dir,
      const std::filesystem::path& path1 = FAITHFUL_ASSET_AUDIO_PATH,
      const std::filesystem::path& path2 = FAITHFUL_ASSET_MODEL_PATH,
      const std::filesystem::path& path3 =
          FAITHFUL_ASSET_TEXTURE_PATH);  // TODO: rename
  void GatherDecodeThreadInfo(const std::filesystem::path& root_dir);

  /** Prevents rewriting existing "active" assets by newer. Called
   * inside the GatherEncodeThreadInfo(), but may be called additionally.
   * Faithful has three dirs for assets (but nothing prevent us from
   * using the function multiple times)
   * */
  void LoadActiveAssets(
      const std::filesystem::path& path = FAITHFUL_ASSET_PATH);

 private:
  void AnalyzePath(const std::filesystem::path& path);
  void AnalyzeDir(const std::filesystem::path& filename,
                  const std::filesystem::path& root_dir);

  bool AreAssetsEqual(const AssetInfo& new_asset, const AssetInfo& old_asset);

  bool ReplaceAssetIORequest(const AssetInfo& new_asset);

  void SubmitTask(AssetInfo&& new_asset);

  std::vector<AssetInfo> processed_assets_;
  std::vector<AssetInfo> unprocessed_assets;

  AssetLoadingThreadPool& thread_pool_;

  bool force_;  // always replace files with identical names

  // options if filename the same but path or size has been changed
  bool all_replace_ = false;
  bool all_keep_ = false;
};

#endif  // FAITHFUL_ASSETSINFO_H
