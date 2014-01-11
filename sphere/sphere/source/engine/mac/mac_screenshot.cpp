#include <string>
#include "mac_video.h"
#include "../../common/Image32.hpp"

#ifndef MAX_PATH
#define MAX_PATH 255
#endif

static std::string s_ScreenshotDirectory;
bool ShouldTakeScreenshot;

void SetScreenshotDirectory(const char* directory)
{
    s_ScreenshotDirectory = directory;
}

void TakeScreenshot()
{
    CImage32 screen(GetScreenWidth(), GetScreenHeight());
    DirectGrab(0, 0, GetScreenWidth(), GetScreenHeight(), screen.GetPixels());

    // try to create the screenshot directory
    MakeDirectory(s_ScreenshotDirectory.c_str());

    char filename[MAX_PATH];
    sprintf(filename, "screenshot.png");

    std::string save_path;
    save_path  = s_ScreenshotDirectory + '/';
    save_path += filename;
    screen.Save(save_path.c_str());
}
