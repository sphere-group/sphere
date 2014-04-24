#ifndef MAC_AUDIO_HPP
#define MAC_AUDIO_HPP

#include <audiere.h>
#include <iostream>

#include "mac_sphere_config.h"

#include "../../common/strcmp_ci.hpp"

namespace audiere {
      inline audiere::FileFormat GuessFormat(const char* filename)
    {
        if (filename == NULL) return audiere::FF_AUTODETECT;
        if (strcmp_ci(filename + strlen(filename) - 4, ".ogg")  == 0) return audiere::FF_OGG;
        if (strcmp_ci(filename + strlen(filename) - 4, ".mp3")  == 0 ||
            strcmp_ci(filename + strlen(filename) - 4, ".mp2")  == 0) return audiere::FF_MP3;
        if (strcmp_ci(filename + strlen(filename) - 3, ".it")   == 0 ||
            strcmp_ci(filename + strlen(filename) - 3, ".xm")   == 0 ||
            strcmp_ci(filename + strlen(filename) - 4, ".mod")  == 0 ||
            strcmp_ci(filename + strlen(filename) - 4, ".s3m")  == 0) return audiere::FF_MOD;
        if (strcmp_ci(filename + strlen(filename) - 4, ".wav")  == 0) return audiere::FF_WAV;
        if (strcmp_ci(filename + strlen(filename) - 4, ".spx")  == 0) return audiere::FF_SPEEX;
        if (strcmp_ci(filename + strlen(filename) - 5, ".aiff") == 0 ||
            strcmp_ci(filename + strlen(filename) - 5, ".aifc") == 0) return audiere::FF_AIFF;
        if (strcmp_ci(filename + strlen(filename) - 5, ".flac") == 0) return audiere::FF_FLAC;
        return audiere::FF_AUTODETECT;
    }

}

bool InitAudio(SPHERECONFIG* config);
void CloseAudio();

audiere::OutputStream* SA_OpenSound(audiere::File* file, const char* filename, bool streaming);
audiere::SoundEffect*  SA_OpenSoundEffect(audiere::File* file, const char* filename, audiere::SoundEffectType type);

#endif
