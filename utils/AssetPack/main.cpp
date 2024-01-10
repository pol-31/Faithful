#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "miniz.h"
#include "picosha2.h"

#include "../../config/Paths.h"
#include "../../config/AssetFormats.h"

struct AssetsInfo {
  std::string url;
  std::string zip_name;
  std::string hash; // sha256
  int redirection_count;
  int audio_count;
  int models_count;
  int textures_count;
};

std::vector<unsigned char> GenerateSha256(const std::string& file_path) {
  std::ifstream f(file_path, std::ios::binary);
  std::vector<unsigned char> s(picosha2::k_digest_size);
  picosha2::hash256(f, s.begin(), s.end());
  return std::move(s);
}

void CountAssets(const std::filesystem::path& assets_directory,
                    AssetsInfo& assets_info) {
  namespace fs = std::filesystem;
  fs::path audio_dir = assets_directory / "audio";
  fs::path models_dir = assets_directory / "models";
  fs::path textures_dir = assets_directory / "textures";
  assets_info.audio_count = static_cast<int>(
      std::distance(fs::directory_iterator(audio_dir),
                    fs::directory_iterator{}));
  assets_info.models_count = static_cast<int>(
      std::distance(fs::directory_iterator(models_dir),
                    fs::directory_iterator{}));
  assets_info.textures_count = static_cast<int>(
      std::distance(fs::directory_iterator(textures_dir),
                    fs::directory_iterator{}));
}

void WriteSha256ToFile(const std::vector<unsigned char>& sha256Hash, const std::string& filePath) {
  std::ofstream outputFile(filePath, std::ios::binary);
  if (!outputFile.is_open()) {
    std::cerr << "Error opening file: " << filePath << std::endl;
    return;
  }
  outputFile.write(reinterpret_cast<const char*>(sha256Hash.data()), sha256Hash.size());
}

bool GenerateZip(const std::filesystem::path& assets_path,
                 AssetsInfo& assets_info) {
  namespace fs = std::filesystem;
  // TODO: this is borrowed code (AssetDownloader) for decompression,
  //  BUT need compression
  /*mz_zip_archive zip_archive;
  memset(&zip_archive, 0, sizeof(zip_archive));

  if (!mz_zip_reader_init_file(&zip_archive, assets_info.zip_name.c_str(), 0)) {
    std::cerr << "Error: can't initialize miniz zip archive reader\n";
    return false;
  }
  std::filesystem::path temp_dir("temp");

  for (mz_uint i = 0; i < mz_zip_reader_get_num_files(&zip_archive); i++) {
    mz_zip_archive_file_stat file_stat;
    if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) {
      std::cerr << "Error: can't get file stat for entry " << i << "\n";
      mz_zip_reader_end(&zip_archive);
      return false;
    }
    fs::path target_path = fs::path(temp_dir) / file_stat.m_filename;
    if (file_stat.m_is_directory) {
      fs::create_directories(target_path);
    } else {
      mz_uint uncompressed_size = static_cast<mz_uint>(file_stat.m_uncomp_size);
      std::vector<char> file_data(uncompressed_size);

      if (!mz_zip_reader_extract_to_mem(&zip_archive, i, file_data.data(),
                                        uncompressed_size, 0)) {
        std::cerr << "Error: can't extract file " << target_path << "\n";
        mz_zip_reader_end(&zip_archive);
        return false;
      }
      std::ofstream output(target_path, std::ios::binary);
      if (!output.is_open()) {
        std::cerr << "Error: can't open file to write asset" << std::endl;
        return false;
      }
      if (!output.write(file_data.data(), uncompressed_size)) {
        std::cerr << "Error: can't write asset" << std::endl;
        mz_zip_reader_end(&zip_archive);
        return false;
      }
    }
  }
  mz_zip_reader_end(&zip_archive);*/
}

void CalculateHash(AssetsInfo& assets_info) {
  // TODO generate hash + write to assets_info.hash
//  std::vector<unsigned char> s = GenerateSha256("/home/pavlo/Desktop/test.zip");
//  WriteSha256ToFile(s, "result_hash.bin");
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

  GenerateZip(assets_dir, assets_info);
  CalculateHash(assets_info);

  std::cout << "CAUTION: link to assets & redirection count should be set "
            << "manually after uploading " << faithful::config::default_zip_name
            << " to Google Drive (or other location)\n" << "You also should use "
            << "https://sites.google.com/site/gdocs2direct/ for Google Drive "
            << "to create direct link" << std::endl;

  return 0;
}

