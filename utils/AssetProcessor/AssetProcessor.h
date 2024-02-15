#ifndef FAITHFUL_ASSETPROCESSOR_H
#define FAITHFUL_ASSETPROCESSOR_H

#include <filesystem>

#include "AssetsAnalyzer.h"
#include "ModelProcessor.h"

class AssetLoadingThreadPool;
class AudioProcessor;
class TextureProcessor;

class AssetProcessor {
 public:
  AssetProcessor(int thread_count);

  void Process(int encode, std::filesystem::path asset_destination,
               std::filesystem::path user_asset_root_dir, bool force);

 private:
  void RemoveTextureDuplicates(
      const std::vector<ModelProcessor::ProcessedImage>& processed,
      std::vector<AssetsAnalyzer::AssetInfo>& to_process);

  void ProcessAudio(AssetsAnalyzer* assets_info,
                    AudioProcessor* audio_processor);
  void ProcessModels(AssetsAnalyzer* assets_info,
                     ModelProcessor* model_processor);
  void ProcessTextures(AssetsAnalyzer* assets_info,
                       TextureProcessor* texture_processor,
                       ModelProcessor* model_processor);

  void DecodeModelsTextures(TextureProcessor* texture_processor);

  AssetLoadingThreadPool* thread_pool_;

  std::filesystem::path user_asset_root_dir_;

  std::filesystem::path asset_destination_;
  int thread_count_;
  bool encode_;
  bool force_;
};

#endif  // FAITHFUL_ASSETPROCESSOR_H
