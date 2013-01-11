#include "FontGradientDialog.hpp"
#include "resource.h"
////////////////////////////////////////////////////////////////////////////////
CFontGradientDialog::CFontGradientDialog()
: CDialog(IDD_FONT_GRADIENT)
, m_top_color(CreateRGBA(255, 255, 255, 255))
, m_bottom_color(CreateRGBA(255, 255, 255, 255))
, m_Text1("Top Color")
, m_Text2("Bottom Color")
, m_Caption("Generate Gradient")
{
}
CFontGradientDialog::CFontGradientDialog(const char* caption, const char* text1, const char* text2)
: CDialog(IDD_FONT_GRADIENT)
, m_Caption(caption)
, m_Text1(text1)
, m_Text2(text2)
, m_top_color(CreateRGBA(255, 255, 255, 255))
, m_bottom_color(CreateRGBA(255, 255, 255, 255))
{
}
CFontGradientDialog::CFontGradientDialog(const char* caption, const char* text1, const char* text2, RGBA color1, RGBA color2)
: CDialog(IDD_FONT_GRADIENT)
, m_Caption(caption)
, m_Text1(text1)
, m_Text2(text2)
, m_top_color(color1)
, m_bottom_color(color2)
{
}
////////////////////////////////////////////////////////////////////////////////
RGBA
CFontGradientDialog::GetTopColor()
{
  return m_top_color;
}
////////////////////////////////////////////////////////////////////////////////
RGBA
CFontGradientDialog::GetBottomColor()
{
  return m_bottom_color;
}
////////////////////////////////////////////////////////////////////////////////
BOOL
CFontGradientDialog::OnInitDialog()
{
  InitializeView(m_top_view,    m_top_color,    IDC_TOP_COLOR);
  InitializeView(m_bottom_view, m_bottom_color, IDC_BOTTOM_COLOR);
  SetWindowText(m_Caption.c_str());
  CStatic* label_control = (CStatic*)GetDlgItem(IDC_TOP_COLOR_TEXT);
  label_control->SetWindowText(m_Text1.c_str());
  label_control = (CStatic*)GetDlgItem(IDC_BOTTOM_COLOR_TEXT);
  label_control->SetWindowText(m_Text2.c_str());
  return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
void
CFontGradientDialog::InitializeView(CRGBAView& view, RGBA color, int control_id)
{
  RECT rect;
  GetDlgItem(control_id)->GetWindowRect(&rect);
  ScreenToClient(&rect);
  view.Create(
    this,
    rect.left,
    rect.top,
    rect.right - rect.left,
    rect.bottom - rect.top,
    color
  );
}
////////////////////////////////////////////////////////////////////////////////
void
CFontGradientDialog::OnOK()
{
  m_top_color    = m_top_view.GetColor();
  m_bottom_color = m_bottom_view.GetColor();
  CDialog::OnOK();
}
////////////////////////////////////////////////////////////////////////////////
