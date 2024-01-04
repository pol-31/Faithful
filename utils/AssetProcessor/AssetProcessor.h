#ifndef FAITHFUL_ASSETPROCESSOR_H
#define FAITHFUL_ASSETPROCESSOR_H

#include <filesystem>

#include "AssetLoadingThreadPool.h"

class AssetProcessor {
 public:
  AssetProcessor(int thread_count,
                 const std::filesystem::path& asset_destination);
  ~AssetProcessor();

  void ProcessEncoding(const std::filesystem::path& path);

  void ProcessDecoding(const std::filesystem::path& path);

 private:
  AssetLoadingThreadPool* thread_pool_;
  int thread_count_;

  std::filesystem::path asset_destination_;
};

#endif //FAITHFUL_ASSETPROCESSOR_H
