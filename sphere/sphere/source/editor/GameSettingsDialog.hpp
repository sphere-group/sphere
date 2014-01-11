#ifndef GAME_SETTINGS_DIALOG_HPP
#define GAME_SETTINGS_DIALOG_HPP
#include <afxwin.h>
class CProject; // #include "Project.hpp"
class CGameSettingsDialog : public CDialog
{
public:
  CGameSettingsDialog(CProject* project);
private:
  BOOL OnInitDialog();
  void OnOK();
private:
  CProject* m_Project;
  int m_FoundScript;
};
#endif
