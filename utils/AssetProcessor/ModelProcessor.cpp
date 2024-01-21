#include "ModelProcessor.h"

#include <cstdlib>
#include <fstream>
#include <iostream>

#include <tiny_gltf.h>
#include <rapidjson/document.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/istreamwrapper.h>

#include "AssetCategory.h"
#include "AssetInfo.h"
#include "AssetsAnalyzer.h"

void ModelProcessor::Process(const std::filesystem::path& model_path,
                             const std::filesystem::path& path_suffix) {
  if (encode_) {
    if (model_path.extension() == ".glb") {
      ConvertGlbToGltf(model_path, path_suffix);
      // TODO: another model path in this case
    } else {
      ExtractBufferData(model_path, path_suffix);
    }
//    ExtractGltfModelTextures();
//    EncodeGltfModelTextures();
  } else {
    DecodeGltfModel(model_path, path_suffix);
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

void ModelProcessor::ExtractBufferData(
    const std::filesystem::path& model_path,
    const std::filesystem::path& path_suffix) {
  // TODO: extract everything embedded to gltf json (encoded in Base64)
  /*
  int data_pos = uri.find(',');
  int extension_start_pos = uri.find('/');
  int extension_end_pos = uri.find(';');
  if (data_pos == static_cast<int>(std::string::npos) ||
      extension_start_pos == static_cast<int>(std::string::npos) ||
      extension_end_pos == static_cast<int>(std::string::npos)) {
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
  assets_analyzer_->AddEntry(
      {out_filename.string(), path_suffix.string(), category});*/
}

void ModelProcessor::ConvertGlbToGltf(
    const std::filesystem::path& model_path,
    const std::filesystem::path& path_suffix) {
  std::ifstream glbFile(model_path, std::ios::binary);
  if (!glbFile.is_open()) {
    std::cerr << "Error opening GLB file: " << model_path << std::endl;
    return;
  }

  char header[12];
  glbFile.read(header, 12);

  if (glbFile.gcount() != 12 || memcmp(header, "glTF", 4) != 0 ||
      memcmp(header + 4, "\x02\x00\x00\x00", 4) != 0) {
    std::cerr << "Invalid GLB file format: " << model_path << std::endl;
    return;
  }

  uint32_t jsonChunkLength;
  glbFile.read(reinterpret_cast<char*>(&jsonChunkLength), sizeof(uint32_t));

  uint32_t chunk_type;
  glbFile.read(reinterpret_cast<char*>(&chunk_type), sizeof(uint32_t));
  // Assume we always have JSON before BIN data
  assert(chunk_type == 0x4E4F534A);

  std::string jsonData(jsonChunkLength, '\0');
  glbFile.read(jsonData.data(), jsonChunkLength);

  rapidjson::Document document;
  document.Parse(jsonData.c_str());
  if (document.HasParseError()) {
    std::cerr << "Error parsing JSON section: "
              << rapidjson::ParseErrorCode(document.GetParseError())
              << std::endl;
    return;
  }

  // write bin data
  uint32_t binChunkLength;
  glbFile.read(reinterpret_cast<char*>(&binChunkLength), sizeof(uint32_t));

  glbFile.read(reinterpret_cast<char*>(&chunk_type), sizeof(uint32_t));
//  assert(chunk_type == 0x4E49424F);

  std::string binData(binChunkLength, '\0');
  glbFile.read(binData.data(), binChunkLength);

  std::filesystem::path bin_file_url = temp_dir_ / path_suffix.parent_path() /
                                   path_suffix.stem() / ".bin";

  std::filesystem::create_directories(bin_file_url.parent_path());

  std::ofstream binFile(bin_file_url, std::ios::binary);
  if (!binFile.is_open()) {
    std::cerr << "Error opening output BIN file." << std::endl;
    return;
  }

  binFile.write(binData.c_str(), binData.size());

  binFile.close();

  //
  //
  std::cout << bin_file_url.c_str() << std::endl;
  if (document.HasMember("buffers") && document["buffers"].IsArray()) {
    auto& buffersArray = document["buffers"];

    // Assuming there is only one buffer entry in the array
    if (buffersArray.Size() == 1) {
      auto& bufferEntry = buffersArray[0];

      // Update the "uri" field with your URL
      rapidjson::Value uriValue;
      uriValue.SetString(bin_file_url.c_str(), document.GetAllocator());

      // Add the "uri" field to the buffer entry
      bufferEntry.AddMember("uri", uriValue, document.GetAllocator());
    } else {
      std::cerr << "Error: Unexpected number of buffer entries in the 'buffers' array." << std::endl;
    }
  } else {
    std::cerr << "Error: 'buffers' array not found in the JSON document." << std::endl;
  }
  //
  //

  std::ofstream jsonFile(temp_dir_ / path_suffix.parent_path() /
                           path_suffix.stem() / ".gltf");
  if (!jsonFile.is_open()) {
    std::cerr << "Error opening output JSON file." << std::endl;
    return;
  }

  rapidjson::OStreamWrapper osw(jsonFile);
  rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
  document.Accept(writer);
  jsonFile.close();
}


void ModelProcessor::ExtractGltfModelTextures(
    int buffer_view_id __attribute__((unused)),
    const std::string& out_filename __attribute__((unused))) {
  // TODO: the worse case
  // TODO: find offset in needed buffer, extract it, sheer rest data to left,
  //      change offset, id for all buffers which are later
  //    - if that buffer is stored separately - open it; otherwise
  std::cout << "Processing embedded data" << std::endl;
  // Your logic for handling embedded data goes here
}


void ModelProcessor::EncodeGltfModelTextures(
    const std::filesystem::path& path_suffix, rapidjson::Value& images,
    rapidjson::Document& document) {

  /// has it been already added to ThreadPool tasks by AssetsAnalyzer
/*  auto image_absolute_path = std::filesystem::current_path() / uri;
  std::filesystem::path relativePath =
      std::filesystem::relative(image_absolute_path, user_asset_root_dir_);
  if (!relativePath.empty() && relativePath.string().find("..") != 0) {
    return;
  }
  if (!dir_created) {
    std::filesystem::create_directories(path_suffix);
  }
  auto image_copy_path = extracted_textures_dir_ / uri;
  std::filesystem::copy_file(image_absolute_path, image_copy_path);
  AssetCategory texture_category = DeduceAssetEncodeCategory(uri);
  assets_analyzer_->AddEntry({image_copy_path.string(),
                              path_suffix.string(), texture_category});*/
  int i = 0;
  bool dir_created = false;
  std::filesystem::path dir_absolute_path =
      (extracted_textures_dir_ / path_suffix).remove_filename();
  std::filesystem::path file_stem;
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
//        ExtractBase64ImageData(uri, path_suffix,
//                               "here need old filename",
//                               category);  // TODO: incorrect path
      } else {
//        ExtractExternalImageData(uri, path_suffix, dir_created);
      }
    } else if (image.HasMember("bufferView")) {
      if (!dir_created) {
        std::filesystem::create_directories(dir_absolute_path);
      }
      int buffer_view_id = image["bufferView"].GetInt();
      ExtractGltfModelTextures(buffer_view_id, outputFileName.string());
    } else {
      return;
    }
    image["name"].SetString(outputFileName.stem().string().c_str(),
                            document.GetAllocator());
    image["mimeType"].SetString("image/astc", document.GetAllocator());
    image["uri"].SetString(outputFileName.string().c_str(),
                           document.GetAllocator());
  }
}



void ModelProcessor::DecodeGltfModel(
    const std::filesystem::path& model_path __attribute__((unused)),
    const std::filesystem::path& path_suffix __attribute__((unused))) {
  std::cout << "Modle decoding is not implemented yet" << std::endl;
  // TODO:
}