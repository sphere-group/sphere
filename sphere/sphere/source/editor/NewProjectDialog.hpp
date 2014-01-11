#ifndef NEW_PROJECT_DIALOG_HPP
#define NEW_PROJECT_DIALOG_HPP
#include <afxwin.h>
#include <string>
class CNewProjectDialog : public CDialog
{
public:
  CNewProjectDialog(CWnd* pParent);
  ~CNewProjectDialog();
  const char* GetProjectName();
  const char* GetGameTitle();
private:
  bool ValidateValues(std::string& error);
private:
  afx_msg void OnOK();
  afx_msg BOOL OnInitDialog();
private:
  char m_szProjectName[32];
  char m_szGameTitle[32];
};
#endif
