#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

#include "../../external/miniz/miniz.h" // TODO:______________

#ifndef FILE_ALREADY_DOWNLOADED
#include "curl/curl.h"
#endif


#if defined(__GNUC__)
// Ensure we get the 64-bit variants of the CRT's file I/O calls
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE 1
#endif
#endif


// TODO: replace by .in.h file (or .h.in -idk:D)

static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream) {
  return fwrite(ptr, size, nmemb, (FILE*)stream);
}

bool DownloadAssetZip(const std::string& url, const std::string& dst) {

#ifndef FILE_ALREADY_DOWNLOADED
  CURL* curl = curl_easy_init();
  if (!curl) {
    std::cerr << "Error initializing libcurl" << std::endl;
    return false;
  }
  FILE* file = fopen(dst.c_str(), "wb");
  if (!file) {
    std::cerr << "Error opening file for writing" << std::endl;
    curl_easy_cleanup(curl);
    return false;
  }

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
  CURLcode res = curl_easy_perform(curl);

  if (res != CURLE_OK) {
    fclose(file);
    curl_easy_cleanup(curl);
    std::cerr << "cURL request failed: " << curl_easy_strerror(res)
              << std::endl;
    return false;
  }

  fclose(file);
  curl_easy_cleanup(curl);

  std::cout << "Download file: " << dst << std::endl;
#endif
  return true;
}

bool UnzipAndInstallAssets(const std::string& dst, const std::string& src) {
}


bool extractZip(const char* targetDir, const char* zipFilename) {
  mz_zip_archive zip_archive;
  memset(&zip_archive, 0, sizeof(zip_archive));

  if (!mz_zip_reader_init_file(&zip_archive, zipFilename, 0)) {
    std::cerr << "Error initializing zip archive reader\n";
    return false;
  }

  for (mz_uint i = 0; i < mz_zip_reader_get_num_files(&zip_archive); i++) {
    mz_zip_archive_file_stat file_stat;
    if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) {
      std::cerr << "Error getting file stat for entry " << i << "\n";
      mz_zip_reader_end(&zip_archive);
      return false;
    }

    std::filesystem::path targetPath =
        std::filesystem::path(targetDir) / file_stat.m_filename;
    if (file_stat.m_is_directory) {
      try {
        std::filesystem::create_directories(targetPath);
      } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error creating directory " << targetPath << ": "
                  << e.what() << "\n";
        mz_zip_reader_end(&zip_archive);
        return false;
      }
    } else {
      std::cout << "Extracting: " << targetPath << "\n";
      mz_uint uncompressed_size = static_cast<mz_uint>(file_stat.m_uncomp_size);
      std::vector<char> file_data(uncompressed_size);

      if (!mz_zip_reader_extract_to_mem(&zip_archive, i, file_data.data(),
                                        uncompressed_size, 0)) {
        std::cerr << "Error extracting file " << targetPath << "\n";
        mz_zip_reader_end(&zip_archive);
        return false;
      }

      std::ofstream output(targetPath, std::ios::binary);
      if (!output.write(file_data.data(), uncompressed_size)) {
        std::cerr << "Error writing to file " << targetPath << "\n";
        mz_zip_reader_end(&zip_archive);
        return false;
      }
    }
  }

  mz_zip_reader_end(&zip_archive);
  return true;
}


enum AssetDownloaderError {
  kArgumentsFaulure = 1,
  kDownloadFailure = 2,
  kUnzipFailure = 3
};

int main(int argc, char** argv) {
  // TODO: don't need command-line arguments
  //       need .h.in file with: audio/models/texture-path, url
  if (argc != 3) {
    std::cout << "incorrect program arguments"
              << "\n\tusage: AssetDownloader <dest_path> <source_url>"
              << "\nwhere: source_url - asset .zip url,"
              << "\nwhere: dest_path - path to assets/" << std::endl;
    return AssetDownloaderError::kArgumentsFaulure;
  }
  curl_global_init(CURL_GLOBAL_DEFAULT);

  std::string assets_path(argv[1]);
  std::string assets_url(argv[2]);

  std::string assets_zip_destination("assets.zip");

  if (DownloadAssetZip(assets_url, assets_zip_destination)) {
    return AssetDownloaderError::kDownloadFailure;
  }

  const char* zipFilename = "../not_mine_test_assets.zip";
  const char* targetDir = "build";

  if (extractZip(zipFilename, targetDir)) {
    std::cout << "Extraction successful!\n";
  } else {
    std::cerr << "Extraction failed.\n";
  }

  // TODO: .glb --> models
  //       .ogg --> audio
  //       .astc -> textures

  if (UnzipAndInstallAssets(assets_destination, assets_zip_destination)) {
    return AssetDownloaderError::kUnzipFailure;
  }

  curl_global_cleanup();
  return 0;
}
