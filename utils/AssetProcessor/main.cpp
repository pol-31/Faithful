/** The purpose of this sub-application to convert all assets from
 * given directory into certain (and further run-time decoding):
 * video:
 *   container: .webm;
 *   codec: VP9.
 * audio:
 *   container: .ogg;
 *   codec: Vorbis.
 * textures:
 *   container: raw .astc if (KHR_texture_compression_astc_hdr,
 *                            KHR_texture_compression_astc_ldr) supported,
 *              otherwise .ktx;
 *              OR simple .bin format for height/displacement map (etc)
 *   codec: astc 4x4
 * 3D models:
 *   glTF (.glb container format)
 * */

#include "../Logger.h"
#include <filesystem>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h> // TODO: find useful and utilize them

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>

#include "../../external/stb_image/stb_image.h"

extern "C" {
#include "astc/astc_codec_internals.h"
#include "astc/astcenccli_internal.h"
#include "ktx.h"
}

// Function to compress an image to ASTC using astc-encoder
void compressToASTC(const std::string& inputPath, const std::string& outputPath, int block_size) {
  // Load the image using stb_image
  int width, height, channels;
  stbi_uc* imageData = stbi_load(inputPath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

  if (!imageData) {
    std::cerr << "Error: Failed to load image." << std::endl;
    exit(EXIT_FAILURE);
  }

  astc_codec_image* astcImage = alloc_image(4, width, height, 1);

  // Populate ASTC image data
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      int src_offset = (y * width + x) * 4;
      int dst_offset = ((y * width + x) * 4) / 16;

      for (int c = 0; c < 4; ++c) {
        astcImage->imagedata8[dst_offset + c] = imageData[src_offset + c];
      }
    }
  }

  astc_codec_image* astcResult = compress_image(astcImage, block_size);

  // Write compressed ASTC data to file
  FILE* astcFile = fopen(outputPath.c_str(), "wb");
  fwrite(astcResult->imagedata8, 1, astcResult->data_size, astcFile);
  fclose(astcFile);

  // Cleanup
  free_image(astcImage);
  free_image(astcResult);
  stbi_image_free(imageData);
}

// Function to create a KTX container from ASTC using libktx
void createKTXFromASTC(const std::string& inputPath, const std::string& outputPath) {
  // Load the compressed ASTC data
  std::ifstream astcFile(inputPath, std::ios::binary);
  std::vector<uint8_t> astcData((std::istreambuf_iterator<char>(astcFile)), std::istreambuf_iterator<char>());
  astcFile.close();

  // Use libktx to create a KTX container
  ktxTextureCreateInfo textureCreateInfo;
  memset(&textureCreateInfo, 0, sizeof(textureCreateInfo));
  textureCreateInfo.baseWidth = 0; // Auto-detect from ASTC data
  textureCreateInfo.baseHeight = 0; // Auto-detect from ASTC data
  textureCreateInfo.baseDepth = 1;
  textureCreateInfo.numDimensions = 2;
  textureCreateInfo.numLevels = 1;
  textureCreateInfo.numLayers = 1;
  textureCreateInfo.isArray = KTX_FALSE;
  textureCreateInfo.pixelFormat = KTX_RGBA32UI;
  textureCreateInfo.colorSpace = KTX_sRGB;

  ktxTexture* ktxTexture;
  ktxResult result = ktxTexture_CreateFromMemory(astcData.data(), astcData.size(),
                                                 KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &textureCreateInfo,
                                                 &ktxTexture);

  if (result != KTX_SUCCESS) {
    std::cerr << "Error: KTX creation failed." << std::endl;
    exit(EXIT_FAILURE);
  }

  // Save the KTX container to a file
  ktxTexture_WriteToNamedFile(ktxTexture, outputPath.c_str());

  // Clean up
  ktxTexture_Destroy(ktxTexture);
}

/* TODO: macroses:
 * - default destination asset directory (Faithful/assets/...);
 * - default source asset directory (from where to load);
 * - macros for Logger.h (Assimp, libav, OpenAL, etc)
 * */

/* TODO (uncompleted section): Supported formats listed there:
 * Assimp: https://assimp.sourceforge.net/lib_html/index.html
 * ffmpeg: https://github.com/nothings/stb/blob/master/stb_image.h
 *    (exception: .gif not supported by now)
 * */

// TODO: executing --> AssetProcessor <source/file> <flags>

//  ------------------------------------------------------
//  ------------------------------------------------------
//  ------------------------------------------------------
// TODO: animation frames optimization (!)
//  ------------------------------------------------------
//  ------------------------------------------------------
//  ------------------------------------------------------

// texture info: path:id - in readable file (e.g. texture_info.txt)
// each texture has its own .bin file (currently) todo
// at the beginning of executing (AssetProcessor) we load this texture_info.txt into std::map (?)
// and then before each loading check out this map

