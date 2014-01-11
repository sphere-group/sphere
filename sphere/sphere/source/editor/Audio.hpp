#ifndef AUDIO_HPP
#define AUDIO_HPP
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif
#include <audiere.h>

////////////////////////////////////////////////////////////////////////////////

class CSound
{
public:
  CSound();
  ~CSound();
  bool Load(const char* filename);
  bool Play();
  void Stop();
  int  GetVolume();
  void SetVolume(int volume);
  bool IsPlaying() const;
  bool IsSeekable();
  void SetPosition(int pos);
  int GetPosition();
  int GetLength();
  void SetPitchShift(double pitch);
  void SetPan(double pan);

private:
  audiere::OutputStreamPtr m_Sound;
  audiere::MIDIStreamPtr   m_Midi;
  bool m_ClosedAudio;
  bool m_ClosedMidi;
  bool m_ClosedCD;
  std::string m_Filename;
  void __GetDevice__();
  void __GetSound__(const char* filename);
};

////////////////////////////////////////////////////////////////////////////////

#endif
