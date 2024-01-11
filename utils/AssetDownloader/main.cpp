#if defined(__GNUC__)
// Ensure we get the 64-bit variants of the CRT's file I/O calls
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE 1
#endif
#endif

// TODO:
//  #include "mimalloc.h"
//  #include "mimalloc-new-delete.h" // overriding of global new/delete

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "curl/curl.h"
#include "miniz.h"
#include "picosha2.h"

#include "../../config/Paths.h"
#include "../../config/AssetFormats.h"

#if defined(FAITHFUL_ASSET_PROCESSOR_CURL_CLI) || \
    defined(FAITHFUL_ASSET_PROCESSOR_WGET) || \
    defined(FAITHFUL_ASSET_PROCESSOR_INVOKE_WEB_REQUEST)
#include <cstdlib>
#endif

struct AssetsInfo {
  std::string url;
  std::string zip_name;
  std::vector<unsigned char> hash; // sha256
  int redirection_count;
  int audio_count;
  int models_count;
  int textures_count;
};

enum AssetDownloaderError {
  kArgumentsFaulure = 1,
  kAssetsInfoDownloadFailure,
  kAssetsInfoProcessFailure,
  kAssetsDownloadFailure,
  kAssetsUnzipFailure
};

size_t CurlWriteCallback(void* ptr, size_t size, size_t nmemb, void* stream) {
  return fwrite(ptr, size, nmemb, (FILE*)stream);
}

std::vector<unsigned char> GenerateSha256(const std::string& file_path);

bool ValidateHash(
    const std::vector<unsigned char>& hash_extracted,
    const std::vector<unsigned char>& hash_generated);

bool ValidateAssets(const std::filesystem::path& assets_directory,
                    const std::string& zip_file_path,
                    const AssetsInfo& assets_info);

bool InstallAssets(const std::filesystem::path& in_dir,
                   const std::filesystem::path& out_dir);

bool ProcessAssetsZip(const std::filesystem::path& assets_zip_file,
                      const std::filesystem::path& out_assets_path,
                      const AssetsInfo& assets_info);

bool DownloadFile(const std::string& file_url, const std::string& out_file,
                  int redirection_count = 0);

AssetsInfo RetainAssetsInfo(const std::string& file_path);

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
//  std::string out_assets_path_default(FAITHFUL_ASSET_PATH);
  std::string out_assets_path_default("/home/pavlo/Desktop/tempor");
  std::string out_assets_path;

//  std::string assets_info_file("faithful_assets_info.txt");
  std::string assets_info_file("/home/pavlo/CLionProjects/Faithful/config/faithful_assets_info.txt");
//  std::string assets_info_file("asset_info.txt");
  std::string assets_info_url(faithful::config::asset_info_url);

  if (argc > 2) {
    std::cerr << "Incorrect program arguments."
              << "\n\tUsage: AssetDownloader <dest_path>"
              << "\nwhere: dest_path - path where to extract assets/"
              << std::endl;
    return AssetDownloaderError::kArgumentsFaulure;
  } else if (argc == 2) {
    out_assets_path = argv[1];
  } else {
    std::cout << "Destination path not specified, used default instead: "
              << out_assets_path_default << std::endl;
    out_assets_path = out_assets_path_default;
  }

#ifdef FAITHFUL_ASSET_PROCESSOR_CURL_LIB
  curl_global_init(CURL_GLOBAL_DEFAULT);
#endif
/*

  if (!DownloadFile(assets_info_url, assets_info_file, true)) {
    std::cerr << "Can't download info file" << std::endl;
    return AssetDownloaderError::kAssetsInfoDownloadFailure;
  } else {
    std::cout << "Successfully downloaded: " << assets_info_file << std::endl;
  }
*/

