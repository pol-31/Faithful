/** The purpose of this sub-application to convert all assets from
 * given directory into certain:
 * audio: .ogg + Vorbis
 * textures:
 *   container: .astc
 * 3D models:
 *   glTF (.gltf)
 * videos: not supported currently (?)
 *
 * See other params in Faithful/config/AssetFormats.h
 *
 * Supported:
 * - textures(images):
 *     LDR: bmp, jpeg, jpg, png, tga, psd, ppm, pgm;
 *     HDR: exr, hdr;
 *     other: dds, ktx
 * - audio: mp3, flac, wav, ogg
 * - 3D models: gltf, glb
 *
 * Currently there is no shader processing (no reason to do this)
 * */


/// NOTE: model textures can't be Hdr (according to gltf 2.0)

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION// defined in tiny_gltf.h
//#define TINYGLTF_USE_RAPIDJSON

#include <iostream>  // TODO: replace by Logger.h
#include <cstring>
#include <filesystem>
#include <string>
#include <fstream>

#include "../../config/Paths.h"

#include "AssetProcessor.h"
#include "AssetLoadingThreadPool.h"

struct AssetController {
  void All(bool mode = true) {
    audio = mode;
    models = mode;
    textures = mode;
  }
  bool audio = false;
  bool models = false;
  bool textures = false;
};

void PrintConfigs(bool encode, AssetController assetController);
void PrintHelpInfo();

void LogProcessingResult(const std::string& path, bool encoded);

// TODO: add force flag (to rewrite) --< rather just -y or -n for ALL at once
// TODO: another arguments for thread_count (user preferences)

// TODO: add README.md with naming convention: __ _nmap.astc __

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Incorrect program's arguments" << std::endl;
    return -1;
  }
  std::cout << argv[1] << std::endl;
  std::filesystem::path user_path = "";
  AssetController asset_controller;
  bool asset_processing = true;
  bool process_all = true;
  bool encode = true;
  bool force = false;
  int thread_count = std::thread::hardware_concurrency();

  // TODO: rewrite (incorrect flags parsing, looks disgraceful)
  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      int flag_len = strlen(argv[i]);
      for (int j = 0; j < flag_len; ++j) {
        if (std::strcmp(argv[j], "d") == 0) {
          encode = false;
        } else if (std::strcmp(argv[j], "-e") == 0) {
          encode = true;
        } else if (std::strcmp(argv[j], "-a") == 0) {
          process_all = false;
          asset_controller.audio = true;
        } else if (std::strcmp(argv[j], "-t") == 0) {
          process_all = false;
          asset_controller.textures = true;
        } else if (std::strcmp(argv[j], "-m") == 0) {
          process_all = false;
          asset_controller.models = true;
        } else if (std::strcmp(argv[j], "-f") == 0) {
          force = true;
        } else if (std::strcmp(argv[j], "-h") == 0) {
          PrintHelpInfo();
          asset_processing = false;
          break;
        } else {
          std::cout << "Incorrect arguments, type -h to see instruction"
                    << std::endl;
          asset_processing = false;
          break;
        }
      }
    } else {
      if (i != (argc - 1)) {
        asset_processing = false;
        std::cout << "Incorrect arguments, type -h to see instruction"
                  << std::endl;
        break;
      } else {
        user_path = argv[i];
      }
    }
  }
  if (!asset_processing)
    return 0;
  asset_controller.All(process_all);

  if (user_path.empty()) {
    std::cout << "There's no provided path, working with current directory:\n"
              << std::filesystem::current_path() << std::endl;
    user_path = std::filesystem::current_path();
  }

  PrintConfigs(encode, asset_controller);

  // TODO: need to check is directory not "active" (not used by Faithful):
  //    dest==src
  //    lexicographical_relative dest <=> src

  AssetProcessor processor(std::thread::hardware_concurrency());
//  AssetProcessor processor(encode, thread_count - 1, FAITHFUL_ASSET_PATH, force);
  std::cout << "user's path " << user_path << std::endl;
  processor.Process(encode, "/home/pavlo/Desktop/to",
                    "/home/pavlo/Desktop/from", force);

  // TODO: LogProcessingResult(FAITHFUL_ASSET_INFO_FILE, encode);

  return 0;
}

void PrintConfigs(bool encode, AssetController assetController) {
  std::cout << "Config: ";
  if (encode) {
    std::cout << "encoding of {";
  } else {
    std::cout << "decoding of {";
  }
  if (!assetController.audio && !assetController.textures &&
      !assetController.models) {
    std::cout << " no asset types chosen }";
    return;
  }
  if (assetController.audio) {
    std::cout << " audio";
  }
  if (assetController.textures) {
    std::cout << " textures";
  }
  if (assetController.models) {
    std::cout << " models";
  }
  std::cout << " }" << std::endl;
}

void PrintHelpInfo() {
  std::cout << "Faithful::AssetProcessor help:\n"
            << "\t\"-h\" for help\n"
            << "\t\"-d\" for decoding\n"
            << "\t\"-e\" for encoding\n"
            << "\t\"-a\" turn processing audio files on\n"
            << "\t\"-t\" turn processing texture(image) files on\n"
            << "\t\"-m\" turn processing 3d-models on\n"
            << "If you'll not specify any of {a, t, m}"
            << " - all enabled by default;\n"
            << "if you'll specify at least one of them"
            << " - all other disabled by default\n"
            << "\ndecoding example (decode only audio and textures):\n"
            << "\tAssetProcessor -dat \"destination-dir\"\n"
            << "encoding example (encode all):\n"  // todo__
            << "\nSupported formats:\n"
            << "\ttextures: bmp, jpeg, png, tga, exr, hdr, dds, ktx;\n"
            << "\taudio: mp3, flac, wav, ogg;\n"
            << "\t3D models: collada (dae), obj, ply, blend, "
            << "fbx, stl, gltf, glb.\n"
            << "To check/adjust configurations open this file"
            << "\t" << FAITHFUL_CONFIG_PATH << std::endl;
}

void LogProcessingResult(const std::string& path, bool encoded) {
  std::ofstream log_file(path);
  if (!log_file.is_open()) {
    std::cout << "Error (no logging): Log file opening issues" << std::endl;
    return;
  }
  if (encoded) {
    // TODO: write to log.txt section "last encode"
  } else {
    // TODO: write to log.txt section "last decode"
  }
}
