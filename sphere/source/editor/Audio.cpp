#include "Audio.hpp"
#include "filename_comparer.hpp"

////////////////////////////////////////////////////////////////////////////////

static int s_AudioInitCount = 0;
static audiere::AudioDevicePtr s_AudioDevice = NULL;
static int s_MidiInitCount  = 0;
static audiere::MIDIDevicePtr  s_MidiDevice  = NULL;

////////////////////////////////////////////////////////////////////////////////

#define CD_AUDIO

#ifdef CD_AUDIO
static int s_CDInitCount  = 0;
static audiere::CDDevicePtr s_CDDevice = NULL;
#endif

////////////////////////////////////////////////////////////////////////////////

static bool IsMidi(const char* filename)
{
  if (extension_compare(filename, ".mid"))  return true;
  if (extension_compare(filename, ".midi")) return true;
  if (extension_compare(filename, ".rmi"))  return true;
  return false;
}

////////////////////////////////////////////////////////////////////////////////

static void InitializeAudio()
{
  if (s_AudioInitCount++ == 0) {
    s_AudioDevice = audiere::OpenDevice("winmm");
    if (!s_AudioDevice) {
      s_AudioDevice = audiere::OpenDevice("null");
    }
  }
  if (s_AudioDevice && s_AudioDevice.get()) {
    const char* device_name = s_AudioDevice.get()->getName();
  }
}

////////////////////////////////////////////////////////////////////////////////

static void InitializeMidi()
{
  if (s_MidiInitCount++ == 0) {
    s_MidiDevice = audiere::OpenMIDIDevice("");
    if (s_MidiDevice == NULL) {
      s_MidiDevice = audiere::OpenMIDIDevice("null");
    }
  }
  if (s_MidiDevice && s_MidiDevice.get()) {
    const char* device_name = s_MidiDevice.get()->getName();
  }
}

////////////////////////////////////////////////////////////////////////////////

#ifdef CD_AUDIO
static void InitializeCD(const char* device)
{
  if (s_CDInitCount++ == 0) {
    s_CDDevice = audiere::OpenCDDevice(device);
    if (s_CDDevice == NULL) {
      s_CDDevice = audiere::OpenCDDevice("null");
    }
  }
  if (s_CDDevice && s_CDDevice.get()) {
    const char* device_name = s_CDDevice.get()->getName();
  }
}
#endif

////////////////////////////////////////////////////////////////////////////////

static void CloseAudio()
{
  if (--s_AudioInitCount == 0) {
    s_AudioDevice = 0;
  }
}

////////////////////////////////////////////////////////////////////////////////

static void CloseMidi()
{
  if (--s_MidiInitCount == 0) {
    s_MidiDevice = 0;
  }
}

////////////////////////////////////////////////////////////////////////////////

#ifdef CD_AUDIO
static void CloseCD()
{
  if (--s_CDInitCount == 0) {
    s_CDDevice = 0;
  }
}
#endif

////////////////////////////////////////////////////////////////////////////////

CSound::CSound()
: m_Sound(NULL)
, m_Midi(NULL)
, m_ClosedAudio(true)
, m_ClosedMidi(true)
#ifdef CD_AUIO
, m_ClosedCD(true)
#endif
{
  InitializeAudio(); m_ClosedAudio = false;
  InitializeMidi();  m_ClosedMidi  = false;
}

////////////////////////////////////////////////////////////////////////////////

CSound::~CSound()
{
  if (!m_ClosedAudio) {
    CloseAudio();
    m_ClosedAudio = true;
  }
  if (!m_ClosedMidi) {
    CloseMidi();
    m_ClosedMidi = true;
  }
#ifdef CD_AUDIO
  if (!m_ClosedCD) {
    CloseCD();
    m_ClosedCD = true;
  }
#endif
}

////////////////////////////////////////////////////////////////////////////////

