#include "AssetCategory.h"

#include <filesystem>
#include <string>
#include <string_view>

#include "../../config/AssetFormats.h"

bool DetectEncodeTexHdr(const std::filesystem::path& filename) {
  return (filename.extension() == ".hdr" || filename.extension() == ".exr");
}
bool DetectEncodeTexNormalMap(const std::filesystem::path& filename) {
  const std::string& stem_string(filename.stem().string());
  // len of "_nmap" + at least 1 char for actual name
  if (stem_string.length() < 6)
    return false;
  size_t suffixPos = stem_string.length() - 5;
  return stem_string.compare(suffixPos, std::string::npos, "_nmap") == 0;
}

bool DetectDecodeTexHdr(const std::filesystem::path& filename) {
  const std::string& stem_string(filename.stem().string());
  // len of "_hdr" + at least 1 char for actual name
  if (stem_string.length() < 5)
    return false;
  size_t suffixPos = stem_string.length() - 4;
  return stem_string.compare(suffixPos, std::string::npos, "_hdr") == 0;
}
bool DetectDecodeTexNormalMap(const std::filesystem::path& filename) {
  const std::string& stem_string(filename.stem().string());
  // len of "_nmap" + at least 1 char for actual name
  if (stem_string.length() < 6)
    return false;
  size_t suffixPos = stem_string.length() - 5;
  return stem_string.compare(suffixPos, std::string::npos, "_nmap") == 0;
}

AssetCategory DeduceAssetEncodeCategory(const std::filesystem::path& filename) {
  using namespace faithful;
  for (const auto& audio_format : config::audio_comp_formats) {
    if (filename.extension().string() == audio_format) {
      if (file_size(filename) > config::audio_comp_thread_threshold) {
        return AssetCategory::kAudioMusic;
      } else {
        return AssetCategory::kAudioSound;
      }
    }
  }
  for (const auto& model_format : config::model_comp_formats) {
    if (filename.extension().string() == model_format)
      return AssetCategory::kModel;
  }
  for (const auto& image_format : config::tex_comp_formats) {
    if (filename.extension().string() == image_format) {
      if (DetectEncodeTexHdr(filename)) {
        return AssetCategory::kTextureHdr;
      } else if (DetectEncodeTexNormalMap(filename)) {
        return AssetCategory::kTextureNmap;
      } else {
        return AssetCategory::kTextureLdr;
      }
    }
  }
  return AssetCategory::kUnknown;
}

/// used both for decoding and getting asset info for encoding
AssetCategory DeduceAssetDecodeCategory(const std::filesystem::path& filename) {
  std::string file_extension = filename.extension().string();
  if (file_extension == ".ogg") {
    return AssetCategory::kAudioMusic;
  } else if (file_extension == ".wav") {
    return AssetCategory::kAudioSound;
  } else if (file_extension == ".gltf") {
    return AssetCategory::kModel;
  } else if (file_extension == ".astc") {
    if (DetectDecodeTexHdr(filename)) {
      return AssetCategory::kTextureHdr;
    } else if (DetectDecodeTexNormalMap(filename)) {
      return AssetCategory::kTextureNmap;
    } else {
      return AssetCategory::kTextureLdr;
    }
  } else if (file_extension == ".bin") {
    return AssetCategory::kBinary;
  } else {
    return AssetCategory::kUnknown;
  }
}
