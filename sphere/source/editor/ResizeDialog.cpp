#include "ResizeDialog.hpp"
#include "Editor.hpp"
#include "../common/str_util.hpp"
#include "resource.h"

////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CResizeDialog, CDialog)
  ON_EN_CHANGE(IDC_WIDTH, OnOptionChanged)
  ON_EN_CHANGE(IDC_HEIGHT, OnOptionChanged)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////

CResizeDialog::CResizeDialog(const char* caption, int default_width, int default_height)
: CDialog(IDD_RESIZE)
, m_Caption(caption)
, m_Width(default_width)
, m_Height(default_height)
, m_DefaultWidth(default_width)
, m_DefaultHeight(default_height)
, m_MinWidth(0)
, m_MaxWidth(65535)
, m_MinHeight(0)
, m_MaxHeight(65536)
, m_AllowPercentages(true)
{
}

////////////////////////////////////////////////////////////////////////////////

void
CResizeDialog::SetRange(int min_width, int max_width, int min_height, int max_height)
{
  m_MinWidth  = min_width;
  m_MaxWidth  = max_width;
  m_MinHeight = min_height;
  m_MaxHeight = max_height;
}

////////////////////////////////////////////////////////////////////////////////

void
CResizeDialog::AllowPercentages(bool allow)
{
  m_AllowPercentages = allow;
}

////////////////////////////////////////////////////////////////////////////////

int
CResizeDialog::GetWidth() const
{
  return m_Width;
}

////////////////////////////////////////////////////////////////////////////////

int
CResizeDialog::GetHeight() const
{
  return m_Height;
}

////////////////////////////////////////////////////////////////////////////////

BOOL
CResizeDialog::OnInitDialog()
{
  SetWindowText(m_Caption.c_str());
  SetDlgItemInt(IDC_WIDTH,  m_Width);
  SetDlgItemInt(IDC_HEIGHT, m_Height);
  GetDlgItem(IDC_WIDTH)->SetFocus();
  ((CEdit*)GetDlgItem(IDC_WIDTH))->SetSel(0, -1);
 
  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////

bool
CResizeDialog::ValidateValues(std::string& error)
{
  CString width_text, height_text;
  GetDlgItem(IDC_WIDTH)->GetWindowText(width_text);
  GetDlgItem(IDC_HEIGHT)->GetWindowText(height_text);

  int w = atoi(width_text);
  int h = atoi(height_text);
  bool width_percentage = false;
  bool height_percentage = false;
  bool floating = false;

  if (IsInvalidNumber(width_text, floating, width_percentage) || floating
   || IsInvalidNumber(height_text, floating, height_percentage) || floating
   || (!m_AllowPercentages && width_percentage)
   || (!m_AllowPercentages && height_percentage)) {
    error = "Invalid number format";
    return false;
  }

  // convert to percentages
  if (width_percentage) w = (w * m_DefaultWidth) / 100 ;
  if (height_percentage) h = (h * m_DefaultHeight) / 100;
  if (w < m_MinWidth || w > m_MaxWidth ||
      h < m_MinHeight || h > m_MaxHeight)
  {
    char message[520];
    sprintf(message,
      "Width must be between %d and %d.\n"
      "Height must be between %d and %d.",
      m_MinWidth, m_MaxWidth,
      m_MinHeight, m_MaxHeight);
    error = message;
    return false;
  }

  m_Width  = w;
  m_Height = h;
  return true;
}

////////////////////////////////////////////////////////////////////////////////

void
CResizeDialog::UpdateButtons()
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
CResizeDialog::OnOptionChanged()
{
  UpdateButtons();
}

////////////////////////////////////////////////////////////////////////////////

void
CResizeDialog::OnOK()
{
  std::string error;
  if (!ValidateValues(error)) {
    MessageBox(error.c_str(), m_Caption.c_str());
    return;
  }
  CDialog::OnOK();
}

////////////////////////////////////////////////////////////////////////////////
