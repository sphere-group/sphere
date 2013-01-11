#include <afxdlgs.h>
#include "ColorView.hpp"
#include "resource.h"
BEGIN_MESSAGE_MAP(CColorView, CWnd)
  ON_WM_PAINT()
  ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////
CColorView::CColorView()
: m_ColorA(CreateRGB(0, 0, 0))
, m_ColorB(CreateRGB(255, 255, 255))
, m_NumColors(1)
{
}
////////////////////////////////////////////////////////////////////////////////
CColorView::~CColorView()
{
  DestroyWindow();
}
////////////////////////////////////////////////////////////////////////////////
BOOL
CColorView::Create(IColorViewHandler* handler, CWnd* parent, RGB color_a, RGB color_b)
{
  m_Handler = handler;
  m_ColorA = color_a;
  m_ColorB = color_b;
  return CWnd::Create(
    AfxRegisterWndClass(0, LoadCursor(NULL, IDC_ARROW), NULL, NULL),
    "CColorView",
    WS_CHILD | WS_VISIBLE,
    CRect(0, 0, 0, 0),
    parent,
    0);
}
////////////////////////////////////////////////////////////////////////////////
BOOL
CColorView::Create(IColorViewHandler* handler, CWnd* parent)
{
  RGB color_a = CreateRGB(255, 255, 255);
  RGB color_b = CreateRGB(0,   0,   0);
  return Create(handler, parent, color_a, color_b);
}
////////////////////////////////////////////////////////////////////////////////
void
CColorView::SetNumColors(int num_colors)
{
  m_NumColors = num_colors;
}
////////////////////////////////////////////////////////////////////////////////
void
CColorView::SetColor(int index, RGB color)
{
  if (index == 0)
    m_ColorA = color;
  else
    m_ColorB = color;
  Invalidate();
}
////////////////////////////////////////////////////////////////////////////////
RGB
CColorView::GetColor(int index) const
{
  return index == 0 ? m_ColorA : m_ColorB;
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CColorView::OnPaint()
{
  CPaintDC dc(this);
  
  if (m_NumColors == 1) {
    CBrush* brush = new CBrush(RGB(m_ColorA.red, m_ColorA.green, m_ColorA.blue));
    if (brush) {
      // fill client rectangle with current color
      RECT ClientRect;
      GetClientRect(&ClientRect);
      dc.FillRect(&ClientRect, brush);
 
      brush->DeleteObject();
      delete brush;
    }
  }
  else
  if (m_NumColors == 2) {
    if (1) {
      RECT ClientRect;
      GetClientRect(&ClientRect);
      int width  = ClientRect.right;
      int height = ClientRect.bottom;
      ClientRect.left  += width / 2;
      ClientRect.bottom -= height / 2;
      dc.FillRect(&ClientRect, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
      ClientRect.left  -= width / 2;
      ClientRect.bottom += height / 2;
    
      ClientRect.top += height / 2;
      ClientRect.right -= width / 2;
      dc.FillRect(&ClientRect, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
      ClientRect.top -= height / 2;
      ClientRect.right += width / 2;
    }
 
    CBrush* brush = new CBrush(RGB(m_ColorA.red, m_ColorA.green, m_ColorA.blue));
    if (brush) {
      // fill client rectangle with current color
      RECT ClientRect;
      GetClientRect(&ClientRect);
      int width  = ClientRect.right;
      int height = ClientRect.bottom;
      ClientRect.right  -= width / 2;
      ClientRect.bottom -= height / 2;
      dc.FillRect(&ClientRect, brush);
 
      brush->DeleteObject();
      delete brush;
    }
    brush = new CBrush(RGB(m_ColorB.red, m_ColorB.green, m_ColorB.blue));
    if (brush) {
      // fill client rectangle with current color
      RECT ClientRect;
      GetClientRect(&ClientRect);
      int width  = ClientRect.right;
      int height = ClientRect.bottom;
      ClientRect.left += width / 2;
      ClientRect.top  += height / 2;
      dc.FillRect(&ClientRect, brush);
      brush->DeleteObject();
      delete brush;
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CColorView::OnLButtonDown(UINT flags, CPoint point)
{
  int color_index = -1;
  
  if (m_NumColors == 1) {
    color_index = 0;
  } else {
    RECT ClientRect;
    GetClientRect(&ClientRect);
    // upper left corner
    if (point.x >= 0 && point.x < ClientRect.right/2
     && point.y >= 0 && point.y < ClientRect.bottom/2) {
      color_index = 0;
    }
    // lower right corner
    if (point.x >= ClientRect.right/2  && point.x < ClientRect.right
     && point.y >= ClientRect.bottom/2 && point.y < ClientRect.bottom) {
      color_index = 1;
    }
    // lower left corner, swap colors round
    if (point.x >= 0 && point.x < ClientRect.right/2
     && point.y >= ClientRect.bottom/2 && point.y < ClientRect.bottom) {
      RGB clr = { m_ColorA.red, m_ColorA.green, m_ColorA.blue };
      m_ColorA = m_ColorB;
      m_ColorB = clr;
      Invalidate();
      m_Handler->CV_ColorChanged(0, m_ColorA);
      m_Handler->CV_ColorChanged(1, m_ColorB);
    }
  }
  if (color_index == -1)
    return;
  RGB clr;
  if (color_index == 0) {
    clr.red   = m_ColorA.red;
    clr.green = m_ColorA.green;
    clr.blue  = m_ColorA.blue;
  } else {
    clr.red   = m_ColorB.red;
    clr.green = m_ColorB.green;
    clr.blue  = m_ColorB.blue;
  }
  CColorDialog ColorDialog(
    RGB(clr.red, clr.green, clr.blue),
    CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT);
  if (ColorDialog.DoModal() == IDOK)
  {
    clr.red   = GetRValue(ColorDialog.GetColor());
    clr.green = GetGValue(ColorDialog.GetColor());
    clr.blue  = GetBValue(ColorDialog.GetColor());
    if (color_index == 0) {
      m_ColorA = clr;
    } else {
      m_ColorB = clr;
    }
    Invalidate();
    m_Handler->CV_ColorChanged(0, m_ColorA);
    m_Handler->CV_ColorChanged(1, m_ColorB);
  }
}
////////////////////////////////////////////////////////////////////////////////
