#ifndef EDITOR_HPP
#define EDITOR_HPP
#include <string>
#include <afxwin.h>
#include <afxext.h>
#include "MainWindow.hpp"

////////////////////////////////////////////////////////////////////////////////

class CInstanceRepository;
class CEditorApplication : public CWinApp
{
public:
  CEditorApplication();
private:
  BOOL InitInstance();
  int  ExitInstance();	
  CInstanceRepository* m_Instances;
};

////////////////////////////////////////////////////////////////////////////////

extern std::string GetSphereDirectory();
extern void        SetStatusBar(CStatusBar* status_bar);
extern CStatusBar* GetStatusBar();
extern CMainWindow* GetMainWindow();
#endif
