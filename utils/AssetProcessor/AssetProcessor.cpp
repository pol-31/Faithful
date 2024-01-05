#include "AssetProcessor.h"

#include <iostream>
#include <filesystem>

#include "AudioProcessor.h"
#include "ModelProcessor.h"
#include "TextureProcessor.h"

#include "AssetsAnalyzer.h"
#include "AssetLoadingThreadPool.h"

#include "../../config/Paths.h"

AssetProcessor::AssetProcessor(int thread_count,
                               const std::filesystem::path& asset_destination)
    : asset_destination_(asset_destination) {
  std::filesystem::create_directories(FAITHFUL_ASSET_TEMP_TEXTURES_PATH);
  thread_count_ = std::max(1, thread_count);
  thread_pool_ = new AssetLoadingThreadPool(thread_count_);
}

AssetProcessor::~AssetProcessor() {
  std::filesystem::path(FAITHFUL_ASSET_TEMP_TEXTURES_PATH).clear();
}

// TODO: SORTING !!! we need process models before textures,
//    because models can contain textures

void AssetProcessor::ProcessEncoding(const std::filesystem::path& path) {
  AssetsAnalyzer assets_info(*thread_pool_);
  assets_info.GatherEncodeThreadInfo(path);

  AudioProcessor audio_processor(true, asset_destination_ / "audio",
                                 thread_pool_);
  TextureProcessor texture_processor(true, asset_destination_ / "textures",
                                     thread_pool_);
  /*
   *
    bool encode,
    AssetsAnalyzer* assets_analyzer,
    const std::filesystem::path& user_asset_root_dir,
    const std::filesystem::path& asset_destination,
    const std::filesystem::path& temp_dir = FAITHFUL_ASSET_TEMP_TEXTURES_PATH)*/
  ModelProcessor model_processor(true, &assets_info, path,
                                 asset_destination_ / "models");

  thread_pool_->Run(&audio_processor, &model_processor);

  while (!thread_pool_->multi_threaded_tasks_.empty()) {
    // TODO: clear completion flags thread_pool.threads_tasks_completed
    AssetCategory category = thread_pool_->multi_threaded_tasks_.top().category;
    auto asset_path = thread_pool_->multi_threaded_tasks_.top().source_path;
    auto asset_suffix = thread_pool_->multi_threaded_tasks_.top().relative_path;
    if (category == AssetCategory::kAudioMusic) {
      audio_processor.ProcessMusic(asset_path, asset_suffix);
    } else {  // ldr, hdr, nmap textures
      texture_processor.Process(asset_path, asset_suffix, category);
    }
    // other tasks are single-threaded and will be processed later
    thread_pool_->multi_threaded_tasks_.pop();
  }
  thread_pool_->SwitchPhase();

  while (true) {
    while (thread_pool_->single_threaded_tasks_accessor_.test_and_set()) {
    }
    if (!thread_pool_->single_threaded_tasks_.empty()) {
      AssetLoadingThreadPool::ProcessSingleThreaded(
          &audio_processor, &model_processor,
          thread_pool_->single_threaded_tasks_.top());

      thread_pool_->single_threaded_tasks_.pop();
    } else {
      thread_pool_->single_threaded_tasks_accessor_
          .clear();  // TODO: memory order
      break;
    }
    thread_pool_->single_threaded_tasks_accessor_
        .clear();  // TODO: memory order
  }
  while (!thread_pool_->Completed()) {
  }
}
void AssetProcessor::ProcessDecoding(
    const std::filesystem::path& path) {  // path is destination folder
  AssetsAnalyzer assets_info(*thread_pool_);
  assets_info.GatherDecodeThreadInfo(FAITHFUL_ASSET_TEXTURE_PATH);

  AudioProcessor audio_processor(false, asset_destination_, thread_pool_);
  TextureProcessor texture_processor(false, asset_destination_, thread_pool_);
  ModelProcessor model_processor(false, &assets_info, path, asset_destination_);

  thread_pool_->Run(&audio_processor, &model_processor);

  while (!thread_pool_->multi_threaded_tasks_.empty()) {
    // TODO: clear completion flags thread_pool.threads_tasks_completed
    AssetCategory category = thread_pool_->multi_threaded_tasks_.top().category;
    auto asset_path = thread_pool_->multi_threaded_tasks_.top().source_path;
    if (category == AssetCategory::kAudioMusic) {
      audio_processor.ProcessMusic(asset_path, asset_destination_);
    } else {  // ldr, hdr, nmap textures
      texture_processor.Process(asset_path, asset_destination_, category);
    }
    // other tasks are single-threaded and will be processed later
    thread_pool_->multi_threaded_tasks_.pop();
  }

  thread_pool_->SwitchPhase();  // currently all decoding single-threaded

  while (true) {
    while (thread_pool_->single_threaded_tasks_accessor_.test_and_set()) {
    }
    if (!thread_pool_->single_threaded_tasks_.empty()) {
      AssetLoadingThreadPool::ProcessSingleThreaded(
          &audio_processor, &model_processor,
          thread_pool_->single_threaded_tasks_.top());

      thread_pool_->single_threaded_tasks_.pop();
    } else {
      thread_pool_->single_threaded_tasks_accessor_
          .clear();  // TODO: memory order
      break;
    }
    thread_pool_->single_threaded_tasks_accessor_
        .clear();  // TODO: memory order
  }
  while (!thread_pool_->Completed()) {
  }

  // TODO: decompress (all tasks single-threaded)
}
