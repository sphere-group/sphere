#include "SpriteBaseView.hpp"
#include "../common/minmax.hpp"
#include "Editor.hpp"
static int s_ViewID = 450;
BEGIN_MESSAGE_MAP(CSpriteBaseView, CWnd)
  ON_WM_SIZE()  
  ON_WM_PAINT()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////
CSpriteBaseView::CSpriteBaseView()
: m_Handler(NULL)
, m_Spriteset(NULL)
, m_Sprite(NULL)
, m_MouseDown(false)
{
}
////////////////////////////////////////////////////////////////////////////////
CSpriteBaseView::~CSpriteBaseView()
{
  DestroyWindow();
}
////////////////////////////////////////////////////////////////////////////////
BOOL
CSpriteBaseView::Create(CWnd* parent, ISpriteBaseViewHandler* handler, sSpriteset* spriteset)
{
  m_Handler = handler;
  m_Spriteset = spriteset;
  return CWnd::Create(
    AfxRegisterWndClass(0, LoadCursor(NULL, IDC_ARROW), NULL, NULL),
    "",
    WS_CHILD | WS_VISIBLE,
    CRect(0, 0, 0, 0),
    parent,
    s_ViewID++);
}
////////////////////////////////////////////////////////////////////////////////
void 
CSpriteBaseView::UpdateStatusBar()
{
  int x1, y1, x2, y2;
  m_Spriteset->GetBase(x1, y1, x2, y2);
  int w = x1 > x2 ? x1 - x2 : x2 - x1;
  int h = y1 > y2 ? y1 - y2 : y2 - y1;
  char text[1024];
  sprintf (text, "Base: (x1: %d y1: %d x2: %d y2: %d) w: %d h: %d", x1, y1, x2, y2, w, h);
  GetStatusBar()->SetWindowText(text);
}
////////////////////////////////////////////////////////////////////////////////
void
CSpriteBaseView::SetSprite(const CImage32* sprite)
{
  m_Sprite = sprite;
  Invalidate();
}
////////////////////////////////////////////////////////////////////////////////
void
CSpriteBaseView::BeginDrag(int x, int y)
{
  WindowToImage(&x, &y);
  m_Spriteset->SetBase(x, y, x, y);
  m_Handler->SBV_SpritesetModified();
  UpdateStatusBar();
  Invalidate();
}
////////////////////////////////////////////////////////////////////////////////
void
CSpriteBaseView::Drag(int x, int y)
{
  WindowToImage(&x, &y);
  int x1, y1, x2, y2;
  m_Spriteset->GetBase(x1, y1, x2, y2);
  m_Spriteset->SetBase(x1, y1, x, y);
  m_Spriteset->Base2Real();
  m_Handler->SBV_SpritesetModified();
  UpdateStatusBar();
  Invalidate();
}
////////////////////////////////////////////////////////////////////////////////
void
CSpriteBaseView::WindowToImage(int* x, int* y)
{
  // get client rectangle
  RECT ClientRect;
  GetClientRect(&ClientRect);
  // calculate size of pixel squares
  int hsize = ClientRect.right / m_Sprite->GetWidth();
  int vsize = ClientRect.bottom / m_Sprite->GetHeight();
  int size = std::min(hsize, vsize);
  if (size < 1)
    size = 1;
  int totalx = size * m_Sprite->GetWidth();
  int totaly = size * m_Sprite->GetHeight();
  int offsetx = (ClientRect.right - totalx) / 2;
  int offsety = (ClientRect.bottom - totaly) / 2;
  *x -= offsetx;
  *y -= offsety;
  *x /= size;
  *y /= size;
  // bounds check
  if (*x < 0)
    *x = 0;
  if (*y < 0)
    *y = 0;
  if (*x > m_Sprite->GetWidth() - 1)
    *x = m_Sprite->GetWidth() - 1;
  if (*y > m_Sprite->GetHeight() - 1)
    *y = m_Sprite->GetHeight() - 1;
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpriteBaseView::OnSize(UINT type, int cx, int cy)
{
  Invalidate();
}
////////////////////////////////////////////////////////////////////////////////
// inclusive
inline bool between(int i, int min, int max)
{
  return ((i >= min && i <= max) ||
          (i <= min && i >= max));
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpriteBaseView::OnPaint()
{
  CPaintDC dc_(this);
  HDC dc = dc_.m_hDC;
  int width  = m_Sprite->GetWidth();
  int height = m_Sprite->GetHeight();
  const RGBA* pixels = m_Sprite->GetPixels();
  // get client rectangle
  RECT ClientRect;
  GetClientRect(&ClientRect);
  // calculate size of pixel squares
  int hsize = ClientRect.right / width;
  int vsize = ClientRect.bottom / height;
  int size = std::min(hsize, vsize);
  if (size < 1)
    size = 1;
  int totalx = size * width;
  int totaly = size * height;
  int offsetx = (ClientRect.right - totalx) / 2;
  int offsety = (ClientRect.bottom - totaly) / 2;
  // draw black rectangles in the empty parts
  HBRUSH black_brush = (HBRUSH)GetStockObject(BLACK_BRUSH);
  RECT Rect;
  // top
  SetRect(&Rect, 0, 0, ClientRect.right, offsety - 1);
  FillRect(dc, &Rect, black_brush);
  // bottom
  SetRect(&Rect, 0, offsety + totaly + 1, ClientRect.right, ClientRect.bottom);
  FillRect(dc, &Rect, black_brush);
  // left
  SetRect(&Rect, 0, offsety - 1, offsetx - 1, offsety + totaly + 1);
  FillRect(dc, &Rect, black_brush);
  // right
  SetRect(&Rect, offsetx + totalx + 1, offsety - 1, ClientRect.right, offsety + totaly + 1);
  FillRect(dc, &Rect, black_brush);
  // get the base rectangle
  int base_x1;
  int base_y1;
  int base_x2;
  int base_y2;
  m_Spriteset->GetBase(base_x1, base_y1, base_x2, base_y2);
  // draw the image
  for (int ix = 0; ix < width; ix++)
    for (int iy = 0; iy < height; iy++)
    {
      RGBA color = pixels[iy * width + ix];
      bool onbaseborder = false;
      if (iy == base_y1 || iy == base_y2)
        if (between(ix, base_x1, base_x2))
          onbaseborder = true;
      if (ix == base_x1 || ix == base_x2)
        if (between(iy, base_y1, base_y2))
          onbaseborder = true;
      // opaque
      if (color.alpha == 255 || onbaseborder)
      {
        HBRUSH brush;
        if (onbaseborder)
          brush = CreateSolidBrush(0xFF00FF);
        else
          brush = CreateSolidBrush(RGB(color.red, color.green, color.blue));
        RECT Rect = { ix * size, iy * size, ix * size + size, iy * size + size };
        OffsetRect(&Rect, offsetx, offsety);
        FillRect(dc, &Rect, brush);
        DeleteObject(brush);
      }
      // translucent
      else
      {
        // calculate background grid colors
        RGB Color1 = CreateRGB(255, 255, 255);
        RGB Color2 = CreateRGB(128, 128, 128);
        Color1.red   = (color.red   * color.alpha + Color1.red   * (256 - color.alpha)) / 256;
        Color1.green = (color.green * color.alpha + Color1.green * (256 - color.alpha)) / 256;
        Color1.blue  = (color.blue  * color.alpha + Color1.blue  * (256 - color.alpha)) / 256;
        Color2.red   = (color.red   * color.alpha + Color2.red   * (256 - color.alpha)) / 256;
        Color2.green = (color.green * color.alpha + Color2.green * (256 - color.alpha)) / 256;
        Color2.blue  = (color.blue  * color.alpha + Color2.blue  * (256 - color.alpha)) / 256;
        HBRUSH Brush1 = CreateSolidBrush(RGB(Color1.red, Color1.green, Color1.blue));
        HBRUSH Brush2 = CreateSolidBrush(RGB(Color2.red, Color2.green, Color2.blue));
        RECT Rect;
        // draw rectangles
        // upper left
        SetRect(&Rect,
          ix * size,
          iy * size,
          ix * size + size / 2,
          iy * size + size / 2);
        OffsetRect(&Rect, offsetx, offsety);
        FillRect(dc, &Rect, Brush1);
        // upper right
        SetRect(&Rect,
          ix * size + size / 2,
          iy * size,
          ix * size + size,
          iy * size + size / 2);
        OffsetRect(&Rect, offsetx, offsety);
        FillRect(dc, &Rect, Brush2);
        // lower left
        SetRect(&Rect,
          ix * size,
          iy * size + size / 2,
          ix * size + size / 2,
          iy * size + size);
        OffsetRect(&Rect, offsetx, offsety);
        FillRect(dc, &Rect, Brush2);
        // lower right
        SetRect(&Rect,
          ix * size + size / 2,
          iy * size + size / 2,
          ix * size + size,
          iy * size + size);
        OffsetRect(&Rect, offsetx, offsety);
        FillRect(dc, &Rect, Brush1);
       
        DeleteObject(Brush1);
        DeleteObject(Brush2);
      }
    }
  // draw a white rectangle around the image
  SetRect(&Rect, offsetx - 1, offsety - 1, offsetx + totalx + 1, offsety + totaly + 1);
  HPEN   white_pen = CreatePen(PS_SOLID, 1, RGB(0xFF, 0xFF, 0xFF));
  HBRUSH old_brush = (HBRUSH)SelectObject(dc, GetStockObject(NULL_BRUSH));
  HPEN   old_pen   = (HPEN)  SelectObject(dc, white_pen);
  
  Rectangle(dc, Rect.left, Rect.top, Rect.right, Rect.bottom);
  SelectObject(dc, old_pen);
  SelectObject(dc, old_brush);
  DeleteObject(white_pen);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpriteBaseView::OnLButtonDown(UINT flags, CPoint point)
{
  BeginDrag(point.x, point.y);
  m_MouseDown = true;
  SetCapture();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpriteBaseView::OnLButtonUp(UINT flags, CPoint point)
{
  Drag(point.x, point.y);
  m_MouseDown = false;
  ReleaseCapture();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpriteBaseView::OnMouseMove(UINT flags, CPoint point)
{
  if (!m_MouseDown)
    return;
  Drag(point.x, point.y);
}
////////////////////////////////////////////////////////////////////////////////
