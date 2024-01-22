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

#include "../../config/Paths.h"

void ModelProcessor::Process(const std::filesystem::path& model_path,
                             const std::filesystem::path& path_suffix) {
  std::filesystem::path model_final_path = asset_destination_ / path_suffix;
  model_final_path.replace_extension(".gltf");
  if (encode_) {
    bool success;
    if (model_path.extension() == ".glb") {
      success = ConvertGlbToGltf(model_path, path_suffix);
    } else {
      success = ReadGltfModel(model_path);
    }
    if (!success) {
      return;
    }

    if (!ExtractBuffers()) {
      return;
    };
    if (!ExtractTextures()) {
      return;
    };
    if (!SaveModel(document, model_final_path)) {
      return;
    };
    if (!OptimizeModel(model_final_path)) {
      return;
    };
  } else {
    std::cout << "Model decoded: " << model_path << std::endl;
    if (!DecodeTextures(model_path, path_suffix)) {
      return;
    };
    if (!SaveModel(document, model_final_path)) {
      return;
    };
  }
}

// if base64 - convert to binary -> assets/models/${model_suffix}/.bin
bool ModelProcessor::ExtractBuffers() {
  //
}

// if url/base64 - convert -> assets/textures/${model_suffix}
// if bufferview - embed from .bin + remark bufferview
bool ModelProcessor::ExtractTextures() {
  //
}

bool ModelProcessor::SaveModel(const rapidjson::Document& document,
                               const std::string& model_path) {
  std::ofstream jsonFile(model_path);
  if (!jsonFile.is_open()) {
    std::cerr << "Error opening output JSON file." << std::endl;
    return false;
  }

  rapidjson::OStreamWrapper osw(jsonFile);
  rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
  document.Accept(writer);
  jsonFile.close();
  return true;
}

bool ModelProcessor::ReadGltfModel(const std::string& model_path) {
  std::ifstream fileStream(model_path, std::ios::in);
  if (!fileStream.is_open()) {
    std::cerr << "Error: can't open model: " << model_path << std::endl;
    return false;
  }

  std::ostringstream fileContent;
  fileContent << fileStream.rdbuf();
  std::string gltfContent = fileContent.str();

  document.Parse(gltfContent.c_str());

  if (document.HasParseError()) {
    std::cerr << "Error parsing JSON: " << document.GetParseError() << std::endl;
    return false;
  }
  return true;
}

// in-place gltfpack
bool ModelProcessor::OptimizeModel(const std::string& model_path) {
  std::string command;
  command.reserve(std::strlen(FAITHFUL_GLTFPACK_PATH) +
                  model_path.size() * 2 + 12); // 12 for flags, whitespaces
  command = FAITHFUL_GLTFPACK_PATH;
  command += " -tr -i "; // no changes for textures
  command += model_path;
  command += " -o ";
  command += model_path;
  if (std::system(command.c_str()) != 0) {
    std::cerr << "Error: unable to optimize model" << std::endl;
    return false;
  }
  std::cout << "Optimized" << std::endl;
  return true;
}

std::string ModelProcessor::DecodeBase64(
    const std::string& base64_data) {
  // TODO: Base64 data (buffers / images)
  //   auto decodedData = tinygltf::base64_decode(base64Data);
  // TODO: how to say about error - return ""
}

bool ModelProcessor::ConvertGlbToGltf(
    const std::filesystem::path& model_path,
    const std::filesystem::path& path_suffix) {
  std::ifstream glbFile(model_path, std::ios::binary);
  if (!glbFile.is_open()) {
    std::cerr << "Error opening GLB file: " << model_path << std::endl;
    return false;
  }

  char header[12];
  glbFile.read(header, 12);

  if (glbFile.gcount() != 12 || memcmp(header, "glTF", 4) != 0 ||
      memcmp(header + 4, "\x02\x00\x00\x00", 4) != 0) {
    std::cerr << "Invalid GLB file format: " << model_path << std::endl;
    return false;
  }

  uint32_t jsonChunkLength;
  glbFile.read(reinterpret_cast<char*>(&jsonChunkLength), sizeof(uint32_t));

  uint32_t chunk_type;
  glbFile.read(reinterpret_cast<char*>(&chunk_type), sizeof(uint32_t));
  // Assume we always have JSON before BIN data
  assert(chunk_type == 0x4E4F534A);

  std::string jsonData(jsonChunkLength, '\0');
  glbFile.read(jsonData.data(), jsonChunkLength);

  document.Parse(jsonData.c_str());
  if (document.HasParseError()) {
    std::cerr << "Error parsing JSON section: "
              << rapidjson::ParseErrorCode(document.GetParseError())
              << std::endl;
    return false;
  }

  // TODO: write to assets/models (!)
  // write bin data
  uint32_t binChunkLength;
  glbFile.read(reinterpret_cast<char*>(&binChunkLength), sizeof(uint32_t));

  glbFile.read(reinterpret_cast<char*>(&chunk_type), sizeof(uint32_t));
//  assert(chunk_type == 0x4E49424F);

  std::string binData(binChunkLength, '\0');
  glbFile.read(binData.data(), binChunkLength);

  std::filesystem::path bin_file_url = "";//temp_dir_ / path_suffix.parent_path() /
//                                   path_suffix.stem() / ".bin";

  std::filesystem::create_directories(bin_file_url.parent_path());

  std::ofstream binFile(bin_file_url, std::ios::binary);
  if (!binFile.is_open()) {
    std::cerr << "Error opening output BIN file." << std::endl;
    return false;
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

  std::filesystem::path model_dir_path("");//temp_dir_ / path_suffix.parent_path() /
//                                  path_suffix.stem());
  std::string model_filename;
  model_filename.reserve(model_dir_path.stem().string().length() + 5);
  model_filename = path_suffix.stem().string();
  model_filename += ".gltf";

  std::ofstream jsonFile(model_dir_path / model_filename);
  if (!jsonFile.is_open()) {
    std::cerr << "Error opening output JSON file." << std::endl;
    return false;
  }

  rapidjson::OStreamWrapper osw(jsonFile);
  rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
  document.Accept(writer);
  jsonFile.close();
  return true;
}


bool ModelProcessor::ExtractGltfModelTextures(
    const std::filesystem::path& model_path) {
  // TODO: check does any textures belongs to buffer
  // TODO: for each from previous step extract them to ${name}
  // TODO: ${name} generated from mime+name or on our own
}


bool ModelProcessor::EncodeGltfModelTextures(
    const std::filesystem::path& model_path) {
  // Now all textures located externally, so we need to work solely with "url"
  // to external file. Members mimetype/name/bufferview __already_deleted__


  // TODO: if texture location(url) is in root search dir OR is outside of the it
  //    add it to assets/textures/ (root of all textures) => suffix == ""

  // TODO: check has it been already added to ThreadPool tasks
  // TODO: ldr/hdr/nmap category deducing
  // TODO: paths: path suffix, rename
  // TODO: change url in .gltf
  // TODO: add textures to TextureProcessor queue for encoding
}


bool ModelProcessor::DecodeTextures(
    const std::filesystem::path& model_path,
    const std::filesystem::path& path_suffix) {
  // TODO: check has it been already added to ThreadPool tasks
  // TODO: change url in .gltf
  // TODO: add textures to TextureProcessor queue for decoding
}
