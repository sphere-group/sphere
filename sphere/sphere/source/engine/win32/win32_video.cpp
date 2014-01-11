
#include <windows.h>
#include <stdio.h>
#include <sstream>

#include "win32_video.hpp"
#include "win32_time.hpp"
#include "win32_internal.hpp"
#include "../sfont.hpp"

static bool InitVDriver(int x, int y);
static void CloseVDriver();

// this function should not be exposed
static void (__fastcall * _FlipScreen)();

void  (__stdcall * SetClippingRectangle)(int x, int y, int w, int h);
void  (__stdcall * GetClippingRectangle)(int* x, int* y, int* w, int* h);
IMAGE (__stdcall * CreateImage)(int width, int height, const RGBA* data);
IMAGE (__stdcall * CloneImage)(IMAGE image);
IMAGE (__stdcall * GrabImage)(int x, int y, int width, int height);
void  (__stdcall * DestroyImage)(IMAGE image);
void  (__stdcall * BlitImage)(IMAGE image, int x, int y, CImage32::BlendMode blendmode);
void  (__stdcall * BlitImageMask)(IMAGE image, int x, int y, CImage32::BlendMode blendmode, RGBA mask, CImage32::BlendMode mask_blendmode);
void  (__stdcall * TransformBlitImage)(IMAGE image, int x[4], int y[4], CImage32::BlendMode blendmode);
void  (__stdcall * TransformBlitImageMask)(IMAGE image, int x[4], int y[4], CImage32::BlendMode blendmode, RGBA mask, CImage32::BlendMode mask_blendmode);
int   (__stdcall * GetImageWidth)(IMAGE image);
int   (__stdcall * GetImageHeight)(IMAGE image);
RGBA* (__stdcall * LockImage)(IMAGE image);
void  (__stdcall * UnlockImage)(IMAGE image, bool pixels_changed);
void  (__stdcall * DirectBlit)(int x, int y, int w, int h, RGBA* pixels);
void  (__stdcall * DirectTransformBlit)(int x[4], int y[4], int w, int h, RGBA* pixels);
void  (__stdcall * DirectGrab)(int x, int y, int w, int h, RGBA* pixels);
void  (__stdcall * DrawPoint)(int x, int y, RGBA color);
void  (__stdcall * DrawPointSeries)(VECTOR_INT** points, int length, RGBA color);
void  (__stdcall * DrawLine)(int x[2], int y[2], RGBA color);
void  (__stdcall * DrawGradientLine)(int x[2], int y[2], RGBA color[2]);
void  (__stdcall * DrawLineSeries)(VECTOR_INT** points, int length, RGBA color, int type);
void  (__stdcall * DrawBezierCurve)(int x[4], int y[4], double step, RGBA color, int cubic);
void  (__stdcall * DrawTriangle)(int x[3], int y[3], RGBA color);
void  (__stdcall * DrawGradientTriangle)(int x[3], int y[3], RGBA color[3]);
void  (__stdcall * DrawPolygon)(VECTOR_INT** points, int length, int invert, RGBA color);
void  (__stdcall * DrawOutlinedRectangle)(int x, int y, int w, int h, int size, RGBA color);
void  (__stdcall * DrawRectangle)(int x, int y, int w, int h, RGBA color);
void  (__stdcall * DrawGradientRectangle)(int x, int y, int w, int h, RGBA color[4]);
void  (__stdcall * DrawOutlinedComplex)(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, RGBA color, int antialias);
void  (__stdcall * DrawFilledComplex)(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, float angle, float frac_size, int fill_empty, RGBA colors[2]);
void  (__stdcall * DrawGradientComplex)(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, float angle, float frac_size, int fill_empty, RGBA colors[3]);
void  (__stdcall * DrawOutlinedEllipse)(int x, int y, int rx, int ry, RGBA color);
void  (__stdcall * DrawFilledEllipse)(int x, int y, int rx, int ry, RGBA color);
void  (__stdcall * DrawOutlinedCircle)(int x, int y, int r, RGBA color, int antialias);
void  (__stdcall * DrawFilledCircle)(int x, int y, int r, RGBA color, int antialias);
void  (__stdcall * DrawGradientCircle)(int x, int y, int r, RGBA color[2], int antialias);

