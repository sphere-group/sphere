#include "SpritesetView.hpp"
#include "ResizeDialog.hpp"
#include "SpritePropertiesDialog.hpp"
#include "StringDialog.hpp"
#include "FileDialogs.hpp"
#include "resource.h"
#include "NumberDialog.hpp"
#include "FontGradientDialog.hpp"
#include "EditRange.hpp"
#include "../common/strcmp_ci.hpp"
#include "ImageRender.hpp"
#include "Editor.hpp"

#define LABEL_WIDTH 80

static int s_SpritesetViewID = 100;

BEGIN_MESSAGE_MAP(CSpritesetView, CWnd)
  ON_WM_SIZE()
  ON_WM_LBUTTONDOWN()
  ON_WM_RBUTTONUP()
  ON_WM_LBUTTONDBLCLK()
  ON_WM_HSCROLL()
  ON_WM_VSCROLL()
  ON_WM_PAINT()
  ON_COMMAND(ID_SPRITESETVIEWDIRECTIONS_INSERT,     OnInsertDirection)
  ON_COMMAND(ID_SPRITESETVIEWDIRECTIONS_DELETE,     OnDeleteDirection)
  ON_COMMAND(ID_SPRITESETVIEWDIRECTIONS_APPEND,     OnAppendDirection)
  ON_COMMAND(ID_SPRITESETVIEWDIRECTIONS_PROPERTIES, OnDirectionProperties)
  ON_COMMAND(ID_SPRITESETVIEWDIRECTIONS_IMPORT_FROM_IMAGE, OnInsertDirectionFromImage)
  ON_COMMAND(ID_SPRITESETVIEWDIRECTIONS_IMPORT_FROM_ANIMATION, OnInsertDirectionFromAnimation)
  ON_COMMAND(ID_SPRITESETVIEWDIRECTIONS_EXPORT_AS_ANIMATION, OnExportDirectionAsAnimation)
  ON_COMMAND(ID_SPRITESETVIEWDIRECTIONS_EXPORT_AS_IMAGE, OnExportDirectionAsImage)
  ON_COMMAND(ID_SPRITESETVIEWDIRECTIONS_INSERTCOPY_FLIPVERTICALLY,   OnInsertCopyFlipVertically)
  ON_COMMAND(ID_SPRITESETVIEWDIRECTIONS_INSERTCOPY_FLIPHORIZONTALLY, OnInsertCopyFlipHorizontally)
  ON_COMMAND(ID_SPRITESETVIEWDIRECTIONS_FILLDELAY,  OnFillDelay)
  ON_COMMAND(ID_SPRITESETVIEWFRAMES_INSERT,     OnInsertFrame)
  ON_COMMAND(ID_SPRITESETVIEWFRAMES_APPEND,     OnAppendFrame)
  ON_COMMAND(ID_SPRITESETVIEWFRAMES_DELETE,     OnDeleteFrame)
  ON_COMMAND(ID_SPRITESETVIEWFRAMES_INSERTFRAMES, OnInsertFrames)
  ON_COMMAND(ID_SPRITESETVIEWFRAMES_APPENDFRAMES, OnAppendFrames)
  ON_COMMAND(ID_SPRITESETVIEWFRAMES_REMOVEFRAMES, OnRemoveFrames)
  ON_COMMAND(ID_SPRITESETVIEWFRAMES_COPY,       OnCopyFrame)
  ON_COMMAND(ID_SPRITESETVIEWFRAMES_PASTE,      OnPasteFrame)
  ON_COMMAND(ID_SPRITESETVIEWFRAMES_PROPERTIES, OnFrameProperties)
  ON_COMMAND(ID_SPRITESETVIEWFRAMES_IMPORT_FROM_IMAGE, OnImportFrameFromImage)
  ON_COMMAND(ID_SPRITESETVIEWFRAMES_EXPORT_TO_IMAGE,   OnExportFrameToImage)
//  ON_COMMAND(ID_SPRITESETVIEWFRAMES_ER_ROTATE_CW,             OnEditRangeRotateCW)
//  ON_COMMAND(ID_SPRITESETVIEWFRAMES_ER_ROTATE_CCW,            OnEditRangeRotateCCW)
  ON_COMMAND(ID_SPRITESETVIEWFRAMES_ER_SLIDE_UP,              OnEditRange)
  ON_COMMAND(ID_SPRITESETVIEWFRAMES_ER_SLIDE_RIGHT,           OnEditRange)
  ON_COMMAND(ID_SPRITESETVIEWFRAMES_ER_SLIDE_DOWN,            OnEditRange)
  ON_COMMAND(ID_SPRITESETVIEWFRAMES_ER_SLIDE_LEFT,            OnEditRange)
  ON_COMMAND(ID_SPRITESETVIEWFRAMES_ER_SLIDE_OTHER,           OnEditRange)
  ON_COMMAND(ID_SPRITESETVIEWFRAMES_ER_FLIP_HORIZONTALLY,     OnEditRange)
  ON_COMMAND(ID_SPRITESETVIEWFRAMES_ER_FLIP_VERTICALLY,       OnEditRange)
//  ON_COMMAND(ID_SPRITESETVIEWFRAMES_ER_FILL_RGB,              OnEditRangeFillRGB)
//  ON_COMMAND(ID_SPRITESETVIEWFRAMES_ER_FILL_ALPHA,            OnEditRangeFillAlpha)
//  ON_COMMAND(ID_SPRITESETVIEWFRAMES_ER_FILL_BOTH,             OnEditRangeFillBoth)
  ON_COMMAND(ID_SPRITESETVIEWFRAMES_ER_REPLACE_RGBA,            OnEditRange)
