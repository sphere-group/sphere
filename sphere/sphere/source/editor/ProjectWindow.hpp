#ifndef PROJECT_WINDOW_HPP
#define PROJECT_WINDOW_HPP

#ifndef USE_SIZECBAR
#ifdef FLOATING_PROJECT_WINDOW
#undef FLOATING_PROJECT_WINDOW
#endif
#endif

#include <afxwin.h>
#include <afxcmn.h>

#ifdef FLOATING_PROJECT_WINDOW
#include "PaletteWindow.hpp"
#endif

class CMainWindow; // MainWindow.hpp
class CProject;    // Project.hpp

#ifdef FLOATING_PROJECT_WINDOW
#define BaseProjectWindow CPaletteWindow
#else
#define BaseProjectWindow CMDIChildWnd
#endif

class CProjectWindow : public BaseProjectWindow
{
public:
  CProjectWindow(CMainWindow* main_window, CProject* project);
  
  BOOL Create();
  BOOL DestroyWindow();

  void Update();
private:
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnSize(UINT type, int cx, int cy);
  afx_msg void OnSetFocus(CWnd* pOldWindow);
  afx_msg void OnDropFiles(HDROP drop_info);

  afx_msg void OnProjectGroupNew();
  afx_msg void OnProjectGroupInsert();

  afx_msg void OnProjectItemOpen();
  afx_msg void OnProjectItemDelete();

  afx_msg void OnKeyDown(NMHDR* notify, LRESULT* result);
  afx_msg void OnDoubleClick(NMHDR* notify, LRESULT* result);
  afx_msg void OnRightClick(NMHDR* notify, LRESULT* result);

  void __OnRightClick__();
  void __OnDoubleClick__(bool allow_expand);

  void SelectItemAtCursor();
private:
  CMainWindow* m_MainWindow;
  CProject*    m_Project;

  CTreeCtrl m_TreeControl;
  DECLARE_MESSAGE_MAP()
};

#endif
