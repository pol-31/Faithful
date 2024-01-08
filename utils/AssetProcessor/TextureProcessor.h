#ifndef ASSETPROCESSOR_IMAGEPROCESSOR_H
#define ASSETPROCESSOR_IMAGEPROCESSOR_H

#include <filesystem>
#include <string>

#include "astc-encoder/Source/astcenc.h"

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

/** Faithful project doesn't uses cases with a lot of small
 * images (because i won't), so all textures are encoded/decoded
 * one by one, so Codec creates only 1 context (per hdr, ldr, nmap) with
 * utilizing of all created threads
 * */
class TextureProcessor {
 public:
  TextureProcessor(bool encode, const std::filesystem::path& asset_destination,
                   AssetLoadingThreadPool* thread_pool);

  ~TextureProcessor();

  bool SwitchToLdr();
  bool SwitchToHdr();
  bool SwitchToNmap();

  void Process(const std::filesystem::path& path,
               const std::filesystem::path& path_suffix,
               AssetCategory category);

 private:
  bool InitContextLdr();
  bool InitContextHdr();
  bool InitContextNmap();

  bool Encode(const std::filesystem::path& path,
              const std::filesystem::path& path_suffix, AssetCategory category);
  /// currently decompresses only as a 4-channels image
  bool Decode(const std::filesystem::path& path,
              const std::filesystem::path& path_suffix, AssetCategory category);

  bool WriteEncodedData(const std::string& filename, unsigned int image_x,
                        unsigned int image_y, int comp_data_size,
                        const uint8_t* comp_data);

  void PrepareCompData(int image_x, int image_y, int& comp_len,
                       uint8_t*& comp_data);

  void PrepareEncodeContextAndFilename(
      const std::filesystem::path& relative_path, AssetCategory category,
      std::filesystem::path& out_filename, astcenc_context*& context);

  void PrepareDecodeContextAndFilename(const std::filesystem::path& path,
                                       AssetCategory category,
                                       std::string& out_filename,
                                       astcenc_context* context);

  bool ReadAstcFile(const std::string& path, int& width, int& height,
                    int& comp_len, uint8_t*& comp_data);

  astcenc_context* context_ldr_ = nullptr;
  astcenc_context* context_hdr_ = nullptr;
  astcenc_context* context_nmap_ = nullptr;
  AssetLoadingThreadPool* thread_pool_ = nullptr;

  /// storing there for convenience
  /// (we could ask thread_pool_ for it each time, but this is better)
  int worker_thread_count_;

  bool encode_;
  std::filesystem::path asset_destination_;
};

#endif  // ASSETPROCESSOR_IMAGEPROCESSOR_H
