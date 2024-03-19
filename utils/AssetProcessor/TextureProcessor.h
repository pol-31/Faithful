#ifndef ASSETPROCESSOR_IMAGEPROCESSOR_H
#define ASSETPROCESSOR_IMAGEPROCESSOR_H

#include <filesystem>
#include <memory>
#include <string>

//#include <astc-encoder/Source/astcenc.h>
#include "../../external/astc-encoder/Source/astcenc.h"

enum class AssetCategory;
class AssetLoadingThreadPool;

struct AstcHeader {
  uint8_t magic[4];  /// format identifier
  uint8_t block_x;
  uint8_t block_y;
  uint8_t block_z;
  uint8_t dim_x[3];
  uint8_t dim_y[3];
  uint8_t dim_z[3];
};

// TODO: explain about "rg" (2-channel swizzle)

/** Faithful project doesn't uses cases with a lot of small
 * images (because i won't), so all textures are encoded/decoded
 * one by one, so Codec creates only 1 context (per hdr, ldr, rg) with
 * utilizing of all created threads
 * */
class TextureProcessor {
 public:
  TextureProcessor(bool encode,
                   const std::filesystem::path& asset_destination,
                   const std::filesystem::path& user_asset_root_dir,
                   AssetLoadingThreadPool* thread_pool);

  ~TextureProcessor();

  bool SwitchToLdr();
  bool SwitchToHdr();
  bool SwitchToRG();

  /// processing from file and memory
  /// for file processing we don't need destination path because it'll
  /// be deduced as a relative to user_asset_root_dir_ from asset_destination_
  void Process(const std::filesystem::path& texture_path,
               AssetCategory category);
  void Process(const std::filesystem::path& dest_path,
               std::unique_ptr<uint8_t[]> image_data,
               int width, int height,
               AssetCategory category);

 private:
  bool InitContextLdr();
  bool InitContextHdr();
  bool InitContextRG();

  bool Encode(const std::filesystem::path& texture_path,
              AssetCategory category);
  bool Encode(const std::filesystem::path& dest_path,
              std::unique_ptr<uint8_t[]> image_data,
              int width, int height,
              AssetCategory category);

  /// currently decompresses only as a 4-channels image
  /// we also don't need destination path there because it'll
  /// be deduced as a relative to user_asset_root_dir_ from asset_destination_
  bool Decode(const std::filesystem::path& texture_path,
              AssetCategory category);

  bool WriteEncodedData(std::string filename, unsigned int image_x,
                        unsigned int image_y, int comp_data_size,
                        const uint8_t* comp_data);
  bool WriteDecodedData(std::string filename, unsigned int image_x,
                        unsigned int image_y, AssetCategory category,
                        std::unique_ptr<uint8_t[]> image_data);

  int CalculateCompLen(int image_x, int image_y);

  std::pair<astcenc_context*, std::string> ProvideEncodeContextAndFilename(
      const std::filesystem::path& dest_path, AssetCategory category);

  astcenc_context* ProvideDecodeContext(AssetCategory category);

  bool ReadAstcFile(const std::string& path, int& width, int& height,
                    int& comp_len, std::unique_ptr<uint8_t[]>& comp_data);

  std::filesystem::path asset_destination_;
  std::filesystem::path user_asset_root_dir_;

  astcenc_context* context_ldr_ = nullptr;
  astcenc_context* context_hdr_ = nullptr;
  astcenc_context* context_rg_ = nullptr;
  AssetLoadingThreadPool* thread_pool_ = nullptr;

  bool encode_;
};

#endif  // ASSETPROCESSOR_IMAGEPROCESSOR_H
