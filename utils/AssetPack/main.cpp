#include <stdlib.h>
#include "mimalloc-override.h"
/// We don't need to #include "mimalloc-new-delete.h"
/// as it has already been overridden by libmimalloc.a

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "miniz.h"
#include "picosha2.h"

#include "../../config/AssetFormats.h"
#include "../../config/Paths.h"

#include "../SystemInfo.h"

struct AssetsInfo {
  std::string zip_name;
  std::string url;
  std::vector<uint8_t> hash; // sha256
  int redirection_count;
  int audio_count;
  int models_count;
  int textures_count;
};

enum FaithfulAssetPackError {
  kOsCompatibilityFailure = 1,
  kArgumentsFailure,
  kZipGenerationFailure,
  kInfoGenerationFailure
};

void CountAssets(const std::filesystem::path& assets_directory,
                 AssetsInfo& assets_info) {
  namespace fs = std::filesystem;
  fs::path audio_dir = assets_directory / "audio";
  fs::path models_dir = assets_directory / "models";
  fs::path textures_dir = assets_directory / "textures";
  assets_info.audio_count = static_cast<int>(
      std::distance(fs::recursive_directory_iterator(audio_dir),
                    fs::recursive_directory_iterator{}));
  assets_info.models_count = static_cast<int>(
      std::distance(fs::recursive_directory_iterator(models_dir),
                    fs::recursive_directory_iterator{}));
  assets_info.textures_count = static_cast<int>(
      std::distance(fs::recursive_directory_iterator(textures_dir),
                    fs::recursive_directory_iterator{}));
}

bool GenerateZip(const std::filesystem::path& assets_directory,
                 AssetsInfo& assets_info) {
  namespace fs = std::filesystem;
  fs::remove(assets_info.zip_name);
  mz_bool status;
  std::string asset_data;
  std::stringstream asset_data_stream;

  std::size_t ram_threshold = GetRamSize() / 2;

  for (const auto& entry : fs::recursive_directory_iterator(assets_directory)) {
    if (fs::is_regular_file(entry)) {
      if (fs::file_size(entry) > ram_threshold) {
        std::cout << "Processing of such big files (streaming) "
                  << "is not supported by now, skip: " << entry << std::endl;
      }
      std::ifstream asset_file(entry.path(), std::ios::binary);
      asset_data_stream.str("");
      asset_data_stream << asset_file.rdbuf();
      if (!asset_data_stream) {
        std::cerr << "Error: file reading to stream error for: "
                  << entry.path() << std::endl;
        return false;
      }
      asset_data = asset_data_stream.str();
      std::cout << "Processing: "
                << entry.path().lexically_relative(assets_directory).c_str()
                << std::endl;
      status = mz_zip_add_mem_to_archive_file_in_place(
          assets_info.zip_name.c_str(), entry.path().lexically_relative(assets_directory).c_str(),
          asset_data.c_str(), asset_data.size(), nullptr, 0, MZ_BEST_COMPRESSION);
      if (!status) {
        std::cerr << "Error: file packing failure for: "
                  << entry.path().lexically_relative(assets_directory).c_str()
                  << std::endl;
        return false;
      }
      asset_data.clear();
    }
  }
  return true;
}

void CalculateHash(AssetsInfo& assets_info) {
  std::cout << "\nHash computation..." << std::endl;
  std::ifstream assets_archive(assets_info.zip_name, std::ios::binary);
  assets_info.hash.resize(picosha2::k_digest_size);
  picosha2::hash256(assets_archive, assets_info.hash.begin(), assets_info.hash.end());
}

bool GenerateInfoFile(const AssetsInfo& assets_info) {
  std::ofstream assets_info_file(faithful::config::default_info_name,
                                 std::ios::binary);
  if (!assets_info_file.is_open()) {
    std::cerr << "Error: can't open file to write assets info" << std::endl;
    return false;
  }
  assets_info_file
      << assets_info.zip_name << '\n'
      << assets_info.url << '\n'
      << assets_info.redirection_count << '\n'
      << assets_info.audio_count << '\n'
      << assets_info.models_count << '\n'
      << assets_info.textures_count << '\n';
  assets_info_file.write(reinterpret_cast<const char*>(assets_info.hash.data()), assets_info.hash.size());
  return true;
}

bool ReplaceZipIORequest() {
  std::cout << "The program will generate a new .zip in the current directory."
            << "\nDo you want to replace " << faithful::config::default_zip_name
            << " located there?\nType \'y\' for \"yes\"" << std::endl;
  char response;
  std::cin >> response;
  if (response == 'y' || response == 'Y') {
    return true;
  } else {
    return false;
  }
}

int main(int argc, char** argv) {
  if ((sizeof(unsigned char) + sizeof(char)) != 2) {
    std::cerr << "The program currently is not supported by your OS:\n"
              << "size incompatibilities of char / unsigned char\n"
              << "provided: sizeof(char) = " << sizeof(char) << "\n"
              << "\tsizeof(unsigned char) = " << sizeof(unsigned char) << "\n"
              << "requires: sizeof(char) = " << 1 << "\n"
              << "\tsizeof(unsigned char) = " << 1 << "\n"
              << std::endl;
    return FaithfulAssetPackError::kOsCompatibilityFailure;
  }

  std::filesystem::path assets_dir;
  if (argc > 2) {
    std::cerr << "Incorrect program arguments."
              << "\n\tUsage: AssetPack <src_path>"
              << "\nwhere: src_path - path with assets/"
              << std::endl;
    return FaithfulAssetPackError::kArgumentsFailure;
  } else if (argc == 2) {
    assets_dir = argv[1];
  } else {
    std::cout << "Destination path not specified, used default instead: "
              << FAITHFUL_ASSET_PATH << std::endl;
    assets_dir = FAITHFUL_ASSET_PATH;
  }

  if (std::filesystem::exists(faithful::config::default_zip_name)) {
    if (!ReplaceZipIORequest()) {
      std::cout << "Asset packing \"graceful failure\": replace request denied"
                << std::endl;
      return EXIT_SUCCESS;
    }
  }

  AssetsInfo assets_info;
  CountAssets(assets_dir, assets_info);
  assets_info.redirection_count =
      faithful::config::default_url_redirections_count;
  assets_info.url = faithful::config::default_url;
  assets_info.zip_name = faithful::config::default_zip_name;

  if (!GenerateZip(assets_dir, assets_info)) {
    return FaithfulAssetPackError::kZipGenerationFailure;
  }
  CalculateHash(assets_info);
  if (!GenerateInfoFile(assets_info)) {
    return FaithfulAssetPackError::kInfoGenerationFailure;
  }
  std::cout << "\nSuccess: see faithful_assets_info.txt and "
            << assets_info.zip_name << std::endl;
  std::cout << "CAUTION: link to assets & redirection count should be set"
            << "manually\nafter uploading "
            << faithful::config::default_zip_name
            << " to (e.g.) Google Drive.\n"
            << "In case of Google Drive you should create direct link:\n"
            << "https://sites.google.com/site/gdocs2direct/\n"
            << std::endl;
  return EXIT_SUCCESS;
}

