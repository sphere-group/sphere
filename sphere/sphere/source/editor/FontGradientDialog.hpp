#ifndef FONT_GRADIENT_DIALOG_HPP
#define FONT_GRADIENT_DIALOG_HPP
#include <afxwin.h>
#include "RGBAView.hpp"
#include <string>
class CFontGradientDialog : public CDialog
{
public:
  CFontGradientDialog(const char* caption, const char* text1, const char* text2, RGBA color1, RGBA color2);
  CFontGradientDialog(const char* caption, const char* text1, const char* text2);
  CFontGradientDialog();
  RGBA GetTopColor();
  RGBA GetBottomColor();
private:
  BOOL OnInitDialog();
  void InitializeView(CRGBAView& view, RGBA color, int control_id);
  void OnOK();
private:
  CRGBAView m_top_view;
  CRGBAView m_bottom_view;
  RGBA m_top_color;
  RGBA m_bottom_color;
  std::string m_Caption;
  std::string m_Text1;
  std::string m_Text2;
};
#endif