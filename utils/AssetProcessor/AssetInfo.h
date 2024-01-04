#ifndef ASSETPROCESSOR_ASSETINFO_H
#define ASSETPROCESSOR_ASSETINFO_H

#include <filesystem>

#include "AssetCategory.h"

struct AssetInfo {
  AssetInfo(std::string&& file_path, std::string&& file_relative_path,
            AssetCategory file_category)
    : source_path(file_path), relative_path(file_relative_path),
      category(file_category) {}
  std::filesystem::path source_path;
  std::filesystem::path relative_path;
  AssetCategory category;
};

#endif //ASSETPROCESSOR_ASSETINFO_H
