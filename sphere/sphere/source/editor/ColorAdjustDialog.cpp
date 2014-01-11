#include "ColorAdjustDialog.hpp"
#include "resource.h"
#include "../common/str_util.hpp"
#include <afxcmn.h>
////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CColorAdjustDialog, CDialog)
  ON_BN_CLICKED(IDC_USE_RED,   OnChannelChanged)
  ON_BN_CLICKED(IDC_USE_GREEN, OnChannelChanged)
  ON_BN_CLICKED(IDC_USE_BLUE,  OnChannelChanged)
  ON_BN_CLICKED(IDC_USE_ALPHA, OnChannelChanged)
  ON_EN_CHANGE(IDC_EDIT_RED,   OnValueChanged)
  ON_EN_CHANGE(IDC_EDIT_GREEN, OnValueChanged)
  ON_EN_CHANGE(IDC_EDIT_BLUE,  OnValueChanged)
  ON_EN_CHANGE(IDC_EDIT_ALPHA, OnValueChanged)
  ON_WM_PAINT()
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////
CColorAdjustDialog::CColorAdjustDialog(const int width, const int height, const RGBA* pixels)
: CDialog(IDD_COLOR_ADJUST)
, m_Width(width)
, m_Height(height)
, m_Pixels(pixels)
, m_blit_tile(NULL)
, m_RedValue(0)
, m_GreenValue(0)
, m_BlueValue(0)
, m_AlphaValue(0)
{
  m_blit_tile = new CDIBSection(32, 32, 32);
}
////////////////////////////////////////////////////////////////////////////////
CColorAdjustDialog::~CColorAdjustDialog()
{
  if (m_blit_tile) {
    delete m_blit_tile;
    m_blit_tile = NULL;
  }
}
////////////////////////////////////////////////////////////////////////////////
BOOL
CColorAdjustDialog::OnInitDialog()
{
  UpdateButtons();
  CheckDlgButton(IDC_USE_RED,   BST_CHECKED);
  CheckDlgButton(IDC_USE_GREEN, BST_CHECKED);
  CheckDlgButton(IDC_USE_BLUE,  BST_CHECKED);
  CheckDlgButton(IDC_USE_ALPHA, BST_UNCHECKED);
  SetDlgItemInt(IDC_EDIT_RED,   m_RedValue);
  SetDlgItemInt(IDC_EDIT_GREEN, m_GreenValue);
  SetDlgItemInt(IDC_EDIT_BLUE,  m_BlueValue);
  SetDlgItemInt(IDC_EDIT_ALPHA, m_AlphaValue);
  CSpinButtonCtrl* spin = (CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_RED);
  if (spin) {
    spin->SetRange(-255, 255);
  }
  spin = (CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_GREEN);
  if (spin) {
    spin->SetRange(-255, 255);
  }
  spin = (CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_BLUE);
  if (spin) {
    spin->SetRange(-255, 255);
  }
  spin = (CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_ALPHA);
  if (spin) {
    spin->SetRange(-255, 255);
  }
  OnChannelChanged();
  return FALSE;
}
////////////////////////////////////////////////////////////////////////////////
//#include "../../../code/image_proc/image/hsi.hpp"
bool
CColorAdjustDialog::DrawPreview(CPaintDC& dc, RECT* rect)
{
  if (!m_blit_tile || m_blit_tile->GetPixels() == NULL || rect == NULL) {
    return false;
  }
  /////////////////////////////////////////////////////////
  const int width  = m_blit_tile->GetWidth();
  const int height = m_blit_tile->GetHeight();
  int current_width = m_Width;
  if (current_width > width)
    current_width = width;
  int current_height = m_Height;
  if (current_height > height)
    current_height = height;
  BGRA* pixels = (BGRA*) m_blit_tile->GetPixels();
  for (int iy = 0; iy < current_height; iy++) {
    for (int ix = 0; ix < current_width; ix++) {
      pixels[iy * width + ix].red   = m_Pixels[iy * m_Width + ix].red;
      pixels[iy * width + ix].green = m_Pixels[iy * m_Width + ix].green;
      pixels[iy * width + ix].blue  = m_Pixels[iy * m_Width + ix].blue;
      pixels[iy * width + ix].alpha = m_Pixels[iy * m_Width + ix].alpha;
    }
  }
  /////////////////////////////////////////////////////////
  int red_value   = GetRedValue();
  int green_value = GetGreenValue();
  int blue_value  = GetBlueValue();
  int alpha_value = GetAlphaValue();
  int use_red   = ShouldUseRedChannel();
  int use_green = ShouldUseGreenChannel();
  int use_blue  = ShouldUseBlueChannel();
  int use_alpha = ShouldUseAlphaChannel();
  int method = 0;
  if (method == 0)
  {
    for (int iy = 0; iy < current_height; iy++) {
      for (int ix = 0; ix < current_width; ix++) {
        if (use_red)   pixels[iy * width + ix].red   += red_value;
        if (use_green) pixels[iy * width + ix].green += green_value;
        if (use_blue)  pixels[iy * width + ix].blue  += blue_value;
        if (use_alpha) pixels[iy * width + ix].alpha += alpha_value;
      }
    } 
  }
  /*
  else
  {
    double h_value = ((double)red_value   / (double)255.0) * ((double)2.0 * 3.14);
    double s_value = ((double)green_value / (double)255.0);
    double i_value = ((double)blue_value  / (double)255.0);
    for (int iy = 0; iy < current_height; iy++) {
      for (int ix = 0; ix < current_width; ix++) {
        double r = pixels[iy * width + ix].red   / 255.0;
        double g = pixels[iy * width + ix].green / 255.0;
        double b = pixels[iy * width + ix].blue  / 255.0;
        double h, s, i;
        RGBtoHSI(r, g, b, &h, &s, &i);
        if (use_red)   h += h_value;
        if (use_green) s += s_value;
        if (use_blue)  i += i_value;
        if (use_alpha) pixels[iy * width + ix].alpha += alpha_value;
        HSItoRGB(h, s, i, &r, &g, &b);
        pixels[iy * width + ix].red   = r * 255;
        pixels[iy * width + ix].green = g * 255;
        pixels[iy * width + ix].blue  = b * 255;
      }
    } 
  }
  */
  /////////////////////////////////////////////////////////
  dc.BitBlt(rect->left, rect->top, current_width, current_height, CDC::FromHandle(m_blit_tile->GetDC()), 0, 0, SRCCOPY);
  if (1) {
    rect->left += current_width;
    dc.FillRect(rect, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
    rect->left -= current_width;
    rect->top += current_height;
    dc.FillRect(rect, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
    rect->top -= current_height;
  }
  return true;
}
////////////////////////////////////////////////////////////////////////////////
void
CColorAdjustDialog::OnPaint()
{
	CPaintDC dc(this);
  RECT rect;
  GetDlgItem(IDC_PREVIEW_FRAME)->GetWindowRect(&rect);
  ScreenToClient(&rect);
  if (DrawPreview(dc, &rect) == false) {    
    FillRect(dc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
  }
}
////////////////////////////////////////////////////////////////////////////////
int
CColorAdjustDialog::ShouldUseRedChannel() const
{
  return m_UseRed;
}
////////////////////////////////////////////////////////////////////////////////
int
CColorAdjustDialog::ShouldUseGreenChannel() const
{
  return m_UseGreen;
}
////////////////////////////////////////////////////////////////////////////////
int
CColorAdjustDialog::ShouldUseBlueChannel() const
{
  return m_UseBlue;
}
////////////////////////////////////////////////////////////////////////////////
int
CColorAdjustDialog::ShouldUseAlphaChannel() const
{
  return m_UseAlpha;
}
////////////////////////////////////////////////////////////////////////////////
int
CColorAdjustDialog::GetRedValue() const
{
  return m_RedValue;
}
////////////////////////////////////////////////////////////////////////////////
int
CColorAdjustDialog::GetGreenValue() const
{
  return m_GreenValue;
}
////////////////////////////////////////////////////////////////////////////////
int
CColorAdjustDialog::GetBlueValue() const
{
  return m_BlueValue;
}
////////////////////////////////////////////////////////////////////////////////
int
CColorAdjustDialog::GetAlphaValue() const
{
  return m_AlphaValue;
}
////////////////////////////////////////////////////////////////////////////////
void
CColorAdjustDialog::OnOK()
{
  CDialog::OnOK();
}
////////////////////////////////////////////////////////////////////////////////
void
CColorAdjustDialog::UpdateButtons()
{
}
////////////////////////////////////////////////////////////////////////////////
void
CColorAdjustDialog::OnValueChanged()
{
  m_RedValue   = GetDlgItemInt(IDC_EDIT_RED);
  m_GreenValue = GetDlgItemInt(IDC_EDIT_GREEN);
  m_BlueValue  = GetDlgItemInt(IDC_EDIT_BLUE);
  m_AlphaValue = GetDlgItemInt(IDC_EDIT_ALPHA);
  UpdateButtons();
  Invalidate(FALSE);
}
////////////////////////////////////////////////////////////////////////////////
void
CColorAdjustDialog::OnChannelChanged()
{
  m_UseRed   =  IsDlgButtonChecked(IDC_USE_RED)   == BST_CHECKED ? 1 : 0;
  m_UseGreen =  IsDlgButtonChecked(IDC_USE_GREEN) == BST_CHECKED ? 1 : 0;
  m_UseBlue  =  IsDlgButtonChecked(IDC_USE_BLUE)  == BST_CHECKED ? 1 : 0;
  m_UseAlpha =  IsDlgButtonChecked(IDC_USE_ALPHA) == BST_CHECKED ? 1 : 0;
  Invalidate(FALSE);
}
////////////////////////////////////////////////////////////////////////////////
