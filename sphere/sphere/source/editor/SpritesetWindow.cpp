// identifier too long
#pragma warning(disable : 4786)
#include "SpritesetWindow.hpp"
#include "FileDialogs.hpp"
#include "ResizeDialog.hpp"
#include "SpritePropertiesDialog.hpp"
#include "SpritesetImagesPalette.hpp"
#include "NumberDialog.hpp"
#include "resource.h"
#include "Editor.hpp"
#include "Configuration.hpp"
#include "keys.hpp"
#define IDC_TAB 800
#define TAB_HEIGHT 24
static const int SPRITESET_TIMER = 9001;
#ifdef USE_SIZECBAR
IMPLEMENT_DYNAMIC(CSpritesetWindow, CMDIChildWnd)
#endif
BEGIN_MESSAGE_MAP(CSpritesetWindow, CSaveableDocumentWindow)
  ON_WM_DESTROY()
  ON_WM_SIZE()
  ON_WM_KEYDOWN()
  ON_WM_KEYUP()
  ON_WM_TIMER()
  ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnTabChanged)
  ON_COMMAND(ID_SPRITESET_ZOOM_1X,         OnZoom1x)
  ON_COMMAND(ID_SPRITESET_ZOOM_2X,         OnZoom2x)
  ON_COMMAND(ID_SPRITESET_ZOOM_4X,         OnZoom4x)
  ON_COMMAND(ID_SPRITESET_ZOOM_8X,         OnZoom8x)
  ON_COMMAND(ID_FILE_ZOOM_IN,         OnZoomIn)
  ON_COMMAND(ID_FILE_ZOOM_OUT,        OnZoomOut)
  
  ON_COMMAND(ID_SPRITESET_RESIZE,          OnResize)
  ON_COMMAND(ID_SPRITESET_RESCALE,         OnRescale)
  ON_COMMAND(ID_SPRITESET_RESAMPLE,        OnResample)
  ON_COMMAND(ID_SPRITESET_FILLDELAY,       OnFillDelay)
  ON_COMMAND(ID_SPRITESET_FRAMEPROPERTIES, OnFrameProperties)
  ON_COMMAND(ID_SPRITESET_EXPORTASIMAGE,   OnExportAsImage)
  ON_UPDATE_COMMAND_UI(ID_SPRITESET_ZOOM_1X, OnUpdateZoom1x)
  ON_UPDATE_COMMAND_UI(ID_SPRITESET_ZOOM_2X, OnUpdateZoom2x)
  ON_UPDATE_COMMAND_UI(ID_SPRITESET_ZOOM_4X, OnUpdateZoom4x)
  ON_UPDATE_COMMAND_UI(ID_SPRITESET_ZOOM_8X, OnUpdateZoom8x)
  ON_COMMAND(ID_FILE_COPY,  OnCopy)
  ON_COMMAND(ID_FILE_PASTE, OnPaste)
  ON_COMMAND(ID_FILE_UNDO,  OnUndo)
  ON_COMMAND(ID_FILE_REDO,  OnRedo)
  ON_COMMAND(ID_SPRITESET_TAB_FRAMES,   OnFramesTab)
  ON_COMMAND(ID_SPRITESET_TAB_EDIT, OnEditTab)
  ON_COMMAND(ID_SPRITESET_TAB_BASE, OnBaseTab)
  ON_UPDATE_COMMAND_UI(ID_SPRITESET_TAB_FRAMES, OnUpdateFramesTab)
  ON_UPDATE_COMMAND_UI(ID_SPRITESET_TAB_EDIT,   OnUpdateEditTab)
  ON_UPDATE_COMMAND_UI(ID_SPRITESET_TAB_BASE,   OnUpdateBaseTab)
  /*
  ON_UPDATE_COMMAND_UI(ID_SPRITESET_ZOOM_IN, OnUpdateZoomIn)
  ON_UPDATE_COMMAND_UI(ID_SPRITESET_ZOOM_OUT, OnUpdateZoomOut)
  */
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////
CSpritesetWindow::CSpritesetWindow(const char* filename)
: CSaveableDocumentWindow(filename, IDR_SPRITESET, CSize(160, 120))
, m_CurrentDirection(0)
, m_CurrentFrame(0)
, m_Created(false)
, m_ImagesPalette(NULL)
, m_AnimationPalette(NULL)
{
  SetSaved(filename != NULL);
  SetModified(false);
  // load spriteset
  bool create_new_spriteset = false;
  if (filename == NULL) {
    create_new_spriteset = true;
  }
  else {
    if (m_Spriteset.Load(filename) == false) {
      create_new_spriteset = true;
    }
  }
  // valid spriteset image indexes
  if (m_Spriteset.GetNumImages() > 0) {
    bool asked_to_prune = false;
    for (int direction = 0; direction < m_Spriteset.GetNumDirections(); direction++)
    {
      for (int frame = 0; frame < m_Spriteset.GetNumFrames(direction); frame++)
      {
        int index = m_Spriteset.GetFrameIndex(direction, frame);
        if (index < 0 || index >= m_Spriteset.GetNumImages())
        {
          if (asked_to_prune == false) {
            if (MessageBox("Error: Spriteset has some invalid image references, remove them?", "Load Spriteset", MB_YESNO) == IDNO) {
              create_new_spriteset = true;
            }
            asked_to_prune = true;
          }
          if (index < 0) {
            m_Spriteset.SetFrameIndex(direction, frame, 0);
          }
          else
          if (index >= m_Spriteset.GetNumImages()) {
            m_Spriteset.SetFrameIndex(direction, frame, m_Spriteset.GetNumImages() - 1);
          }
        }
      }
    }
  }
  // create default spriteset
  if (create_new_spriteset) {
    if (filename) {
      char string[MAX_PATH + 1024];
      sprintf (string, "Could not load spriteset: '%s'\nCreating empty spriteset.", filename);
      MessageBox(string);
    }
    m_Spriteset.Create(16, 32, 1, 8, 1);
    m_Spriteset.SetDirectionName(0, "north");
    m_Spriteset.SetDirectionName(1, "northeast");
    m_Spriteset.SetDirectionName(2, "east");
    m_Spriteset.SetDirectionName(3, "southeast");
    m_Spriteset.SetDirectionName(4, "south");
    m_Spriteset.SetDirectionName(5, "southwest");
    m_Spriteset.SetDirectionName(6, "west");
    m_Spriteset.SetDirectionName(7, "northwest");
    // the set base to the lower half of the spriteset
    m_Spriteset.SetBase(0, (m_Spriteset.GetFrameHeight()/2),
                         m_Spriteset.GetFrameWidth() - 1, m_Spriteset.GetFrameHeight() - 1);
    SetSaved(false);
  }
  m_DocumentType = WA_SPRITESET;
  Create();
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetWindow::Create()
{
  CSaveableDocumentWindow::Create(AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW, LoadCursor(NULL, IDC_ARROW), NULL, AfxGetApp()->LoadIcon(IDI_SPRITESET)));
  m_TabControl.Create(WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, IDC_TAB);
  m_TabControl.SetFont(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));
  m_TabControl.InsertItem(0, "Frames");
  m_TabControl.InsertItem(1, "Edit");
  m_TabControl.InsertItem(2, "Base");
  // create the views
  m_SpritesetView.Create(this, this, &m_Spriteset);
  m_ImageView.Create(this, this, this);
  m_PaletteView.Create(this, this);
  m_ColorView.SetNumColors(2);
  m_ColorView.Create(this, this);
  m_AlphaView.Create(this, this);
  m_SpriteBaseView.Create(this, this, &m_Spriteset);
  int frame = m_Spriteset.GetFrameIndex(m_CurrentDirection, m_CurrentFrame);
  m_SpriteBaseView.SetSprite(&m_Spriteset.GetImage(frame));
  // create the palette
  m_ImagesPalette = new CSpritesetImagesPalette(this, this, &m_Spriteset);
  m_AnimationPalette = new CSpritesetAnimationPalette(this, &m_Spriteset);
	// the window and its children are ready!
  m_Created = true;
  double zoom_factor = Configuration::Get(KEY_SPRITESET_ZOOM_FACTOR);
  if (zoom_factor != 0) {
    m_SpritesetView.SetZoomFactor(zoom_factor);
  }
  // make sure everything is moved to the correct place
  RECT client_rect;
  GetClientRect(&client_rect);
  OnSize(SIZE_RESTORED, client_rect.right, client_rect.bottom);
  UpdateImageView();
  TabChanged(0);
