#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include <windows.h>
#include <stdio.h>
#include <list>

#include "win32_audio.hpp"
#include "win32_internal.hpp"
#include "win32_sphere_config.hpp"
#include <audiere.h>


static audiere::AudioDevicePtr s_AudioDevice = NULL;

#if defined(WIN32) && defined(USE_MIDI)
    static audiere::MIDIDevicePtr s_MidiDevice = NULL;
#endif

////////////////////////////////////////////////////////////////////////////////
bool InitAudio(HWND window, SPHERECONFIG* config)
{
    const char* audiodriver = config->audiodriver.c_str();

    switch (config->sound)
    {
        case SOUND_AUTODETECT:
            s_AudioDevice = audiere::OpenDevice(audiodriver);
            if (!s_AudioDevice.get())
            {

                s_AudioDevice = audiere::OpenDevice("null");
            }
#if defined(WIN32) && defined(USE_MIDI)
            s_MidiDevice = audiere::OpenMIDIDevice("");
            if (!s_MidiDevice)
                s_MidiDevice = audiere::OpenMIDIDevice("null"); // BUG: Does not work

            return bool(s_AudioDevice.get() && s_MidiDevice.get());
#else
            return bool(s_AudioDevice.get());
#endif

        case SOUND_ON:
            s_AudioDevice = audiere::OpenDevice(audiodriver);
#if defined(WIN32) && defined(USE_MIDI)
            s_MidiDevice  = audiere::OpenMIDIDevice("");
            return bool(s_AudioDevice.get() && s_MidiDevice.get());
#else
            return bool(s_AudioDevice.get());
#endif

        case SOUND_OFF:
            s_AudioDevice = audiere::OpenDevice("null");
#if defined(WIN32) && defined(USE_MIDI)
            s_MidiDevice  = audiere::OpenMIDIDevice("null"); // BUG: midi always opens.
            return bool(s_AudioDevice.get() && s_MidiDevice.get());
#else
            return bool(s_AudioDevice.get());
#endif

        default:
            return false;
    }
}

////////////////////////////////////////////////////////////////////////////////
void CloseAudio()
{
    s_AudioDevice = 0;
#if defined(WIN32) && defined(USE_MIDI)
    s_MidiDevice = 0;
#endif
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
#if defined(WIN32) && defined(USE_MIDI)
audiere::MIDIStream* SA_OpenMIDI(const char* filename)
{
	if (!s_MidiDevice.get() || s_MidiDevice->getName() == "null") // again BUG: name always MDI
        return NULL;
    return s_MidiDevice.get()->openStream(filename);
}
#endif

////////////////////////////////////////////////////////////////////////////////

