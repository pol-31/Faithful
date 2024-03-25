#include "AssetsAnalyzer.h"

#include <algorithm>
#include <iostream>

#include "../../config/AssetFormats.h"

AssetsAnalyzer::AssetsAnalyzer(
    const std::filesystem::path& path, bool encode)
    : encode_(encode) {
  AnalyzePath(path);
  SortEntries();
}

void AssetsAnalyzer::AnalyzePath(const std::filesystem::path& path) {
  if (std::filesystem::is_regular_file(path)) {
    AddEntry(path);
  } else if (std::filesystem::is_directory(path)) {
    AnalyzeDir(path);
  } else {
    std::cerr << "Error: incorrect path, try file or directory instead" << std::endl;
  }
}

void AssetsAnalyzer::AnalyzeDir(const std::filesystem::path& path) {
  for (const std::filesystem::path& entry :
       std::filesystem::directory_iterator(path)) {
    if (std::filesystem::is_regular_file(entry)) {
      AddEntry(entry);
    } else if (std::filesystem::is_directory(entry)) {
      AnalyzeDir(entry);
    }
  }
}

void AssetsAnalyzer::SortEntries() {
  /// lexicographical sorting indeed helps (e.g. map_1.png, map_2.png, map_3.png)
  std::sort(audio_to_process_.begin(), audio_to_process_.end());
  std::sort(models_to_process_.begin(), models_to_process_.end());
  std::sort(textures_to_process_.begin(), textures_to_process_.end());
  std::cout << "audio num " << audio_to_process_.size() << std::endl;
  std::cout << "models num " << models_to_process_.size() << std::endl;
  std::cout << "textures num " << textures_to_process_.size() << std::endl;
}

AssetsAnalyzer::AssetCategory AssetsAnalyzer::DeduceAssetCategory(
    const std::filesystem::path& path) {
  if (encode_) { // order from more_supported_num to less
    for (const auto& extension : faithful::config::kModelCompFormats) {
      if (extension == path.extension()) {
        return AssetCategory::kModel;
      }
    }
    for (const auto& extension : faithful::config::kAudioCompFormats) {
      if (extension == path.extension()) {
        return AssetCategory::kAudio;
      }
    }
    for (const auto& extension : faithful::config::kTexCompFormats) {
      if (extension == path.extension()) {
        return AssetCategory::kTexture;
      }
    }
  } else { // order from more_checks to less
    if (path.extension() == ".gltf") {
      return AssetCategory::kModel;
    } else if (path.extension() == ".astc") {
      return AssetCategory::kTexture;
    } else if (path.extension() == ".ogg" || path.extension() == ".wav") {
      return AssetCategory::kAudio;
    }
  }
  return AssetCategory::kUnknown;
}

void AssetsAnalyzer::AddEntry(const std::filesystem::path& path) {
  auto category = DeduceAssetCategory(path);
  switch (category) {
    case AssetCategory::kAudio:
      std::cout << "add kAudio " << path << std::endl;
      AddEntryImpl(path, audio_to_process_);
      break;
    case AssetCategory::kModel:
      std::cout << "add kModel " << path << std::endl;
      AddEntryImpl(path, models_to_process_);
      break;
    case AssetCategory::kTexture:
      std::cout << "add kTexture " << path << std::endl;
      AddEntryImpl(path, textures_to_process_);
      break;
    case AssetCategory::kUnknown:
      std::cout << "add none " << path << std::endl;
      break;
  }
}

void AssetsAnalyzer::AddEntryImpl(const std::filesystem::path& new_asset,
                                  std::vector<std::filesystem::path>& assets) {
  for (const auto& old_asset : assets) {
    if (old_asset.stem() == new_asset.stem()) {
      std::cerr << "Source path has at least two files with the same name:\n"
                << old_asset.string() << "\n" << new_asset.string()
                << "\nOnly first will be processed"
                << "\nRename second and run program again to process second"
                << std::endl;
      return;
    }
  }
  assets.push_back(new_asset.string());
}
