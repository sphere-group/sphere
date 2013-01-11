#ifndef CONSOLE_WINDOW_HPP
#define CONSOLE_WINDOW_HPP
#include <afxext.h>
#include "DocumentWindow.hpp"
#include <Scintilla.h>
#include <SciLexer.h>
#include "Scripting.hpp"
class CConsoleWindow 
: public CDocumentWindow
{
public:
  CConsoleWindow();
  ~CConsoleWindow();
private:
  bool Create();
  void Initialize();
  LRESULT SendEditor(UINT msg, WPARAM wparam = 0, LPARAM lparam = 0) {
    return ::SendMessage(__m_Editor__, msg, wparam, lparam);
  }
  LRESULT SendInputBar(UINT msg, WPARAM wparam = 0, LPARAM lparam = 0) {
    return ::SendMessage(__m_InputBar__, msg, wparam, lparam);
  }
  void SetScriptStyles();
  void SetStyle(int style, COLORREF fore, COLORREF back = 0xFFFFFF, int size = -1, const char* face = 0);
  void GetEditorText(CString& text);
  CString GetSelection();
  afx_msg void OnSize(UINT type, int cx, int cy);
  afx_msg void OnSetFocus(CWnd* old);
  afx_msg void OnPosChanged(NMHDR* nmhdr, LRESULT* result);
  afx_msg void OnCharAdded(NMHDR* nmhdr, LRESULT* result);
  afx_msg void OnKeyDown(UINT vk, UINT repeat, UINT flags);
  afx_msg void OnCopy();
  afx_msg void OnPaste();
  void OnToolChanged(UINT id, int tool_index);
  BOOL IsToolAvailable(UINT id);
private:
  void EvaluateString(const char* string);
public:
  void AddString(const char* string)
  {
    SendEditor(SCI_SETREADONLY, 0);
    int num_lines = SendEditor(SCI_GETLINECOUNT) - 1;
    if (num_lines >= 500) {
      SendEditor(SCI_SETTARGETSTART, 0);
      SendEditor(SCI_SETTARGETEND, SendEditor(SCI_LINELENGTH, 0));
      SendEditor(SCI_REPLACETARGET, 0, (WPARAM)"");
    }
 
    if (strlen(string) < 8192) {
      char message[8200] = {0};
      sprintf (message, "%s\n", string);
      SendEditor(SCI_APPENDTEXT, strlen(message), (LPARAM)message);
    }
    else {
      SendEditor(SCI_APPENDTEXT, strlen(string), (LPARAM)string);
      SendEditor(SCI_APPENDTEXT, 1, (LPARAM)"\n");
    }
    SendEditor(SCI_SETREADONLY, 1);
    if ( !(SendEditor(SCI_GETSELECTIONSTART) - SendEditor(SCI_GETSELECTIONEND)) ) {
      SendEditor(SCI_GOTOPOS, SendEditor(SCI_GETTEXTLENGTH));
    }
  }
  void Clear()
  {
    SendEditor(SCI_SETREADONLY, 0);
    SendEditor(SCI_CLEARALL);
    SendEditor(SCI_SETREADONLY, 1);
  }
private:
  static DWORD WINAPI ThreadRoutine(LPVOID parameter);
public:
  bool m_script_running;
  sScripting m_Scripter;
private:
  bool m_Created;
  HWND __m_Editor__;
  HWND __m_InputBar__;
  std::string m_Fontface;
  DECLARE_MESSAGE_MAP()
};
#endif