//  auto assets_info = RetainAssetsInfo("/home/pavlo/CLionProjects/Faithful/cmake-build-debug/utils/AssetPack/faithful_assets_info.txt");
  auto assets_info = RetainAssetsInfo(assets_info_file);
  if (assets_info.audio_count == -1 &&
      assets_info.models_count == -1 &&
      assets_info.textures_count == -1) {
    return AssetDownloaderError::kAssetsInfoProcessFailure;
  }

  if (!DownloadFile(assets_info.url, assets_info.zip_name, true)) {
    return AssetDownloaderError::kAssetsDownloadFailure;
  } else {
    std::cout << "Successfully downloaded: "
              << assets_info.zip_name
              << std::endl;
  }

  if (!ProcessAssetsZip(assets_info.zip_name, out_assets_path,
                        assets_info)) {
    std::cerr << "Extraction failed\n" << std::endl;
    return AssetDownloaderError::kAssetsUnzipFailure;
  }

#ifdef FAITHFUL_ASSET_PROCESSOR_CURL_LIB
  curl_global_cleanup();
#endif
  return 0;
}

std::vector<unsigned char> GenerateSha256(const std::string& file_path) {
  std::cout << "Hash computation..." << std::endl;
  std::ifstream f(file_path, std::ios::binary);
  std::vector<unsigned char> hash(picosha2::k_digest_size);
  picosha2::hash256(f, hash.begin(), hash.end());
  return std::move(hash);
}

bool ValidateHash(
    const std::vector<unsigned char>& hash_extracted,
    const std::vector<unsigned char>& hash_generated) {
  if (hash_extracted.size() != 32 ||
      hash_generated.size() != 32) {
    std::cerr << "Error: ValidateHash() - provided invalid hashes (length): "
              << "\nhash_extracted: " << hash_extracted.size()
              << "\nhash_generated: " << hash_generated.size()
              << std::endl;
    return false;
  }
  for (int i = 0; i < hash_extracted.size(); ++i) {
    if (hash_generated[i] != static_cast<unsigned char>(hash_extracted[i])) {
      return false;
    }
  }
  return true;
}

bool ValidateAssets(const std::filesystem::path& assets_directory,
                    const std::filesystem::path& zip_file_path,
                    const AssetsInfo& assets_info) {
  namespace fs = std::filesystem;
  fs::path audio_dir = assets_directory / "audio";
  fs::path models_dir = assets_directory / "models";
  fs::path textures_dir = assets_directory / "textures";
  bool status = true;
  status &=
      !(assets_info.audio_count !=
            static_cast<int>(std::distance(
                fs::recursive_directory_iterator(audio_dir),
                fs::recursive_directory_iterator{})) ||
        assets_info.models_count !=
            static_cast<int>(std::distance(
                fs::recursive_directory_iterator(models_dir),
                fs::recursive_directory_iterator{})) ||
        assets_info.textures_count !=
            static_cast<int>(std::distance(
                fs::recursive_directory_iterator(textures_dir),
                fs::recursive_directory_iterator{})));
  status &= ValidateHash(assets_info.hash, GenerateSha256(zip_file_path));
  return status;
}

/// Downloaded dir already has needed hierarchy:
/// in_dir/:
///       |----- audio/
///       |----- models/
///       |----- textures/
bool InstallAssets(const std::filesystem::path& in_dir,
                   const std::filesystem::path& out_dir) {
  namespace fs = std::filesystem;
  if (!fs::exists(out_dir)) {
    fs::create_directory(out_dir);
  } else {
    fs::remove_all(out_dir);
    fs::create_directory(out_dir);
  }
  for (const auto& entry : fs::recursive_directory_iterator(in_dir)) {
    const fs::path& source_file = entry.path();
    const fs::path& destination_file = out_dir / entry.path().lexically_relative(in_dir);
    if (fs::is_regular_file(source_file)) {
      if (fs::exists(destination_file)) {
        fs::remove(destination_file);
      }
      fs::rename(source_file, destination_file);
    } else if (fs::is_directory(source_file) && !fs::exists(destination_file)) {
      fs::create_directory(destination_file);
    }
  }
  std::cout << "Successfully installed" << std::endl;
  return true;
}