// the same with models/audio/video - that's how LoadPathManager.h works

// all images has the same params, so the only data we want to save in .bin its texture_id
// which can be the same as global_image_id

// load texture_info.txt (only full_path(?) : texture_id) and create LoadPathManager_instance
//    by path we find texture_id
// load each texture looking at LoadPathManager_instance map
// append texture_info.txt if register new entry in LoadPathManager

// TODO: find all assets recursively

bool ProcessModel(const std::string& filePath) {
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    // Assimp couldn't open the file
    std::cerr << "Assimp failed to open file: " << importer.GetErrorString() << std::endl;
    return false;
  }

  std::cout << "Assimp successfully opened the file." << std::endl;
  // Process the file using Assimp
  return true;
}

bool ProcessImage(const std::string& filePath) {
  // TODO: rewrite using ffmpeg
}

bool ProcessMultimedia(const std::string& filePath) {
  // we need 3 streams: audio, video, subtitles
  // TODO:
  return true;
}

bool ProcessAsset(const std::filesystem::path& filename) {
  // TODO: rewrite (there is no stb_image now)
  if (!ProcessModel(filename))
    if (!ProcessImage(filename))
      if (!ProcessMultimedia(filename))
        return false;

  return true;
}


int main(int argc, char* argv[]) {
  const std::string astcOutputPath = "../output.astc";
  const std::string ktxOutputPath = "../output.ktx";

  // Compress the image to ASTC
  compressToASTC("../input.png", astcOutputPath, ASTCEncode::ASTC_ENC_4x4);

  // Create a KTX container from the compressed ASTC image
  createKTXFromASTC(astcOutputPath, ktxOutputPath);

/*
  const std::string filePath = "../test.wav";  // Replace with the actual file path
//  const std::string filePath = "../Be'lakor - Absit Omen (HD) ( 160kbps ).mp3";  // Replace with the actual file path


  faithful::ConsoleLogger logger;
  AVFormatContext *pFormatContext = avformat_alloc_context();
  logger.LogIf(faithful::LogType::kFatal, !pFormatContext,
               "could not allocate memory for Format Context");
  logger.LogIf(
    faithful::LogType::kFatal,
    avformat_open_input(&pFormatContext, filePath.c_str(), NULL, NULL) != 0,
    "could not open the file"
  );
  logger.LogIf(
    faithful::LogType::kFatal,
    avformat_find_stream_info(pFormatContext,  NULL) < 0,
    "could not get the stream info"
  );

  AVCodec *pCodec = NULL;
  AVCodecParameters *pCodecParameters =  NULL;
  int audio_stream_index = -1;

  for (int i = 0; i < pFormatContext->nb_streams; ++i) {
    AVCodecParameters *pLocalCodecParameters =  NULL;
    pLocalCodecParameters = pFormatContext->streams[i]->codecpar;
    AVCodec *pLocalCodec = nullptr;
    pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);
    logger.LogIf(faithful::LogType::kFatal, !pLocalCodec,
                 "unsupported codec");

    // when the stream is a video we store its index, codec parameters and codec
    if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
      logging("Video Codec: resolution %d x %d", pLocalCodecParameters->width, pLocalCodecParameters->height);
    } else if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
      if (audio_stream_index == -1) {
        audio_stream_index = i;
        pCodec = pLocalCodec;
        pCodecParameters = pLocalCodecParameters;
      }
      logging("Audio Codec: %d channels, sample rate %d", pLocalCodecParameters->channels, pLocalCodecParameters->sample_rate);
    }

    logging("\tCodec %s ID %d bit_rate %lld", pLocalCodec->name, pLocalCodec->id, pLocalCodecParameters->bit_rate);
  }

  if (audio_stream_index == -1) {
    logging("File %s does not contain a video stream!", filePath.c_str());
    return -1;
  }

  AVCodecContext *pCodecContext = avcodec_alloc_context3(pCodec);
  if (!pCodecContext)
  {
    logging("failed to allocated memory for AVCodecContext");
    return -1;
  }

  if (avcodec_parameters_to_context(pCodecContext, pCodecParameters) < 0)
  {
    logging("failed to copy codec params to codec context");
    return -1;
  }

  // Initialize the AVCodecContext to use the given AVCodec.
  // https://ffmpeg.org/doxygen/trunk/group__lavc__core.html#ga11f785a188d7d9df71621001465b0f1d
  if (avcodec_open2(pCodecContext, pCodec, NULL) < 0)
  {
    logging("failed to open codec through avcodec_open2");
    return -1;
  }

//  check_al_errors();
  // Initialize OpenAL
  std::cout << "---------------------------" << std::endl;
  // OpenAL initialization

  list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));

  ALCdevice* device = alcOpenDevice("OpenAL Soft");
  //ALCdevice* device = alcOpenDevice(nullptr);  // Use the default audio device
  if (!device) {
    std::cerr << "Failed to open OpenAL device" << std::endl;
    return 1;
  }

  ALCcontext* context = alcCreateContext(device, nullptr);
  if (!context) {
    std::cerr << "Failed to create OpenAL context" << std::endl;
    alcCloseDevice(device);
    return 1;
  }

  if (!alcMakeContextCurrent(context)) {
    std::cerr << "Failed to make OpenAL context current" << std::endl;
    alcDestroyContext(context);
    alcCloseDevice(device);
    return 1;
  }

  // Print some OpenAL information
  const ALchar* version = alGetString(AL_VERSION);
  const ALchar* renderer = alGetString(AL_RENDERER);
  const ALchar* vendor = alGetString(AL_VENDOR);

  std::cout << "OpenAL Version: " << version << std::endl;
  std::cout << "OpenAL Renderer: " << renderer << std::endl;
  std::cout << "OpenAL Vendor: " << vendor << std::endl;

  // Perform some OpenAL operations
  ALuint buffer;
  alGenBuffers(1, &buffer);

  ALenum error = alGetError();
  if (error != AL_NO_ERROR) {
    std::cerr << "Error creating OpenAL buffer: " << error << std::endl;
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    alcCloseDevice(device);
    return 1;
  }

  std::cout << "OpenAL buffer created successfully" << std::endl;

  ALuint source;
  alGenSources(1, &source);
  //check_al_errors();

  AVFrame* frame = av_frame_alloc();
  if (!frame) {
    std::cerr << "Failed to allocate AVFrame." << std::endl;
  }

  AVPacket* packet = av_packet_alloc();
  if (!packet) {
    std::cerr << "Failed to allocate AVPacket." << std::endl;
  }

  std::cout << audio_stream_index << std::endl;

  // Read audio data from FFmpeg and fill OpenAL buffer
  while (av_read_frame(pFormatContext, packet) >= 0) {
    if (packet->stream_index == audio_stream_index) {
      int response = avcodec_send_packet(pCodecContext, packet);
      if (response < 0) {
        std::cout << "Error while sending a packet to the decoder" << std::endl;
        return response;
      }
      while (response >= 0) {
        response = avcodec_receive_frame(pCodecContext, frame);
        if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
          break;
        } else if (response < 0) {
          std::cout << "Error while receiving a frame from the decoder" << std::endl;
          return response;
        }

        // Unqueue processed buffers
//        ALint buffersProcessed;
//        alGetSourcei(source, AL_BUFFERS_PROCESSED, &buffersProcessed);
//        while (buffersProcessed > 0) {
//          ALuint buffer;
//          alSourceUnqueueBuffers(source, 1, &buffer);
//          buffersProcessed--;
//        }

        // Fill and queue new buffer
        alBufferData(buffer, AL_FORMAT_STEREO16, frame->data[0], frame->linesize[0], frame->sample_rate);
        alSourceQueueBuffers(source, 1, &buffer);

        */
/*if (response >= 0) {
          alSourceUnqueueBuffers(source, 1, &buffer);
          alBufferData(buffer, AL_FORMAT_STEREO16, frame->data[0], frame->linesize[0], frame->sample_rate);
          alSourceQueueBuffers(source, 1, &buffer);
        }*//*

      }
    }
    av_packet_unref(packet);
  }

  alSourcePlay(source);
  //check_al_errors();

  // Wait until audio playback is complete
  ALint state;
  do {
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    check_al_errors();
  } while (state == AL_PLAYING);

  // Clean up
  av_packet_free(&packet);
  alDeleteBuffers(1, &buffer);
  alcMakeContextCurrent(nullptr);
  alcDestroyContext(context);
  alcCloseDevice(device);

  CleanUpFFmpeg(pFormatContext, pCodecContext, frame);


  alDeleteBuffers(1, &buffer);
  alcMakeContextCurrent(nullptr);
  alcDestroyContext(context);
  alcCloseDevice(device);


  return 0;

  faithful::ConsoleLogger logger;
  logger.LogIf(faithful::LogType::kFatal, argc != 2, "Usage: <source_path> <destination_path>");
  std::filesystem::path path(argv[1]);

  try {
    if (std::filesystem::is_directory(path)) {
      for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
        if (std::filesystem::is_regular_file(path))
          logger.LogIf(faithful::LogType::kError, ProcessAsset(path), path.c_str());
      }
    } else {
      if (std::filesystem::is_regular_file(path)) {
        logger.LogIf(faithful::LogType::kError, ProcessAsset(path), "can't be decoded");
      } else {
        logger.LogIf(faithful::LogType::kError, ProcessAsset(path), "is not a regular file");
      }
    }
  } catch (const std::exception& e) {
    logger.Log(faithful::LogType::kError, e.what());
  }
*/

  return 0;
}

#include <audio/wave.h>
