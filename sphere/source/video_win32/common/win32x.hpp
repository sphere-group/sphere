#ifndef WIN32X_HPP
#define WIN32X_HPP

#include <windows.h>
extern BOOL WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, INT nInt, LPCTSTR lpFileName);
extern void CenterWindow(HWND window, int screen_width, int screen_height);

#endif
