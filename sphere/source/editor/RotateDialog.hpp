#ifndef IMAGE_ROTATE_DIALOG_HPP
#define IMAGE_ROTATE_DIALOG_HPP
#include <afxwin.h>
#include <string>
class CRotateDialog : public CDialog
{
public:
  CRotateDialog(const char* caption, const char* text, double value);
  double GetValue();
private:
  BOOL OnInitDialog();
  void OnOK();
private:
  double         m_Value;
  std::string m_Title;
  std::string m_Text;
  // dialog controls
  CStatic* m_Static;
  CEdit*   m_NumberEdit;
  DECLARE_MESSAGE_MAP()
};
#endif
