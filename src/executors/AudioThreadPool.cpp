#include "AudioThreadPool.h"

namespace faithful {
namespace details {

void AudioThreadPool::PlayCurrent() {
  alCall(alSourceStop, audioData.source);
  alCall(alSourcePlay, audioData.source);
}

void AudioThreadPool::UpdateCurrent() {
  ALint buffersProcessed = 0;
  alCall(alGetSourcei, audioData.source, AL_BUFFERS_PROCESSED, &buffersProcessed);
  if(buffersProcessed <= 0)
  {
    return;
  }
  while(buffersProcessed--)
  {
    ALuint buffer;
    alCall(alSourceUnqueueBuffers, audioData.source, 1, &buffer);

    char* data = new char[BUFFER_SIZE];
    std::memset(data,0,BUFFER_SIZE);

    ALsizei dataSizeToBuffer = 0;
    std::int32_t sizeRead = 0;

    while(sizeRead < BUFFER_SIZE)
    {
      std::int32_t result = ov_read(&audioData.oggVorbisFile, &data[sizeRead], BUFFER_SIZE - sizeRead, 0, 2, 1, reinterpret_cast<int*>(&audioData.oggCurrentSection));
      if(result == OV_HOLE)
      {
        std::cerr << "ERROR: OV_HOLE found in update of buffer " << std::endl;
        break;
      }
      else if(result == OV_EBADLINK)
      {
        std::cerr << "ERROR: OV_EBADLINK found in update of buffer " << std::endl;
        break;
      }
      else if(result == OV_EINVAL)
      {
        std::cerr << "ERROR: OV_EINVAL found in update of buffer " << std::endl;
        break;
      }
      else if(result == 0)
      {
        std::int32_t seekResult = ov_raw_seek(&audioData.oggVorbisFile, 0);
        if(seekResult == OV_ENOSEEK)
          std::cerr << "ERROR: OV_ENOSEEK found when trying to loop" << std::endl;
        else if(seekResult == OV_EINVAL)
          std::cerr << "ERROR: OV_EINVAL found when trying to loop" << std::endl;
        else if(seekResult == OV_EREAD)
          std::cerr << "ERROR: OV_EREAD found when trying to loop" << std::endl;
        else if(seekResult == OV_EFAULT)
          std::cerr << "ERROR: OV_EFAULT found when trying to loop" << std::endl;
        else if(seekResult == OV_EOF)
          std::cerr << "ERROR: OV_EOF found when trying to loop" << std::endl;
        else if(seekResult == OV_EBADLINK)
          std::cerr << "ERROR: OV_EBADLINK found when trying to loop" << std::endl;

        if(seekResult != 0)
        {
          std::cerr << "ERROR: Unknown error in ov_raw_seek" << std::endl;
          return;
        }
      }
      sizeRead += result;
    }
    dataSizeToBuffer = sizeRead;

    if(dataSizeToBuffer > 0)
    {
      alCall(alBufferData, buffer, audioData.format, data, dataSizeToBuffer, audioData.sampleRate);
      alCall(alSourceQueueBuffers, audioData.source, 1, &buffer);
    }

    if(dataSizeToBuffer < BUFFER_SIZE)
    {
      std::cout << "Data missing" << std::endl;
    }

    ALint state;
    alCall(alGetSourcei, audioData.source, AL_SOURCE_STATE, &state);
    if(state != AL_PLAYING)
    {
      alCall(alSourceStop, audioData.source);
      alCall(alSourcePlay, audioData.source);
    }

    delete[] data;
  }
}

void AudioThreadPool::StopCurrent() {
  alCall(alSourceStop, audioData.source);
}


} // namespace details
} // namespace faithful
