#include "AdjustBordersDialog.hpp"
#include "Editor.hpp"
#include "resource.h"
#include "../common/str_util.hpp"
////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CAdjustBordersDialog, CDialog)
  ON_EN_CHANGE(IDC_TOP, OnOptionChanged)
  ON_EN_CHANGE(IDC_RIGHT, OnOptionChanged)
  ON_EN_CHANGE(IDC_BOTTOM, OnOptionChanged)
  ON_EN_CHANGE(IDC_LEFT, OnOptionChanged)
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////
CAdjustBordersDialog::CAdjustBordersDialog(int current_x, int current_y, int current_width, int current_height,
                                           int min_x, int min_y, int max_x, int max_y,
                                           int min_width, int min_height, int max_width, int max_height)
: CDialog(IDD_ADJUST_BORDER)
, m_CurrentX(current_x)
, m_CurrentY(current_y)
, m_CurrentWidth(current_width)
, m_CurrentHeight(current_height)
, m_MinWidth(min_width)
, m_MaxWidth(max_width)
, m_MinHeight(min_height)
, m_MaxHeight(max_height)
, m_MinX(min_x)
, m_MinY(min_y)
, m_MaxX(max_x)
, m_MaxY(max_y)
{
}
////////////////////////////////////////////////////////////////////////////////
BOOL
CAdjustBordersDialog::OnInitDialog()
{
  m_TopPixels = 0;
  m_RightPixels = 0;
  m_BottomPixels = 0;
  m_LeftPixels = 0;
  SetDlgItemInt(IDC_TOP,  m_TopPixels);
  SetDlgItemInt(IDC_RIGHT, m_RightPixels);
  SetDlgItemInt(IDC_BOTTOM, m_BottomPixels);
  SetDlgItemInt(IDC_LEFT, m_LeftPixels);
  UpdateButtons();
  ((CEdit*)GetDlgItem(IDC_TOP))->SetSel(0, -1);
  return FALSE;
}
////////////////////////////////////////////////////////////////////////////////
bool
CAdjustBordersDialog::ValidateValues(std::string& error)
{
  CString top_text, right_text, bottom_text, left_text;
  GetDlgItem(IDC_TOP)->GetWindowText(top_text);
  GetDlgItem(IDC_RIGHT)->GetWindowText(right_text);
  GetDlgItem(IDC_BOTTOM)->GetWindowText(bottom_text);
  GetDlgItem(IDC_LEFT)->GetWindowText(left_text);
  const int t = atoi(top_text);
  const int r = atoi(right_text);
  const int b = atoi(bottom_text);
  const int l = atoi(left_text);
  bool percentage = false;
  bool floating = false;
  if (IsInvalidNumber(top_text, floating, percentage) || floating || percentage
   || IsInvalidNumber(right_text, floating, percentage) || floating || percentage
   || IsInvalidNumber(bottom_text, floating, percentage) || floating || percentage
   || IsInvalidNumber(left_text, floating, percentage) || floating || percentage) {
    error = "Invalid number format";
    return false;
  }
  const int w = m_CurrentWidth + (l + r);
  const int x = (m_CurrentX + m_CurrentWidth) - w;
  const int h = m_CurrentHeight + (t + b);
  const int y = (m_CurrentY + m_CurrentHeight) - h;
  char message[520] = "";
  bool failed = false;
  if (/*x < m_MinX
   //|| (x + w) >= m_MaxX
   ||*/ w < m_MinWidth
   || w >= m_MaxWidth) {
    sprintf(message + strlen(message), "Width (%d) must be between %d and %d.\n", w, m_MinWidth, m_MaxWidth);
    //sprintf(message + strlen(message), "X (%d) must be between %d and %d.\n", x, m_MinX, m_MaxX);
    failed = true;
  }
  if (/*y < m_MinY
   //|| (y + h) >= m_MaxY
   ||*/ h < m_MinHeight
   || h >= m_MaxHeight) {
    sprintf(message + strlen(message), "Height (%d) must be between %d and %d.\n", h, m_MinHeight, m_MaxHeight);
    //sprintf(message + strlen(message), "Y (%d) must be between %d and %d.", y, m_MinY, m_MaxY);
    error = message;
    failed = true;
  }
  if (failed) {
    error = message;
    return false;
  }
  m_TopPixels    = t;
  m_RightPixels  = r;
  m_BottomPixels = b;
  m_LeftPixels   = l;
  return true;
}
////////////////////////////////////////////////////////////////////////////////
void
CAdjustBordersDialog::OnOK()
{
  std::string error;
  if (!ValidateValues(error)) {
    MessageBox(error.c_str(), "Adjust Borders");
    return;
  }
  if (m_TopPixels == 0 && m_RightPixels == 0 && m_BottomPixels == 0 && m_LeftPixels == 0) {
    CDialog::OnCancel();
    return;
  }
  CDialog::OnOK();
}
////////////////////////////////////////////////////////////////////////////////
void
CAdjustBordersDialog::UpdateButtons()
{
  BOOL enabled = TRUE;
  std::string error;
  if (!ValidateValues(error)) {
    enabled = FALSE;
    GetStatusBar()->SetWindowText(error.c_str());
  } else {
    GetStatusBar()->SetWindowText("");
  }
  if (GetDlgItem(IDOK)) {
    GetDlgItem(IDOK)->EnableWindow(enabled); 
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CAdjustBordersDialog::OnOptionChanged()
{
  UpdateButtons();
}
////////////////////////////////////////////////////////////////////////////////
int
CAdjustBordersDialog::GetTopPixels() {
  return m_TopPixels;
}
int
CAdjustBordersDialog::GetRightPixels() {
  return m_RightPixels;
}
int
CAdjustBordersDialog::GetBottomPixels() {
  return m_BottomPixels;
}
int
CAdjustBordersDialog::GetLeftPixels() {
  return m_LeftPixels;
}
////////////////////////////////////////////////////////////////////////////////
