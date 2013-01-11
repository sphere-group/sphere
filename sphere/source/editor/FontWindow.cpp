#pragma warning(disable : 4786)
#include "FontWindow.hpp"
#include "FileDialogs.hpp"
#include "ResizeDialog.hpp"
#include "FontGradientDialog.hpp"
#include "Editor.hpp"
#include "../common/minmax.hpp"
#include "resource.h"
#include "EditRange.hpp"
#include "FileDialogs.hpp"
#include "ListDialog.hpp"
#include "AdjustBordersDialog.hpp"
#define IDC_FONTSCROLL 900
#define MIN_CHARACTER 0
#define MAX_CHARACTER 255
#define DEFAULT_CHARACTER_WIDTH  8
#define DEFAULT_CHARACTER_HEIGHT 12
#define COLOR_WIDTH      32
#define COLOR_HEIGHT     32
#define ALPHA_WIDTH      32
#define SCROLLBAR_HEIGHT 16
#ifdef USE_SIZECBAR
IMPLEMENT_DYNAMIC(CFontWindow, CMDIChildWnd)
#endif
BEGIN_MESSAGE_MAP(CFontWindow, CSaveableDocumentWindow)
  ON_WM_CHAR()
  ON_WM_KEYDOWN()
  ON_WM_SIZE()
  ON_WM_HSCROLL()
  ON_COMMAND(ID_FONT_RESIZE,               OnFontResize)
  //ON_COMMAND(ID_FONT_RESIZEALL,            OnFontResizeAll)
  ON_COMMAND(ID_FONT_SIMPLIFY,             OnFontSimplify)
  ON_COMMAND(ID_FONT_MAKECOLORTRANSPARENT, OnFontMakeColorTransparent)
  ON_COMMAND(ID_FONT_GENERATEGRADIENT,     OnFontGenerateGradient)
  ON_COMMAND(ID_FONT_ADJUSTBORDERS,        OnFontAdjustBorders)
  ON_COMMAND(ID_FONT_EXPORTTOIMAGE,      OnFontExportToImage)
  ON_COMMAND(ID_FILE_COPY,  OnCopy)
  ON_COMMAND(ID_FILE_PASTE, OnPaste)
  ON_COMMAND(ID_FILE_UNDO,  OnUndo)
  ON_COMMAND(ID_FILE_REDO,  OnRedo)
  ON_COMMAND(ID_FILE_ZOOM_IN, OnZoomIn)
  ON_COMMAND(ID_FILE_ZOOM_OUT, OnZoomOut)
//  ON_COMMAND(ID_FONT_ER_ROTATE_CW,             OnEditRangeRotateCW)
//  ON_COMMAND(ID_FONT_ER_ROTATE_CCW,            OnEditRangeRotateCCW)
  ON_COMMAND(ID_FONT_ER_SLIDE_UP,              OnEditRange)
  ON_COMMAND(ID_FONT_ER_SLIDE_RIGHT,           OnEditRange)
  ON_COMMAND(ID_FONT_ER_SLIDE_DOWN,            OnEditRange)
  ON_COMMAND(ID_FONT_ER_SLIDE_LEFT,            OnEditRange)
  ON_COMMAND(ID_FONT_ER_SLIDE_OTHER,           OnEditRange)
  ON_COMMAND(ID_FONT_ER_FLIP_HORIZONTALLY,     OnEditRange)
  ON_COMMAND(ID_FONT_ER_FLIP_VERTICALLY,       OnEditRange)
//  ON_COMMAND(ID_FONT_ER_FILL_RGB,              OnEditRangeFillRGB)
//  ON_COMMAND(ID_FONT_ER_FILL_ALPHA,            OnEditRangeFillAlpha)
//  ON_COMMAND(ID_FONT_ER_FILL_BOTH,             OnEditRangeFillBoth)
  ON_COMMAND(ID_FONT_ER_REPLACE_RGBA,            OnEditRange)
