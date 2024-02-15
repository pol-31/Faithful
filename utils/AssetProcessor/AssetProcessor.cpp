#include "AssetProcessor.h"

#include <iostream>
#include <filesystem>

#include "AudioProcessor.h"
#include "ModelProcessor.h"
#include "TextureProcessor.h"

#include "AssetsAnalyzer.h"
#include "AssetLoadingThreadPool.h"

AssetProcessor::AssetProcessor(int thread_count) {
  thread_count_ = std::max(1, thread_count);
  thread_pool_ = new AssetLoadingThreadPool(thread_count_);
}

void AssetProcessor::Process(
    int encode, std::filesystem::path asset_destination,
    std::filesystem::path user_asset_root_dir, bool force) {
  asset_destination_ = asset_destination;
  user_asset_root_dir_ = user_asset_root_dir;
  force_ = force;
  encode_ = encode;
  // each should know about thread pool
  auto main_assets_destination = asset_destination_ / "main";
  std::filesystem::create_directories(main_assets_destination);
  std::filesystem::create_directories(asset_destination_ / "models_textures");

  AssetsAnalyzer assets_info(encode_, main_assets_destination,
                             user_asset_root_dir_, force_);
  assets_info.CollectAssetsInfo(user_asset_root_dir);

  AudioProcessor audio_processor(encode_, main_assets_destination,
                                 user_asset_root_dir_, thread_pool_);
  TextureProcessor texture_processor(encode_, main_assets_destination,
                                     user_asset_root_dir_, thread_pool_);
  ModelProcessor model_processor(encode_, main_assets_destination,
                                 user_asset_root_dir_, &texture_processor);

  thread_pool_->Run();

  /*ProcessAudio(&assets_info, &audio_processor);
  if (!encode_) {
    DecodeModelsTextures(&texture_processor);
  }*/
  ProcessModels(&assets_info, &model_processor);
//  ProcessTextures(&assets_info, &texture_processor, &model_processor);

  thread_pool_->Join();
}


void AssetProcessor::ProcessAudio(AssetsAnalyzer* assets_info,
                                  AudioProcessor* audio_processor) {
  assets_info->HandleAudioDuplicates();
  auto audio_to_process = assets_info->GetAudioToProcess();
  for (const auto& audio : audio_to_process) {
    audio_processor->Process(audio.source_path, audio.category);
  }
}

void AssetProcessor::ProcessModels(AssetsAnalyzer* assets_info,
                                   ModelProcessor* model_processor) {
  assets_info->HandleModelDuplicates();
  auto& models_to_process = assets_info->GetModelsToProcess();
  for (auto& model : models_to_process) {
    model_processor->Process(model.source_path);
  }
}

void AssetProcessor::ProcessTextures(AssetsAnalyzer* assets_info,
                                     TextureProcessor* texture_processor,
                                     ModelProcessor* model_processor) {
  /// models have either ldr or 2-channel (rg),
  /// so we care both about ldr and rg there
  std::vector<AssetsAnalyzer::AssetInfo> ldr_textures_to_process =
      assets_info->GetLdrTexturesToProcess();
  RemoveTextureDuplicates(model_processor->GetProcessedImagesList(),
                          ldr_textures_to_process);

  std::vector<AssetsAnalyzer::AssetInfo> rg_textures_to_process =
      assets_info->GetRGTexturesToProcess();
  RemoveTextureDuplicates(model_processor->GetProcessedImagesList(),
                          rg_textures_to_process);

  /// delete duplications only after model processing and erasing of
  /// already processed textures (related to models)
  assets_info->HandleTextureDuplicates();

  for (const auto& texture : ldr_textures_to_process) {
    texture_processor->Process(texture.source_path, texture.category);
  }
  for (const auto& texture : rg_textures_to_process) {
    texture_processor->Process(texture.source_path, texture.category);
  }

  /// finally process hdr
  std::vector<AssetsAnalyzer::AssetInfo> hdr_textures_to_process =
      assets_info->GetHdrTexturesToProcess();
  for (const auto& texture : hdr_textures_to_process) {
    texture_processor->Process(texture.source_path, texture.category);
  }
}

void AssetProcessor::DecodeModelsTextures(
    TextureProcessor* texture_processor) {
  auto models_textures_path = asset_destination_ / "models_textures";
  std::filesystem::create_directories(models_textures_path);
  std::cout << "created " << models_textures_path << std::endl;
  // optimizing of context switching inside the TextureProcessor
  std::vector<std::filesystem::path> ldr_textures;
  std::vector<std::filesystem::path> hdr_textures;
  std::vector<std::filesystem::path> rg_textures;
  for (const auto& entry : std::filesystem::directory_iterator(models_textures_path)) {
    switch (DeduceAssetDecodeCategory(entry)) {
      case AssetCategory::kTextureLdr:
        ldr_textures.push_back(entry);
        break;
      case AssetCategory::kTextureHdr:
        hdr_textures.push_back(entry);
        break;
      case AssetCategory::kTextureRG:
        rg_textures.push_back(entry);
        break;
      default:
        std::cerr
            << "AssetProcessor::DecodeModelsTextures should contain only textures"
            << "\nSkip:" << entry << std::endl;
        continue;
    }
  }
  for (const auto& texture : ldr_textures) {
    texture_processor->Process(texture, AssetCategory::kTextureLdr);
  }
  for (const auto& texture : hdr_textures) {
    texture_processor->Process(texture, AssetCategory::kTextureHdr);
  }
  for (const auto& texture : rg_textures) {
    texture_processor->Process(texture, AssetCategory::kTextureRG);
  }
}

void AssetProcessor::RemoveTextureDuplicates(
    const std::vector<ModelProcessor::ProcessedImage>& processed,
    std::vector<AssetsAnalyzer::AssetInfo>& to_process) {
  if (encode_) {
    for (const auto& processed_img : processed) {
      /// elements are unique, no need to use std::remove_if
      auto found = std::find_if(
          to_process.begin(), to_process.end(),
          [&](const AssetsAnalyzer::AssetInfo& asset_info) {
            return asset_info.source_path.string() == processed_img.path;
          });
      if (found != to_process.end()) {
        to_process.erase(found);
      }
    }
  }
}
