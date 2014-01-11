#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif
#include "SoundWindow.hpp"
#include "Editor.hpp"
#include "translate.hpp"
#include "filename_comparer.hpp"
#include "Configuration.hpp"
#include "Keys.hpp"
#include "resource.h"

////////////////////////////////////////////////////////////////////////////////

const int TIMER_UPDATE_SOUND_WINDOW = 987;
const int ID_MUSIC_VOLUMEBAR   = 40102;
const int ID_MUSIC_POSITIONBAR = 40103;
const int ID_MUSIC_PANBAR = 40104;
const int ID_MUSIC_PITCHBAR = 40105;

////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSoundWindow, CDocumentWindow)
  
  ON_WM_SIZE()
  ON_WM_TIMER()
  ON_WM_VSCROLL()
  ON_WM_HSCROLL()
  ON_WM_DROPFILES()
  ON_COMMAND(ID_SOUND_PLAY,   OnSoundPlay)
  //ON_COMMAND(ID_SOUND_PAUSE,  OnSoundPause)
  ON_COMMAND(ID_SOUND_STOP,   OnSoundStop)
  ON_COMMAND(ID_SOUND_REPEAT, OnSoundRepeat)
  ON_UPDATE_COMMAND_UI(ID_SOUND_PLAY,  OnUpdatePlayCommand)
  ON_UPDATE_COMMAND_UI(ID_SOUND_PAUSE, OnUpdatePauseCommand)
  ON_UPDATE_COMMAND_UI(ID_SOUND_STOP,  OnUpdateStopCommand)
  ON_UPDATE_COMMAND_UI(ID_SOUND_REPEAT, OnUpdateRepeatCommand)
  ON_UPDATE_COMMAND_UI(ID_SOUND_NEXT, OnUpdateNextCommand)
  ON_UPDATE_COMMAND_UI(ID_SOUND_PREV, OnUpdatePrevCommand)
  ON_UPDATE_COMMAND_UI(ID_SOUND_AUTO_ADVANCE, OnUpdateAutoAdvanceCommand)
  ON_UPDATE_COMMAND_UI(ID_SOUND_RANDOM_ORDER, OnUpdateRandomOrderCommand)
  ON_COMMAND(ID_SOUND_NEXT, OnSoundNext)
  ON_COMMAND(ID_SOUND_PREV, OnSoundPrev)
  ON_COMMAND(ID_SOUND_AUTO_ADVANCE, OnAutoAdvance)
  ON_COMMAND(ID_SOUND_RANDOM_ORDER, OnRandomOrder)
  ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnNeedText)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////