//  ON_COMMAND(ID_FONT_ER_REPLACE_RGB,           OnEditRangeReplaceRGB)
//  ON_COMMAND(ID_FONT_ER_REPLACE_ALPHA,         OnEditRangeReplaceAlpha)
//  ON_COMMAND(ID_FONT_ER_FLT_BLUR,              OnEditRangeFilterBlur)
//  ON_COMMAND(ID_FONT_ER_FLT_NOISE,              OnEditRangeFilterNoise)
//  ON_COMMAND(ID_FONT_ER_FLT_ADJUST_BRIGHTNESS,  OnEditRangeFilterAdjustBrightness)
//  ON_COMMAND(ID_FONT_ER_FLT_ADJUST_GAMMA,       OnEditRangeFilterAdjustGamma)
//  ON_COMMAND(ID_FONT_ER_FLT_NEGATIVE_IMAGE_RGB, OnEditRangeFilterNegativeImageRGB)
//  ON_COMMAND(ID_FONT_ER_FLT_NEGATIVE_IMAGE_ALPHA, OnEditRangeFilterNegativeImageAlpha)
//  ON_COMMAND(ID_FONT_ER_FLT_NEGATIVE_IMAGE_RGBA,  OnEditRangeFilterNegativeImageRGBA)
//  ON_COMMAND(ID_FONT_ER_FLT_SOLARIZE,             OnEditRangeFilterSolarize)
//  ON_COMMAND(ID_FONT_ER_SETCOLORALPHA,         OnEditRangeSetColorAlpha)
//  ON_COMMAND(ID_FONT_ER_SCALEALPHA,            OnEditRangeScaleAlpha)
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////
CFontWindow::CFontWindow(const char* font)
: CSaveableDocumentWindow(font, IDR_FONT)
, m_CurrentCharacter(MIN_CHARACTER)
, m_CurrentColor(CreateRGBA(0, 0, 0, 255))
, m_FontPreviewPalette(NULL)
, m_Created(false)
{
  if (font) {
    if (m_Font.Load(font)) {
      SetSaved(true);
      SetModified(false);
    } else {
      AfxGetApp()->m_pMainWnd->MessageBox("Could not load font, creating new");
      m_Font.SetNumCharacters(256);
      for (int i = 0; i < m_Font.GetNumCharacters(); i++) {
        m_Font.GetCharacter(i).Resize(DEFAULT_CHARACTER_WIDTH, DEFAULT_CHARACTER_HEIGHT);
      }
      SetSaved(false);
      SetModified(false);
    }
  } else {
    m_Font.SetNumCharacters(256);
    for (int i = 0; i < m_Font.GetNumCharacters(); i++) {
      m_Font.GetCharacter(i).Resize(DEFAULT_CHARACTER_WIDTH, DEFAULT_CHARACTER_HEIGHT);
    }
    SetSaved(false);
    SetModified(false);
  }
  m_DocumentType = WA_FONT;
  Create();
}
////////////////////////////////////////////////////////////////////////////////
CFontWindow::~CFontWindow()
{
  if (m_FontPreviewPalette) {
		m_FontPreviewPalette->Destroy();
    m_FontPreviewPalette = NULL;
  }
}
////////////////////////////////////////////////////////////////////////////////
void
CFontWindow::Create()
{
  // create window
  CSaveableDocumentWindow::Create(AfxRegisterWndClass(0, ::LoadCursor(NULL, IDC_ARROW), NULL, AfxGetApp()->LoadIcon(IDI_FONT)));
  // create children
  m_ImageView.Create(this, this, this);
  m_PaletteView.Create(this, this);
  m_ColorView.Create(this, this);
  m_AlphaView.Create(this, this);
  m_ScrollBar.Create(WS_CHILD | WS_VISIBLE | SBS_HORZ, CRect(0, 0, 0, 0), this, IDC_FONTSCROLL);
  m_ScrollBar.SetScrollRange(MIN_CHARACTER, MAX_CHARACTER);
  m_ScrollBar.SetScrollPos(m_CurrentCharacter);
  m_Created = true;
  // make sure everything is the right size
  RECT ClientRect;
  GetClientRect(&ClientRect);
  OnSize(0, ClientRect.right, ClientRect.bottom);
  SetImage();
  m_ColorView.SetNumColors(2);
  m_ColorView.SetColor(0, CreateRGB(255, 255, 255));
  m_ColorView.SetColor(1, CreateRGB(0, 0, 0));
  m_ImageView.SetColor(0, CreateRGBA(255, 255, 255, 255));
  m_ImageView.SetColor(1, CreateRGBA(0, 0, 0, 255));
  m_AlphaView.SetAlpha(255);
  m_FontPreviewPalette = new CFontPreviewPalette(this, &m_Font);
  UpdateWindowTitle();

#ifdef USE_SIZECBAR
		LoadPaletteStates();
#endif
}
////////////////////////////////////////////////////////////////////////////////
void
CFontWindow::UpdateWindowTitle()
{
  char title[520];
  strcpy(title, GetDocumentTitle());
  char append[520];
  sprintf(append, " - %d/%d - %c",
    m_CurrentCharacter,
    m_Font.GetNumCharacters() - 1,
    (unsigned char)m_CurrentCharacter
    );
  strcat(title, append);
  SetCaption(title);
}
////////////////////////////////////////////////////////////////////////////////
void
CFontWindow::SetImage()
{
  sFontCharacter& c = m_Font.GetCharacter(m_CurrentCharacter);
  m_ImageView.SetImage(c.GetWidth(), c.GetHeight(), c.GetPixels(), true);
  m_ScrollBar.SetScrollPos(m_CurrentCharacter);
  if (m_FontPreviewPalette)  m_FontPreviewPalette->OnCharacterChanged(m_CurrentCharacter);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CFontWindow::OnChar(UINT c, UINT repeat, UINT flags)
{
  if (c >= MIN_CHARACTER && c <= MAX_CHARACTER) {
    if (m_CurrentCharacter != c) {
      m_CurrentCharacter = c;
      SetImage();
      UpdateWindowTitle();
      Invalidate();
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CFontWindow::OnKeyDown(UINT vk, UINT repeat, UINT flags)
{
  int old_char = m_CurrentCharacter;
  if (vk == VK_RIGHT) {
    m_CurrentCharacter = std::min(m_CurrentCharacter + 1, MAX_CHARACTER);
  } else if (vk == VK_LEFT) {
    m_CurrentCharacter = std::max(m_CurrentCharacter - 1, MIN_CHARACTER);
  }
  if (m_CurrentCharacter != old_char) {
    SetImage();
    UpdateWindowTitle();
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CFontWindow::OnSize(UINT type, int cx, int cy)
{
  if (m_Created)
  {
    m_ImageView.MoveWindow(0, 0, cx - COLOR_WIDTH - ALPHA_WIDTH, cy - SCROLLBAR_HEIGHT);
    m_PaletteView.MoveWindow(cx - COLOR_WIDTH - ALPHA_WIDTH, 0, COLOR_WIDTH, cy - SCROLLBAR_HEIGHT - COLOR_HEIGHT);
    m_ColorView.MoveWindow(cx - COLOR_WIDTH - ALPHA_WIDTH, cy - SCROLLBAR_HEIGHT - COLOR_HEIGHT, COLOR_WIDTH, COLOR_HEIGHT);
    m_AlphaView.MoveWindow(cx - ALPHA_WIDTH, 0, ALPHA_WIDTH, cy - SCROLLBAR_HEIGHT);
    m_ScrollBar.MoveWindow(0, cy - SCROLLBAR_HEIGHT, cx, SCROLLBAR_HEIGHT);
  }
  CSaveableDocumentWindow::OnSize(type, cx, cy);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CFontWindow::OnHScroll(UINT sbcode, UINT pos, CScrollBar* scroll_bar)
{
  // scroll bar
  if (scroll_bar->m_hWnd == m_ScrollBar.m_hWnd)
  {
    switch (sbcode)
    {
      case SB_LEFT:
        m_CurrentCharacter = 0;
        break;
      case SB_RIGHT:
        m_CurrentCharacter = m_Font.GetNumCharacters() - 1;
        break;
      case SB_LINELEFT:
      case SB_PAGELEFT:
        m_CurrentCharacter--;
        break;
      case SB_LINERIGHT:
      case SB_PAGERIGHT:
        m_CurrentCharacter++;
        break;
      case SB_THUMBPOSITION:
      case SB_THUMBTRACK:
        m_CurrentCharacter = pos;
        break;
      default:
        return;
     }
    if (m_CurrentCharacter < MIN_CHARACTER)
      m_CurrentCharacter = MIN_CHARACTER;
    if (m_CurrentCharacter > MAX_CHARACTER)
      m_CurrentCharacter = MAX_CHARACTER;
    SetImage();
    UpdateWindowTitle();
  }
}
////////////////////////////////////////////////////////////////////////////////
std::vector<int>
CFontWindow::GetWhichCharacters() const
{
  std::vector<int> character_list;
  CListDialog dialog;
  dialog.SetCaption("Which character?");
  dialog.AddItem("This character");
  dialog.AddItem("All characters");
  if (dialog.DoModal() != IDOK)
    return character_list;
  if (dialog.GetSelection() == 0) {
    character_list.push_back(m_CurrentCharacter);
  }
  if (dialog.GetSelection() == 1) {
    for (int i = MIN_CHARACTER; i < MAX_CHARACTER; i++) {
      character_list.push_back(i);
    }
  }
  return character_list;
}
////////////////////////////////////////////////////////////////////////////////
void
CFontWindow::GetFontMinMax(const std::vector<int> character_list, int& min_x, int& min_y, int& max_x, int& max_y) const
{
  min_x = -1;
  min_y = -1;
  max_x = 0;
  max_y = 0;
  for (unsigned int i = 0; i < character_list.size(); i++)
  {
    int ch = character_list[i];
    if (ch >= 0 && ch < m_Font.GetNumCharacters())
    {
      const sFontCharacter& c = m_Font.GetCharacter(ch);
      if (c.GetWidth() > max_x) {
        max_x = c.GetWidth();
      }
      if (min_x == -1 || c.GetWidth() < min_x) {
        min_x = c.GetWidth();
      }
      if (c.GetHeight() > max_y) {
        max_y = c.GetHeight();
      }
      if (min_y == -1 || c.GetHeight() < min_y) {
        min_y = c.GetHeight();
      }
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CFontWindow::OnFontResize()
{
  std::vector<int> character_list = GetWhichCharacters();
  if (character_list.size() == 0)
    return;
  int max_x = 0, max_y = 0, min_x = -1, min_y = -1;
  GetFontMinMax(character_list, min_x, min_y, max_x, max_y);
  CResizeDialog dialog("Resize Font Character(s)", max_x, max_y);
	dialog.SetRange(1, 4096, 1, 4096);
  dialog.AllowPercentages(character_list.size() == 1);
  if (dialog.DoModal() != IDOK)
    return;
	bool modified = false;
  for (unsigned int i = 0; i < character_list.size(); i++)
  {
    int ch = character_list[i];
    if (ch >= 0 && ch < m_Font.GetNumCharacters())
    {
    	sFontCharacter& c = m_Font.GetCharacter(ch);
      const int width = c.GetWidth();
      const int height = c.GetHeight();
      c.Resize(dialog.GetWidth(), dialog.GetHeight());
      modified |= ( !(width == c.GetWidth() && height == c.GetHeight()) );
    }
  }
	if (modified) {
    SetModified(true);
    SetImage();
    if (m_FontPreviewPalette) m_FontPreviewPalette->OnCharacterChanged(-1);
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CFontWindow::OnFontAdjustBorders()
{
  std::vector<int> character_list = GetWhichCharacters();
  if (character_list.size() == 0)
    return;
  int max_x = 0, max_y = 0, min_x = -1, min_y = -1;
  GetFontMinMax(character_list, min_x, min_y, max_x, max_y);
  CAdjustBordersDialog dialog(0, 0, min_x, min_y, 0, 0, 0, 0);
  if (dialog.DoModal() != IDOK)
    return;
	bool modified = false;
  for (unsigned int i = 0; i < character_list.size(); i++)
  {
    int ch = character_list[i];
    if (ch >= 0 && ch < m_Font.GetNumCharacters())
    {
    	sFontCharacter& c = m_Font.GetCharacter(ch);
      const int width = c.GetWidth();
      const int height = c.GetHeight();
      c.AdjustBorders(dialog.GetTopPixels(), dialog.GetRightPixels(), dialog.GetBottomPixels(), dialog.GetLeftPixels());
      modified |= ( !(width == c.GetWidth() && height == c.GetHeight()) );
    }
  }
	if (modified) {
    SetModified(true);
    SetImage();
    if (m_FontPreviewPalette) m_FontPreviewPalette->OnCharacterChanged(-1);
  }
}
////////////////////////////////////////////////////////////////////////////////
bool SimplifyImage(CImage32& c)
{
  bool modified = false;
  int j;
  if (!modified) {
    for (j = 0; j < c.GetWidth() * c.GetHeight(); j++) {
      if (c.GetPixels()[j].alpha != 0
       && c.GetPixels()[j].alpha != 255) {
        modified = true;
        break;
      }
    }
  }
  if (modified) {
    for (j = 0; j < c.GetWidth() * c.GetHeight(); j++)
    {
      if (c.GetPixels()[j].alpha < 128)
        c.GetPixels()[j].alpha = 0;
      else
        c.GetPixels()[j].alpha = 255;
    }
  }
  return modified;
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CFontWindow::OnFontSimplify()
{
  std::vector<int> character_list = GetWhichCharacters();
  if (character_list.size() == 0)
    return;
  bool modified = false;

  for (unsigned int i = 0; i < character_list.size(); i++)
  {
    int ch = character_list[i];
    if (ch >= 0 && ch < m_Font.GetNumCharacters())
    {
      sFontCharacter& c = m_Font.GetCharacter(i);
      modified |= SimplifyImage(c);
    }
  }
  if (modified) {
    SetModified(true);
    SetImage();
    if (m_FontPreviewPalette) m_FontPreviewPalette->OnCharacterChanged(-1);
  }
}
////////////////////////////////////////////////////////////////////////////////
bool
MakeColorTransparent(CImage32& c, RGB color)
{
  bool modified = false;
  for (int j = 0; j < c.GetWidth() * c.GetHeight(); ++j) {
    if (c.GetPixels()[j].red   == color.red &&
        c.GetPixels()[j].green == color.green &&
        c.GetPixels()[j].blue  == color.blue)
    {
      if (c.GetPixels()[j].alpha != 0) {
        c.GetPixels()[j].alpha = 0;
        modified = true;
     }
    }
  }
  return modified;
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CFontWindow::OnFontMakeColorTransparent()
{
  std::vector<int> character_list = GetWhichCharacters();
  if (character_list.size() == 0)
    return;
  RGB color = m_ColorView.GetColor();
  bool modified = false;
  for (unsigned int i = 0; i < character_list.size(); i++)
  {
    int ch = character_list[i];
    if (ch >= 0 && ch < m_Font.GetNumCharacters())
    {
      sFontCharacter& c = m_Font.GetCharacter(i);
      modified |= MakeColorTransparent(c, color);
    }
  }
  if (modified) {
    SetModified(true);
    SetImage();
    if (m_FontPreviewPalette) m_FontPreviewPalette->OnCharacterChanged(-1);
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CFontWindow::OnFontGenerateGradient()
{
  std::vector<int> character_list = GetWhichCharacters();
  if (character_list.size() == 0)
    return;
  CFontGradientDialog dialog;
  if (dialog.DoModal() == IDOK) {
    m_Font.GenerateGradient(dialog.GetTopColor(), dialog.GetBottomColor());
    SetModified(true);
    SetImage();
    if (m_FontPreviewPalette) m_FontPreviewPalette->OnCharacterChanged(-1);
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CFontWindow::OnFontExportToImage()
{
  CImageFileDialog FileDialog(FDM_SAVE);
  if (FileDialog.DoModal() != IDOK)
    return;
  int font_width = 0;
  int font_height = 0;
  int i;

  for (i = 0; i < m_Font.GetNumCharacters(); i++) {
    if (font_width < m_Font.GetCharacter(i).GetWidth())
      font_width = m_Font.GetCharacter(i).GetWidth();
    if (font_height < m_Font.GetCharacter(i).GetHeight())
      font_height = m_Font.GetCharacter(i).GetHeight();
  }

  if (!(font_width > 0 && font_height > 0))
    return;

  int border_size = 0;
  if (MessageBox("Use a 1 pixel border?", "Font", MB_ICONQUESTION | MB_YESNO) == IDYES) {
    border_size = 1;
  }

  int num_characters_per_row = 16;
  int num_characters_per_col = 16;

  CImage32 image;
  if (!image.Create(font_width * num_characters_per_row + ((num_characters_per_row + 1) * border_size),
                    font_height * num_characters_per_col + ((num_characters_per_col + 1) * border_size)))
    return;

  image.Rectangle(0, 0, image.GetWidth(), image.GetHeight(), CreateRGBA(255, 0, 0, 255));
  image.SetBlendMode(CImage32::REPLACE);

  i = 0;
  for (int fy = 0; fy < num_characters_per_col; fy++)
  {
    for (int fx = 0; fx < num_characters_per_row; fx++)
    {
      CImage32 c = m_Font.GetCharacter(i);
      int x = ((fx + 1) * border_size) + (font_width  * fx);
      int y = ((fy + 1) * border_size) + (font_height * fy);
      //image.Rectangle(x, y, x + font_width - 1, y + font_height - 1, CreateRGBA(255, 255, 0, 255));
      image.BlitImage(c, x, y);
      i += 1;
    }
  }

  if (!image.Save(FileDialog.GetPathName())) {
    MessageBox("Could not save image");
  }
  else {
    MessageBox("Exported font!");
  }
}

////////////////////////////////////////////////////////////////////////////////

bool
CFontWindow::GetSavePath(char* path)
{
  std::string directory = GetMainWindow()->GetDefaultFolder(m_DocumentType);
  SetCurrentDirectory(directory.c_str());
  CFontFileDialog Dialog(FDM_SAVE);
  // set current directory on Win98/2000
  Dialog.m_ofn.lpstrInitialDir = directory.c_str();
  if (Dialog.DoModal() != IDOK)
    return false;
  strcpy(path, Dialog.GetPathName());
  return true;
}
////////////////////////////////////////////////////////////////////////////////
bool
CFontWindow::SaveDocument(const char* path)
{
  return m_Font.Save(path);
}
////////////////////////////////////////////////////////////////////////////////
void
CFontWindow::IV_ImageChanged()
{
  sFontCharacter& c = m_Font.GetCharacter(m_CurrentCharacter);
  memcpy(c.GetPixels(), m_ImageView.GetPixels(), c.GetWidth() * c.GetHeight() * sizeof(RGBA));
  SetModified(true);
  UpdateWindowTitle();
  if (m_FontPreviewPalette) m_FontPreviewPalette->OnCharacterChanged(m_CurrentCharacter);
}
////////////////////////////////////////////////////////////////////////////////
void
CFontWindow::IV_ColorChanged(RGBA color)
{
  m_CurrentColor = color;
  RGB rgb = { color.red, color.green, color.blue };
  m_ColorView.SetColor(0, rgb);
  m_AlphaView.SetAlpha(color.alpha);
}
////////////////////////////////////////////////////////////////////////////////
void
CFontWindow::PV_ColorChanged(int index, RGB color)
{
  m_CurrentColor.red   = color.red;
  m_CurrentColor.green = color.green;
  m_CurrentColor.blue  = color.blue;
  RGB rgb = { color.red, color.green, color.blue };
  m_ImageView.SetColor(index, m_CurrentColor);
  m_ColorView.SetColor(index, rgb);
}
////////////////////////////////////////////////////////////////////////////////
void
CFontWindow::CV_ColorChanged(int index, RGB color)
{
  m_CurrentColor.red   = color.red;
  m_CurrentColor.green = color.green;
  m_CurrentColor.blue  = color.blue;
  m_ImageView.SetColor(index, m_CurrentColor);
}
////////////////////////////////////////////////////////////////////////////////
void
CFontWindow::AV_AlphaChanged(byte alpha)
{
  m_CurrentColor.alpha = alpha;
  m_ImageView.SetColor(0, m_CurrentColor);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CFontWindow::OnEditRange()
{
  const unsigned int id = GetCurrentMessage()->wParam;
  if (EditRange::OnEditRange("font", id, false, (void*) &m_Font, m_CurrentCharacter)) {
    SetModified(true);
    SetImage();
    if (m_FontPreviewPalette) m_FontPreviewPalette->OnCharacterChanged(-1);
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CFontWindow::OnZoomIn()
{
  if (GetFocus() != this) {
    if ((m_FontPreviewPalette != NULL) && (GetFocus() == m_FontPreviewPalette)) {
      m_FontPreviewPalette->SendMessage(WM_COMMAND, MAKEWPARAM(ID_FILE_ZOOM_IN, 0), 0);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CFontWindow::OnZoomOut()
{
  if (GetFocus() != this) {
    if ((m_FontPreviewPalette != NULL) && (GetFocus() == m_FontPreviewPalette)) {
      m_FontPreviewPalette->SendMessage(WM_COMMAND, MAKEWPARAM(ID_FILE_ZOOM_OUT, 0), 0);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CFontWindow::OnCopy()
{
  m_ImageView.SendMessage(WM_COMMAND, MAKEWPARAM(ID_IMAGEVIEW_COPY, 0), 0);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CFontWindow::OnPaste()
{
  m_ImageView.SendMessage(WM_COMMAND, MAKEWPARAM(ID_IMAGEVIEW_PASTE, 0), 0);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CFontWindow::OnUndo()
{
  m_ImageView.SendMessage(WM_COMMAND, MAKEWPARAM(ID_IMAGEVIEW_UNDO, 0), 0);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CFontWindow::OnRedo()
{
  m_ImageView.SendMessage(WM_COMMAND, MAKEWPARAM(ID_IMAGEVIEW_REDO, 0), 0);
}
////////////////////////////////////////////////////////////////////////////////
void
CFontWindow::OnToolChanged(UINT id, int tool_index)
{
  m_ImageView.OnToolChanged(id, tool_index);
}
////////////////////////////////////////////////////////////////////////////////
BOOL
CFontWindow::IsToolAvailable(UINT id)
{
  return m_ImageView.IsToolAvailable(id);
}
////////////////////////////////////////////////////////////////////////////////
