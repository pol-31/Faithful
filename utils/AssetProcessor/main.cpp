/** AssetProcessor converts assets into formats used by Faithful game internally:
 * - audio: .ogg + Vorbis
 * - textures: .astc
 * - 3D models: .gltf
 *
 * See compress config in Faithful/config/AssetFormats.h
 *
 * Supported:
 * - textures(images): bmp, hdr, HDR, jpeg, jpg, pgm, png, ppm, psd, tga;
 *     (for more information see faithful/external/stb/stb_image.h)
 * - audio: flac, mp3, ogg, wav
 * - 3D models: glb, gltf
 * */

#include <iostream>

#include "AssetProcessor.h"

#include "../../config/AssetFormats.h"

void UpdateAssetsInfoFile(const std::string& path, bool encoded) {
  std::ofstream log_file(path);
  if (!log_file.is_open()) {
    std::cout << "Error (no logging): Log file opening issues" << std::endl;
    return;
  }
  if (encoded) {
    // TODO: write to log.txt section "last encode"
  } else {
    // TODO: write to log.txt section "last decode"
  }
  std::terminate(); // NOT IMPLEMENTED
}

int main(int argc, char** argv) {
  /*if (argc != 3) {
    std::cout << "Incorrect program's arguments:\n"
              << "for encode: <destination> <source> e"
              << "for decode: <destination> <source> d"
              << std::endl;
    return 1;
  }
  std::string destination = argv[0];
  std::string source = argv[1];
  bool encode;
  if (argv[2][0] == 'e') {
    encode = true;
  } else if (argv[2][0] == 'd') {
    encode = false;
  } else {
    std::cout << "Incorrect program's arguments:\n"
              << "for encode: <destination> <source> e"
              << "for decode: <destination> <source> d"
              << std::endl;
    return 2;
  }

  if (destination == source) {
    std::cerr << "source can't be equal to destination" << std::endl;
    return 3;
  }*/

  AssetProcessor processor_encoder(faithful::config::asset_processor_thread_num);
  processor_encoder.Process("/home/pavlo/Desktop/assets_encoded",
                            "/home/pavlo/Desktop/assets_", true);

  AssetProcessor processor_decoder(faithful::config::asset_processor_thread_num);
  processor_decoder.Process("/home/pavlo/Desktop/assets_decoded",
                            "/home/pavlo/Desktop/assets_encoded", false);

  UpdateAssetsInfoFile("", false);

  //TODO:
  // ASK ONLY after each processed file to replace: if not - just add some
  //  random symbol or id to the end of the file and ask again... and so on...

  return 0;
}
