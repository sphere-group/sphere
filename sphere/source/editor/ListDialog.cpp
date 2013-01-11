#include "ListDialog.hpp"
#include "resource.h"
////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CListDialog, CDialog)
  ON_CBN_SELCHANGE(IDC_LIST, OnSelectionChanged)
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////
CListDialog::CListDialog()
: CDialog(IDD_LIST)
, m_Value(0)
, m_Caption("")
//, m_Text("")
{
}
////////////////////////////////////////////////////////////////////////////////
CListDialog::~CListDialog()
{
}
////////////////////////////////////////////////////////////////////////////////
BOOL
CListDialog::OnInitDialog()
{
  SetWindowText(m_Caption.c_str());
  for (int i = 0; i < int(m_Items.size()); i++) {
    SendDlgItemMessage(IDC_LIST, CB_ADDSTRING, 0, (LPARAM)m_Items[i].c_str());
    if (m_Value == i)
      SendDlgItemMessage(IDC_LIST, CB_SETCURSEL, m_Value);
  }
  UpdateButtons();
  return FALSE;
}
////////////////////////////////////////////////////////////////////////////////
void
CListDialog::SetCaption(const char* caption)
{
  m_Caption = caption;
  if (::IsWindow(m_hWnd))
    SetWindowText(m_Caption.c_str());
}
////////////////////////////////////////////////////////////////////////////////
int
CListDialog::AddItem(const char* string)
{
  int num_items = m_Items.size();
  m_Items.push_back(string);
  
  return (m_Items.size() == num_items + 1) ? num_items : -1;
}
////////////////////////////////////////////////////////////////////////////////
void
CListDialog::SetSelection(int v)
{
  m_Value = v;
  if (::IsWindow(m_hWnd)) {
    if (m_Value >= 0 && m_Value < SendDlgItemMessage(IDC_LIST, CB_GETCOUNT)) {
      SendDlgItemMessage(IDC_LIST, CB_SETCURSEL, m_Value);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
int
CListDialog::GetSelection() const
{
  return m_Value;
}
////////////////////////////////////////////////////////////////////////////////
const char*
CListDialog::GetItemText(int index) const
{
  return m_Items[index].c_str();
}
////////////////////////////////////////////////////////////////////////////////
void
CListDialog::UpdateButtons()
{
  GetDlgItem(IDOK)->EnableWindow( (m_Value >= 0 && m_Value < SendDlgItemMessage(IDC_LIST, CB_GETCOUNT)) ? TRUE : FALSE);
}
////////////////////////////////////////////////////////////////////////////////
void
CListDialog::OnOK()
{
  if (m_Value >= 0 && m_Value < SendDlgItemMessage(IDC_LIST, CB_GETCOUNT))
    CDialog::OnOK();
}
////////////////////////////////////////////////////////////////////////////////
void
CListDialog::OnSelectionChanged()
{
  m_Value = SendDlgItemMessage(IDC_LIST, CB_GETCURSEL);
  UpdateButtons();
}
////////////////////////////////////////////////////////////////////////////////
