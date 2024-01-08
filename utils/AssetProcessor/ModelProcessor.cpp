#include "ModelProcessor.h"

#include <cstdlib>
#include <fstream>
#include <iostream>

//#define TINYGLTF_IMPLEMENTATION
//#define TINYGLTF_USE_RAPIDJSON
//#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "tiny_gltf.h"
#include "rapidjson/document.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/istreamwrapper.h"

#include "AssetCategory.h"
#include "AssetInfo.h"
#include "AssetsAnalyzer.h"

void ModelProcessor::Process(const std::filesystem::path& model_path,
                             const std::filesystem::path& path_suffix) {
  std::cout << "Model processing is not available by now" << std::endl;
  /*
   * TODO: read bin data, extract textures data
   * TODO: update other bufferViews
   *
   * */
  return;
  if (model_path.extension() == ".glb") {
    if (encode_) {
      EncodeGlbModel(model_path, path_suffix);
    } else {
      DecodeGlbModel(model_path, path_suffix);
    }
  } else {  // .gltf
    if (encode_) {
      EncodeGltfModel(model_path, path_suffix);
    } else {
      DecodeGltfModel(model_path, path_suffix);
    }
  }
  // TODO: optimization
  ////   TODO: -noq?
  //  const char* command =
  //  "/home/pavlo/CLionProjects/provingGround/cmake-build-debug/glad-build/gltfpack
  //  -noq -i /home/pavlo/Downloads/DamagedHelmet.glb  -o final_test.gltf"; if
  //  (std::system(command) == 0) {
  //    std::cout << "Optimized" << std::endl;
  //  }
}

void ModelProcessor::EncodeGlbModel(const std::filesystem::path& model_path,
                                    const std::filesystem::path& path_suffix) {
  std::ifstream model_file(model_path, std::ios::binary);
  if (!model_file.is_open()) {
    std::cerr << "Failed to open the glb file." << std::endl;
    return;
  }

  std::vector<uint8_t> glbBuffer((std::istreambuf_iterator<char>(model_file)),
                                 std::istreambuf_iterator<char>());
  model_file.close();

  if (glbBuffer.size() < 8 || memcmp(glbBuffer.data(), "glTF", 4) != 0) {
    std::cerr << "Invalid glb file format." << std::endl;
    return;
  }

  uint32_t jsonChunkSize = *reinterpret_cast<uint32_t*>(&glbBuffer[12]);
  size_t jsonChunkOffset = 20;  // Header size + JSON chunk header size
  size_t binaryChunkOffset = jsonChunkOffset + jsonChunkSize;

  std::string jsonChunk(glbBuffer.begin() + jsonChunkOffset,
                        glbBuffer.begin() + binaryChunkOffset);

  rapidjson::Document document;
  document.Parse(jsonChunk.c_str());

  uint32_t binaryChunkSize =
      *reinterpret_cast<uint32_t*>(&glbBuffer[binaryChunkOffset]);
  uint32_t binaryChunkType =
      *reinterpret_cast<uint32_t*>(&glbBuffer[binaryChunkOffset + 4]);

  if (binaryChunkType != 0x004E4942) {
    std::cerr << "Invalid binary chunk type." << std::endl;
    return;
  }

  std::string binaryChunk(glbBuffer.begin() + binaryChunkOffset + 8,
                          glbBuffer.begin() + binaryChunkType);

  // TODO: read bin data, extract textures data
  // TODO: update other bufferViews

  std::string base64BinaryData = tinygltf::base64_encode(
      reinterpret_cast<const unsigned char*>(binaryChunk.c_str()),
      binaryChunk.size());

  std::string uri_data;
  uri_data.reserve(binaryChunkSize + 37);  // + prefix "data:application...:
  uri_data += "data:application/octet-stream;base64,";
  uri_data += base64BinaryData;
  document["buffers"][0]["uri"].SetString(uri_data.c_str(), uri_data.size());

  std::filesystem::path out_model_name =
      model_path.filename().replace_extension(".gltf");
  std::filesystem::path out_model_path =
      asset_destination_ / path_suffix / out_model_name;
  std::ofstream outputJsonFile(out_model_path);
  rapidjson::OStreamWrapper jsonStream(outputJsonFile);
  rapidjson::Writer<rapidjson::OStreamWrapper> jsonWriter(jsonStream);
  document.Accept(jsonWriter);
  std::cout << "converted glb to gltf successfully" << std::endl;
}

void ModelProcessor::EncodeGltfModel(const std::filesystem::path& model_path,
                                     const std::filesystem::path& path_suffix) {
  rapidjson::Document document;
  std::ifstream model_in_file(model_path);
  rapidjson::IStreamWrapper gltfStream(model_in_file);
  document.ParseStream(gltfStream);

  // TODO: check is open, use std::fstream instead
  // TODO: create Temp directory(if not created yet) for extracted but not
  // processed yet

  // we need: path_suffix (generated for each model)
  //     including model name (as a dir to create after)

  if (document.HasMember("images") && document["images"].IsArray()) {
    rapidjson::Value& images = document["images"];
    EncodeGltfModelTextures(path_suffix, images, document);
    model_in_file.close();
    std::filesystem::path out_model_path =
        asset_destination_ / path_suffix / model_path.filename();
    std::ofstream model_out_file(out_model_path);
    rapidjson::OStreamWrapper jsonStream(model_out_file);
    rapidjson::Writer<rapidjson::OStreamWrapper> jsonWriter(jsonStream);
    document.Accept(jsonWriter);
    model_out_file.close();
  }
}

