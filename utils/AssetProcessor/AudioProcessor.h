#ifndef ASSETPROCESSOR_AUDIOPROCESSOR_H
#define ASSETPROCESSOR_AUDIOPROCESSOR_H


#include <filesystem>

#include "dr_libs/dr_libs/dr_mp3.h"
#include "vorbis/vorbis/include/vorbis/vorbisenc.h"
#include "vorbis/vorbis/include/vorbis/vorbisfile.h"
#include "vorbis/vorbis/include/vorbis/codec.h"

void decompressAudio(const char* inputFile, float** pPCM, uint64_t* frames, unsigned int* channels, unsigned int* sampleRate) {
  drmp3 mp3;
  if (!drmp3_init_file(&mp3, inputFile, NULL)) {
    // Error handling
    return;
  }

  *channels = mp3.channels;
  *sampleRate = mp3.sampleRate;

  // Read PCM data in chunks
  size_t chunkFrames = 4096;  // Adjust the chunk size as needed

  // Allocate initial buffer (you can increase it later if needed)
  size_t initialBufferSize = chunkFrames * (*channels);
  *pPCM = (float*)malloc(sizeof(float) * initialBufferSize);

  // Read PCM data
  *frames = 0;
  size_t framesRead;
  while ((framesRead = drmp3_read_pcm_frames_f32(&mp3, chunkFrames, *pPCM + (*frames * (*channels)))) > 0) {
    *frames += framesRead;
    // Resize buffer if needed
    size_t newSize = (*frames + chunkFrames) * (*channels);
    if (newSize > initialBufferSize) {
      *pPCM = (float*)realloc(*pPCM, sizeof(float) * newSize);
      initialBufferSize = newSize;
    }
  }

  std::cout << (*frames) << std::endl;


  // Clean up
  drmp3_uninit(&mp3);
}

void compressAudio(const char* outputFile, float* pPCM, uint64_t frames, unsigned int channels, unsigned int sampleRate) {
  ogg_stream_state os;
  vorbis_info vi;
  vorbis_comment vc;
  vorbis_dsp_state vd;
  vorbis_block vb;
  ogg_packet op;
  ogg_page og;

  // Initialize Vorbis structures
  vorbis_info_init(&vi);
  vorbis_encode_init_vbr(&vi, channels, sampleRate, 0.4);

  vorbis_comment_init(&vc);
  vorbis_comment_add_tag(&vc, "ENCODER", "My Encoder");

  vorbis_analysis_init(&vd, &vi);
  vorbis_block_init(&vd, &vb);

  // Open file for writing
  FILE* outFile = fopen(outputFile, "wb");

  // Initialize Ogg stream
  ogg_stream_init(&os, rand()); // TODO:________

  // Write Vorbis header to stream
  ogg_packet header, header_comm, header_code;
  vorbis_analysis_headerout(&vd, &vc, &header, &header_comm, &header_code);
  ogg_stream_packetin(&os, &header);
  ogg_stream_packetin(&os, &header_comm);
  ogg_stream_packetin(&os, &header_code);
  while (ogg_stream_flush(&os, &og)) {
    fwrite(og.header, 1, og.header_len, outFile);
    fwrite(og.body, 1, og.body_len, outFile);
  }
  int cur_pos = 0;
  int chunk_size = 1024 * 64;
  while (true) {
    float** buffer = vorbis_analysis_buffer(&vd, chunk_size);
    for (uint64_t i = 0; (i < chunk_size) && (cur_pos < frames); ++i, ++cur_pos) {
      for (unsigned int j = 0; j < channels; ++j) {
        buffer[j][i] = pPCM[cur_pos * channels + j];
      }
    }
    if (cur_pos >= frames) break;

    vorbis_analysis_wrote(&vd, chunk_size);
    //vorbis_analysis_wrote(&vd, 0);

    while (vorbis_analysis_blockout(&vd, &vb) == 1) {
      vorbis_analysis(&vb, NULL);
      vorbis_bitrate_addblock(&vb);
      vorbis_bitrate_flushpacket(&vd, &op);
      ogg_stream_packetin(&os, &op);
      while (ogg_stream_pageout(&os, &og)) {
        fwrite(og.header, 1, og.header_len, outFile);
        fwrite(og.body, 1, og.body_len, outFile);
      }
    }
  }


  // Clean up
  ogg_stream_clear(&os);
  vorbis_block_clear(&vb);
  vorbis_dsp_clear(&vd);
  vorbis_comment_clear(&vc);
  vorbis_info_clear(&vi);
  fclose(outFile);
}



void ProcessAudio(const std::filesystem::path& filename) {
  float* pPCM;
  uint64_t frames;
  unsigned int channels, sampleRate;

  const char* outputFile = "output.ogg";
  decompressAudio(filename.c_str(), &pPCM, &frames, &channels, &sampleRate);
  compressAudio(outputFile, pPCM, frames, channels, sampleRate);

  drmp3_free(pPCM, NULL);


  OggVorbis_File vorbisFile;

  // Open an Ogg Vorbis file (replace "your_audio_file.ogg" with your actual file)
  if (ov_fopen("output.ogg", &vorbisFile) < 0) {
    std::cerr << "Failed to open Ogg Vorbis file." << std::endl;
    return;
  }

  // Get information about the Ogg Vorbis file
  vorbis_info* info = ov_info(&vorbisFile, -1);
  if (info) {
    std::cout << "Channels: " << info->channels << std::endl;
    std::cout << "Sample rate: " << info->rate << " Hz" << std::endl;
    std::cout << "Total samples: " << ov_pcm_total(&vorbisFile, -1) << std::endl;
  } else {
    std::cerr << "Failed to retrieve Ogg Vorbis file information." << std::endl;
  }

  // Close the Ogg Vorbis file
  ov_clear(&vorbisFile);
}


#endif //ASSETPROCESSOR_AUDIOPROCESSOR_H
