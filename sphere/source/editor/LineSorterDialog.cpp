#include "LineSorterDialog.hpp"
#include "../common/str_util.hpp"
#include "Editor.hpp"
#include "resource.h"
////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CLineSorterDialog, CDialog)
  ON_BN_CLICKED(IDC_LINESORT_SORTLINES,        OnOptionChanged)
  ON_BN_CLICKED(IDC_LINESORT_REVERSELINES,     OnOptionChanged)
  ON_BN_CLICKED(IDC_LINESORT_DELETEDUPLICATES, OnOptionChanged)
  ON_BN_CLICKED(IDC_LINESORT_IGNORECASE,       OnOptionChanged)
  ON_BN_CLICKED(IDC_LINESORT_COMPARE_NUMERIC,  OnOptionChanged)
  
  ON_EN_CHANGE(IDC_LINESORT_START_CHARACTER, OnOptionChanged)
  ON_EN_CHANGE(IDC_LINESORT_START_TAB, OnOptionChanged)
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////
CLineSorterDialog::CLineSorterDialog()
: CDialog(IDD_LINE_SORTER)
{
}
////////////////////////////////////////////////////////////////////////////////
BOOL
CLineSorterDialog::OnInitDialog()
{
  m_SortLines = true;
  m_ReverseLines = false;
  m_RemoveDuplicateLines = false;
  m_IgnoreCase = false;
  m_CompareNumeric = false;
  m_StartCharacter = 0;
  m_StartTab = 0;
  SetDlgItemText(IDC_LINESORT_START_CHARACTER, "0");
  SetDlgItemText(IDC_LINESORT_START_TAB, "0");
  m_SortLines = true;
  m_ReverseLines = false;
  m_RemoveDuplicateLines = false;
  m_IgnoreCase = false;
  m_CompareNumeric = false;
  m_StartCharacter = 0;
  m_StartTab = 0;
  UpdateButtons();
  return FALSE;
}
////////////////////////////////////////////////////////////////////////////////
void
CLineSorterDialog::OnOK()
{
  CDialog::OnOK();
}
////////////////////////////////////////////////////////////////////////////////
void
CLineSorterDialog::UpdateButtons()
{
  BOOL enabled = TRUE;
  const char* error = NULL;
  if (1)
  {
    CString text;
    GetDlgItemText(IDC_LINESORT_START_CHARACTER, text);
 
    bool percentage = false;
    bool floating = false;
    int num = atoi(text);
    if (IsInvalidNumber(text, floating, percentage) || floating || percentage)
    {
      error = "Invalid number format";
      enabled = FALSE;
    }
    else
    {
      m_StartCharacter = num;
    }
  }
  
  if (1)
  {
    CString text;
    GetDlgItemText(IDC_LINESORT_START_TAB, text);
    bool percentage = false;
    bool floating = false;
    int num = atoi(text);
    if (IsInvalidNumber(text, floating, percentage) || floating || percentage)
    {
      error = "Invalid number format";
      enabled = FALSE;
    }
    else
    {
      m_StartTab = num;
    }
  }
  CheckDlgButton(IDC_LINESORT_SORTLINES,        m_SortLines    ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton(IDC_LINESORT_REVERSELINES,     m_ReverseLines ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton(IDC_LINESORT_DELETEDUPLICATES, m_RemoveDuplicateLines ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton(IDC_LINESORT_IGNORECASE,       m_IgnoreCase     ? BST_CHECKED : BST_UNCHECKED);
  CheckDlgButton(IDC_LINESORT_COMPARE_NUMERIC,  m_CompareNumeric ? BST_CHECKED : BST_UNCHECKED);
  if (IsDlgButtonChecked(IDC_LINESORT_SORTLINES) != BST_CHECKED
   && IsDlgButtonChecked(IDC_LINESORT_REVERSELINES) != BST_CHECKED
   && IsDlgButtonChecked(IDC_LINESORT_DELETEDUPLICATES) != BST_CHECKED) {
    enabled = FALSE;
    error = "One of sort lines or reverse lines or delete duplicate lines must be selected...";
  }
  if (GetDlgItem(IDOK)) {
    GetDlgItem(IDOK)->EnableWindow(enabled); 
  }
  if (error == NULL) {
    GetStatusBar()->SetWindowText("");
  } else {
    GetStatusBar()->SetWindowText(error);
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CLineSorterDialog::OnOptionChanged()
{
  m_SortLines    = (IsDlgButtonChecked(IDC_LINESORT_SORTLINES)    == BST_CHECKED);
  m_ReverseLines = (IsDlgButtonChecked(IDC_LINESORT_REVERSELINES) == BST_CHECKED);
  m_RemoveDuplicateLines = (IsDlgButtonChecked(IDC_LINESORT_DELETEDUPLICATES) == BST_CHECKED);
  m_IgnoreCase     = (IsDlgButtonChecked(IDC_LINESORT_IGNORECASE)      == BST_CHECKED);
  m_CompareNumeric = (IsDlgButtonChecked(IDC_LINESORT_COMPARE_NUMERIC) == BST_CHECKED);
  UpdateButtons();
}
////////////////////////////////////////////////////////////////////////////////
bool
CLineSorterDialog::ShouldSortLines() {
  return m_SortLines;
}
bool
CLineSorterDialog::ShouldReverseLines() {
  return m_ReverseLines;
}
bool
CLineSorterDialog::ShouldRemoveDuplicateLines() {
  return m_RemoveDuplicateLines;
}
bool
CLineSorterDialog::ShouldIgnoreCase() {
  return m_IgnoreCase;
}
bool
CLineSorterDialog::ShouldCompareNumeric() {
  return m_CompareNumeric;
}
int
CLineSorterDialog::GetStartCharacter() {
  return m_StartCharacter;
}
int
CLineSorterDialog::GetStartTab() {
  return m_StartTab;
}
////////////////////////////////////////////////////////////////////////////////
