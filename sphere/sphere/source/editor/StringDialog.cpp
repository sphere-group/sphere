#include "StringDialog.hpp"
#include "resource.h"
////////////////////////////////////////////////////////////////////////////////
CStringDialog::CStringDialog(const char* caption, const char* default_value)
: CDialog(IDD_STRING)
{
  m_caption = caption;
  m_value = default_value;
}
////////////////////////////////////////////////////////////////////////////////
const char*
CStringDialog::GetValue() const
{
  return m_value.c_str();
}
////////////////////////////////////////////////////////////////////////////////
BOOL
CStringDialog::OnInitDialog()
{
  // set dialog caption
  SetWindowText(m_caption.c_str());
  // set default value
  GetDlgItem(IDC_VALUE)->SetWindowText(m_value.c_str());
  // select all of the text
  SendDlgItemMessage(IDC_VALUE, EM_SETSEL, 0, -1);
  return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
void
CStringDialog::OnOK()
{
  CString value;
  GetDlgItemText(IDC_VALUE, value);
  m_value = value;
  CDialog::OnOK();
}
////////////////////////////////////////////////////////////////////////////////
