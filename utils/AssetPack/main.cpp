#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "miniz.h"
#include "picosha2.h"

#include "../../config/Paths.h"
#include "../../config/AssetFormats.h"

struct AssetsInfo {
  std::string url;
  std::string zip_name;
  std::vector<unsigned char> hash; // sha256
  int redirection_count;
  int audio_count;
  int models_count;
  int textures_count;
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

// TODO: add precaution if file too big (should be streamed)
bool GenerateZip(const std::filesystem::path& assets_path,
                 AssetsInfo& assets_info) {
  namespace fs = std::filesystem;
  fs::remove(assets_info.zip_name);
  mz_bool status;
  for (const auto& entry : fs::recursive_directory_iterator(assets_path)) {
    if (fs::is_regular_file(entry)) {
      std::ifstream asset_file(entry.path(), std::ios::binary);
      std::stringstream asset_data_stream;
      asset_data_stream << asset_file.rdbuf();
      std::string asset_data = asset_data_stream.str();
      std::cout << entry.path().lexically_relative(assets_path).c_str() << std::endl;
      status = mz_zip_add_mem_to_archive_file_in_place(
          assets_info.zip_name.c_str(), entry.path().lexically_relative(assets_path).c_str(),
          asset_data.c_str(), asset_data.size(), nullptr, 0, MZ_BEST_COMPRESSION);
      if (!status) {
        std::cerr << "Error: file zipping failure for: "
                  << entry.path().lexically_relative(assets_path).c_str()
                  << std::endl;
        return false;
      }
    }
  }
  return true;
}

void CalculateHash(AssetsInfo& assets_info) {
  std::cout << "Hash computation..." << std::endl;
  std::ifstream f(assets_info.zip_name, std::ios::binary);
  assets_info.hash.resize(picosha2::k_digest_size);
  picosha2::hash256(f, assets_info.hash.begin(), assets_info.hash.end());
}


bool GenerateInfoFile(const AssetsInfo& assets_info) {
  std::ofstream assets_info_file("faithful_assets_info.txt");
  if (!assets_info_file.is_open()) {
    std::cerr << "Error: can't open file to write assets info" << std::endl;
    return false;
  }
  assets_info_file
      << assets_info.zip_name << '\n'
      << assets_info.url << '\n';
  for (auto c : assets_info.hash) {
    assets_info_file << c;
  }
  assets_info_file << '\n'
      << assets_info.redirection_count << '\n'
      << assets_info.audio_count << '\n'
      << assets_info.models_count << '\n'
      << assets_info.textures_count;
  return true;
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
    return -1;
  }

  std::filesystem::path assets_dir;
  if (argc > 2) {
    std::cerr << "Incorrect program arguments."
              << "\n\tUsage: AssetPach <src_path>"
              << "\nwhere: src_path - path with assets/"
              << std::endl;
    return -1;
  } else if (argc == 2) {
    assets_dir = argv[1];
  } else {
    std::cout << "Destination path not specified, used default instead: "
              << FAITHFUL_ASSET_PATH << std::endl;
    assets_dir = FAITHFUL_ASSET_PATH;
  }

  AssetsInfo assets_info;
  CountAssets(assets_dir, assets_info);
  assets_info.redirection_count =
      faithful::config::default_url_redirections_count;
  assets_info.url = faithful::config::default_url;
  assets_info.zip_name = faithful::config::default_zip_name;

  if (!GenerateZip(assets_dir, assets_info)) {
    return -1;
  }
  CalculateHash(assets_info);
  if (!GenerateInfoFile(assets_info)) {
    return -1;
  }
  std::cout << "\nSuccess: see faithful_assets_info.txt and "
            << assets_info.zip_name << std::endl;
  std::cout << "CAUTION: link to assets & redirection count should be set "
            << "manually after uploading " << faithful::config::default_zip_name
            << " to Google Drive (or other location)\n" << "You also should use "
            << "https://sites.google.com/site/gdocs2direct/ for Google Drive "
            << "to create direct link" << std::endl;

  return 0;
}

