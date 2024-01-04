#ifndef FAITHFUL_ASSETCATEGORY_H
#define FAITHFUL_ASSETCATEGORY_H

#include <filesystem>

enum class AssetCategory {
  kAudioMusic,
  kAudioSound, /// tiny files for 1-2 seconds playback
  kBinary, /// for models buffer data
  kModel,
  kTextureLdr,
  kTextureHdr,
  kTextureNmap,
  kUnknown
};

bool DetectEncodeTexHdr(const std::filesystem::path& filename);
bool DetectEncodeTexNormalMap(const std::filesystem::path& filename);

bool DetectDecodeTexHdr(const std::filesystem::path& filename);
bool DetectDecodeTexNormalMap(const std::filesystem::path& filename);

AssetCategory DeduceAssetEncodeCategory(const std::filesystem::path& filename);
AssetCategory DeduceAssetDecodeCategory(const std::filesystem::path& filename);



#endif //FAITHFUL_ASSETCATEGORY_H