CSoundWindow::CSoundWindow(const char* sound)
: CDocumentWindow(sound, IDR_SOUND, CSize(200, 120))
{
  m_PositionDown = false;
  m_CurrentSound = 0;
  m_Playing = false;
  m_Stopped = true;
  m_Repeat = Configuration::Get(KEY_SOUND_REPEAT);
  m_AutoAdvance  = Configuration::Get(KEY_SOUND_AUTOADVANCE);
  m_RandomOrder  = Configuration::Get(KEY_SOUND_RANDOMORDER);
  Create(AfxRegisterWndClass(0, NULL, NULL, AfxGetApp()->LoadIcon(IDI_SOUND)));
  CFont* pFont = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
  
  // create the buttons
  m_PlayButton.Create(TranslateString("Play"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(), this, ID_SOUND_PLAY);
  m_PlayButton.SetFont(pFont);
  m_StopButton.Create(TranslateString("Stop"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(), this, ID_SOUND_STOP);
  m_StopButton.SetFont(pFont);
  // create the volume bar and its associated friends.
  m_VolumeBar.Create(WS_CHILD | WS_VISIBLE | TBS_VERT | TBS_TOOLTIPS, CRect(), this, ID_MUSIC_VOLUMEBAR);
  m_VolumeBar.SetLineSize(20);
  m_VolumeBar.SetRange(0, 255, TRUE);
  m_VolumeBar.SetPos(0);
  m_VolumeBarBitmap.LoadBitmap(IDB_VOLUMEBAR);
  m_VolumeBarGraphic.Create(NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | SS_BITMAP, CRect(), this);
  m_VolumeBarGraphic.SetBitmap((HBITMAP)m_VolumeBarBitmap);
  m_Blank.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(), this);
  if (1) {
    m_PanBar.Create(WS_CHILD | WS_VISIBLE | TBS_VERT | TBS_TOOLTIPS, CRect(), this, ID_MUSIC_PANBAR);
    m_PanBar.SetRange(-255, 255, true);
    m_PanBar.SetPos(0);
    m_PanBar.SetLineSize(20);
    m_PitchBar.Create(WS_CHILD | WS_VISIBLE | TBS_VERT | TBS_TOOLTIPS, CRect(), this, ID_MUSIC_PITCHBAR);
    m_PitchBar.SetRange(0, 255 * 2, TRUE);
    m_PitchBar.SetPos(255);
    m_PitchBar.SetLineSize(20);
  }
  /*
  m_Playlist.AppendFile("cda://F,1");
  m_Playlist.AppendFile("cda://F,2");
  m_Playlist.AppendFile("cda://F,3");
  m_Playlist.AppendFile("cda://F,4");
  m_Playlist.AppendFile("cda://F,5");
  */
  if (sound != NULL) {
    const char* cda = "cda://";
    if (memcmp(sound, cda, strlen(cda)) == 0) {
      std::string device = sound + strlen(cda);
      if (device.find(",") == -1) {
        int num_tracks = 3;
        for (int i = 0; i < num_tracks; i++) {
          char track[100] = {0};
          sprintf (track, "%s%s,%d", cda, device.c_str(), i);
          m_Playlist.AppendFile(track);
        }
      }
      else {
        m_Playlist.AppendFile(sound);
      }
    }
    else {
      m_Playlist.AppendFile(sound);
    }
  }
  m_PlayButton.EnableWindow(TRUE);
  m_StopButton.EnableWindow(FALSE);
  // make sure the buttons are in the right position
  RECT Rect;
  GetClientRect(&Rect);
  OnSize(0, Rect.right - Rect.left, Rect.bottom - Rect.top);
  DragAcceptFiles();
  SetTimer(TIMER_UPDATE_SOUND_WINDOW, 100, NULL);
  OnTimer(TIMER_UPDATE_SOUND_WINDOW);
}

////////////////////////////////////////////////////////////////////////////////

CSoundWindow::~CSoundWindow()
{
  m_VolumeBarBitmap.DeleteObject();
}

////////////////////////////////////////////////////////////////////////////////

void
CSoundWindow::LoadSound(const char* sound)
{
  // load the sample
  if (!m_Sound.Load(sound))
  {
    // if it fails, show a message box and close the window
    char string[MAX_PATH + 1024];
    sprintf (string, "Error: Could not load sound file\n'%s'", sound);
    //MessageBox(string);
    GetStatusBar()->SetWindowText(string);
    return;
  }
  if (m_Sound.IsSeekable()) {
    if (m_PositionBar.m_hWnd == NULL) {
      m_PositionBar.Create(WS_CHILD | WS_VISIBLE | TBS_HORZ, CRect(), this, ID_MUSIC_POSITIONBAR);
      m_PositionBar.SetLineSize(20);
    }
    if (m_PositionBar.m_hWnd) {
      m_PositionBar.SetRange(0, m_Sound.GetLength(), true);
      m_PositionBar.SetPos(0);
      m_PositionBar.ShowWindow(SW_SHOW);
    }
  }
  else {
    if (m_PositionBar.m_hWnd) {
      m_PositionBar.ShowWindow(SW_HIDE);
    }
  }
  RECT Rect;
  GetClientRect(&Rect);
  OnSize(0, Rect.right - Rect.left, Rect.bottom - Rect.top);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CSoundWindow::OnSize(UINT type, int cx, int cy)
{
  int button_height = cy;
  if (m_PositionBar.m_hWnd != NULL && m_PositionBar.IsWindowVisible())
    button_height -= 25;
  if (m_PlayButton.m_hWnd != NULL)
    m_PlayButton.MoveWindow(CRect(0, 0, (cx-50) / 2, button_height));
  if (m_StopButton.m_hWnd != NULL)
    m_StopButton.MoveWindow(CRect((cx-50) / 2, 0, cx-50, button_height));
  
  if (m_VolumeBar.m_hWnd != NULL)
    m_VolumeBar.MoveWindow(CRect(cx-50, 0, cx-30, 100));
  if (m_VolumeBarGraphic.m_hWnd != NULL)
    m_VolumeBarGraphic.MoveWindow(CRect(cx-30, 0, cx, 100));
  if (m_Blank.m_hWnd != NULL)
    m_Blank.MoveWindow(CRect(cx-50, 90, cx, cy));
  if (m_PositionBar.m_hWnd != NULL) {
    m_PositionBar.MoveWindow(CRect(0, button_height, cx, cy));
  }
  if (m_PanBar.m_hWnd != NULL && m_PitchBar.m_hWnd != NULL) {
    m_PanBar.MoveWindow(  CRect(cx-50, 90, cx-30, 200));
    m_PitchBar.MoveWindow(CRect(cx-30, 90, cx,    200));
  }
  CDocumentWindow::OnSize(type, cx, cy);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CSoundWindow::OnTimer(UINT timerID)
{
  if ( !m_Playing && m_Stopped )
    return;
  if (m_Sound.IsPlaying())
  {
    if (m_PositionBar.m_hWnd != NULL) {
      if (!m_PositionDown) {
        m_PositionBar.SetPos(m_Sound.GetPosition());
      }
    }
    m_PlayButton.EnableWindow(FALSE);
    m_StopButton.EnableWindow(TRUE);
  }
  else {
    if (m_Playing) {
      if (m_Playlist.GetNumFiles() > 1 && m_AutoAdvance) {
        if (NextSound()) {
          OnSoundPlay();
        }
      }
      else {
        m_Sound.Stop();
        m_Playing = m_Repeat;
        if (m_Repeat) {
          PlaySound();
        }
      }
    }
    if (!m_Playing && !m_Stopped) {
      m_PlayButton.EnableWindow(TRUE);
      m_StopButton.EnableWindow(FALSE);     
      OnSoundStop();
      if (m_PositionBar.m_hWnd != NULL) {
        m_PositionBar.SetPos(0);
      }
      m_Stopped = true;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CSoundWindow::OnVScroll(UINT code, UINT pos, CScrollBar *scroll_bar)
{
  if (!scroll_bar) return;
  if (scroll_bar->m_hWnd == m_VolumeBar.m_hWnd) {
    //if (!m_VolumeBar.MouseDown)
      m_Sound.SetVolume(GetVolume());
  }
  if (scroll_bar->m_hWnd == m_PitchBar.m_hWnd) {
    m_Sound.SetPitchShift(GetPitchShift());
  }
  if (scroll_bar->m_hWnd == m_PanBar.m_hWnd) {
    m_Sound.SetPan(GetPan() / 255.0f);
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CSoundWindow::OnHScroll(UINT code, UINT pos, CScrollBar *scroll_bar)
{
  if (!scroll_bar) return;
  switch (code) {
    case SB_THUMBTRACK:    m_PositionDown = true; break;
    case SB_THUMBPOSITION: m_PositionDown = false; break;
  }
  if (scroll_bar->m_hWnd == m_PositionBar.m_hWnd) {
    if (code != SB_THUMBTRACK) {
      m_Sound.SetPosition(m_PositionBar.GetPos());
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

int
CSoundWindow::GetPan() {
  if (m_PanBar.m_hWnd != NULL)
    return m_PanBar.GetPos();
  return 0;
}

////////////////////////////////////////////////////////////////////////////////

float
CSoundWindow::GetPitchShift() {
  if (m_PitchBar.m_hWnd != NULL)
  {
    float pitch = (float)m_PitchBar.GetPos() / 255.0f;
    if (pitch < 0.1f)
      pitch = 0.1f;
    return  pitch;
  }
  return 1.0;
}

////////////////////////////////////////////////////////////////////////////////

int
CSoundWindow::GetVolume() {
  if (m_VolumeBar.m_hWnd != NULL)
    return (255 - m_VolumeBar.GetPos());
  return 255;
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CSoundWindow::OnSoundPlay()
{
  // ignore this if the user managed to hit the play button while it was playing
  if (m_Sound.IsPlaying())
    return;
  if ( m_CurrentSound < 0 || m_CurrentSound >= m_Playlist.GetNumFiles() ) {
    m_Playing = false;
    return;
  }
    
  LoadSound(m_Playlist.GetFile(m_CurrentSound));
  UpdateCaption();
  
  if (PlaySound()) {
    m_Playing = true;
    m_Stopped = false;
    if (m_PanBar.m_hWnd != NULL && m_PitchBar.m_hWnd != NULL && m_VolumeBar.m_hWnd != NULL) {
      m_Sound.SetPan(GetPan());
      m_Sound.SetPitchShift(GetPitchShift());
      m_Sound.SetVolume(GetVolume());
    } 
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CSoundWindow::OnSoundPause()
{
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CSoundWindow::OnSoundStop()
{
  m_Sound.Stop();
  m_Playing = false;
}

////////////////////////////////////////////////////////////////////////////////

bool
CSoundWindow::AdvanceSound(bool forward, bool allow_repeating)
{
  const int delta = forward ? 1 : -1;
  const int original_sound = m_CurrentSound;
  if (m_RandomOrder) {
    int random_song = rand() % (m_Playlist.GetNumFiles() - 1);
    if (random_song >= m_CurrentSound)
      random_song += 1;
    m_CurrentSound = random_song;
  }
  else {
    m_CurrentSound += delta;
    if (m_CurrentSound < 0) {
      m_CurrentSound   = (!allow_repeating) ? original_sound : (m_Playlist.GetNumFiles() - 1);
    } 
    else {
      if (m_CurrentSound >= m_Playlist.GetNumFiles()) {
        m_CurrentSound = (!allow_repeating) ? original_sound : 0;
      } 
    }
  }
  return m_CurrentSound != original_sound;
}

////////////////////////////////////////////////////////////////////////////////

bool
CSoundWindow::PlaySound()
{
  if (!m_Sound.Play()) {
    if (m_CurrentSound >= 0 && m_CurrentSound < m_Playlist.GetNumFiles()) {
      // if it fails, show an error message
      char string[MAX_PATH + 1024];
      sprintf (string, "Error: Could not load sound file\n'%s'", m_Playlist.GetFile(m_CurrentSound));
      GetStatusBar()->SetWindowText(string);
    }
    return false;
  }
  return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CSoundWindow::NextSound()
{
  return AdvanceSound(true, m_Repeat);
}

////////////////////////////////////////////////////////////////////////////////

bool
CSoundWindow::PrevSound()
{
  return AdvanceSound(false, m_Repeat);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CSoundWindow::OnSoundNext()
{
  const bool was_playing = m_Playing;
  m_Sound.Stop();
  if (AdvanceSound(true, m_Repeat) && was_playing) {
    OnSoundPlay();
  }
  else {
    UpdateCaption();
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CSoundWindow::OnSoundPrev()
{
  const bool was_playing = m_Playing;
  m_Sound.Stop();
  if (AdvanceSound(false, m_Repeat) && was_playing) {
    OnSoundPlay();
  }
  else {
    UpdateCaption();
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CSoundWindow::OnSoundRepeat()
{
  m_Repeat = !m_Repeat;
  Configuration::Set(KEY_SOUND_REPEAT, m_Repeat);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CSoundWindow::OnUpdatePlayCommand(CCmdUI* cmdui)
{
  cmdui->Enable(!m_Sound.IsPlaying());
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CSoundWindow::OnUpdatePauseCommand(CCmdUI* cmdui)
{
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CSoundWindow::OnUpdateStopCommand(CCmdUI* cmdui)
{
  cmdui->Enable(m_Sound.IsPlaying() || m_Playing);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CSoundWindow::OnUpdateRepeatCommand(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_Repeat);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CSoundWindow::OnUpdateNextCommand(CCmdUI* cmdui)
{
  BOOL enabled = FALSE;
  if (m_Playlist.GetNumFiles() > 1) {
    if (m_RandomOrder) {
      enabled = TRUE;
    }
    else if (m_Repeat || m_CurrentSound < m_Playlist.GetNumFiles() - 1) {
      enabled = TRUE;
    }
  }
  cmdui->Enable(enabled);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CSoundWindow::OnUpdatePrevCommand(CCmdUI* cmdui)
{
  BOOL enabled = FALSE;
  if (m_Playlist.GetNumFiles() > 1) {
    if (m_RandomOrder) {
      enabled = TRUE;
    }
    else if (m_Repeat || m_CurrentSound > 0) {
      enabled = TRUE;
    }
  }
  cmdui->Enable(enabled);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CSoundWindow::OnAutoAdvance()
{
  m_AutoAdvance = !m_AutoAdvance;
  Configuration::Set(KEY_SOUND_AUTOADVANCE, m_AutoAdvance);
}

////////////////////////////////////////////////////////////////////////////////
  
afx_msg void
CSoundWindow::OnUpdateAutoAdvanceCommand(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_AutoAdvance ? TRUE : FALSE);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CSoundWindow::OnRandomOrder()
{
  m_RandomOrder = !m_RandomOrder;
  Configuration::Set(KEY_SOUND_RANDOMORDER, m_RandomOrder);
}

////////////////////////////////////////////////////////////////////////////////
  
afx_msg void
CSoundWindow::OnUpdateRandomOrderCommand(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_RandomOrder ? TRUE : FALSE);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg BOOL
CSoundWindow::OnNeedText(UINT /*id*/, NMHDR* nmhdr, LRESULT* result)
{
  if (!nmhdr || !nmhdr) return TRUE;
  TOOLTIPTEXT* ttt = (TOOLTIPTEXT*)nmhdr;
  UINT id = nmhdr->idFrom;
  if (ttt->uFlags & TTF_IDISHWND) {
    id = ::GetDlgCtrlID((HWND)id);
  }
  static char string[1024] = {0};
  switch (id) {
    case ID_MUSIC_PANBAR:
      if (m_PanBar.m_hWnd != NULL) {
        sprintf (string, "%s %3d",  TranslateString("pan"),  GetPan());
        ttt->lpszText = string;
      }
    break;
    case ID_MUSIC_PITCHBAR:
      if (m_PanBar.m_hWnd != NULL) {
        sprintf (string, "%s %1.3f", TranslateString("pitch"), ((float)GetPitchShift()));
        ttt->lpszText = string;
      }
    break;
    case ID_MUSIC_VOLUMEBAR:
      if (m_VolumeBar.m_hWnd != NULL) {
        sprintf (string, "%s %3d %s", TranslateString("volume"), GetVolume(), (m_Sound.IsPlaying() ? "playing" : "not playing"));
        ttt->lpszText = string;
      }
    break;
    default:
      ttt->lpszText = "";
  }
  *result = 0;
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////

void
CSoundWindow::UpdateCaption()
{
  if (m_CurrentSound >= 0 && m_CurrentSound < m_Playlist.GetNumFiles()) {
    char szWindowTitle[MAX_PATH + 1024];
    const char* filename = m_Playlist.GetFile(m_CurrentSound);
    if (filename != NULL) {
      const char* ptr = strrchr(filename, '\\');
      if (ptr == NULL)
        ptr = filename;
      else
        ptr += 1;
      sprintf (szWindowTitle, "%s [%d / %d]", ptr, m_CurrentSound, m_Playlist.GetNumFiles());
      SetCaption(szWindowTitle);
    }
  }
  else {
    SetCaption("...");
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CSoundWindow::OnDropFiles(HDROP drop_info)
{
  if (!drop_info)
    return;
  UINT num_files = DragQueryFile(drop_info, 0xFFFFFFFF, NULL, 0);
  // add all files to the playlist
  for (unsigned int i = 0; i < num_files; i++) {
    char path[MAX_PATH + 1] = {0};
    if (DragQueryFile(drop_info, i, path, MAX_PATH) != 0) {
      if (extension_compare(path, ".m3u"))
        m_Playlist.LoadFromFile(path);
      else
        m_Playlist.AppendFile(path);
    }
  }
  DragFinish(drop_info);
  UpdateCaption();
}

////////////////////////////////////////////////////////////////////////////////