void
CSound::__GetDevice__()
{
  if (!s_MidiDevice) {
    InitializeMidi();
    m_ClosedMidi = false;
  }
  else {
    if (s_MidiDevice.get() && s_MidiDevice.get()->getName() != NULL) {
      const char* device_name = s_MidiDevice.get()->getName();
      if (strcmp("null", device_name) == 0) {
        CloseMidi();
        m_ClosedMidi = true;
        InitializeMidi();
        m_ClosedMidi = false;
      }
    }
  }
  if (!s_AudioDevice) {
    InitializeAudio();
    m_ClosedAudio = false;
  }
  else {
    if (s_AudioDevice.get() && s_AudioDevice.get()->getName() != NULL) {
      const char* device_name = s_AudioDevice.get()->getName();
      if (strcmp("null", device_name) == 0) {
        CloseAudio();
        m_ClosedAudio = true;
        InitializeAudio();
        m_ClosedAudio = false;
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

void
CSound::__GetSound__(const char* filename)
{
  if (!m_Sound && !m_Midi && s_AudioDevice.get() && !IsMidi(filename)) {
    m_Sound = audiere::OpenSound(s_AudioDevice.get(), filename, true);
  }
  if (!m_Sound && !m_Midi && s_MidiDevice.get()  && IsMidi(filename)) {
    /*
    audiere::File* file = audiere::OpenFile(filename, false);
    if (file) {
      m_Midi = s_MidiDevice.get()->openStream(file);
      file = NULL;
    }
    */
    m_Midi = s_MidiDevice.get()->openStream(filename);
  }
}

////////////////////////////////////////////////////////////////////////////////

static bool get_cda_details(const std::string filename, std::string& device, int& track_number)
{
  const char* cda = "cda://";
  int device_start = filename.find(cda);
  int device_end = filename.find(",");
  if (device_start == 0 && (unsigned int) device_end >= strlen(cda) + 1) {
    device_start += strlen(cda);
    device = filename.substr(device_start, device_end - device_start);
    int track_start = device_end + 1;
    int track_end = filename.size();
    std::string track = filename.substr(track_start, track_end - track_start);
    for (unsigned int i = 0; i < track.length(); i++) {
      if (track[i] < '0' || track[i] > '9') {
        return false;
      }
    }
    track_number = atoi(track.c_str());
    return true;
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////

static bool is_cda(const std::string filename)
{
  std::string device = "";
  int track_number = 0;
  return get_cda_details(filename, device, track_number);
}

////////////////////////////////////////////////////////////////////////////////

bool
CSound::Load(const char* filename)
{
  if (m_Sound || m_Midi) {
    Stop();
  }
  m_Filename = filename;
  if (is_cda(filename))
    return true;
  __GetDevice__();
  __GetSound__(filename);
  if (!m_Sound && !m_Midi) {
    Stop();
  }
  return bool(m_Sound || m_Midi);
}

////////////////////////////////////////////////////////////////////////////////

bool
CSound::Play()
{
  if (!IsPlaying()) {
    if (!is_cda(m_Filename)) {
      __GetDevice__();
      __GetSound__(m_Filename.c_str());
    }
    if (m_Sound) {
      m_Sound->play();
    }
    if (m_Midi) {
      m_Midi->play();
    }
#ifdef CD_AUDIO
    if (is_cda(m_Filename)) {
      
      std::string device = "";
      int track_number = 0;
      if (get_cda_details(m_Filename, device, track_number)) {
        device += ":";
        InitializeCD(device.c_str());
        m_ClosedCD = false;
        if (s_CDDevice && s_CDDevice.get())
          s_CDDevice->play(track_number);
        return true;
      }
      return false;
    }
#endif
  }
  return (m_Sound || m_Midi);
}

////////////////////////////////////////////////////////////////////////////////

void
CSound::Stop()
{
  if (m_Sound) {
    m_Sound->stop();
    m_Sound->reset();
    m_Sound = 0;
  }
  if (m_Midi) {
    m_Midi->stop();
    m_Midi = 0;
  }
#ifdef CD_AUDIO
  if (s_CDDevice && s_CDDevice.get()) {
    s_CDDevice->stop();
  }
  if (!m_ClosedCD) {
    CloseCD();
    m_ClosedCD = true;
  }
#endif
  if (!m_ClosedAudio) {
    CloseAudio();
    m_ClosedAudio = true;
  }
  if (!m_ClosedMidi) {
    CloseMidi();
    m_ClosedMidi = true;
  }
}

////////////////////////////////////////////////////////////////////////////////

int
CSound::GetVolume()
{
  if (m_Sound) return (int) (m_Sound->getVolume() * 255);
  return 0;
}

////////////////////////////////////////////////////////////////////////////////

void
CSound::SetVolume(int Volume)
{
  if (m_Sound) {
    m_Sound->setVolume(Volume / 255.0f);
  }
}

////////////////////////////////////////////////////////////////////////////////

bool
CSound::IsPlaying() const
{
  if (m_Sound) return m_Sound->isPlaying();
  if (m_Midi)  return m_Midi->isPlaying();
#ifdef CD_AUDIO
  if (s_CDDevice && s_CDDevice.get()) return true; // s_CDDevice->isPlaying();
#endif
  return false;
}

////////////////////////////////////////////////////////////////////////////////

bool
CSound::IsSeekable()  {
  if (m_Sound) return m_Sound->isSeekable();
  if (m_Midi)  return true;
  return false;
}

////////////////////////////////////////////////////////////////////////////////

void
CSound::SetPosition(int pos) {
  if (m_Sound) m_Sound->setPosition(pos);
  if (m_Midi)  m_Midi->setPosition(pos);
}

////////////////////////////////////////////////////////////////////////////////

int
CSound::GetPosition() {
  if (m_Sound) return m_Sound->getPosition();
  if (m_Midi)  return m_Midi->getPosition();
  return 0;
}

////////////////////////////////////////////////////////////////////////////////

int
CSound::GetLength() {
  if (m_Sound) return m_Sound->getLength();
  if (m_Midi)  return m_Midi->getLength();
  return 0;
}

////////////////////////////////////////////////////////////////////////////////

void
CSound::SetPitchShift(double pitch) {
  if (pitch < 0.1)
    pitch = 0.1;
  if (m_Sound) m_Sound->setPitchShift((float) pitch);
}

////////////////////////////////////////////////////////////////////////////////

void
CSound::SetPan(double pan) {
  if (m_Sound) m_Sound->setPan((float) pan);
}

////////////////////////////////////////////////////////////////////////////////
