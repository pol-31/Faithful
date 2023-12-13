/** The purpose of this sub-application to convert all assets from
 * given directory into certain:
 * video: not supported
 * audio: .ogg + Vorbis
 * textures:
 *   container: raw .astc if (KHR_texture_compression_astc_hdr,
 *                            KHR_texture_compression_astc_ldr) supported,
 *              otherwise .ktx;
 *              OR simple .bin format for height/displacement map (etc)
 *   codec: astc 4x4
 * 3D models:
 *   glTF (.glb container format)
 *
 *
 * Supported formats:
 * - textures(images):
 *     LDR: BMP, JPEG, PNG, TGA;
 *     HDR: EXR, HDR;
 *     other: DDS, KTX.
 * - audio: mp3, flac, wav, ogg
 * - 3D models: collada (dae), obj, ply, blend, fbx, stl, gltf, glb
 *
 * */

#include <iostream> // TODO: replace by Logger.h
#include <filesystem>
#include <string>
#include <vector>

#include "../Logger.h"

#include "AudioProcessor.h"
#include "ModelProcessor.h"
#include "TextureProcessor.h"

enum class AssetCategory {
  kTexture,
  kAudio,
  kModel,
  kUnknown
};

AssetCategory DeduceAssetCategory(std::filesystem::path filename) {
  static const std::vector<std::string> supported_image_formats = {
    ".bmp", ".jpeg", ".png", ".tga",
    ".exr", ".hdr",
    ".dds", ".ktx"
  };
  static const std::vector<std::string> supported_audio_formats = {
    ".mp3", ".flac", ".wav", ".ogg"
  };
  static const std::vector<std::string> supported_model_formats = {
    ".dae", ".obj", ".ply", ".blend", ".fbx", "stl", ".gltf", ".glb"
  };

  for (const std::string& image_format : supported_image_formats) {
    if (filename == image_format)
      return AssetCategory::kTexture;
  }
  for (const std::string& audio_format : supported_audio_formats) {
    if (filename == audio_format)
      return AssetCategory::kAudio;
  }
  for (const std::string& model_format : supported_model_formats) {
    if (filename == model_format)
      return AssetCategory::kModel;
  }
  return AssetCategory::kUnknown;
}

void ProcessAsset(const std::filesystem::path& asset_path) {
  AssetCategory asset_category = DeduceAssetCategory(asset_path.extension());
  switch (asset_category) {
    case AssetCategory::kTexture:
      ProcessTexture(asset_path);
      break;
    case AssetCategory::kAudio:
      ProcessAudio(asset_path);
      break;
    case AssetCategory::kModel:
      ProcessModel(asset_path);
      break;
    case AssetCategory::kUnknown:
      std::cout << "Error: unsupported file format: " << asset_path.extension() << std::endl;
      break;
  }
}

void ProcessAssetDirectory(const std::filesystem::path& dir_path) {
  for (const std::filesystem::path& entry : std::filesystem::recursive_directory_iterator(dir_path)) {
    if (std::filesystem::is_regular_file(entry)) {
      ProcessAsset(entry);
    } else if (std::filesystem::is_directory(entry)) {
      ProcessAssetDirectory(entry);
    }
  }
}


int main(int argc, char **argv) {
  std::filesystem::path input_path;

  if (argc > 2) {
    std::cout << "incorrect program arguments"
              << "\nusage: AssetProcessor <source_path>" << std::endl;
    return -1;
  } else if (argc == 2) {
    input_path = argv[1];
    std::cout << "working with current path: " << std::filesystem::current_path();
  } else {
    input_path = "../monkey.fbx";
//    input_path = "../Pantera - Strength Beyond Strength ( 160kbps ).mp3";
  }

  if (std::filesystem::is_directory(input_path)) {
    ProcessAssetDirectory(input_path);
  } else if (std::filesystem::is_regular_file(input_path)) {
    ProcessAsset(input_path);
  } else {
    std::cout << "Error: incorrect path, should be file or directory" << std::endl;
  }

  return 0;
}
