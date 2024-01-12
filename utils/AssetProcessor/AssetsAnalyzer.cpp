#include "AssetsAnalyzer.h"

#include <filesystem>
#include <iostream>

#include "AssetCategory.h"
#include "AssetLoadingThreadPool.h"

void AssetsAnalyzer::LoadActiveAssets(const std::filesystem::path& path) {
  // TODO: need to check is directory not "active" (not used by Faithful)
  AnalyzePath(path);
  for (auto& entry : unprocessed_assets) {
    entry.category = DeduceAssetDecodeCategory(entry.source_path);
    entry.source_path.clear();
  }
  processed_assets_.insert(processed_assets_.end(),
                           std::make_move_iterator(unprocessed_assets.begin()),
                           std::make_move_iterator(unprocessed_assets.end()));
  unprocessed_assets.clear();
}

void AssetsAnalyzer::GatherEncodeThreadInfo(
    const std::filesystem::path& root_dir, const std::filesystem::path& path1,
    const std::filesystem::path& path2, const std::filesystem::path& path3) {
  // TODO: need to check is directory not "active" (not used by Faithful)
  LoadActiveAssets(path1);
  LoadActiveAssets(path2);
  LoadActiveAssets(path3);
  AnalyzePath(root_dir);
  std::cout << "size of unprocessed in AssetsAnalyzer: "
            << unprocessed_assets.size() << std::endl;
  for (auto& entry : unprocessed_assets) {
    entry.category = DeduceAssetEncodeCategory(entry.source_path);
    AddEntry(std::move(entry));
  }
  unprocessed_assets.clear();
}

void AssetsAnalyzer::GatherDecodeThreadInfo(
    const std::filesystem::path& root_dir) {
  // TODO: need to check is directory not "active" (not used by Faithful)
  AnalyzePath(root_dir);
  for (auto& entry : unprocessed_assets) {
    entry.category = DeduceAssetDecodeCategory(entry.source_path);
    AddEntry(std::move(entry));
  }
  unprocessed_assets.clear();
}

/// used for encoding, decoding, getting active assets info
/// We ARE NOT deducing category, but only source_path and relative_path
/// encoding/decoding:_ after call DeduceCategory for each entry
/// asset info:________ we can delete source path for each (need only relative
/// path)
void AssetsAnalyzer::AnalyzeDir(const std::filesystem::path& cur_dir,
                                const std::filesystem::path& root_dir) {
  for (const std::filesystem::path& entry :
       std::filesystem::directory_iterator(cur_dir)) {
    if (std::filesystem::is_regular_file(entry)) {
      /// AssetCategory will be recomputed after
      unprocessed_assets.emplace_back(
          entry.string(), entry.lexically_relative(root_dir).string(),
          AssetCategory::kUnknown);
    } else if (std::filesystem::is_directory(entry)) {
      AnalyzeDir(entry, root_dir);
    }
  }
}

void AssetsAnalyzer::AnalyzePath(const std::filesystem::path& path) {
  if (std::filesystem::is_regular_file(path)) {
    unprocessed_assets.emplace_back(path.string(), ".",
                                    AssetCategory::kUnknown);
  } else if (std::filesystem::is_directory(path)) {
    AnalyzeDir(path, path);
  } else {
    std::cout
        << "Error (AssetsInfo): incorrect path, should be file or directory"
        << std::endl;
  }
}

// COMPARE ONLY AFTER PROCESSING (filename with relative path)
bool AssetsAnalyzer::AreAssetsEqual(const AssetInfo& new_asset,
                                    const AssetInfo& old_asset) {
  // for hdr and ldr names can't be the same, because for future possible
  // decompress format differs (ldr Vs hdr format).
  // hdr can have the same name with ldr or nmap, but
  // ldr with nmap can't have the same name
  if (new_asset.category != old_asset.category) {
    if (!(new_asset.category == AssetCategory::kTextureLdr &&
          old_asset.category == AssetCategory::kTextureNmap) &&
        !(new_asset.category == AssetCategory::kTextureNmap &&
          old_asset.category == AssetCategory::kTextureLdr)) {
      return false;
    }
  }
  if (new_asset.relative_path == old_asset.relative_path) {
    return true;
  } else {
    return false;
  }
}

void AssetsAnalyzer::SubmitTask(AssetInfo&& new_asset) {
  /// AddEntry (called before SubmitTask) has already discarded
  /// AssetCategory kBinary and kUnknown
  switch (new_asset.category) {
    case AssetCategory::kAudioSound:
      [[fallthrough]];
    case AssetCategory::kModel:
      thread_pool_.PutSingleThreadedTask(std::move(new_asset));
      break;
    case AssetCategory::kAudioMusic:
      [[fallthrough]];
    default:  // ldr, hdr, nmap textures
      thread_pool_.PutMultiThreadedTask(std::move(new_asset));
  }
}

// TODO 0: namespace fs ?
// TODO 1: sort tasks textures(because all full-threaded) --> audio -->
// models(all single-threaded and not balances?)
// TODO 2: replace filenames -->-> LoadActiveAssets

// TODO: PRECAUTION section
// what if model has texture which is outside the considered dir and which name
// can coincide with already processed? -- we add __PRECAUTION__ section
// so if texture is outside, then we don't process it.
// if embedded - mark as ${model name},
// if not - we've already processed it on texture processing step, so
// just changing path inside model SO if it wasn't processed, we're getting
// error only in Faithful runtime, or not even an error (depends on how there
// textures are loaded))
void AssetsAnalyzer::AddEntry(AssetInfo&& new_asset) {
  switch (new_asset.category) {
    case AssetCategory::kBinary:
      [[fallthrough]];
    case AssetCategory::kUnknown:
      return;
    default:
      break;
  }
  for (const auto& old_asset : processed_assets_) {
    if (AreAssetsEqual(
            new_asset,
            old_asset)) {  // TODO: need to process either one or another
      if (ReplaceAssetIORequest(new_asset)) {
        SubmitTask(std::move(new_asset));
        std::cout << "Processed: " << new_asset.source_path << std::endl;
        return;
      } else {
        std::cout << "Skip: " << new_asset.source_path
                  << " (replace request denied)" << std::endl;
        return;
      }
    }
    /**
    /// we can compare size, possible extensions here (missing by now)
    else {
      std::cout << "Skip: " << new_asset.source_path
                << " (already processed)" << std::endl;
    }*/
    SubmitTask(std::move(new_asset));
    std::cout << "Processed: " << new_asset.source_path << std::endl;
    return;
  }
  SubmitTask(std::move(new_asset));
  std::cout << "Processed: " << new_asset.source_path << std::endl;
}

bool AssetsAnalyzer::ReplaceAssetIORequest(const AssetInfo& new_asset) {
  if (force_)
    return true;
  if (all_keep_) {
    return false;
  } else if (all_replace_) {
    return true;
  }
  std::cout << "Do you want to replace "
            << std::filesystem::path(new_asset.source_path).filename() << " by "
            << new_asset.source_path
            << "?\ny(yes), n(no), a(yes for all), 0(no for all))" << std::endl;
  // "y" means "yes"
  // "n" means "no"
  // "a" means "yes for all"
  // "0" means "no for all"
  char response;
  std::cin >> response;
  if (response == 'y' || response == 'Y') {
    return true;
  } else if (response == 'n' || response == 'N') {
    return false;
  } else if (response == 'a' || response == 'A') {
    all_replace_ = true;
  } else if (response == '0') {
    all_keep_ = true;
  }
  return false;
}