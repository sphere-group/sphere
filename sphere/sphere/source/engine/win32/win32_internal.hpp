#ifndef INTERNAL_H
#define INTERNAL_H
#include <windows.h>
#include "win32_sphere_config.hpp"
#include "../system.hpp"
// video
extern bool InitVideo(HWND window, SPHERECONFIG* config);
extern void CloseVideo();
extern void ToggleFPS();
extern bool ToggleFullScreen();
// audio
extern bool InitAudio(HWND window, SPHERECONFIG* config);
extern void CloseAudio();
// input
extern bool InitInput(HWND window, SPHERECONFIG* config);
extern bool CloseInput();
extern void OnKeyDown(int virtual_key);
extern void OnKeyUp(int virtual_key);
extern void OnMouseMove(int x, int y);
extern void OnMouseDown(int button);
extern void OnMouseUp(int button);
extern void OnMouseWheel(int dist);
// network
extern bool InitNetworkSystem();
extern bool CloseNetworkSystem();
// screenshot
extern bool ShouldTakeScreenshot;
extern void SetScreenshotDirectory(const char* directory);
extern void TakeScreenshot();
#endif
