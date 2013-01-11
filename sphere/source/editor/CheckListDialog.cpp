#include "CheckListDialog.hpp"
#include "resource.h"
#include "Editor.hpp"
static int IDC_CHECKLIST = 1233;
////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CCheckListDialog, CDialog)
  //ON_CBN_SELCHANGE(IDC_LIST, OnSelectionChanged)
  ON_WM_SIZE()
  ON_WM_SIZING()
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////
CCheckListDialog::CCheckListDialog()
: CDialog(IDD_CHECKLIST)
, m_Caption("")
//, m_Text("")
, m_MinChecked(0)
, m_MaxChecked(-1)
{
}
////////////////////////////////////////////////////////////////////////////////
CCheckListDialog::~CCheckListDialog()
{
}
////////////////////////////////////////////////////////////////////////////////
BOOL
CCheckListDialog::OnInitDialog()
{
  SetWindowText(m_Caption.c_str());
  RECT rect;
  GetClientRect(&rect);
  rect.left += 20;
  rect.right -= 20;
  rect.top += 20;
  rect.bottom -= 45;
  m_CheckList.Create(WS_VISIBLE | WS_VSCROLL | WS_TABSTOP | WS_BORDER | 
                     LBS_NOINTEGRALHEIGHT | LBS_HASSTRINGS | LBS_EXTENDEDSEL,
                     rect, this, IDC_CHECKLIST);
  for (int i = 0; i < int(m_Items.size()); i++) {
    m_CheckList.AddString(m_Items[i].c_str());
    m_CheckList.SetCheck(i, m_CheckStates[i] ? TRUE : FALSE);
  }
  UpdateButtons();
  return FALSE;
}
////////////////////////////////////////////////////////////////////////////////
void
CCheckListDialog::SetCaption(const char* caption)
{
  m_Caption = caption;
  if (::IsWindow(m_hWnd))
    SetWindowText(m_Caption.c_str());
}
////////////////////////////////////////////////////////////////////////////////
bool
CCheckListDialog::AddItem(const char* string, bool checked)
{
  unsigned int expected_size = m_Items.size() + 1;
  m_Items.push_back(string);
  if (m_Items.size() != expected_size)
    return false;
  m_CheckStates.push_back(checked);
  if (m_CheckStates.size() != expected_size) {
    m_Items.erase(m_Items.begin() + expected_size - 1);
    return false;
  }
  return true;
}
////////////////////////////////////////////////////////////////////////////////
bool
CCheckListDialog::IsChecked(int i) const
{
  return m_CheckStates[i];
}
////////////////////////////////////////////////////////////////////////////////
void
CCheckListDialog::SetMinChecked(int num)
{
  m_MinChecked = num;
}
////////////////////////////////////////////////////////////////////////////////
void
CCheckListDialog::SetMaxChecked(int num)
{
  m_MaxChecked = num;
}
////////////////////////////////////////////////////////////////////////////////
/*
void
CCheckListDialog::SetSelection(int v)
{
  m_Value = v;
  if (m_Value >= 0 && m_Value < SendDlgItemMessage(IDC_LIST, CB_GETCOUNT)) {
    SendDlgItemMessage(IDC_LIST, CB_SETCURSEL, m_Value);
  }
}
////////////////////////////////////////////////////////////////////////////////
int
CCheckListDialog::GetSelection() const
{
  return m_Value;
}
*/
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CCheckListDialog::OnSizing(UINT side, LPRECT rect)
{
  if (!rect)
    return;
  if (rect->right - rect->left < 300) {
    rect->right = rect->left + 300;
  }
  if (rect->bottom - rect->top < 200) {
    rect->bottom = rect->top + 200;
  }
  CDialog::OnSizing(side, rect);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CCheckListDialog::OnSize(UINT type, int cx, int cy)
{
  int button_width = 80;
  int button_height = 25;
  if (GetDlgItem(IDOK)) {
    RECT rect;
    GetDlgItem(IDOK)->GetWindowRect(&rect);
    button_width  = rect.right - rect.left;
    button_height = rect.bottom - rect.top;
  }
  if (::IsWindow(m_CheckList.m_hWnd)) {
    m_CheckList.MoveWindow(20, 20, cx - 40, cy - 65);
  }
  if (GetDlgItem(IDOK))
    GetDlgItem(IDOK)->MoveWindow(40, cy - button_height - 10, button_width, button_height);
  if (GetDlgItem(IDCANCEL))
    GetDlgItem(IDCANCEL)->MoveWindow(40 + button_width + 10, cy - button_height - 10, button_width, button_height);
  CDialog::OnSize(type, cx, cy);
}
////////////////////////////////////////////////////////////////////////////////
bool
CCheckListDialog::ValidateValues(std::string& error)
{
  if (m_MinChecked > 0 || m_MaxChecked > -1) {
    int num_checked = 0;
    for (int i = 0; i < m_CheckList.GetCount(); i++) {
      if (i < int(m_CheckStates.size())) {
        m_CheckStates[i] = m_CheckList.GetCheck(i) ? true : false;
        if (m_CheckStates[i])
          num_checked += 1;
      }
    }
    if (m_MinChecked > 0 && num_checked < m_MinChecked) {
      char string[1024];
      sprintf (string, "Please check atleast %d items", m_MinChecked);
      error = string;
      return false;
    }
    if (m_MaxChecked > -1 && num_checked >= m_MaxChecked) {
      char string[1024];
      sprintf (string, "Please check at-most %d items", m_MaxChecked);
      error = string;
      return false;
    }
  }
  return true;
}
////////////////////////////////////////////////////////////////////////////////
void
CCheckListDialog::UpdateButtons()
{
  BOOL enabled = TRUE;
  std::string error;
  if (!ValidateValues(error)) {
    GetStatusBar()->SetWindowText(error.c_str());
    enabled = FALSE;
  }
  if (GetDlgItem(IDOK)) {
    GetDlgItem(IDOK)->EnableWindow( enabled );
  }
}
////////////////////////////////////////////////////////////////////////////////
void
CCheckListDialog::OnOK()
{
  std::string error;
  if (!ValidateValues(error)) {
    MessageBox(error.c_str());
    return;
  }
  CDialog::OnOK();
}
////////////////////////////////////////////////////////////////////////////////
/*
void
CCheckListDialog::OnSelectionChanged()
{
  m_Value = SendDlgItemMessage(IDC_LIST, CB_GETCURSEL);
  UpdateButtons();
}
*/
////////////////////////////////////////////////////////////////////////////////
