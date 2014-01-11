#include "unix_audio.h"


static audiere::AudioDevicePtr s_AudioDevice;


////////////////////////////////////////////////////////////////////////////////

bool InitAudio(SPHERECONFIG* config)
{
  switch (config->sound) {

    case SOUND_AUTODETECT: // this doesn't really autodetect at all
      s_AudioDevice = audiere::OpenDevice();
    break;

    case SOUND_ON:
      s_AudioDevice = audiere::OpenDevice();
    break;

    case SOUND_OFF:
      s_AudioDevice = audiere::OpenDevice("null");
    break;
  }

  if (!s_AudioDevice) {
    s_AudioDevice = audiere::OpenDevice("null");
    std::cout << "Error opening sound device, is it busy perhaps?" << std::endl;
  }

  return bool(s_AudioDevice);
}

////////////////////////////////////////////////////////////////////////////////

void CloseAudio()
{
  s_AudioDevice = 0;
}

////////////////////////////////////////////////////////////////////////////////

audiere::AudioDevice* SA_GetAudioDevice()
{
  return s_AudioDevice.get();
}

////////////////////////////////////////////////////////////////////////////////

audiere::OutputStream* SA_OpenSound(audiere::File* file, const char* filename, bool streaming)
{
  return audiere::OpenSound(s_AudioDevice.get(), file, streaming, audiere::GuessFormat(filename));
}

////////////////////////////////////////////////////////////////////////////////

audiere::SoundEffect* SA_OpenSoundEffect(audiere::File* file, const char* filename, audiere::SoundEffectType type)
{
    return audiere::OpenSoundEffect(s_AudioDevice.get(), file, type, audiere::GuessFormat(filename));
}

////////////////////////////////////////////////////////////////////////////////