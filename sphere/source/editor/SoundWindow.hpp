#ifndef SOUND_WINDOW_HPP
#define SOUND_WINDOW_HPP
#include <afxcmn.h>
#include "DocumentWindow.hpp"
#include "Audio.hpp"
#include "../common/Playlist.hpp"
class CSoundWindow : public CDocumentWindow
{
public:
  CSoundWindow(const char* sound);
  ~CSoundWindow();
private:
  void LoadSound(const char* sound);
private:
  afx_msg void OnSize(UINT type, int cx, int cy);
  afx_msg void OnTimer(UINT timerID);
  afx_msg void OnVScroll(UINT code, UINT pos, CScrollBar *scroll_bar);
  afx_msg void OnHScroll(UINT code, UINT pos, CScrollBar *scroll_bar);
  afx_msg void OnSoundPlay();
  afx_msg void OnSoundPause();
  afx_msg void OnSoundStop();
  afx_msg void OnSoundRepeat();
  bool AdvanceSound(bool forward, bool allow_repeating);
  bool PlaySound();
  bool NextSound();
  bool PrevSound();
  afx_msg void OnSoundNext();
  afx_msg void OnSoundPrev();
  afx_msg void OnUpdatePlayCommand(CCmdUI* cmdui);
  afx_msg void OnUpdatePauseCommand(CCmdUI* cmdui);
  afx_msg void OnUpdateStopCommand(CCmdUI* cmdui);
  afx_msg void OnUpdateRepeatCommand(CCmdUI* cmdui);
  afx_msg void OnUpdateNextCommand(CCmdUI* cmdui);
  afx_msg void OnUpdatePrevCommand(CCmdUI* cmdui);
  afx_msg void OnAutoAdvance();
  afx_msg void OnUpdateAutoAdvanceCommand(CCmdUI* cmdui);
  afx_msg void OnRandomOrder();
  afx_msg void OnUpdateRandomOrderCommand(CCmdUI* cmdui);
  afx_msg BOOL OnNeedText(UINT id, NMHDR* nmhdr, LRESULT* result);
  afx_msg void OnDropFiles(HDROP drop_info);
  void UpdateCaption();
public:
  int GetPan();
  float GetPitchShift();
  int GetVolume();
private:
  CSound  m_Sound;
  CButton m_PlayButton;
  CButton m_StopButton;
  CSliderCtrl m_VolumeBar;
  CSliderCtrl m_PositionBar;
  bool m_PositionDown;
  CSliderCtrl m_PanBar;
  CSliderCtrl m_PitchBar;
  CStatic m_VolumeBarGraphic;
  CBitmap m_VolumeBarBitmap;
  CStatic m_Blank;
  bool m_Repeat; 
  bool m_Playing;
  bool m_Stopped;
  bool m_AutoAdvance;
  bool m_RandomOrder;
  
  CPlaylist m_Playlist;
  int m_CurrentSound;
  DECLARE_MESSAGE_MAP()
};
#endif