//  ON_COMMAND(ID_SPRITESETVIEWFRAMES_ER_REPLACE_RGB,           OnEditRangeReplaceRGB)
//  ON_COMMAND(ID_SPRITESETVIEWFRAMES_ER_REPLACE_ALPHA,         OnEditRangeReplaceAlpha)
//  ON_COMMAND(ID_SPRITESETVIEWFRAMES_ER_FLT_BLUR,              OnEditRangeFilterBlur)
//  ON_COMMAND(ID_SPRITESETVIEWFRAMES_ER_FLT_NOISE,              OnEditRangeFilterNoise)
//  ON_COMMAND(ID_SPRITESETVIEWFRAMES_ER_FLT_ADJUST_BRIGHTNESS,  OnEditRangeFilterAdjustBrightness)
//  ON_COMMAND(ID_SPRITESETVIEWFRAMES_ER_FLT_ADJUST_GAMMA,       OnEditRangeFilterAdjustGamma)
//  ON_COMMAND(ID_SPRITESETVIEWFRAMES_ER_FLT_NEGATIVE_IMAGE_RGB, OnEditRangeFilterNegativeImageRGB)
//  ON_COMMAND(ID_SPRITESETVIEWFRAMES_ER_FLT_NEGATIVE_IMAGE_ALPHA, OnEditRangeFilterNegativeImageAlpha)
//  ON_COMMAND(ID_SPRITESETVIEWFRAMES_ER_FLT_NEGATIVE_IMAGE_RGBA,  OnEditRangeFilterNegativeImageRGBA)
//  ON_COMMAND(ID_SPRITESETVIEWFRAMES_ER_FLT_SOLARIZE,             OnEditRangeFilterSolarize)
//  ON_COMMAND(ID_SPRITESETVIEWFRAMES_ER_SETCOLORALPHA,         OnEditRangeSetColorAlpha)
//  ON_COMMAND(ID_SPRITESETVIEWFRAMES_ER_SCALEALPHA,            OnEditRangeScaleAlpha)
	ON_MESSAGE(WM_GETACCELERATOR, OnGetAccelerator)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////
