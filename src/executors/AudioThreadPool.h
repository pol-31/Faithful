#ifndef FAITHFUL_SRC_EXECUTORS_AUDIOTHREADPOOL_H_
#define FAITHFUL_SRC_EXECUTORS_AUDIOTHREADPOOL_H_

#include "Executor.h"

#include <string>
#include <type_traits>
#include <iostream> // todo: replace
#include <fstream>
#include <cstring>

#include <AL/al.h>
#include <AL/alc.h>
#include <ogg/ogg.h>

#include <vorbis/vorbisfile.h>

namespace faithful {
namespace details {


const std::size_t NUM_BUFFERS = 4;
const ALsizei BUFFER_SIZE = 65536;

struct StreamingAudioData
{
  ALuint buffers[NUM_BUFFERS];
  std::string filename;
  std::ifstream file;
  std::uint8_t channels;
  std::int32_t sampleRate;
  std::uint8_t bitsPerSample;
  ALsizei size;
  ALuint source;
  ALsizei sizeConsumed = 0;
  ALenum format;
  OggVorbis_File oggVorbisFile;
  std::int_fast32_t oggCurrentSection = 0;
  std::size_t duration;
};


#define alCall(function, ...) alCallImpl(__FILE__, __LINE__, function, __VA_ARGS__)
#define alcCall(function, device, ...) alcCallImpl(__FILE__, __LINE__, function, device, __VA_ARGS__)

class AudioThreadPool : public Executor {
 public:

  void check_al_errors(const std::string& filename, const std::uint_fast32_t line)
  {
    ALCenum error = alGetError();
    if(error != AL_NO_ERROR)
    {
      std::cerr << "***ERROR*** (" << filename << ": " << line << ")\n" ;
      switch(error)
      {
        case AL_INVALID_NAME:
          std::cerr << "AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function";
          break;
        case AL_INVALID_ENUM:
          std::cerr << "AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function";
          break;
        case AL_INVALID_VALUE:
          std::cerr << "AL_INVALID_VALUE: an invalid value was passed to an OpenAL function";
          break;
        case AL_INVALID_OPERATION:
          std::cerr << "AL_INVALID_OPERATION: the requested operation is not valid";
          break;
        case AL_OUT_OF_MEMORY:
          std::cerr << "AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory";
          break;
        default:
          std::cerr << "UNKNOWN AL ERROR: " << error;
      }
      std::cerr << std::endl;
    }
  }

  template<typename alFunction, typename... Params>
  auto alCallImpl(const char* filename, const std::uint_fast32_t line, alFunction function, Params... params)
      ->typename std::enable_if<std::is_same<void,decltype(function(params...))>::value,decltype(function(params...))>::type
  {
    function(std::forward<Params>(params)...);
    check_al_errors(filename,line);
  }

  template<typename alFunction, typename... Params>
  auto alCallImpl(const char* filename, const std::uint_fast32_t line, alFunction function, Params... params)
      ->typename std::enable_if<!std::is_same<void,decltype(function(params...))>::value,decltype(function(params...))>::type
  {
    auto ret = function(std::forward<Params>(params)...);
    check_al_errors(filename,line);
    return ret;
  }

  std::size_t read_ogg_callback(void* destination, std::size_t size1, std::size_t size2, void* fileHandle)
  {
    StreamingAudioData* audioData = reinterpret_cast<StreamingAudioData*>(fileHandle);

    ALsizei length = size1 * size2;

    if(audioData->sizeConsumed + length > audioData->size)
      length = audioData->size - audioData->sizeConsumed;

    if(!audioData->file.is_open())
    {
      audioData->file.open(audioData->filename, std::ios::binary);
      if(!audioData->file.is_open())
      {
        std::cerr << "ERROR: Could not re-open streaming file \"" << audioData->filename << "\"" << std::endl;
        return 0;
      }
    }

    char* moreData = new char[length];

    audioData->file.clear();
    audioData->file.seekg(audioData->sizeConsumed);
    if(!audioData->file.read(&moreData[0],length))
    {
      if(audioData->file.eof())
      {
        audioData->file.clear(); // just clear the error, we will resolve it later
      }
      else if(audioData->file.fail())
      {
        std::cerr << "ERROR: OGG stream has fail bit set " << audioData->filename << std::endl;
        audioData->file.clear();
        return 0;
      }
      else if(audioData->file.bad())
      {
        perror(("ERROR: OGG stream has bad bit set " + audioData->filename).c_str());
        audioData->file.clear();
        return 0;
      }
    }
    audioData->sizeConsumed += length;

    std::memcpy(destination, &moreData[0], length);

    delete[] moreData;

    audioData->file.clear();

    return length;
  }

  std::int32_t seek_ogg_callback(void* fileHandle, ogg_int64_t to, std::int32_t type)
  {
    StreamingAudioData* audioData = reinterpret_cast<StreamingAudioData*>(fileHandle);

    if(type == SEEK_CUR)
    {
      audioData->sizeConsumed += to;
    }
    else if(type == SEEK_END)
    {
      audioData->sizeConsumed = audioData->size - to;
    }
    else if(type == SEEK_SET)
    {
      audioData->sizeConsumed = to;
    }
    else
      return -1; // what are you trying to do vorbis?

    if(audioData->sizeConsumed < 0)
    {
      audioData->sizeConsumed = 0;
      return -1;
    }
    if(audioData->sizeConsumed > audioData->size)
    {
      audioData->sizeConsumed = audioData->size;
      return -1;
    }

    return 0;
  }

  long int tell_ogg_callback(void* fileHandle)
  {
    StreamingAudioData* audioData = reinterpret_cast<StreamingAudioData*>(fileHandle);
    return audioData->sizeConsumed;
  }

  void PlayCurrent();
  void UpdateCurrent();
  void StopCurrent();

};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_EXECUTORS_AUDIOTHREADPOOL_H_
