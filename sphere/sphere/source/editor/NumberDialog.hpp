#ifndef NUMBER_DIALOG_HPP
#define NUMBER_DIALOG_HPP
#include <afxwin.h>
#include <string>
class CNumberDialog : public CDialog
{
public:
  CNumberDialog(const char* title, const char* text, int value, int min, int max);
  CNumberDialog(const char* tile, const char* text, double value, double min, double max);
  int GetValue() const;
  double GetDoubleValue() const;
private:
  afx_msg BOOL OnInitDialog();
  afx_msg void OnOK();
private:
  std::string m_Title;
  std::string m_Text;
  double m_Value;
  const double m_Min;
  const double m_Max;
  bool m_FloatingPointAllowed;
  // dialog controls
  CStatic* m_Static;
  CEdit*   m_NumberEdit;
};
#endif
