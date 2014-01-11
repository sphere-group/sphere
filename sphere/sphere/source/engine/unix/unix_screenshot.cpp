#include <string>
//#include "win32_internal.hpp"
#include "unix_video.h"
#include "../../common/Image32.hpp"

#ifndef MAX_PATH
#define MAX_PATH 255
#endif

static std::string s_ScreenshotDirectory;
bool ShouldTakeScreenshot;

void SetScreenshotDirectory(const char* directory) {
  s_ScreenshotDirectory = directory;
}

void TakeScreenshot() {
  CImage32 screen(GetScreenWidth(), GetScreenHeight());
  DirectGrab(0, 0, GetScreenWidth(), GetScreenHeight(), screen.GetPixels());

  // try to create the screenshot directory
  //CreateDirectory(s_ScreenshotDirectory.c_str(), NULL);

  //char old_directory[MAX_PATH];
  //GetCurrentDirectory(MAX_PATH, old_directory);
  //SetCurrentDirectory(s_ScreenshotDirectory.c_str());

  // build a filename based on current date and time
  //SYSTEMTIME st;
  //GetLocalTime(&st);

  char filename[MAX_PATH];
  //sprintf(filename, "screenshot %04d.%02d.%02d %02d.%02d.%02d.%04d.png",
  //  st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

  sprintf(filename, "test.png");

  // now write the screenshot to disk
  screen.Save(filename);

  //SetCurrentDirectory(old_directory);
}
