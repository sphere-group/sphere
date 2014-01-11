#include "NewProjectDialog.hpp"
#include "resource.h"
#include <ctype.h>

////////////////////////////////////////////////////////////////////////////////

CNewProjectDialog::CNewProjectDialog(CWnd* pParent) :
  CDialog(IDD_NEW_PROJECT, pParent)
{
  strcpy(m_szProjectName, "");
  strcpy(m_szGameTitle, "");
}

////////////////////////////////////////////////////////////////////////////////

CNewProjectDialog::~CNewProjectDialog()
{
}

////////////////////////////////////////////////////////////////////////////////

const char*
CNewProjectDialog::GetProjectName()
{
  return m_szProjectName;
}

////////////////////////////////////////////////////////////////////////////////

const char*
CNewProjectDialog::GetGameTitle()
{
  return m_szGameTitle;
}

////////////////////////////////////////////////////////////////////////////////

bool
CNewProjectDialog::ValidateValues(std::string& error)
{
  GetDlgItemText(IDC_PROJECTNAME, m_szProjectName, sizeof(m_szProjectName));
  GetDlgItemText(IDC_GAMETITLE,   m_szGameTitle,   sizeof(m_szGameTitle));
  if (strlen(m_szProjectName) == 0) {
    error = "Please enter a project folder name";
    return false;
  }
  for (unsigned int i = 0; i < strlen(m_szProjectName); i++) {
    if (m_szProjectName[i] != '_' && !isalpha(m_szProjectName[i]) && !isdigit(m_szProjectName[i])) {
      error = "Project folder name must contain only A-Z, 0-9 or _";
      return false;
    }
  }
  if (strlen(m_szGameTitle) == 0) {
    error = "Please enter a game title";
    return false;
  }
  return true;
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CNewProjectDialog::OnOK()
{
  std::string error;
  if (!ValidateValues(error)) {
    MessageBox(error.c_str(), "New Project");
    return;
  }
  CDialog::OnOK();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg BOOL
CNewProjectDialog::OnInitDialog()
{
  CDialog::OnInitDialog();
  GetDlgItem(IDC_PROJECTNAME)->SetFocus();
  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
