#ifndef FAITHFUL_ASSETCATEGORY_H
#define FAITHFUL_ASSETCATEGORY_H

#include <filesystem>

enum class AssetCategory {
  kAudioMusic,
  kAudioSound,
  kModel,
  kTextureLdr,
  kTextureHdr,
  kTextureRG, // 2-channel textures
  kUnknown
};

bool DetectTexHdrExtension(const std::filesystem::path& filename);

bool DetectTexHdrSuffix(const std::filesystem::path& filename);
bool DetectTexRGSuffix(const std::filesystem::path& filename);

AssetCategory DeduceAssetEncodeCategory(const std::filesystem::path& filename);
AssetCategory DeduceAssetDecodeCategory(const std::filesystem::path& filename);

AssetCategory DeduceAssetCategory(const std::filesystem::path& filename,
                                  bool encode);

#endif  // FAITHFUL_ASSETCATEGORY_H
