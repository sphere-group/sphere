#ifndef WINDOW_STYLE_PROPERTIES_DIALOG_HPP
#define WINDOW_STYLE_PROPERTIES_DIALOG_HPP
#include "RGBAView.hpp"
#include "../common/WindowStyle.hpp"
class CWindowStylePropertiesDialog : public CDialog
{
public:
  CWindowStylePropertiesDialog(sWindowStyle* ws);
  ~CWindowStylePropertiesDialog();
private:
  BOOL OnInitDialog();
  void InitializeView(CRGBAView& view, int corner, int control_id);
  void OnOK();
private:
  CRGBAView m_UpperLeftView;
  CRGBAView m_UpperRightView;
  CRGBAView m_LowerLeftView;
  CRGBAView m_LowerRightView;
  sWindowStyle* m_WindowStyle;
};
#endif
