#ifndef ASSETPROCESSOR_MODELPROCESSOR_H
#define ASSETPROCESSOR_MODELPROCESSOR_H

#include <filesystem>
#include <string>

#include "AssetCategory.h"
#include "../../config/Paths.h"

class TextureProcessor;

//#define STB_IMAGE_IMPLEMENTATION defined in tiny_gltf.h
#include "tinygltf/tiny_gltf.h"

// TODO: for HDR we still need prefix, still using u8 only
// TODO: function SafeDelete() which will delete model / Scan
//     all textures and check are they still used --> NOT DELETE BUT
//     AT LEAST PROVIDE INFO
// TODO: in case of tilemap make intend 6x6 on borders of textures
// TODO: explicitly say about not supporting of float16 / short int types
//     - we use only 8bit


// blocking, not thread-safe (crt not used in current RapidJSON "version")
class ModelProcessor {
 public:
  // used in TextureProcessor to know does the images was already processed
  struct ProcessedImage {
    std::string path;
    int id;
  };

  ModelProcessor(
      bool encode,
      const std::filesystem::path& asset_destination,
      const std::filesystem::path& user_asset_root_dir,
      TextureProcessor* texture_processor);

  void Process(std::filesystem::path model_path);

  const std::vector<ProcessedImage>& GetProcessedImagesList() const {
    return processed_images_;
  }

  private:
  bool Read();
  bool Write(const std::string& destination);

  void CompressTextures();
  void DecompressTextures();

  // for assets/{external, embedded}
  void FindLastTextureId();

  int MarkImageAsAProcessed(const std::string& image_path);
  AssetCategory DeduceTextureCategory(int image_id);

  bool OptimizeModel(const std::string& destination);

  mutable TextureProcessor* texture_processor_;
  mutable std::filesystem::path asset_destination_;

  mutable std::filesystem::path user_asset_root_dir_;
  mutable bool encode_;

  // not thread-safe, not safe at calling from multiple instances
  mutable int last_texture_id_ = 0;

  mutable std::vector<ProcessedImage> processed_images_;

  mutable std::filesystem::path cur_model_path_;

  mutable tinygltf::Model model_;
  mutable tinygltf::TinyGLTF loader_;
  mutable std::string error_string_;
  mutable std::string warning_string_;
};

#endif  // ASSETPROCESSOR_MODELPROCESSOR_H
