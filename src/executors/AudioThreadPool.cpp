#include "AudioThreadPool.h"

namespace faithful {
namespace details {

namespace audio {

std::size_t OggReadCallback(void* ptr, std::size_t size,
                            std::size_t nmemb, void* datasource) {
  StreamingAudioData* audio = reinterpret_cast<StreamingAudioData*>(datasource);
  ALsizei length = size * nmemb;

  if(audio->sizeConsumed + length > audio->size) {
    length = audio->size - audio->sizeConsumed;
  }

  if(!audio->file.is_open()) {
    audio->file.open(audio->filename, std::ios::binary);
    if(!audio->file.is_open()) {
      std::cerr << "Error: Can't open streaming file: "
                << audio->filename << std::endl;
      return 0;
    }
  }

  auto buffer = std::make_unique<char>(length);

  audio->file.clear();
  audio->file.seekg(audio->sizeConsumed);
  if(!audio->file.read(buffer.get(), length)) {
    if (audio->file.eof()) {
      audio->file.clear();
    } else if(audio->file.fail()) {
      std::cerr << "Error libogg: fail / bad stream" << audio->filename << std::endl;
      audio->file.clear();
      return 0;
    }
  }
  audio->sizeConsumed += length;

  std::memcpy(ptr, buffer.get(), length);

  audio->file.clear();

  return length;
}

int OggSeekCallback(void *datasource, ogg_int64_t offset, int whence) {
  StreamingAudioData* audio =
      reinterpret_cast<StreamingAudioData*>(datasource);

  if(whence == SEEK_CUR) {
    audio->sizeConsumed += offset;
  } else if(whence == SEEK_END) {
    audio->sizeConsumed = audio->size - offset;
  } else if(whence == SEEK_SET) {
    audio->sizeConsumed = offset;
  } else {
    return -1;
  }

  if(audio->sizeConsumed < 0) {
    audio->sizeConsumed = 0;
    return -1;
  }

  if(audio->sizeConsumed > audio->size) {
    audio->sizeConsumed = audio->size;
    return -1;
  }

  return 0;
}

long OggTellCallback(void* datasource) {
  StreamingAudioData* audio =
      reinterpret_cast<StreamingAudioData*>(datasource);
  return audio->sizeConsumed;
}

} // namespace audio



void AudioThreadPool::PlayCurrent() {
  alCall(alSourceStop, audioData.source);
  alCall(alSourcePlay, audioData.source);
}

void AudioThreadPool::UpdateCurrent() {
  ALint buffersProcessed = 0;
  alCall(alGetSourcei, audioData.source, AL_BUFFERS_PROCESSED, &buffersProcessed);
  if(buffersProcessed <= 0) {
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