static HWND      SphereWindow   = NULL;
static HINSTANCE GraphicsDriver = NULL;

static int ScreenWidth  = 0;
static int ScreenHeight = 0;

static SFONT* FPSFont = NULL;
static RGBA   fps_color;
static bool   FPSDisplayed = false;

static int VideoCaptureMode = VIDEO_CAPTURE_SCREENSHOT_ONLY;
static int VideoCaptureTimer = 1000;
static int VideoCaptureFrameRate = 20;
static int VideoCaptureFrameStart = 0;
static int VideoCaptureTimerStart = 0;

////////////////////////////////////////////////////////////////////////////////
// helps eliminate warnings
template<typename T, typename U>
void assign(T& dest, U src)
{
    dest = (T&)src;
}

bool InitVideo(HWND window, SPHERECONFIG* config)
{
    SphereWindow          = window;
    VideoCaptureMode      = config->video_capture_mode;
    VideoCaptureTimer     = config->video_capture_timer;
    VideoCaptureFrameRate = config->video_capture_framerate;

    // Loads driver
    std::string graphics_driver =  "system/video/" + config->videodriver;
    GraphicsDriver = LoadLibrary(graphics_driver.c_str());

    if (GraphicsDriver == NULL)
    {
        printf("LoadLibrary failed to load %s\nWindows error code: %d",
               graphics_driver.c_str(),
               GetLastError());

        return false;
    }

    // Gets addresses of all of the graphics functions
    assign(_FlipScreen,            GetProcAddress(GraphicsDriver, "FlipScreen"));
    assign(SetClippingRectangle,   GetProcAddress(GraphicsDriver, "SetClippingRectangle"));
    assign(GetClippingRectangle,   GetProcAddress(GraphicsDriver, "GetClippingRectangle"));
    assign(CreateImage,            GetProcAddress(GraphicsDriver, "CreateImage"));
    assign(CloneImage,             GetProcAddress(GraphicsDriver, "CloneImage"));
    assign(GrabImage,              GetProcAddress(GraphicsDriver, "GrabImage"));
    assign(DestroyImage,           GetProcAddress(GraphicsDriver, "DestroyImage"));
    assign(BlitImage,              GetProcAddress(GraphicsDriver, "BlitImage"));
    assign(BlitImageMask,          GetProcAddress(GraphicsDriver, "BlitImageMask"));
    assign(TransformBlitImage,     GetProcAddress(GraphicsDriver, "TransformBlitImage"));
    assign(TransformBlitImageMask, GetProcAddress(GraphicsDriver, "TransformBlitImageMask"));
    assign(GetImageWidth,          GetProcAddress(GraphicsDriver, "GetImageWidth"));
    assign(GetImageHeight,         GetProcAddress(GraphicsDriver, "GetImageHeight"));
    assign(LockImage,              GetProcAddress(GraphicsDriver, "LockImage"));
    assign(UnlockImage,            GetProcAddress(GraphicsDriver, "UnlockImage"));
    assign(DirectBlit,             GetProcAddress(GraphicsDriver, "DirectBlit"));
    assign(DirectTransformBlit,    GetProcAddress(GraphicsDriver, "DirectTransformBlit"));
    assign(DirectGrab,             GetProcAddress(GraphicsDriver, "DirectGrab"));
    assign(DrawPoint,              GetProcAddress(GraphicsDriver, "DrawPoint"));
    assign(DrawPointSeries,        GetProcAddress(GraphicsDriver, "DrawPointSeries"));
    assign(DrawLine,               GetProcAddress(GraphicsDriver, "DrawLine"));
    assign(DrawGradientLine,       GetProcAddress(GraphicsDriver, "DrawGradientLine"));
    assign(DrawLineSeries,         GetProcAddress(GraphicsDriver, "DrawLineSeries"));
    assign(DrawBezierCurve,        GetProcAddress(GraphicsDriver, "DrawBezierCurve"));
    assign(DrawTriangle,           GetProcAddress(GraphicsDriver, "DrawTriangle"));
    assign(DrawGradientTriangle,   GetProcAddress(GraphicsDriver, "DrawGradientTriangle"));
    assign(DrawPolygon,            GetProcAddress(GraphicsDriver, "DrawPolygon"));
    assign(DrawOutlinedRectangle,  GetProcAddress(GraphicsDriver, "DrawOutlinedRectangle"));
    assign(DrawRectangle,          GetProcAddress(GraphicsDriver, "DrawRectangle"));
    assign(DrawGradientRectangle,  GetProcAddress(GraphicsDriver, "DrawGradientRectangle"));
    assign(DrawOutlinedComplex,    GetProcAddress(GraphicsDriver, "DrawOutlinedComplex"));
    assign(DrawFilledComplex,      GetProcAddress(GraphicsDriver, "DrawFilledComplex"));
    assign(DrawGradientComplex,    GetProcAddress(GraphicsDriver, "DrawGradientComplex"));
    assign(DrawOutlinedEllipse,    GetProcAddress(GraphicsDriver, "DrawOutlinedEllipse"));
    assign(DrawFilledEllipse,      GetProcAddress(GraphicsDriver, "DrawFilledEllipse"));
    assign(DrawOutlinedCircle,     GetProcAddress(GraphicsDriver, "DrawOutlinedCircle"));
    assign(DrawFilledCircle,       GetProcAddress(GraphicsDriver, "DrawFilledCircle"));
    assign(DrawGradientCircle,     GetProcAddress(GraphicsDriver, "DrawGradientCircle"));

    // ensure that we got all entry points
    if (!_FlipScreen ||
        !SetClippingRectangle ||
        !GetClippingRectangle ||
        !CreateImage ||
        !CloneImage ||
        !GrabImage ||
        !DestroyImage ||
        !BlitImage ||
        !BlitImageMask ||
        !TransformBlitImage ||
        !TransformBlitImageMask ||
        !GetImageWidth ||
        !GetImageHeight ||
        !LockImage ||
        !UnlockImage ||
        !DirectBlit ||
        !DirectTransformBlit ||
        !DirectGrab ||
        !DrawPoint ||
        !DrawPointSeries ||
        !DrawLine ||
        !DrawGradientLine ||
        !DrawLineSeries ||
        !DrawBezierCurve ||
        !DrawTriangle ||
        !DrawGradientTriangle ||
        !DrawPolygon ||
        !DrawOutlinedRectangle ||
        !DrawRectangle ||
        !DrawGradientRectangle ||
        !DrawOutlinedComplex ||
        !DrawFilledComplex ||
        !DrawGradientComplex ||
        !DrawOutlinedEllipse ||
        !DrawFilledEllipse ||
        !DrawOutlinedCircle ||
        !DrawFilledCircle ||
        !DrawGradientCircle)
    {
        puts("Couldn't get all entry points");
        FreeLibrary(GraphicsDriver);
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
void CloseVideo()
{
    if (GraphicsDriver)
    {
        CloseVDriver();
        FreeLibrary(GraphicsDriver);
    }
}

////////////////////////////////////////////////////////////////////////////////
void SetFPSFont(SFONT* font)
{
    FPSFont = font;
}

////////////////////////////////////////////////////////////////////////////////
void ToggleFPS()
{
    FPSDisplayed = !FPSDisplayed;
}

////////////////////////////////////////////////////////////////////////////////
bool SetWindowTitle(const char* text)
{

    return (SetWindowText(SphereWindow, text) == TRUE);
}

////////////////////////////////////////////////////////////////////////////////
bool InitVDriver(int w, int h)
{
    bool (__stdcall * init_video_driver)(HWND window, int screen_width, int screen_height);

    ScreenWidth  = w;
    ScreenHeight = h;

    // Gets address of driver init func
    init_video_driver = (bool (__stdcall *)(HWND, int, int))GetProcAddress(GraphicsDriver, "InitVideoDriver");

    if (init_video_driver == NULL)
        return false;

    return init_video_driver(SphereWindow, ScreenWidth, ScreenHeight);
}

////////////////////////////////////////////////////////////////////////////////
void CloseVDriver()
{
    void (__stdcall * close_video_driver)();

    // Gets address of
    close_video_driver = (void (__stdcall *)())GetProcAddress(GraphicsDriver, "CloseVideoDriver");

    if (close_video_driver == NULL)
        return;

    close_video_driver();
}

////////////////////////////////////////////////////////////////////////////////
bool ToggleFullScreen()
{
    bool (__stdcall * toggle_fullscreen)();

    toggle_fullscreen = (bool (__stdcall *)())GetProcAddress(GraphicsDriver, "ToggleFullScreen");

    if (toggle_fullscreen == NULL)
        return false;

    return toggle_fullscreen();
}

////////////////////////////////////////////////////////////////////////////////
bool SwitchResolution(int x, int y)
{
    if (x == ScreenWidth && y == ScreenHeight)
        return true;

    // if we haven't set a screen size, don't close the old driver
    if (ScreenWidth != 0 || ScreenHeight != 0)
        CloseVDriver();

    if (!InitVDriver(x, y))
    {
        FreeLibrary(GraphicsDriver);
        QuitMessage("Could not switch resolutions.  Try configuring\n"
                    "Sphere to run in a window (non-fullscreen.)");
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
inline int GetScreenWidth()
{
    return ScreenWidth;
}

////////////////////////////////////////////////////////////////////////////////
inline int GetScreenHeight()
{
    return ScreenHeight;
}

////////////////////////////////////////////////////////////////////////////////
inline void FlipScreen()
{
    static bool FirstCall    = true;
    static int LastUpdate    = 0;
    static int FPS           = 0;
    static int CurrentFrames = 0;

    // initialize variables
    if (FirstCall)
    {
        LastUpdate = GetTime();
        FPS = 0;
        CurrentFrames = 0;
        FirstCall = false;
    }

    // display FPS
    if (FPSDisplayed && FPSFont)
    {

        // update FPS
        if (GetTime() > dword(LastUpdate + 1000))
        {

            FPS = CurrentFrames;
            CurrentFrames = 0;
            LastUpdate = GetTime();
        }
        char fps[32];
        sprintf(fps, "FPS: %d", FPS);
        FPSFont->DrawString(0, 0, fps, CreateRGBA(255, 255, 255, 255));
    }

    CurrentFrames++;
    static int NumFlips;

    if (NumFlips++ % 8 == 0)
        UpdateSystem();

    if (ShouldTakeScreenshot)
    {

        if (VideoCaptureTimerStart == 0)
        {

            VideoCaptureTimerStart = GetTime();
        }

        if (VideoCaptureMode == VIDEO_CAPTURE_SCREENSHOT_ONLY
                || VideoCaptureFrameStart++ % VideoCaptureFrameRate == 0)
        {

            TakeScreenshot();
        }

        if ( !(VideoCaptureMode == VIDEO_CAPTURE_UNTIL_F12_KEYED ||
               VideoCaptureMode == VIDEO_CAPTURE_UNTIL_OUTOFTIME ))
        {

            ShouldTakeScreenshot = false;
        }

        if (VideoCaptureMode == VIDEO_CAPTURE_UNTIL_OUTOFTIME)
        {

            if ((unsigned int)(VideoCaptureTimerStart + VideoCaptureTimer) < GetTime())
            {

                ShouldTakeScreenshot = false;
            }
        }

        if (!ShouldTakeScreenshot)
        {

            VideoCaptureTimerStart = 0;
            VideoCaptureFrameStart = 0;
        }
    }

    _FlipScreen();
}

////////////////////////////////////////////////////////////////////////////////
