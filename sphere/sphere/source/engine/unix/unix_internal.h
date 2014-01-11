#ifndef INTERNAL_H
#define INTERNAL_H

// video
extern bool InitVideo(SPHERECONFIG* config);
extern void CloseVideo();

// screenshot
extern bool ShouldTakeScreenshot;
extern void SetScreenshotDirectory(const char* directory);
extern void TakeScreenshot();

// mouse
extern void OnMouseMove(int x, int y);
extern void OnMouseDown(int button);
extern void OnMouseUp(int button);


#endif
