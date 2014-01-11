#ifndef COLOR_ADJUST_DIALOG_HPP
#define COLOR_ADJUST_DIALOG_HPP
#include <afxwin.h>
#include <string>
#include <vector>
#include "../common/rgb.hpp"
#include "DIBSection.hpp"
///////////////////////////////////////////////////////////
class CColorAdjustDialog : public CDialog
{
public:
  CColorAdjustDialog(const int width, const int height, const RGBA* pixels);
  ~CColorAdjustDialog();
  int ShouldUseRedChannel() const;
  int ShouldUseGreenChannel() const;
  int ShouldUseBlueChannel() const;
  int ShouldUseAlphaChannel() const;
  int GetRedValue() const;
  int GetGreenValue() const;
  int GetBlueValue() const;
  int GetAlphaValue() const;
private:
  const int m_Width;
  const int m_Height;
  const RGBA* m_Pixels;
private:
  void UpdateButtons();
  int m_UseRed, m_UseGreen, m_UseBlue, m_UseAlpha;
  int m_RedValue, m_GreenValue, m_BlueValue, m_AlphaValue;
private:
  CDIBSection* m_blit_tile;
private:
  BOOL OnInitDialog();
  void OnOK();
  bool DrawPreview(CPaintDC& dc, RECT* rect);
  afx_msg void OnPaint();
  afx_msg void OnValueChanged();
  afx_msg void OnChannelChanged();
private:
  DECLARE_MESSAGE_MAP()
};
///////////////////////////////////////////////////////////
#endif
