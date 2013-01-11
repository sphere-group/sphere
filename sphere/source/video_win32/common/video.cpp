
#include <windows.h>
#include <string.h>
#include "../../common/types.h"
#include "video.hpp"

HINSTANCE DriverInstance;
int       ScreenWidth;
int       ScreenHeight;
RECT      ClippingRectangle;

////////////////////////////////////////////////////////////////////////////////
extern "C" BOOL WINAPI DllMain(HINSTANCE inst, DWORD reason, LPVOID reserved)
{
    DriverInstance = inst;
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) SetClippingRectangle(int x, int y, int w, int h)
{
    int x1 = x;
    int y1 = y;
    int x2 = x + w - 1;
    int y2 = y + h - 1;

    // validate x values
    if (x1 < 0)
        x1 = 0;
    else if (x1 > ScreenWidth - 1)
        x1 = ScreenWidth - 1;

    if (x2 < 0)
        x2 = 0;
    else if (x2 > ScreenWidth - 1)
        x2 = ScreenWidth - 1;

    // validate y values
    if (y1 < 0)
        y1 = 0;
    else if (y1 > ScreenHeight - 1)
        y1 = ScreenHeight - 1;

    if (y2 < 0)
        y2 = 0;
    else if (y2 > ScreenHeight - 1)
        y2 = ScreenHeight - 1;

    SetRect(&ClippingRectangle, x1, y1, x2, y2);
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) GetClippingRectangle(int* x, int* y, int* w, int* h)
{
    *x = ClippingRectangle.left;
    *y = ClippingRectangle.top;
    *w = ClippingRectangle.right - ClippingRectangle.left + 1;
    *h = ClippingRectangle.bottom - ClippingRectangle.top + 1;
}

////////////////////////////////////////////////////////////////////////////////
void GetDriverConfigFile(char* config_file)
{
    GetModuleFileName(DriverInstance, config_file, MAX_PATH);

    // convert driver filename into configuration filename
    if (strrchr(config_file, '.') > strrchr(config_file, '\\'))
        strcpy(strrchr(config_file, '.'), ".cfg");
    else
        strcat(config_file, ".cfg");
}

////////////////////////////////////////////////////////////////////////////////
