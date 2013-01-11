#ifndef MAC_INTERNAL_H
#define MAC_INTERNAL_H

#include "mac_sphere_config.h"
#include "../system.hpp"

// video
extern bool InitVideo(SPHERECONFIG* config);
extern void CloseVideo();

// input
extern void InitInput();
extern void OnKeyDown(int key);
extern void OnKeyUp(int key);

// screenshot
extern bool ShouldTakeScreenshot;
extern void SetScreenshotDirectory(const char* directory);
extern void TakeScreenshot();

// mouse
extern void OnMouseMove(int x, int y);
extern void OnMouseDown(int button);
extern void OnMouseUp(int button);


#endif
