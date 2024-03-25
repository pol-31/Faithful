#ifndef ASSETPROCESSOR_MODELPROCESSOR_H
#define ASSETPROCESSOR_MODELPROCESSOR_H

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>

#include <tiny_gltf.h>

#include "TextureProcessor.h"
#include "ReplaceRequest.h"

/// while decompression we load images on our own because of ".astc" extension,
/// which is not supported by GLTF 2.0 spec
bool TinygltfLoadTextureStub(tinygltf::Image *image, const int image_idx,
                             std::string *err, std::string *warn, int req_width,
                             int req_height, const unsigned char *bytes,
                             int size, void *user_data);

class ModelProcessor {
 public:
  ModelProcessor() = delete;
  ModelProcessor(TextureProcessor& texture_processor,
                 ReplaceRequest& replace_request);

  void Encode(const std::filesystem::path& path);
  void Decode(const std::filesystem::path& path);

  const std::vector<std::string>& GetProcessedImages() const {
    return processed_images_;
  }

  void SetDestinationDirectory(const std::filesystem::path& path);

  private:
   struct ModelTextureConfig {
     std::filesystem::path out_path;
     TextureProcessor::TextureCategory category;
   };
  bool Read();
  bool Write(const std::string& destination);

  void CompressTextures();
  void DecompressTextures();

  ModelTextureConfig DeduceEncodeTextureCategory(int model_image_id);

  /// filename stem as an input parameter
  ModelTextureConfig DeduceDecodeTextureCategory(std::string_view path);

  bool OptimizeModel(const std::string& path);

  /// in case if texture embedded, we directly ask texture processor to process
  TextureProcessor& texture_processor_;

  ReplaceRequest& replace_request_;

  std::vector<std::string> processed_images_;

  std::filesystem::path cur_model_path_;
  std::unique_ptr<tinygltf::Model> model_;
  tinygltf::TinyGLTF loader_;
  std::string error_string_;
  std::string warning_string_;
  std::filesystem::path models_destination_path_;
};

#endif  // ASSETPROCESSOR_MODELPROCESSOR_H
