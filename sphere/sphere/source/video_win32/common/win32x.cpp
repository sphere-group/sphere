
#include <stdio.h>
#include "win32x.hpp"

////////////////////////////////////////////////////////////////////////////////
BOOL WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, INT nInt, LPCTSTR lpFileName)
{
    char str[80];
    sprintf(str, "%d", nInt);
    return WritePrivateProfileString(lpAppName, lpKeyName, str, lpFileName);
}
////////////////////////////////////////////////////////////////////////////////
void CenterWindow(HWND window, int screen_width, int screen_height)
{

    // center the window
    RECT WindowRect = { 0, 0, screen_width, screen_height };
    AdjustWindowRectEx(
        &WindowRect,
        GetWindowLong(window, GWL_STYLE),
        (GetMenu(window) ? TRUE : FALSE),
        GetWindowLong(window, GWL_EXSTYLE));
    int window_width  = WindowRect.right - WindowRect.left;
    int window_height = WindowRect.bottom - WindowRect.top;
    MoveWindow(
        window,
        (GetSystemMetrics(SM_CXSCREEN) - window_width) / 2,
        (GetSystemMetrics(SM_CYSCREEN) - window_height) / 2,
        window_width,
        window_height,
        TRUE);
}
////////////////////////////////////////////////////////////////////////////////