void ModelProcessor::ExtractBase64ImageData(
    const std::string& uri, const std::filesystem::path& path_suffix,
    const std::filesystem::path& out_filename, AssetCategory category) {
  int data_pos = uri.find(',');
  int extension_start_pos = uri.find('/');
  int extension_end_pos = uri.find(';');
  if (data_pos == std::string::npos ||
      extension_start_pos == std::string::npos ||
      extension_end_pos == std::string::npos) {
    std::cout << "incorrect uri for " << out_filename << std::endl;
    return;
  }
  std::string base64Data = uri.substr(data_pos + 1);
  base64Data.pop_back();
  auto decodedData = tinygltf::base64_decode(base64Data);

  extension_start_pos += 1;
  int extension_len = extension_end_pos - extension_start_pos;
  std::ofstream outputFile(out_filename, std::ios::binary);

  std::cout << out_filename << "<-- here we writing" << std::endl;

  outputFile.write(reinterpret_cast<const char*>(decodedData.data()),
                   decodedData.size());
  assets_analyzer_->AddEntry({std::move(out_filename), path_suffix, category});
}

// TODO: we can skip it for now, because it's not common case
void ModelProcessor::ExtractBufferImageData(int buffer_view_id,
                                            const std::string& out_filename) {
  // TODO: the worse case
  // TODO: find offset in needed buffer, extract it, sheer rest data to left,
  //      change offset, id for all buffers which are later
  //    - if that buffer is stored separately - open it; otherwise
  std::cout << "Processing embedded data" << std::endl;
  // Your logic for handling embedded data goes here
}

void ModelProcessor::ExtractExternalImageData(
    const std::string& uri, const std::filesystem::path& path_suffix,
    bool dir_created) {
  /// has it been already added to ThreadPool tasks by AssetsAnalyzer
  auto image_absolute_path = std::filesystem::current_path() / uri;
  std::filesystem::path relativePath =
      std::filesystem::relative(image_absolute_path, user_asset_root_dir_);
  if (!relativePath.empty() && relativePath.native().find("..") != 0) {
    return;
  }
  if (!dir_created) {
    std::filesystem::create_directories(path_suffix);
  }
  auto image_copy_path = extracted_textures_dir_ / uri;
  std::filesystem::copy_file(image_absolute_path, image_copy_path);
  AssetCategory texture_category = DeduceAssetEncodeCategory(uri);
  assets_analyzer_->AddEntry(
      {std::move(image_copy_path), path_suffix, texture_category});
}

void ModelProcessor::EncodeGltfModelTextures(
    const std::filesystem::path& path_suffix, rapidjson::Value& images,
    rapidjson::Document& document) {
  int i = 0;
  bool dir_created = false;
  std::filesystem::path dir_absolute_path =
      (extracted_textures_dir_ / path_suffix).remove_filename();
  std::string file_stem;
  for (rapidjson::Value::ValueIterator itr = images.Begin();
       ++i, itr != images.End(); ++itr) {
    rapidjson::Value& image = *itr;
    std::filesystem::path outputFileName(dir_absolute_path);

    std::string buffer;
    buffer.reserve(10);
    AssetCategory category;
    if (image.HasMember("name") &&
        image.HasMember("mimeType")) {  // TODO: what a allocation mess...
      buffer += image["mimeType"].GetString();
      buffer = buffer.substr(buffer.find('/') + 1);  // TODO; 1
      category = DeduceAssetEncodeCategory(image["name"].GetString() + buffer);
      buffer.clear();
      if (category == AssetCategory::kTextureLdr) {
        buffer += "_ldr.astc";
      } else if (category == AssetCategory::kTextureHdr) {
        buffer += "_hdr.astc";
      } else {
        buffer += "_nmap.astc";
      }
      file_stem = outputFileName /
                  (std::to_string(i) + "_" +
                   std::filesystem::path(image["name"].GetString()).string());
      ;  // TODO; 2
      outputFileName /=
          std::to_string(i) + "_" +
          std::filesystem::path(image["name"].GetString()).string() + buffer;
    } else {
      outputFileName /= std::to_string(i) + "_" + buffer;
    }  // TODO: uri, mime...

    if (image.HasMember("uri")) {
      std::string uri = image["uri"].GetString();
      if (uri.compare(0, 5, "data:") == 0) {
        if (!dir_created) {
          std::filesystem::create_directories(dir_absolute_path);
        }
        ExtractBase64ImageData(uri, path_suffix,
                               /* here need old filename */ "",
                               category);  // TODO: incorrect path
      } else {
        ExtractExternalImageData(uri, path_suffix, dir_created);
      }
    } else if (image.HasMember("bufferView")) {
      if (!dir_created) {
        std::filesystem::create_directories(dir_absolute_path);
      }
      int buffer_view_id = image["bufferView"].GetInt();
      ExtractBufferImageData(buffer_view_id, outputFileName);
    } else {
      return;
    }
    image["name"].SetString(outputFileName.stem().c_str(),
                            document.GetAllocator());
    image["mimeType"].SetString("image/astc", document.GetAllocator());
    image["uri"].SetString(outputFileName.c_str(), document.GetAllocator());
  }
}

void ModelProcessor::DecodeGlbModel(const std::filesystem::path& model_path,
                                    const std::filesystem::path& path_suffix) {
}

void ModelProcessor::DecodeGltfModel(const std::filesystem::path& model_path,
                                     const std::filesystem::path& path_suffix) {
}