bool ProcessAssetsZip(const std::filesystem::path& assets_zip_file,
                      const std::filesystem::path& out_assets_path,
                      const AssetsInfo& assets_info) {
  namespace fs = std::filesystem;
  mz_zip_archive zip_archive;
  memset(&zip_archive, 0, sizeof(zip_archive));

  if (!mz_zip_reader_init_file(&zip_archive, assets_zip_file.c_str(), 0)) {
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
    std::cout << "Processing of " << target_path << std::endl;
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
      if (!exists(target_path.parent_path())) {
        fs::create_directories(target_path.parent_path());
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
  mz_zip_reader_end(&zip_archive);
  if (!ValidateAssets(temp_dir, assets_zip_file, assets_info)) {
    std::cerr
        << "Error: invalid assets (assets info doesn't matches with downloaded files)"
        << std::endl;
    fs::remove_all(temp_dir);
    return false;
  } else {
    std::cout << "Successfully validated" << std::endl;
  }
  return InstallAssets(temp_dir, out_assets_path);
}

bool DownloadFile(const std::string& file_url, const std::string& out_file,
                  int redirection_count) {
#if defined(FAITHFUL_ASSET_PROCESSOR_CURL_CLI)
#elif defined(FAITHFUL_ASSET_PROCESSOR_WGET)
#elif defined(FAITHFUL_ASSET_PROCESSOR_INVOKE_WEB_REQUEST)
#else // defined(FAITHFUL_ASSET_PROCESSOR_CURL_LIB)
  CURL* curl = curl_easy_init();
  if (!curl) {
    std::cerr << "Error: can't initialize libcurl" << std::endl;
    return false;
  }
  FILE* file = fopen(out_file.c_str(), "wb");
  if (!file) {
    std::cerr << "Error: can't open file to writing downloaded content: "
              << file_url << std::endl;
    curl_easy_cleanup(curl);
    return false;
  }

  curl_easy_setopt(curl, CURLOPT_URL, file_url.c_str());
  if (redirection_count > 0) {
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, redirection_count);
  }
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
  CURLcode res = curl_easy_perform(curl);

  if (res != CURLE_OK) {
    fclose(file);
    curl_easy_cleanup(curl);
    std::cerr << "Error: libcurl request failed: " << curl_easy_strerror(res)
              << std::endl;
    return false;
  }
  fclose(file);
  curl_easy_cleanup(curl);
#endif
    return true;
}

AssetsInfo RetainAssetsInfo(const std::string& file_path) { // TODO: rename ________
  AssetsInfo assets_info;
  std::ifstream info_file(file_path);

  if (!info_file.is_open()) {
    std::cerr << "Error: can't open assets info file" << std::endl;
    assets_info.audio_count = -1;
    assets_info.models_count = -1;
    assets_info.textures_count = -1;
    return assets_info;
  }
  std::string line;
  std::getline(info_file, line); // zip filename
  assets_info.zip_name = std::move(line);
  std::getline(info_file, line); // url
  assets_info.url = std::move(line);
  std::cout << assets_info.url << std::endl;
  assets_info.hash.resize(picosha2::k_digest_size); // hash
  for (int i = 0; i < 32; ++i)
    std::cout << info_file.get() << std::endl;
//  info_file.read(reinterpret_cast<char*>(assets_info.hash.data()),
//                 picosha2::k_digest_size);
  std::getline(info_file, line);
  std::cout << line << std::endl;
  std::getline(info_file, line); // redirection count
  std::cout << line << std::endl;
  assets_info.redirection_count = std::stoi(line);
  std::getline(info_file, line); // audio count
  assets_info.audio_count = std::stoi(line);
  std::getline(info_file, line); // models count
  assets_info.models_count = std::stoi(line);
  std::getline(info_file, line); // textures count
  assets_info.textures_count = std::stoi(line);
  if (!info_file) {
    std::cerr << "Error: can't read assets info file" << std::endl;
    assets_info.audio_count = -1;
    assets_info.models_count = -1;
    assets_info.textures_count = -1;
  }
  return assets_info;
}
