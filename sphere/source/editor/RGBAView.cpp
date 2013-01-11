#include "RGBAView.hpp"
////////////////////////////////////////////////////////////////////////////////
CRGBAView::CRGBAView()
{
}
////////////////////////////////////////////////////////////////////////////////
BOOL
CRGBAView::Create(CWnd* parent, int x, int y, int w, int h, RGBA color)
{
  m_Colors[0] = color;
  m_Colors[1] = CreateRGBA(0, 0, 0, 255);
  m_ColorView.Create(this, parent,
                     CreateRGB(m_Colors[0].red, m_Colors[0].green, m_Colors[0].blue),
                     CreateRGB(m_Colors[1].red, m_Colors[1].green, m_Colors[1].blue));
  m_AlphaView.Create(this, parent);
  m_AlphaView.SetAlpha(m_Colors[0].alpha);
  m_ColorView.MoveWindow(x, y, w - 32, h);
  m_AlphaView.MoveWindow(x + w - 32, y, 32, h);
  return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
void
CRGBAView::SetNumColors(int num_colors)
{
  m_ColorView.SetNumColors(num_colors);
}
////////////////////////////////////////////////////////////////////////////////
RGBA
CRGBAView::GetColor(int index)
{
  return m_Colors[index];
}
////////////////////////////////////////////////////////////////////////////////
void
CRGBAView::CV_ColorChanged(int index, RGB color)
{
  m_Colors[index].red   = color.red;
  m_Colors[index].green = color.green;
  m_Colors[index].blue  = color.blue;
}
////////////////////////////////////////////////////////////////////////////////
void
CRGBAView::AV_AlphaChanged(byte alpha)
{
  m_Colors[0].alpha = alpha;
  m_Colors[1].alpha = alpha;
}
////////////////////////////////////////////////////////////////////////////////
