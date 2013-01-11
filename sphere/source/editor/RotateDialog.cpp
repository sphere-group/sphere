#include "RotateDialog.hpp"
#include "Project.hpp"
#include "FileDialogs.hpp"
#include "resource.h"
#include "../common/str_util.hpp"
BEGIN_MESSAGE_MAP(CRotateDialog, CDialog)
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////
CRotateDialog::CRotateDialog(const char* caption, const char* text, double value)
: CDialog(IDD_ROTATE)
, m_Title(caption)
, m_Text("")
, m_Value(value)
{
}
////////////////////////////////////////////////////////////////////////////////
BOOL
CRotateDialog::OnInitDialog()
{
  CDialog::OnInitDialog();
  // get pointers to all dialog controls
  m_Static = (CStatic*)GetDlgItem(IDC_ROTATETEXT);
  m_NumberEdit = (CEdit*)GetDlgItem(IDC_ROTATEVALUE);
  // initialize the dialog box
  SetWindowText(m_Title.c_str());
  m_Static->SetWindowText(m_Text.c_str());
  m_NumberEdit->SetFocus();
  char number[80];
  sprintf(number, "%.2f", m_Value);
  m_NumberEdit->SetWindowText(number);
  m_NumberEdit->SetSel(0, -1);
  CheckDlgButton(IDC_ROTATE_LEFT,     BST_CHECKED);
  return FALSE;
}
////////////////////////////////////////////////////////////////////////////////
void
CRotateDialog::OnOK()
{
  CString text;
  m_NumberEdit->GetWindowText(text);
  double value = atof(text);
  bool percentage, floating;
  if (IsInvalidNumber(text, floating, percentage) || percentage) {
    MessageBox("Invalid number format", m_Title.c_str());
  }
  else
  {
    const double PI = 3.14159;
    value = value * PI / 180.0; // convert to radians
    if (IsDlgButtonChecked(IDC_ROTATE_LEFT) == BST_CHECKED) {
    }
    if (IsDlgButtonChecked(IDC_ROTATE_RIGHT) == BST_CHECKED) {
      value = (2.0*PI) - value;
    }
    m_Value = value;
    CDialog::OnOK();
  }
}
////////////////////////////////////////////////////////////////////////////////
double
CRotateDialog::GetValue()
{
  return m_Value;
}
