#include "PaletteView.hpp"
#include "DIBSection.hpp"
#include "../common/rgb.hpp"
#include "Editor.hpp"
// 2004-06-24 //
static int s_iPaletteViewID = 1000;
BEGIN_MESSAGE_MAP(CPaletteView, CWnd)
  ON_WM_PAINT()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_RBUTTONDOWN()
  ON_WM_RBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_WM_SIZE()
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////
CPaletteView::CPaletteView()
: m_pPaletteDIB(NULL)
{
  m_Colors[0] = CreateRGB(255, 255, 255);
  m_Colors[1] = CreateRGB(0, 0, 0);
  m_bMouseDown[0] = m_bMouseDown[1] = false;
}
////////////////////////////////////////////////////////////////////////////////
CPaletteView::~CPaletteView()
{
  delete m_pPaletteDIB;
  m_pPaletteDIB = NULL;
  DestroyWindow();
}
////////////////////////////////////////////////////////////////////////////////
BOOL
CPaletteView::Create(IPaletteViewHandler* pHandler, CWnd* pParentWindow)
{
  m_pHandler = pHandler;
  return CWnd::Create(
    AfxRegisterWndClass(0, LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)), NULL, NULL),
    "PaletteView",
    WS_CHILD | WS_VISIBLE,
    CRect(0, 0, 0, 0),
    pParentWindow, s_iPaletteViewID++);
}
////////////////////////////////////////////////////////////////////////////////
RGB
CPaletteView::GetColor(int index) const
{
  return m_Colors[index];
}
////////////////////////////////////////////////////////////////////////////////
void
CPaletteView::UpdatePalette(int w, int h, int pitch, BGR* pixels)
{
  // fill it with the rainbow
  static const int NumColorSections = 4;
  static const BGR Rainbow[NumColorSections] =
  {
    { 0,   0,   255, }, // red
    { 0,   255, 255, }, // yellow
    { 255, 255, 0,   }, // green
    { 255, 0,   255, }, // blue
  };
  // calculate section boundaries
  int SectionBoundaries[NumColorSections + 1];
  for (int i = 0; i <= NumColorSections; i++)
    SectionBoundaries[i] = i * h / NumColorSections;
  // fill the DIB section with colors
  for (int iy = 0; iy < h; iy++)
  {
    // figure out which section (and weight!) we're in
    int section = 0;
    int weight  = 0; // from 0 to 255
    for (int i = NumColorSections - 1; i >= 0; i--)
    {
      if (iy >= SectionBoundaries[i])
      {
        section = i;
        int height = (SectionBoundaries[i + 1] - SectionBoundaries[i]);
        weight  = 256 * (iy - SectionBoundaries[i]) / height;
        break;
      }
    }
    BGR color1 = Rainbow[(section + 0) % NumColorSections];
    BGR color2 = Rainbow[(section + 1) % NumColorSections];
    // calculate average color
    BGR rgb;
    rgb.red   = (color1.red   * (255 - weight) + color2.red   * weight) / 256;
    rgb.green = (color1.green * (255 - weight) + color2.green * weight) / 256;
    rgb.blue  = (color1.blue  * (255 - weight) + color2.blue  * weight) / 256;
    const BGR color = rgb;
    for (int ix = 0; ix < w; ix++)
    {
      rgb = color;
      // now calculate luminosity (0..127 = darker, 128..255 = brighter)
      int luminosity = ix * 256 / w;
      if (luminosity < 128)  // average color with black
      {
        int weight = (127 - luminosity) * 255 / 127;
        rgb.red   = (rgb.red   * (255 - weight) + 0 * weight) / 256;
        rgb.green = (rgb.green * (255 - weight) + 0 * weight) / 256;
        rgb.blue  = (rgb.blue  * (255 - weight) + 0 * weight) / 256;
      }
      else                   // average color with white
      {
        int weight = (luminosity - 128) * 255 / 127;
        rgb.red   = (rgb.red   * (255 - weight) + 255 * weight) / 256;
        rgb.green = (rgb.green * (255 - weight) + 255 * weight) / 256;
        rgb.blue  = (rgb.blue  * (255 - weight) + 255 * weight) / 256;
      }
      // draw pixels
      pixels[iy * pitch + ix] = rgb;
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CPaletteView::OnPaint()
{
  CPaintDC dc(this);
  RECT ClientRect;
  GetClientRect(&ClientRect);
  if (m_pPaletteDIB && m_pPaletteDIB->GetPixels() != NULL)
  {
    dc.BitBlt(0, 0, ClientRect.right, ClientRect.bottom, CDC::FromHandle(m_pPaletteDIB->GetDC()), 0, 0, SRCCOPY);
  }
  else {
    dc.FillRect(&ClientRect, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
  }
}
////////////////////////////////////////////////////////////////////////////////
bool
CPaletteView::UpdateColor(int index, UINT flags, CPoint point)
{
  if (!m_pPaletteDIB || m_pPaletteDIB->GetPixels() == NULL) return false;
  // bounds check
  RECT ClientRect;
  GetClientRect(&ClientRect);
  if (point.x < 0 ||
      point.y < 0 ||
      point.x >= ClientRect.right ||
      point.y >= ClientRect.bottom)
    return false;
  // get color
  BGR* pixels = (BGR*)m_pPaletteDIB->GetPixels();
  BGR color = pixels[point.y * m_pPaletteDIB->GetWidth() + point.x];
  m_Colors[index].red   = color.red;
  m_Colors[index].green = color.green;
  m_Colors[index].blue  = color.blue;
  m_pHandler->PV_ColorChanged(index, m_Colors[index]);
  return true;
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CPaletteView::OnLButtonDown(UINT flags, CPoint point)
{
  if (UpdateColor(0, flags, point)) {
    m_bMouseDown[0] = true;
    SetCapture();
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CPaletteView::OnLButtonUp(UINT flags, CPoint point)
{
  m_bMouseDown[0] = false;
  ReleaseCapture();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CPaletteView::OnRButtonDown(UINT flags, CPoint point)
{
  if (UpdateColor(1, flags, point)) {
    m_bMouseDown[1] = true;
    SetCapture();
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CPaletteView::OnRButtonUp(UINT flags, CPoint point)
{
  m_bMouseDown[1] = false;
  ReleaseCapture();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CPaletteView::OnMouseMove(UINT flags, CPoint point)
{
  if (m_bMouseDown[0])
    UpdateColor(0, flags, point);
  if (m_bMouseDown[1])
    UpdateColor(1, flags, point);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CPaletteView::OnSize(UINT type, int cx, int cy)
{
  // lock the cw and ch to boundaries to a multiple of cSize, but make sure they're bigger than cx and cy
  const int cSize = 80;
  int cw = (cx + cSize - 1) / cSize * cSize;
  int ch = (cy + cSize - 1) / cSize * cSize;
  // this stops invalid cw and ch values appearing
  if (cw < cSize)
    cw = cSize;
  if (ch < cSize)
    ch = cSize;
  if (m_pPaletteDIB == NULL ||
    cw != m_pPaletteDIB->GetWidth() ||
    ch != m_pPaletteDIB->GetHeight())
  {
    delete m_pPaletteDIB;
    m_pPaletteDIB = new CDIBSection(cw, ch, 24);
  }
  if (m_pPaletteDIB && m_pPaletteDIB->GetPixels() != NULL) {
    BGR* pixels = (BGR*)m_pPaletteDIB->GetPixels();
    UpdatePalette(cx, cy, cw, pixels);
  }
  Invalidate();
}
////////////////////////////////////////////////////////////////////////////////
