#ifndef FAITHFUL_ASSETPROCESSOR_H
#define FAITHFUL_ASSETPROCESSOR_H

#include <filesystem>

#include "AssetsAnalyzer.h"
#include "AssetLoadingThreadPool.h"
#include "ReplaceRequest.h"

#include "AudioProcessor.h"
#include "ModelProcessor.h"
#include "TextureProcessor.h"

class AssetProcessor {
 public:
  AssetProcessor(int thread_count);

  void Process(std::filesystem::path destination,
               std::filesystem::path source,
               bool encode);

 private:
  void EncodeAssets(AssetsAnalyzer& assets_analyzer);
  void DecodeAssets(AssetsAnalyzer& assets_analyzer);

  AssetLoadingThreadPool thread_pool_;
  ReplaceRequest replace_request_;
  AudioProcessor audio_processor_;
  TextureProcessor texture_processor_;
  ModelProcessor model_processor_;
};

#endif  // FAITHFUL_ASSETPROCESSOR_H