CSpritesetView::CSpritesetView()
: m_Handler(NULL)
, m_Spriteset(NULL)
, m_CurrentDirection(0)
, m_CurrentFrame(0)
, m_DrawBitmap(NULL)
, m_MaxFrameWidth(0)
, m_MaxFrameHeight(0)
, m_TopDirection(0)
, m_LeftFrame(0)
, m_MenuDisplayed(false)
{
}
////////////////////////////////////////////////////////////////////////////////
CSpritesetView::~CSpritesetView()
{
  delete m_DrawBitmap;
  DestroyWindow();
}
////////////////////////////////////////////////////////////////////////////////
BOOL
CSpritesetView::Create(CWnd* parent, ISpritesetViewHandler* handler, sSpriteset* spriteset)
{
  m_Handler = handler;
  m_Spriteset = spriteset;
  UpdateMaxSizes();
  CWnd::Create(
    AfxRegisterWndClass(CS_DBLCLKS, LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)), NULL, NULL),
    "",
    WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL,
    CRect(0, 0, 0, 0),
    parent,
    s_SpritesetViewID++);
  // everything should start out in the right place
  RECT rect;
  GetClientRect(&rect);
  OnSize(0, rect.right, rect.bottom);

  return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetView::SetDirection(int direction)
{
  m_CurrentDirection = direction;
  Invalidate();
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetView::SetFrame(int frame)
{
  m_CurrentFrame = frame;
  Invalidate();
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetView::SetZoomFactor(double zoom)
{
  m_ZoomFactor.SetZoomFactor(zoom);
	m_Handler->SV_ZoomFactorChanged(m_ZoomFactor.GetZoomFactor());
  UpdateMaxSizes();
  Invalidate();
  UpdateScrollBars();
}
////////////////////////////////////////////////////////////////////////////////
double
CSpritesetView::GetZoomFactor() const
{
  return m_ZoomFactor.GetZoomFactor();
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetView::SpritesetResized()
{
  UpdateMaxSizes();
  UpdateScrollBars();
  m_Handler->SV_SpritesetModified();
  Invalidate();
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetView::UpdateMaxSizes()
{
  // apply the zoom ratio
  m_MaxFrameWidth  = (int) (m_Spriteset->GetFrameWidth()  * m_ZoomFactor.GetZoomFactor());
  m_MaxFrameHeight = (int) (m_Spriteset->GetFrameHeight() * m_ZoomFactor.GetZoomFactor());
  // update the draw bitmap
  delete m_DrawBitmap;
  m_DrawBitmap = new CDIBSection(m_MaxFrameWidth, m_MaxFrameHeight, 32);
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetView::DrawDirection(CPaintDC& dc_, int direction, int y)
{
  HDC dc = dc_.m_hDC;
  HBRUSH brush = CreateSolidBrush(0xFFFFFF);
  HBRUSH old_brush = (HBRUSH)SelectObject(dc, brush);
  HFONT old_font = (HFONT)SelectObject(dc, GetStockObject(DEFAULT_GUI_FONT));
  RECT r = { 0, y, LABEL_WIDTH, y + m_MaxFrameHeight };
  FillRect(dc, &r, brush);
  DrawText(
    dc,
    m_Spriteset->GetDirectionName(direction),
    -1,
    &r,
    DT_CENTER | DT_VCENTER | DT_SINGLELINE
  );
  SelectObject(dc, old_font);
  SelectObject(dc, old_brush);
  DeleteObject(brush);
  RECT rect;
  GetClientRect(&rect);
  for (int i = 0; i < rect.right / m_MaxFrameWidth + 1; i++)
  {
    RECT c;
    c.left   = LABEL_WIDTH + i * m_MaxFrameWidth;
    c.top    = y;
    c.right  = c.left + m_MaxFrameWidth;
    c.bottom = y + m_MaxFrameHeight;
    if (RectVisible(dc, &c))
    {
      if ((i + m_LeftFrame < m_Spriteset->GetNumFrames(direction))
       && !(!m_DrawBitmap || m_DrawBitmap->GetPixels() == NULL))
      {
        UpdateDrawBitmap(direction, i + m_LeftFrame);
        DrawFrame(dc_, LABEL_WIDTH + i * m_MaxFrameWidth, y, direction, i + m_LeftFrame);
      }
      else
        FillRect(dc, &c, (HBRUSH)GetStockObject(BLACK_BRUSH));
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetView::DrawFrame(CPaintDC& dc_, int x, int y, int direction, int frame)
{
  HDC dc = dc_.m_hDC;
  if (!m_DrawBitmap || m_DrawBitmap->GetPixels() == NULL)
    return;
  
  int index = m_Spriteset->GetFrameIndex(direction, frame);
  CImage32& sprite = m_Spriteset->GetImage(index);
  BitBlt(dc, x, y, m_MaxFrameWidth, m_MaxFrameHeight, m_DrawBitmap->GetDC(), 0, 0, SRCCOPY);
  /*
  StretchedBlit(dc_, m_DrawBitmap,
              (int)m_ZoomFactor.GetZoomFactor(), (int)m_ZoomFactor.GetZoomFactor(),
              sprite.GetWidth(), sprite.GetHeight(),
              sprite.GetPixels(), true, NULL,
              0, 0, m_MaxFrameWidth, m_MaxFrameHeight,
              x, y);
  */
  if (direction == m_CurrentDirection && frame == m_CurrentFrame)
  {
    HPEN new_pen = CreatePen(PS_SOLID, 1, 0xFF00FF);
    HBRUSH brush = (HBRUSH)GetStockObject(NULL_BRUSH);
    HPEN old_pen = (HPEN)SelectObject(dc, new_pen);
    HBRUSH old_brush = (HBRUSH)SelectObject(dc, brush);
    Rectangle(dc, x, y, x + (int) (sprite.GetWidth() * m_ZoomFactor.GetZoomFactor()), y + (int) (sprite.GetHeight() * m_ZoomFactor.GetZoomFactor()));
    SelectObject(dc, old_pen);
    SelectObject(dc, old_brush);
    DeleteObject(new_pen);
  }
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetView::UpdateDrawBitmap(int direction, int frame)
{
  int index = m_Spriteset->GetFrameIndex(direction, frame);
  CImage32& sprite = m_Spriteset->GetImage(index);
  int   src_width  = sprite.GetWidth();
  int   src_height = sprite.GetHeight();
  RGBA* src_pixels = sprite.GetPixels();
  int   dst_width  = m_DrawBitmap->GetWidth();
  int   dst_height = m_DrawBitmap->GetHeight();
  BGRA* dst_pixels = (BGRA*)m_DrawBitmap->GetPixels();
  for (int iy = 0; iy < dst_height; iy++)
    for (int ix = 0; ix < dst_width; ix++)
    {
      if (ix < src_width * m_ZoomFactor.GetZoomFactor() && iy < src_height * m_ZoomFactor.GetZoomFactor())
      {
        int sx = (int) (ix / m_ZoomFactor.GetZoomFactor());
        int sy = (int) (iy / m_ZoomFactor.GetZoomFactor());
        dst_pixels[iy * dst_width + ix].red   = src_pixels[sy * src_width + sx].red;
        dst_pixels[iy * dst_width + ix].green = src_pixels[sy * src_width + sx].green;
        dst_pixels[iy * dst_width + ix].blue  = src_pixels[sy * src_width + sx].blue;
      }
      else
        dst_pixels[iy * dst_width + ix] = CreateBGRA(0, 0, 0, 255);
    }
  // draw a white border around the frame
  HDC dc = m_DrawBitmap->GetDC();
  HPEN   old_pen   = (HPEN)SelectObject(dc, GetStockObject(WHITE_PEN));
  HBRUSH old_brush = (HBRUSH)SelectObject(dc, GetStockObject(NULL_BRUSH));
  Rectangle(dc, 0, 0, (int) (src_width * m_ZoomFactor.GetZoomFactor()), (int) (src_height * m_ZoomFactor.GetZoomFactor()));
  SelectObject(dc, old_pen);
  SelectObject(dc, old_brush);
}
////////////////////////////////////////////////////////////////////////////////
static int GetMaxNumFrames(sSpriteset* spriteset)
{
  int max = 0;
  for (int i = 0; i < spriteset->GetNumDirections(); i++)
    if (spriteset->GetNumFrames(i) > max)
      max = spriteset->GetNumFrames(i);
  return max;
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetView::UpdateScrollBars()
{
  if (m_MaxFrameWidth == 0 || m_MaxFrameHeight == 0)
    return;

  RECT client_rect;
  GetClientRect(&client_rect);
  int num_x_sprites = GetMaxNumFrames(m_Spriteset);
  int x_page_size   = (client_rect.right - LABEL_WIDTH) / m_MaxFrameWidth;
  int num_y_sprites = m_Spriteset->GetNumDirections();
  int y_page_size   = client_rect.bottom / m_MaxFrameHeight;
  SCROLLINFO si;
  si.cbSize = sizeof(si);
  si.fMask = SIF_ALL;
  si.nMin  = 0;
  // horizontal scrollbar (frames)
  if (x_page_size < num_x_sprites)
  {
    si.nMax  = num_x_sprites - 1;
    si.nPage = x_page_size;
    si.nPos  = m_LeftFrame;
  }
  else
  {
    si.nMax  = 0xFFFF;
    si.nPage = 0xFFFE;
    si.nPos  = 0;
  }
  SetScrollInfo(SB_HORZ, &si);
  // vertical scrollbar (directions)
  if (y_page_size < num_y_sprites)
  {
    si.nMax  = num_y_sprites - 1;
    si.nPage = y_page_size;
    si.nPos  = m_TopDirection;
  }
  else
  {
    si.nMax  = 0xFFFF;
    si.nPage = 0xFFFE;
    si.nPos  = 0;
  }
  SetScrollInfo(SB_VERT, &si);
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetView::InvalidateFrame(int direction, int frame)
{
  direction -= m_TopDirection;
  frame     -= m_LeftFrame;
  RECT r;
  r.left   = LABEL_WIDTH + frame * m_MaxFrameWidth;
  r.top    = direction * m_MaxFrameHeight;
  r.right  = r.left + m_MaxFrameWidth;
  r.bottom = r.top + m_MaxFrameHeight;
  InvalidateRect(&r);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetView::OnSize(UINT type, int cx, int cy)
{
  if (m_MaxFrameWidth == 0 || m_MaxFrameHeight == 0)
    return;

  if (cx != 0 && cy != 0)
  {
    int x_page_size = (cx - LABEL_WIDTH) / m_MaxFrameWidth;
    int y_page_size = cy / m_MaxFrameHeight;
    int x_num_frames = GetMaxNumFrames(m_Spriteset);
    int y_num_frames = m_Spriteset->GetNumDirections();
    // update m_LeftFrame
    if (m_LeftFrame > x_num_frames - x_page_size)
    {
      m_LeftFrame = x_num_frames - x_page_size;
      if (m_LeftFrame < 0)
        m_LeftFrame = 0;
    }
    // update m_TopDirection
    if (m_TopDirection > y_num_frames - y_page_size)
    {
      m_TopDirection = y_num_frames - y_page_size;
      if (m_TopDirection < 0)
        m_TopDirection = 0;
    }
  }
  UpdateScrollBars();
  CWnd::OnSize(type, cx, cy);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetView::OnLButtonDown(UINT flags, CPoint point)
{
  if (m_MenuDisplayed)
    return;

  if (point.x < LABEL_WIDTH)
  {
    m_CurrentDirection = point.y / m_MaxFrameHeight + m_TopDirection;
    if (m_CurrentDirection >= m_Spriteset->GetNumDirections())
      return;
    if (m_CurrentFrame > m_Spriteset->GetNumFrames(m_CurrentDirection) - 1)
      m_CurrentFrame = m_Spriteset->GetNumFrames(m_CurrentDirection) - 1;
    m_Handler->SV_CurrentFrameChanged(m_CurrentDirection, m_CurrentFrame);
    Invalidate();
    return;
  }
  int direction = point.y / m_MaxFrameHeight + m_TopDirection;
  int frame = (point.x - LABEL_WIDTH) / m_MaxFrameWidth + m_LeftFrame;
  if (direction < 0 || direction >= m_Spriteset->GetNumDirections())
    return;
  if (frame < 0 || frame >= m_Spriteset->GetNumFrames(direction))
    return;
  InvalidateFrame(m_CurrentDirection, m_CurrentFrame);
  // set the new selection values
  m_CurrentDirection = direction;
  m_CurrentFrame = frame;
  // tell the handler that things have changed
  m_Handler->SV_CurrentFrameChanged(direction, frame);
  InvalidateFrame(m_CurrentDirection, m_CurrentFrame);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetView::OnRButtonUp(UINT flags, CPoint point)
{
  if (m_MenuDisplayed)
    return;

  // select the frame
  OnLButtonDown(flags, point);
  // if user right-clicked on a direction
  if (point.x < LABEL_WIDTH)
  {
    // load the menu
    HMENU base_menu = LoadMenu(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_SPRITESETVIEW));
    HMENU menu = GetSubMenu(base_menu, 0);
    if (m_Spriteset->GetNumDirections() < 2)
      EnableMenuItem(menu, ID_SPRITESETVIEWDIRECTIONS_DELETE, MF_BYCOMMAND | MF_GRAYED);
    m_MenuDisplayed = true;
    // display it
    POINT p;
    GetCursorPos(&p);
    TrackPopupMenu(menu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, p.x, p.y, 0, m_hWnd, NULL);
    m_MenuDisplayed = false;
    DestroyMenu(base_menu);
  }
  if (point.x >= LABEL_WIDTH)
  {
    int direction = point.y / m_MaxFrameHeight + m_TopDirection;
    int frame = (point.x - LABEL_WIDTH) / m_MaxFrameWidth;
    if (frame >= m_Spriteset->GetNumFrames(direction))
      return;
    // load the menu
    HMENU base_menu = LoadMenu(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_SPRITESETVIEW));
    HMENU menu = GetSubMenu(base_menu, 1);
    // enable/disable the menu items
    if (m_Spriteset->GetNumFrames(m_CurrentDirection) <= 1) {
      EnableMenuItem(menu, ID_SPRITESETVIEWFRAMES_DELETE,       MF_BYCOMMAND | MF_GRAYED);
      EnableMenuItem(menu, ID_SPRITESETVIEWFRAMES_REMOVEFRAMES, MF_BYCOMMAND | MF_GRAYED);
    }
    m_MenuDisplayed = true;
    // display it
    POINT p;
    GetCursorPos(&p);
    TrackPopupMenu(menu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, p.x, p.y, 0, m_hWnd, NULL);
    m_MenuDisplayed = false;
    DestroyMenu(base_menu);
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetView::OnLButtonDblClk(UINT flags, CPoint point)
{
  if (m_MenuDisplayed)
    return;
  if (point.x - LABEL_WIDTH < 0)
    return;

  int direction = point.y / m_MaxFrameHeight + m_TopDirection;
  int frame = (point.x - LABEL_WIDTH) / m_MaxFrameWidth + m_LeftFrame;
  if (direction < 0 || direction >= m_Spriteset->GetNumDirections())
    return;
  if (frame < 0 || frame >= m_Spriteset->GetNumFrames(direction))
    return;
  InvalidateFrame(m_CurrentDirection, m_CurrentFrame);
  m_CurrentDirection = direction;
  m_CurrentFrame = frame;
  m_Handler->SV_CurrentFrameChanged(direction, frame);
  m_Handler->SV_EditFrame();
  InvalidateFrame(m_CurrentDirection, m_CurrentFrame);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetView::OnHScroll(UINT code, UINT pos, CScrollBar* scroll_bar)
{
  RECT client_rect;
  GetClientRect(&client_rect);
  int page_width = (client_rect.right - LABEL_WIDTH) / m_MaxFrameWidth;
  int x_num_frames = GetMaxNumFrames(m_Spriteset);
  switch (code)
  {
    case SB_LEFT:       m_LeftFrame = 0; break;
    case SB_RIGHT:      m_LeftFrame = x_num_frames - page_width; break;
    case SB_PAGELEFT:   m_LeftFrame -= page_width; break;
    case SB_PAGERIGHT:  m_LeftFrame += page_width; break;
    case SB_LINELEFT:   m_LeftFrame--; break;
    case SB_LINERIGHT:  m_LeftFrame++; break;
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK: m_LeftFrame = pos; break;
  }
  if (m_LeftFrame > x_num_frames - page_width)
    m_LeftFrame = x_num_frames - page_width;
  if (m_LeftFrame < 0)
    m_LeftFrame = 0;
  UpdateScrollBars();
  Invalidate();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetView::OnVScroll(UINT code, UINT pos, CScrollBar* scroll_bar)
{
  RECT client_rect;
  GetClientRect(&client_rect);
  int page_width = client_rect.bottom / m_MaxFrameHeight;
  int y_num_frames = m_Spriteset->GetNumDirections();
  switch (code)
  {
    case SB_TOP:        m_TopDirection = 0; break;
    case SB_BOTTOM:     m_TopDirection = y_num_frames - page_width; break;
    case SB_PAGEUP:     m_TopDirection -= page_width; break;
    case SB_PAGEDOWN:   m_TopDirection += page_width; break;
    case SB_LINEUP:     m_TopDirection--; break;
    case SB_LINEDOWN:   m_TopDirection++; break;
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK: m_TopDirection = pos; break;
  }
  if (m_TopDirection > y_num_frames - page_width)
    m_TopDirection = y_num_frames - page_width;
  if (m_TopDirection < 0)
    m_TopDirection = 0;
  UpdateScrollBars();
  Invalidate();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetView::OnPaint()
{
  CPaintDC dc_(this);
  HDC dc = dc_.m_hDC;
  RECT rect;
  GetClientRect(&rect);
  for (int i = 0; i < rect.bottom / m_MaxFrameHeight + 1; i++)
  {
    if (m_TopDirection + i < m_Spriteset->GetNumDirections())
      DrawDirection(dc_, m_TopDirection + i, i * m_MaxFrameHeight);
    else
    {
      RECT r;
      r.left = 0;
      r.top = i * m_MaxFrameHeight;
      r.right = rect.right;
      r.bottom = i * m_MaxFrameHeight + m_MaxFrameHeight;
      if (RectVisible(dc, &r))
        FillRect(dc, &r, (HBRUSH)GetStockObject(BLACK_BRUSH));
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetView::OnInsertDirection()
{
  m_Spriteset->InsertDirection(m_CurrentDirection);
  if (m_CurrentDirection > m_Spriteset->GetNumDirections() - 1)
    m_CurrentDirection = m_Spriteset->GetNumDirections() - 1;
  UpdateMaxSizes();
  UpdateScrollBars();
  Invalidate();
  m_Handler->SV_CurrentFrameChanged(m_CurrentDirection, m_CurrentFrame);
  m_Handler->SV_SpritesetModified();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetView::OnDeleteDirection()
{
  m_Spriteset->DeleteDirection(m_CurrentDirection);
  if (m_CurrentDirection > m_Spriteset->GetNumDirections() - 1)
    m_CurrentDirection = m_Spriteset->GetNumDirections() - 1;
  UpdateMaxSizes();
  UpdateScrollBars();
  Invalidate();
  m_Handler->SV_CurrentFrameChanged(m_CurrentDirection, m_CurrentFrame);
  m_Handler->SV_SpritesetModified();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetView::OnAppendDirection()
{
  m_Spriteset->InsertDirection(m_Spriteset->GetNumDirections());
  UpdateMaxSizes();
  UpdateScrollBars();
  Invalidate();
  m_Handler->SV_CurrentFrameChanged(m_CurrentDirection, m_CurrentFrame);
  m_Handler->SV_SpritesetModified();
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetView::CopyDirection(bool flip_horizontally, bool flip_vertically)
{
  int old_current_direction = m_CurrentDirection;
  int new_current_direction = m_CurrentDirection + 1;
  int num_directions = m_Spriteset->GetNumDirections() + 1;

  m_Spriteset->InsertDirection(new_current_direction);
  if (m_Spriteset->GetNumDirections() != num_directions)
    return;
  std::string direction_name = m_Spriteset->GetDirectionName(old_current_direction);
  if (flip_horizontally) {
    int pos = direction_name.find("north");
    if (pos != -1)
      direction_name.replace(pos, strlen("north"), "south");
    else
    {
      pos = direction_name.find("south");
      if (pos != -1)
        direction_name.replace(pos, strlen("south"), "north");
      else
      {
        pos = direction_name.find("up");
        if (pos != -1)
          direction_name.replace(pos, strlen("up"), "down");
        else
        {
          pos = direction_name.find("down");
          if (pos != -1)
            direction_name.replace(pos, strlen("down"), "up");
        }
      }
    }
  }
  if (flip_horizontally) {
    int pos = direction_name.find("east");
    if (pos != -1)
      direction_name.replace(pos, strlen("east"), "west");
    else
    {
      pos = direction_name.find("west");
      if (pos != -1)
        direction_name.replace(pos, strlen("west"), "east");
      else
      {
        pos = direction_name.find("left");
        if (pos != -1)
          direction_name.replace(pos, strlen("left"), "right");
        else
        {
          pos = direction_name.find("right");
          if (pos != -1)
            direction_name.replace(pos, strlen("right"), "left");
        }
      }
    }
  }
  m_Spriteset->SetDirectionName(new_current_direction, direction_name.c_str());
  for (int i = 0; i < m_Spriteset->GetNumFrames(old_current_direction); i++)
  {
    int image_index = m_Spriteset->GetFrameIndex(old_current_direction, i);
    if (flip_horizontally || flip_vertically) {
      CImage32 image = m_Spriteset->GetImage(image_index);
      image_index = -1;
      if (flip_horizontally)
        image.FlipHorizontal();
      if (flip_vertically)
        image.FlipVertical();
      for (int i = 0; i < m_Spriteset->GetNumImages(); i++) {
        if (m_Spriteset->GetImage(i) == image) {
          image_index = i;
          break;
        }
      }
      if (image_index == -1) {
        image_index = m_Spriteset->GetNumImages();
        m_Spriteset->InsertImage(image_index);
        if (image_index != m_Spriteset->GetNumImages() - 1)
          return;
        m_Spriteset->GetImage(image_index) = image;
      }
    }
    int num_frames = m_Spriteset->GetNumFrames(new_current_direction) + 1;
    m_Spriteset->InsertFrame(new_current_direction, i);
    if (num_frames != m_Spriteset->GetNumFrames(new_current_direction))
      return;
    
    m_Spriteset->SetFrameIndex(new_current_direction, i, image_index);
  }
  if (m_Spriteset->GetNumFrames(new_current_direction) > 0)
    m_Spriteset->DeleteFrame(new_current_direction, m_Spriteset->GetNumFrames(new_current_direction) - 1);
  UpdateMaxSizes();
  UpdateScrollBars();
  Invalidate();
  m_Handler->SV_CurrentFrameChanged(m_CurrentDirection, m_CurrentFrame);
  m_Handler->SV_SpritesetModified();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetView::OnInsertCopyFlipHorizontally()
{
  CopyDirection(true, false);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetView::OnInsertCopyFlipVertically()
{
  CopyDirection(false, true);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetView::OnDirectionProperties()
{
  CStringDialog dialog(
    "Direction Name",
    m_Spriteset->GetDirectionName(m_CurrentDirection)
  );
  if (dialog.DoModal() == IDOK) {
    m_Spriteset->SetDirectionName(m_CurrentDirection, dialog.GetValue());
    Invalidate();
    m_Handler->SV_SpritesetModified();
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetView::OnInsertFrame()
{
  m_Spriteset->InsertFrame(m_CurrentDirection, m_CurrentFrame);
  UpdateMaxSizes();
  UpdateScrollBars();
  Invalidate();
  m_Handler->SV_SpritesetModified();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetView::OnDeleteFrame()
{
  m_Spriteset->DeleteFrame(m_CurrentDirection, m_CurrentFrame);
  if (m_CurrentFrame > m_Spriteset->GetNumFrames(m_CurrentDirection) - 1)
    m_CurrentFrame = m_Spriteset->GetNumFrames(m_CurrentDirection) - 1;
  UpdateMaxSizes();
  UpdateScrollBars();
  Invalidate();
  m_Handler->SV_CurrentFrameChanged(m_CurrentDirection, m_CurrentFrame);
  m_Handler->SV_SpritesetModified();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetView::OnInsertFrames()
{
  CNumberDialog dialog("Insert Frames", "Number of Frames", 1, 1, 4096);
  if (dialog.DoModal() == IDOK)
  {
    int num_frames = dialog.GetValue();
    while (num_frames-- > 0)
      m_Spriteset->InsertFrame(m_CurrentDirection, m_CurrentFrame);
    
    UpdateMaxSizes();
    UpdateScrollBars();
    Invalidate();
    m_Handler->SV_SpritesetModified();
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetView::OnAppendFrames()
{
  CNumberDialog dialog("Append Frames", "Number of Frames", 1, 1, 4096);
  if (dialog.DoModal() == IDOK)
  {
    int num_frames = dialog.GetValue();
    while (num_frames-- > 0)
      m_Spriteset->InsertFrame(m_CurrentDirection, m_Spriteset->GetNumFrames(m_CurrentDirection));
    
    UpdateMaxSizes();
    UpdateScrollBars();
    Invalidate();
    m_Handler->SV_SpritesetModified();
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetView::OnRemoveFrames()
{
  CNumberDialog dialog("Delete Frames", "Number of Frames", 1, 1, m_Spriteset->GetNumFrames(m_CurrentDirection) - m_CurrentFrame - 1);
  if (dialog.DoModal() == IDOK)
  {
    int num_images = dialog.GetValue();
    while (num_images-- > 0)
      OnDeleteFrame();
    
    // make sure selected tile is still valid
    if (m_CurrentFrame >= m_Spriteset->GetNumFrames(m_CurrentDirection))
      m_CurrentFrame = m_Spriteset->GetNumFrames(m_CurrentDirection) - 1;
    
    UpdateMaxSizes();
    UpdateScrollBars();
    Invalidate();
    m_Handler->SV_CurrentFrameChanged(m_CurrentDirection, m_CurrentFrame);
    m_Handler->SV_SpritesetModified();
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetView::OnAppendFrame()
{
  m_Spriteset->InsertFrame(m_CurrentDirection, m_Spriteset->GetNumFrames(m_CurrentDirection));
  UpdateMaxSizes();
  UpdateScrollBars();
  Invalidate();
  m_Handler->SV_SpritesetModified();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetView::OnCopyFrame()
{
  m_Handler->SV_CopyCurrentFrame();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetView::OnPasteFrame()
{
  m_Handler->SV_PasteCurrentFrame();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetView::OnFillDelay()
{
  CNumberDialog dialog("Spriteset Delay", "Delay", 8, 1, 4096);
  if (dialog.DoModal() == IDOK) {
    bool modified = false;
    if (m_CurrentDirection >= 0 && m_CurrentDirection < m_Spriteset->GetNumDirections()) {
      for (int j = 0; j < m_Spriteset->GetNumFrames(m_CurrentDirection); j++) {
        if (m_Spriteset->GetFrameDelay(m_CurrentDirection, j) != dialog.GetValue())
          modified = true;
        m_Spriteset->SetFrameDelay(m_CurrentDirection, j, dialog.GetValue());
      }
    }
    if (modified)
      m_Handler->SV_SpritesetModified();
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetView::OnFrameProperties()
{
  CSpritePropertiesDialog dialog(m_Spriteset, m_CurrentDirection, m_CurrentFrame);
  if (dialog.DoModal() == IDOK)
    m_Handler->SV_SpritesetModified();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetView::OnImportFrameFromImage()
{
  if (!(m_CurrentDirection >= 0 && m_CurrentDirection < m_Spriteset->GetNumDirections()))
    return;
  if (!(m_CurrentFrame >= 0 && m_CurrentFrame < m_Spriteset->GetNumFrames(m_CurrentDirection)))
    return;

  CImageFileDialog dialog(FDM_OPEN | FDM_MULTISELECT, "Insert Frame From Image");
  if (dialog.DoModal() != IDOK)
    return;
  POSITION pos = dialog.GetStartPosition();
  int current_frame = m_CurrentFrame;
  while (pos != NULL)
  {
    CString path_name = dialog.GetNextPathName(pos);
    CImage32 image;
    if ( !image.Load( path_name ) ) {
      MessageBox("Error loading image.\n'" + path_name + "'");
      return;
    }
    int frame_width = m_Spriteset->GetFrameWidth();
    int frame_height = m_Spriteset->GetFrameHeight();
    if (image.GetWidth() != frame_width
        || image.GetHeight() != frame_height) {
      MessageBox("Image and frame sizes do not match.");
      return;
    }
    int image_index = -1;
    for (int i = 0; i < m_Spriteset->GetNumImages(); i++) {
      if (m_Spriteset->GetImage(i) == image) {
        image_index = i;
        break;
      }
    }
    if (image_index == -1) {
      image_index = m_Spriteset->GetNumImages();
      m_Spriteset->InsertImage(image_index);
      if (image_index != m_Spriteset->GetNumImages() - 1)
        return;
      m_Spriteset->GetImage(image_index) = image;
    }
    int num_frames = m_Spriteset->GetNumFrames(m_CurrentDirection);
    m_Spriteset->InsertFrame(m_CurrentDirection, current_frame);
    if (num_frames != m_Spriteset->GetNumFrames(m_CurrentDirection) - 1)
      return;
    m_Spriteset->SetFrameIndex(m_CurrentDirection, current_frame, image_index);
    //MessageBox("Imported frame successfully");
    current_frame += 1;
  }
  UpdateMaxSizes();
  UpdateScrollBars();
  Invalidate();
  m_Handler->SV_SpritesetModified();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetView::OnExportFrameToImage()
{
  if (!(m_CurrentDirection >= 0 && m_CurrentDirection < m_Spriteset->GetNumDirections()))
    return;
  if (!(m_CurrentFrame >= 0 && m_CurrentFrame < m_Spriteset->GetNumFrames(m_CurrentDirection)))
    return;

  CImageFileDialog dialog(FDM_SAVE, "Export Frame To Image");
  if (dialog.DoModal() == IDOK) {
    CImage32& i = m_Spriteset->GetImage(m_Spriteset->GetFrameIndex(m_CurrentDirection, m_CurrentFrame));
    if (i.Save(dialog.GetPathName()))
      MessageBox("Exported frame succesfully");
    else
      MessageBox("Error saving image");
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetView::OnInsertDirectionFromImage()
{
  CImageFileDialog dialog(FDM_OPEN | FDM_MULTISELECT, "Insert Direction From Image");
  if (dialog.DoModal() != IDOK)
    return;

  int frames_so_far = 0;
  int current_direction = m_CurrentDirection;
  POSITION pos = dialog.GetStartPosition();
  
  while (pos != NULL)
  {
    CString path_name = dialog.GetNextPathName(pos);
    const int frame_width = m_Spriteset->GetFrameWidth();
    const int frame_height = m_Spriteset->GetFrameHeight();
    CImage32 image;
    if ( !image.Load( path_name ) ) {
      MessageBox("Error loading image.\n'" + path_name + "'");
      return;
    }
    if (image.GetWidth() % frame_width > 0
        || image.GetHeight() % frame_height > 0) {
      MessageBox("Invalid image width or height.\n'" + path_name + "'");
      return;
    }
    if (frames_so_far > 0 && true) {
      if (frames_so_far > 0) {
        current_direction += 1;
        frames_so_far = 0;
      }
    }
    int num_frames = image.GetWidth() / frame_width;
    int old_current_frame = m_CurrentFrame + frames_so_far;
    int x = 0;
    int y = 0;
    if (frames_so_far == 0)
      m_Spriteset->InsertDirection(current_direction);
    
    for (int i = 0; i < num_frames; i++) {
      int current_frame = old_current_frame + i;
      int current_image = -1;
      for (int i = 0; i < m_Spriteset->GetNumImages(); i++) {
        const CImage32& img = m_Spriteset->GetImage(i);
        if (image == img) {
          current_image = i;
          break;
        }
      }
      if (current_image == -1) {
        current_image = m_Spriteset->GetNumImages();
        m_Spriteset->InsertImage(current_image);
        if (m_Spriteset->GetNumImages() != current_image + 1)
          return;
        m_Spriteset->GetImage(current_image) = image;
      }
      if (current_image < m_Spriteset->GetNumImages()) {
        CImage32& frame = m_Spriteset->GetImage(current_image);
        // having to set the blend mode seems weird to me
        frame.SetBlendMode(CImage32::REPLACE);  // Formerly CImage32::BlendMode::REPLACE
        for (int sy = 0; sy < frame.GetHeight(); sy++)
          for (int sx = 0; sx < frame.GetWidth(); sx++)
            frame.SetPixel(sx, sy, image.GetPixel(x + sx, y + sy));
        x += frame.GetWidth();
        if (current_direction < m_Spriteset->GetNumDirections())
        {
          int __num_frames__ = m_Spriteset->GetNumFrames(current_direction) + 1;
          m_Spriteset->InsertFrame(current_direction, current_frame);
          if (__num_frames__ == m_Spriteset->GetNumFrames(current_direction))
            m_Spriteset->SetFrameIndex(current_direction, current_frame, current_image);
        }
      }
    }
    if (frames_so_far == 0 && m_Spriteset->GetNumFrames(current_direction) > 0)
      m_Spriteset->DeleteFrame(current_direction, m_Spriteset->GetNumFrames(current_direction) - 1);
    frames_so_far += num_frames;
  }
  UpdateMaxSizes();
  UpdateScrollBars();
  Invalidate();
  m_Handler->SV_SpritesetModified();
  //MessageBox("Imported direction successfully");
}
///////////////////////////////////////////////////////////////////////////////
#include "../common/AnimationFactory.hpp"
afx_msg void
CSpritesetView::OnInsertDirectionFromAnimation()
{
  CAnimationFileDialog dialog(FDM_OPEN, "Insert Direction From Animation");
  if (dialog.DoModal() == IDOK) {
    int frame_width = m_Spriteset->GetFrameWidth();
    int frame_height = m_Spriteset->GetFrameHeight();
    IAnimation* animation = LoadAnimation(dialog.GetPathName());
    if ( !animation ) {
      MessageBox("Error loading animation.");
      return;
    }
    /*
    if (animation->GetWidth() != frame_width
     || animation->GetHeight() != frame_height) {
      char message[1000];
      sprintf(message, "Invalid animation width or height: %d %d",
        animation->GetWidth(), animation->GetHeight());
      MessageBox(message);
      return;
    }
    */
    int current_direction = m_CurrentDirection;
    m_Spriteset->InsertDirection(current_direction);
    m_Spriteset->SetDirectionName(current_direction, dialog.GetFileName());
    int max_frames = animation->GetNumFrames() == 0 ? 255 : animation->GetNumFrames();
    for (int frame_number = 0; frame_number < max_frames; frame_number++) {
      if (animation->GetWidth()  < 0 || animation->GetWidth() > 4096
          || animation->GetHeight() < 0 || animation->GetHeight() > 4096)
        break;
      if (animation->IsEndOfAnimation())
        break;
      
      RGBA* pixels = new RGBA[animation->GetWidth() * animation->GetHeight()];
      if ( !pixels )
        return;
      if (animation->ReadNextFrame((RGBA*) pixels) == false) {
        delete[] pixels;
        break;
      }
      CImage32 image;
      if (!image.Create(m_Spriteset->GetFrameWidth(), m_Spriteset->GetFrameHeight())) {
        delete[] pixels;
        break;
      }
      for (int y = 0; (y < image.GetHeight() && y < animation->GetHeight()); y++)
        for (int x = 0; (x < image.GetWidth() && x < animation->GetWidth()); x++)
          image.SetPixel(x, y, pixels[y * animation->GetWidth() + x]);
      delete[] pixels;
      int current_image = -1;
      for (int i = 0; i < m_Spriteset->GetNumImages(); i++) {
        const CImage32& img = m_Spriteset->GetImage(i);
        if (image == img) {
          current_image = i;
          break;
        }
      }
      if (current_image == -1) {
        current_image = m_Spriteset->GetNumImages();
        m_Spriteset->InsertImage(current_image);
        if (m_Spriteset->GetNumImages() != current_image + 1)
          return;
        m_Spriteset->GetImage(current_image) = image;
      }
      int __num_frames__ = m_Spriteset->GetNumFrames(current_direction) + 1;
      m_Spriteset->InsertFrame(current_direction, frame_number);
      if (m_Spriteset->GetNumFrames(current_direction) == __num_frames__) {
        int delay = animation->GetDelay();
        if (delay)
          m_Spriteset->SetFrameDelay(current_direction, frame_number, delay / 10);
        m_Spriteset->SetFrameIndex(current_direction, frame_number, current_image);
      }
    }
    if (m_Spriteset->GetNumFrames(current_direction) > 0)
      m_Spriteset->DeleteFrame(current_direction, m_Spriteset->GetNumFrames(current_direction) - 1);
  }
  UpdateMaxSizes();
  UpdateScrollBars();
  Invalidate();
  m_Handler->SV_SpritesetModified();
}

///////////////////////////////////////////////////////////////////////////////

#include "../common/write_mng.hpp"

///////////////////////////////////////////////////////////////////////////////

struct userwritedata {
  sSpriteset spriteset;
  int direction;
};

mng_bool MNG_DECL GetImageFromSpriteset(int index, CImage32& image, void* data) {
  userwritedata* s = (userwritedata*) data;
  if (index < 0 || index >= s->spriteset.GetNumFrames(s->direction))
    return MNG_FALSE;
  image = s->spriteset.GetImage(s->spriteset.GetFrameIndex(s->direction, index));
  return MNG_TRUE;
}

mng_uint32 MNG_DECL GetDelayFromSpriteset(int index, void* data) {
  userwritedata* s = (userwritedata*) data;
  return 10 * s->spriteset.GetFrameDelay(s->direction, index);
  // spritesets use a frame rate and this is converting it into a sort of hashed time based system
}

///////////////////////////////////////////////////////////////////////////////
static mng_bool MNG_DECL ContinueProcessing(int index, int total)
{
  mng_bool ret = MNG_TRUE;
  char string[255] = {0};
  if (index == -1) {
    sprintf (string, "%d...", total);
  } else {
    int percent = (int) ( ((double)index / (double)total) * 100 );
    sprintf (string, "%3d%% Complete", percent);
  }
  GetStatusBar()->SetWindowText(string);
  /*
  MSG msg;
  int count = 0;
  while (count++ < 4 && PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
  {
    if (msg.message != WM_QUIT) {
      DispatchMessage(&msg);
    }
    else {
      ret = false;
      break;
    }
  }
  */
  return ret;
}

///////////////////////////////////////////////////////////////////////////////

#if 0
#include "../common/system.hpp"
mng_retcode TestAnimationCode() {
  std::vector<std::string> filelist = GetFileList("*");
  std::vector<std::string> imagefilelist;
  for (int i = 0; i < filelist.size(); i++) {
    //CImage32 image;
    const char* filename = filelist[i].c_str();
    if (filelist[i].rfind(".jpg") == strlen(filename) - 4) {
      imagefilelist.push_back(filename);
    }
    else {
    //if (image.Load(filename)) {
//      imagefilelist.push_back(filename);
    //}
    }
  }
  return SaveMNGAnimationFromImages("comics.mng", GetNextImageFromFileList, GetDelayFromImageFileList, ContinueProcessing, (void*) &imagefilelist);
}
#endif

///////////////////////////////////////////////////////////////////////////////

afx_msg void
CSpritesetView::OnExportDirectionAsAnimation()
{
  //SetCurrentDirectory("c:\\windows\\desktop\\8bit");
  //mng_retcode ret = TestAnimationCode();
  //MessageBox(mng_get_error_message(ret), "Export Comics As Animation", MB_OK);
  //return;
  CAnimationFileDialog dialog(FDM_SAVE, "Export Direction As Animation");
  if (dialog.DoModal() == IDOK) {
    bool is_mng = strcmp_ci(dialog.GetFileExt(), "mng") == 0;
    bool is_fli = strcmp_ci(dialog.GetFileExt(), "flic") == 0
        || strcmp_ci(dialog.GetFileExt(), "flc")  == 0
        || strcmp_ci(dialog.GetFileExt(), "fli")  == 0;
    if (is_mng) {
      struct userwritedata data;
      data.spriteset = *m_Spriteset; 
      data.direction = m_CurrentDirection;
      mng_retcode iRC = SaveMNGAnimationFromImages(dialog.GetPathName(), GetImageFromSpriteset, GetDelayFromSpriteset, ContinueProcessing, (void*) &data);
      if (iRC == 0)
        MessageBox("Exported Animation!", "Export Direction As Animation", MB_OK);
      else
        MessageBox(mng_get_error_message(iRC), "Error Exporting Direction As Animation", MB_OK);
    } else {
      MessageBox("Unsupported save mode", "Error Exporting Direction As Animation", MB_OK);
    }
  }
}

///////////////////////////////////////////////////////////////////////////////

afx_msg void
CSpritesetView::OnExportDirectionAsImage()
{
  // get file name to export to
  CImageFileDialog dialog(FDM_SAVE, "Export Direction As Image");
  if (dialog.DoModal() == IDOK) {
    int image_width = 0;
    int image_height = 0;
    int i;

    for (i = 0; i < m_Spriteset->GetNumFrames(m_CurrentDirection); i++) {
      int height = m_Spriteset->GetImage(m_Spriteset->GetFrameIndex(m_CurrentDirection, i)).GetHeight();
      image_height = image_height > height ? image_height : height;
      image_width  += m_Spriteset->GetImage(m_Spriteset->GetFrameIndex(m_CurrentDirection, i)).GetWidth();
    }
    if (image_width <= 0 || image_height <= 0) {
      MessageBox("Nothing to export!", "Export Direction As Image", MB_OK);
      return;
    }
    CImage32 image(image_width, image_height);
    image.SetBlendMode(CImage32::REPLACE); // Formerly CImage32::BlendMode::REPLACE
    int x = 0;
    int y = 0;
    for (i = 0; i < m_Spriteset->GetNumFrames(m_CurrentDirection); ++i) {
      CImage32& frame = m_Spriteset->GetImage(m_Spriteset->GetFrameIndex(m_CurrentDirection, i));
      image.BlitImage(frame, x, y);
      x += frame.GetWidth();
    }
    if( !image.Save(dialog.GetPathName()) )
      MessageBox("Could not save image", "Export Direction As Image", MB_OK);
    else
      MessageBox("Exported direction!", "Export Direction As Image", MB_OK);
  }
}

///////////////////////////////////////////////////////////////////////////////

afx_msg void
CSpritesetView::OnEditRange()
{
  const unsigned int id = GetCurrentMessage()->wParam;
  if( EditRange::OnEditRange("spriteset", id, false, (void*) m_Spriteset, m_CurrentDirection) ) {
    m_Handler->SV_SpritesetModified();
    Invalidate();
  }
}

////////////////////////////////////////////////////////////////////////////////

BOOL CSpritesetView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	{
		// use document specific accelerator table over m_hAccelTable
		HACCEL hAccel = ((CFrameWnd*)AfxGetMainWnd())->GetDefaultAccelerator();
		return hAccel != NULL &&
		   ::TranslateAccelerator(m_hWnd, hAccel, pMsg);
	}
	return CWnd::PreTranslateMessage(pMsg);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg LRESULT
CSpritesetView::OnGetAccelerator(WPARAM wParam, LPARAM lParam)
{
	// If you want to provide a custom accelerator, copy from CImageView::OnGetAccelerator!
	// Also a message map entry is needed like:
	//		ON_MESSAGE(WM_GETACCELERATOR, OnGetAccelerator)
	HACCEL* ret = ((HACCEL*)wParam);
	*ret = LoadAccelerators(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_SPRITESETVIEW));
	return 1;
}

////////////////////////////////////////////////////////////////////////////////
