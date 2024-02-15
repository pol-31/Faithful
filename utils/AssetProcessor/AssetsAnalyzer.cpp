#include "AssetsAnalyzer.h"

#include <filesystem>
#include <iostream>

#include "AssetCategory.h"
#include "AssetLoadingThreadPool.h"

void AssetsAnalyzer::LoadActiveAssets(const std::filesystem::path& path) {
  AnalyzePath(path);
  for (auto& entry : assets_to_process_) {
    entry.category = DeduceAssetDecodeCategory(entry.source_path);
    entry.source_path.replace_extension();
    switch (entry.category) {
      case AssetCategory::kTextureLdr:
        ldr_textures_processed_.push_back(entry);
        break;
      case AssetCategory::kTextureHdr:
        hdr_textures_processed_.push_back(entry);
        break;
      case AssetCategory::kTextureRG:
        rg_textures_processed_.push_back(entry);
        break;
      case AssetCategory::kAudioMusic:
        [[fallthrough]];
      case AssetCategory::kAudioSound:
        audio_processed_.push_back(entry);
        break;
      case AssetCategory::kModel:
        models_processed_.push_back(entry);
        break;
      default:
        break; // just ignore
    }
  }
  assets_to_process_.clear();
}

void AssetsAnalyzer::CollectAssetsInfo(
    const std::filesystem::path& src_path) {
  if (encode_) {
    LoadActiveAssets();
  }
  AnalyzePath(src_path);
  std::cout << "Total size of unprocessed in AssetsAnalyzer: "
            << assets_to_process_.size() << std::endl;
  for (auto& entry : assets_to_process_) {
    entry.category = DeduceAssetCategory(entry.source_path, encode_);
    switch (entry.category) {
      case AssetCategory::kTextureLdr:
        ldr_textures_to_process_.push_back(entry);
        break;
      case AssetCategory::kTextureHdr:
        hdr_textures_to_process_.push_back(entry);
        break;
      case AssetCategory::kTextureRG:
        rg_textures_to_process_.push_back(entry);
        break;
      case AssetCategory::kAudioMusic:
        [[fallthrough]];
      case AssetCategory::kAudioSound:
        audio_to_process_.push_back(entry);
        break;
      case AssetCategory::kModel:
        models_to_process_.push_back(entry);
        break;
      default:
        break; // just ignore
    }
  }
  assets_to_process_.clear();
  /// for encoding we should do this manually to solve problem with
  /// duplication the same texture as a module_texture and as a stand-alone texture
  /// for details see AssetProcessor::Process()
  if (!encode_) {
    HandleAudioDuplicates();
    HandleModelDuplicates();
    HandleTextureDuplicates();
  }
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
      assets_to_process_.emplace_back(entry.string(), AssetCategory::kUnknown);
    } else if (std::filesystem::is_directory(entry)) {
      AnalyzeDir(entry, root_dir);
    }
  }
}

void AssetsAnalyzer::AnalyzePath(const std::filesystem::path& path) {
  if (std::filesystem::is_regular_file(path)) {
    assets_to_process_.emplace_back(path.string(), AssetCategory::kUnknown);
  } else if (std::filesystem::is_directory(path)) {
    AnalyzeDir(path, path);
  } else {
    std::cout
        << "Error (AssetsInfo): incorrect path, should be file or directory"
        << std::endl;
  }
}

void AssetsAnalyzer::HandleAudioDuplicates() {
  HandleDuplicatesImpl(audio_processed_, audio_to_process_);
}
void AssetsAnalyzer::HandleModelDuplicates() {
  HandleDuplicatesImpl(models_processed_, models_to_process_);
}
void AssetsAnalyzer::HandleTextureDuplicates() {
  HandleDuplicatesImpl(ldr_textures_processed_, ldr_textures_to_process_);
  HandleDuplicatesImpl(hdr_textures_processed_, hdr_textures_to_process_);
  HandleDuplicatesImpl(rg_textures_processed_, rg_textures_to_process_);
}

void AssetsAnalyzer::HandleDuplicatesImpl(
    const std::vector<AssetInfo>& processed,
    std::vector<AssetInfo>& unprocessed) {
  for (const auto& audio : processed) {
    /// compare relative to their "root" directory paths
    /// ("root" means assets_destination for processed; assets_source for unprocessed)
    auto relative_prefix = audio.source_path.lexically_relative(asset_destination_).string();
    AssetsAnalyzer::RemoveTextureSuffix(relative_prefix);
    /// elements are unique, no need to use std::remove_if
    auto found = std::find_if(
        unprocessed.begin(), unprocessed.end(),
        [&] (const AssetInfo& asset_info) {
          /// compare without extension,
          /// processed don't have extensions (see AssetsAnalyzer.h)
          auto source_path = asset_info.source_path
                                 .lexically_relative(user_asset_root_dir_)
                                 .replace_extension()
                                 .string();
          AssetsAnalyzer::RemoveTextureSuffix(source_path);
          return relative_prefix == source_path;
        });
    if (found != unprocessed.end()) {
      if (ReplaceAssetIORequest(found->source_path)) {
        std::cout << "Processed: " << found->source_path << std::endl;
      } else {
        std::cout << "Skip: " << found->source_path
                  << " (replace request denied)" << std::endl;
        unprocessed.erase(found);
      }
    }
  }
}

void AssetsAnalyzer::RemoveTextureSuffix(std::string& path) {
  if (DetectTexHdrSuffix(path)) {
    /// removing _hdr from the end
    path.resize(path.size() - 4);
  } else if (DetectTexRGSuffix(path)) {
    /// removing _rrrg from the end
    path.resize(path.size() - 5);
  }
}

bool AssetsAnalyzer::ReplaceAssetIORequest(
    const std::filesystem::path& asset_path) {
  if (force_)
    return true;
  if (all_keep_) {
    return false;
  } else if (all_replace_) {
    return true;
  }
  std::cout << "Do you want to replace "
            << asset_path.filename()
            << " by "
            << asset_path
            << "?\ny(yes), n(no), a(yes for all), 0(no for all))"
            << std::endl;
  // "y" means "yes"
  // "n" means "no"
  // "a" means "yes for all"
  // "0" means "no for all"
  // default: no
  char response;
  std::cin >> response;
  if (response == 'y' || response == 'Y') {
    return true;
  } else if (response == 'n' || response == 'N') {
    return false;
  } else if (response == 'a' || response == 'A') {
    all_replace_ = true;
    return true;
  } else if (response == '0') {
    all_keep_ = true;
  }
  return false;
}