#ifdef USE_SIZECBAR
	LoadPaletteStates();
#endif
  m_Timer = SetTimer(SPRITESET_TIMER, 100, NULL);
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetWindow::TabChanged(int tab)
{
  GetStatusBar()->SetWindowText("");
  ShowFramesTab(tab == 0 ? SW_SHOW : SW_HIDE);
  ShowEditTab  (tab == 1 ? SW_SHOW : SW_HIDE);
  ShowBaseTab  (tab == 2 ? SW_SHOW : SW_HIDE);
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetWindow::ShowFramesTab(int show)
{
  m_SpritesetView.ShowWindow(show);
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetWindow::ShowEditTab(int show)
{
  m_ImageView.ShowWindow(show);
  m_PaletteView.ShowWindow(show);
  m_ColorView.ShowWindow(show);
  m_AlphaView.ShowWindow(show);
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetWindow::ShowBaseTab(int show)
{
  m_SpriteBaseView.ShowWindow(show);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnKeyDown(UINT vk, UINT repeat, UINT flags)
{
  m_ImageView.OnKeyDown(vk, repeat, flags);
}
afx_msg void
CSpritesetWindow::OnKeyUp(UINT vk, UINT repeat, UINT flags)
{
  m_ImageView.OnKeyUp(vk, repeat, flags);
}
afx_msg void
CSpritesetWindow::OnTimer(UINT event)
{
  m_ImageView.OnTimer(event);
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetWindow::UpdateImageView()
{  
  int index = m_Spriteset.GetFrameIndex(m_CurrentDirection, m_CurrentFrame);
  CImage32& sprite = m_Spriteset.GetImage(index);
  m_ImageView.SetImage(sprite.GetWidth(), sprite.GetHeight(), sprite.GetPixels(), true);
  m_SpriteBaseView.SetSprite(&sprite);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnDestroy()
{
  Configuration::Set(KEY_SPRITESET_ZOOM_FACTOR, m_SpritesetView.GetZoomFactor());
  if (m_ImagesPalette)    { m_ImagesPalette->Destroy();    m_ImagesPalette    = NULL; }
  if (m_AnimationPalette) { m_AnimationPalette->Destroy(); m_AnimationPalette = NULL; }
  m_SpritesetView.DestroyWindow();
  m_ImageView.DestroyWindow();
  m_PaletteView.DestroyWindow();
  m_ColorView.DestroyWindow();
  m_AlphaView.DestroyWindow();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnSize(UINT type, int cx, int cy)
{
  const int SEPARATOR     = 96;
  const int PALETTE_WIDTH = 64;
  const int COLOR_HEIGHT  = 64;
  const int ALPHA_WIDTH   = SEPARATOR - PALETTE_WIDTH;
  if (m_Created)
  {
    m_TabControl.MoveWindow(0, 0, cx, TAB_HEIGHT);
    // frames tab
    m_SpritesetView.MoveWindow(0, TAB_HEIGHT, cx, cy - TAB_HEIGHT);
    // edit tab
    // left side
    m_PaletteView.MoveWindow(0, TAB_HEIGHT, PALETTE_WIDTH, cy - COLOR_HEIGHT - TAB_HEIGHT);
    m_ColorView.MoveWindow(0, cy - COLOR_HEIGHT, PALETTE_WIDTH, COLOR_HEIGHT);
    m_AlphaView.MoveWindow(PALETTE_WIDTH, TAB_HEIGHT, ALPHA_WIDTH, cy - TAB_HEIGHT);
    // right side
    m_ImageView.MoveWindow(SEPARATOR, TAB_HEIGHT, cx - SEPARATOR, cy - TAB_HEIGHT);
    // base tab
    m_SpriteBaseView.MoveWindow(0, TAB_HEIGHT, cx, cy - TAB_HEIGHT);
  }
  CSaveableDocumentWindow::OnSize(type, cx, cy);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnZoomIn()
{
  if (GetFocus() != this) {
    if ((m_ImagesPalette != NULL) && (GetFocus() == m_ImagesPalette)) {
      m_ImagesPalette->SendMessage(WM_COMMAND, MAKEWPARAM(ID_FILE_ZOOM_IN, 0), 0);
    }
  }
  else {
    if (m_TabControl.GetCurSel() == 0) {
      switch ((int)m_SpritesetView.GetZoomFactor()) {
        case 1: m_SpritesetView.SetZoomFactor(2); break;
        case 2: m_SpritesetView.SetZoomFactor(4); break;
        case 4: m_SpritesetView.SetZoomFactor(8); break;
      }
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnZoomOut()
{
  if (GetFocus() != this) {
    if ((m_ImagesPalette != NULL) && (GetFocus() == m_ImagesPalette)) {
      m_ImagesPalette->SendMessage(WM_COMMAND, MAKEWPARAM(ID_FILE_ZOOM_OUT, 0), 0);
    }
  }
  else {
    if (m_TabControl.GetCurSel() == 0) {
      switch ((int)m_SpritesetView.GetZoomFactor()) {
        case 2: m_SpritesetView.SetZoomFactor(1); break;
        case 4: m_SpritesetView.SetZoomFactor(2); break;
        case 8: m_SpritesetView.SetZoomFactor(4); break;
      }
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnCopy()
{
  if (GetFocus() == this) {
    if (m_TabControl.GetCurSel() == 0) {
      m_SpritesetView.SendMessage(WM_COMMAND, MAKEWPARAM(ID_SPRITESETVIEWFRAMES_COPY, 0), 0);
    }
    else
    if (m_TabControl.GetCurSel() == 1) {
      m_ImageView.SendMessage(WM_COMMAND, MAKEWPARAM(ID_IMAGEVIEW_COPY, 0), 0);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnPaste()
{
  if (GetFocus() == this) {
    if (m_TabControl.GetCurSel() == 0) {
      m_SpritesetView.SendMessage(WM_COMMAND, MAKEWPARAM(ID_SPRITESETVIEWFRAMES_PASTE, 0), 0);
    }
    else
    if (m_TabControl.GetCurSel() == 1) {
      m_ImageView.SendMessage(WM_COMMAND, MAKEWPARAM(ID_IMAGEVIEW_PASTE, 0), 0);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnUndo()
{
  if (GetFocus() == this) {
    if (m_TabControl.GetCurSel() == 0) {
      //m_SpritesetView.SendMessage(WM_COMMAND, MAKEWPARAM(ID_SPRITESETVIEWFRAMES_UNDO, 0), 0);
    }
    else
    if (m_TabControl.GetCurSel() == 1) {
      m_ImageView.SendMessage(WM_COMMAND, MAKEWPARAM(ID_IMAGEVIEW_UNDO, 0), 0);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnRedo()
{
  if (GetFocus() == this) {
    if (m_TabControl.GetCurSel() == 0) {
      //m_SpritesetView.SendMessage(WM_COMMAND, MAKEWPARAM(ID_SPRITESETVIEWFRAMES_REDO, 0), 0);
    }
    else
    if (m_TabControl.GetCurSel() == 1) {
      m_ImageView.SendMessage(WM_COMMAND, MAKEWPARAM(ID_IMAGEVIEW_REDO, 0), 0);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnZoom1x()
{
  m_SpritesetView.SetZoomFactor(1);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnZoom2x()
{
  m_SpritesetView.SetZoomFactor(2);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnZoom4x()
{
  m_SpritesetView.SetZoomFactor(4);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnZoom8x()
{
  m_SpritesetView.SetZoomFactor(8);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnResize()
{
  CResizeDialog dialog("Resize Spriteset", m_Spriteset.GetFrameWidth(), m_Spriteset.GetFrameHeight());
  if (dialog.DoModal() == IDOK) {
    m_Spriteset.ResizeFrames(dialog.GetWidth(), dialog.GetHeight());
    m_SpritesetView.SpritesetResized();
    UpdateImageView();
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnRescale()
{
  CResizeDialog dialog("Rescale Spriteset", m_Spriteset.GetFrameWidth(), m_Spriteset.GetFrameHeight());
  if (dialog.DoModal() == IDOK) {
    m_Spriteset.RescaleFrames(dialog.GetWidth(), dialog.GetHeight());
    m_SpritesetView.SpritesetResized();
    UpdateImageView();
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnResample()
{
  CResizeDialog dialog("Resample Spriteset", m_Spriteset.GetFrameWidth(), m_Spriteset.GetFrameHeight());
  if (dialog.DoModal() == IDOK) {
    m_Spriteset.ResampleFrames(dialog.GetWidth(), dialog.GetHeight());
    m_SpritesetView.SpritesetResized();
    UpdateImageView();
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnFillDelay()
{
  CNumberDialog dialog("Spriteset Delay", "Delay", 8, 1, 4096);
  if (dialog.DoModal() == IDOK) {
    bool modified = false;
    
    for (int i = 0; i < m_Spriteset.GetNumDirections(); i++) {
      for (int j = 0; j < m_Spriteset.GetNumFrames(i); j++) {
        if (m_Spriteset.GetFrameDelay(i, j) != dialog.GetValue())
          modified = true;
        m_Spriteset.SetFrameDelay(i, j, dialog.GetValue());
      }
    }
    if (modified) {
      SetModified(modified);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnFrameProperties()
{
  CSpritePropertiesDialog Dialog(&m_Spriteset, m_CurrentDirection, m_CurrentFrame);
  if (Dialog.DoModal() == IDOK)
    SetModified(true);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnExportAsImage()
{
  CImageFileDialog dialog(FDM_SAVE, "Export Spriteset as Image");
  if (dialog.DoModal() == IDOK) {
    if (!m_Spriteset.Export_PNG(dialog.GetPathName())) {
      MessageBox("Error writing image", "Export Spriteset as Image");
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnUpdateZoom1x(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_SpritesetView.GetZoomFactor() == 1);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnUpdateZoom2x(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_SpritesetView.GetZoomFactor() == 2);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnUpdateZoom4x(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_SpritesetView.GetZoomFactor() == 4);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnUpdateZoom8x(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_SpritesetView.GetZoomFactor() == 8);
}
////////////////////////////////////////////////////////////////////////////////
/*
afx_msg void
CSpritesetWindow::OnUpdateZoomIn(CCmdUI* cmdui)
{
  cmdui->Enable(m_SpritesetView.GetZoomFactor() >= 1.0/8.0);
}
*/
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnFramesTab()
{
  m_TabControl.SetCurSel(0);
  TabChanged(0);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnEditTab()
{
  m_TabControl.SetCurSel(1);
  TabChanged(1);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnBaseTab()
{
  m_TabControl.SetCurSel(2);
  TabChanged(2);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnUpdateFramesTab(CCmdUI* cmdui)
{
  cmdui->SetCheck((m_TabControl.GetCurSel() == 0) ? 1 : 0);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnUpdateEditTab(CCmdUI* cmdui)
{
  cmdui->SetCheck((m_TabControl.GetCurSel() == 1) ? 1 : 0);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnUpdateBaseTab(CCmdUI* cmdui)
{
  cmdui->SetCheck((m_TabControl.GetCurSel() == 2) ? 1 : 0);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetWindow::OnTabChanged(NMHDR* ns, LRESULT* result)
{
  if (ns->idFrom == IDC_TAB) {
    TabChanged(m_TabControl.GetCurSel());
  }
}
////////////////////////////////////////////////////////////////////////////////
bool
CSpritesetWindow::GetSavePath(char* path)
{
  std::string directory = GetMainWindow()->GetDefaultFolder(m_DocumentType);
  SetCurrentDirectory(directory.c_str());
  CSpritesetFileDialog Dialog(FDM_SAVE);
  // set current directory on Win98/2000
  Dialog.m_ofn.lpstrInitialDir = directory.c_str();
  if (Dialog.DoModal() != IDOK)
    return false;
  strcpy(path, Dialog.GetPathName());
  return true;
}
////////////////////////////////////////////////////////////////////////////////
bool
CSpritesetWindow::SaveDocument(const char* path)
{
  return m_Spriteset.Save(path);
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetWindow::SV_CurrentFrameChanged(int direction, int frame)
{
  m_CurrentDirection = direction;
  m_CurrentFrame = frame;
  UpdateImageView();
  if (m_ImagesPalette)    m_ImagesPalette->SetCurrentImage(m_Spriteset.GetFrameIndex(direction, frame));
  if (m_AnimationPalette) m_AnimationPalette->SetCurrentDirection(direction);
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetWindow::SV_EditFrame()
{
  // switch to the edit tab
  m_TabControl.SetCurSel(1);
  TabChanged(1);
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetWindow::SV_SpritesetModified()
{
  UpdateImageView();
  if (m_ImagesPalette) {
    m_ImagesPalette->SpritesetResized();
    m_ImagesPalette->Invalidate();
  }
  SetModified(true);
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetWindow::SV_ZoomFactorChanged(double zoom)
{
  if (m_AnimationPalette) m_AnimationPalette->OnZoom((double)zoom);
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetWindow::SV_CopyCurrentFrame()
{
  m_ImageView.Copy();
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetWindow::SV_PasteCurrentFrame()
{
  m_ImageView.Paste();
  m_SpritesetView.Invalidate();
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetWindow::IV_ImageChanged()
{
  // store the current sprite
  int index = m_Spriteset.GetFrameIndex(m_CurrentDirection, m_CurrentFrame);
  CImage32& frame = m_Spriteset.GetImage(index);
  memcpy(frame.GetPixels(), m_ImageView.GetPixels(), frame.GetWidth() * frame.GetHeight() * sizeof(RGBA));
  SetModified(true);
  m_SpritesetView.Invalidate();
  if (m_ImagesPalette) m_ImagesPalette->Invalidate();
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetWindow::IV_ColorChanged(RGBA color)
{
  RGB rgb = { color.red, color.green, color.blue };
  m_ColorView.SetColor(0, rgb);
  m_AlphaView.SetAlpha(color.alpha);
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetWindow::PV_ColorChanged(int index, RGB color)
{
  RGBA rgba = { color.red, color.green, color.blue, m_AlphaView.GetAlpha() };
  m_ImageView.SetColor(index, rgba);
  m_ColorView.SetColor(index, color);
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetWindow::CV_ColorChanged(int index, RGB color)
{
  RGBA rgba = { color.red, color.green, color.blue, m_AlphaView.GetAlpha() };
  m_ImageView.SetColor(index, rgba);
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetWindow::AV_AlphaChanged(byte alpha)
{
  RGBA rgba = m_ImageView.GetColor();
  rgba.alpha = alpha;
  m_ImageView.SetColor(0, rgba);
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetWindow::SBV_SpritesetModified()
{
  SetModified(true);
}
////////////////////////////////////////////////////////////////////////////////
void 
CSpritesetWindow::SP_ColorSelected(RGBA color)
{
  byte alpha = color.alpha;
  RGB  rgb   = { color.red, color.green, color.blue };
  m_ImageView.SetColor(0, color);
  m_ColorView.SetColor(0, rgb);
  m_AlphaView.SetAlpha(alpha);
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetWindow::SIP_IndexChanged(int index)
{
  int old_index = m_Spriteset.GetFrameIndex(m_CurrentDirection, m_CurrentFrame);
  if (old_index != index) {
    m_Spriteset.SetFrameIndex(m_CurrentDirection, m_CurrentFrame, index);
    UpdateImageView();
    SetModified(true);
    Invalidate();
  }
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetWindow::SIP_SpritesetModified()
{
  SetModified(true);
  m_SpritesetView.Invalidate();
  UpdateImageView();
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetWindow::OnToolChanged(UINT id, int tool_index)
{
  m_ImageView.OnToolChanged(id, tool_index);
}
////////////////////////////////////////////////////////////////////////////////
BOOL
CSpritesetWindow::IsToolAvailable(UINT id) {
  BOOL available = FALSE;
  if (m_Created) {
    if (m_TabControl.GetCurSel() == 0) {
      switch (id) {
        case ID_FILE_COPY:  available = TRUE; break;
        case ID_FILE_PASTE: {
          if (IsClipboardFormatAvailable(CF_BITMAP))
            available = TRUE;
        }
        break;
        case ID_FILE_UNDO: available = FALSE; break;
        case ID_FILE_REDO: available = FALSE; break;
      }
    }
    else if (m_TabControl.GetCurSel() ==  1) {
      available = m_ImageView.IsToolAvailable(id);
    }
  }
  return available;
}
////////////////////////////////////////////////////////////////////////////////
