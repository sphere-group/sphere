#ifndef WIN32_AUDIO_HPP
#define WIN32_AUDIO_HPP

#include <audiere.h>

/*
// This function is now included in audiere, it may however be needed for unix or mac if audiere is not updated...
#include "../../common/strcmp_ci.hpp"

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

*/

audiere::OutputStream* SA_OpenSound(audiere::File* file, const char* filename, bool streaming);
audiere::SoundEffect*  SA_OpenSoundEffect(audiere::File* file, const char* filename, audiere::SoundEffectType type);

#if defined(WIN32) && defined(USE_MIDI)
audiere::MIDIStream* SA_OpenMIDI(const char* filename);
#endif


#endif
