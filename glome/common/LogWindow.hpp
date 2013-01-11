#ifndef __LOGWINDOW_HPP
#define __LOGWINDOW_HPP


#include <windows.h>
#include "Log.hpp"


#define MAX_STRINGS       500
#define MAX_STRING_LENGTH 1024


class CLogWindow : public CLog
{
public:
  CLogWindow(HINSTANCE instance, const char* caption);
  ~CLogWindow();

private:
  virtual void WriteString(const char* string);
  
  void AddString(const char* string);

  static DWORD WINAPI ThreadRoutine(LPVOID parameter);
  static LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

  void OnVScroll(int code, int pos);
  void OnSize(int cx, int cy);
  void OnPaint(HDC dc);

  void BracketTopString(int min, int max);
  void UpdateScrollBar(int page_size);

private:
  HINSTANCE m_Instance;
  char*     m_Caption;

  HWND  m_Window;
  HFONT m_Font;

  CRITICAL_SECTION StringManagement;

  int  m_TopString;
  int  m_NumStrings;
  char m_CurrentString[MAX_STRING_LENGTH + 1];
  char m_Strings[MAX_STRINGS][MAX_STRING_LENGTH + 1];
};


#endif
