#include "NumberDialog.hpp"
#include "../common/str_util.hpp"
#include "resource.h"
////////////////////////////////////////////////////////////////////////////////
CNumberDialog::CNumberDialog(const char* title, const char* text, int value, int min, int max)
: CDialog(IDD_NUMBER)
, m_Title(title)
, m_Text(text)
, m_Value(value)
, m_Min(min)
, m_Max(max)
, m_FloatingPointAllowed(false)
{
}
////////////////////////////////////////////////////////////////////////////////
CNumberDialog::CNumberDialog(const char* title, const char* text, double value, double min, double max)
: CDialog(IDD_NUMBER)
, m_Title(title)
, m_Text(text)
, m_Value(value)
, m_Min(min)
, m_Max(max)
, m_FloatingPointAllowed(true)
{
}
////////////////////////////////////////////////////////////////////////////////
int
CNumberDialog::GetValue() const
{
  return (int) m_Value;
}
////////////////////////////////////////////////////////////////////////////////
double
CNumberDialog::GetDoubleValue() const
{
  return m_Value;
}
////////////////////////////////////////////////////////////////////////////////
afx_msg BOOL
CNumberDialog::OnInitDialog()
{
  CDialog::OnInitDialog();
  // get pointers to all dialog controls
  m_Static = (CStatic*)GetDlgItem(IDC_NUMBERTEXT);
  m_NumberEdit = (CEdit*)GetDlgItem(IDC_NUMBER);
  // initialize the dialog box
  SetWindowText(m_Title.c_str());
  m_Static->SetWindowText(m_Text.c_str());
  m_NumberEdit->SetFocus();
  char number[80];
  if (m_FloatingPointAllowed) {
    sprintf(number, "%.2f", m_Value);
  }
  else {
    sprintf(number, "%d", (int) m_Value);
  }
  SetDlgItemText(IDC_NUMBER, number);
  m_NumberEdit->SetSel(0, -1);
  return FALSE;
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CNumberDialog::OnOK()
{
  CString text;
  GetDlgItemText(IDC_NUMBER, text);
  double value = atof(text);
  bool percentage, floating;
  if (IsInvalidNumber(text, floating, percentage) || (floating && !m_FloatingPointAllowed) || percentage) {
    MessageBox("Invalid number format", m_Title.c_str());
  }
  else
  if (value < m_Min || value > m_Max)
  {
    char string[80];
    if (m_FloatingPointAllowed) {
      sprintf(string, "Value must be between %.3f and %.3f", m_Min, m_Max);
    }
    else {
      sprintf(string, "Value must be between %d and %d", (int) m_Min, (int) m_Max);
    }
    MessageBox(string, m_Title.c_str());
  }
  else
  {
    m_Value = value;
    CDialog::OnOK();
  }
}
////////////////////////////////////////////////////////////////////////////////
