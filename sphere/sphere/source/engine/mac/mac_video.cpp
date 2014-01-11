#include <dlfcn.h>
#include <string>

#include "mac_video.h"
#include "mac_input.h"
#include "mac_time.h"
#include "mac_internal.h"
#include "../sfont.hpp"


static int    ScreenWidth  = 0;
static int    ScreenHeight = 0;

static SFONT* FPSFont;
static bool   FPSDisplayed;

static void*  GraphicsDriver;


// this function should not be exposed
static void (STDCALL * _FlipScreen)();

void  (STDCALL * SetClippingRectangle)(int x, int y, int w, int h);
void  (STDCALL * GetClippingRectangle)(int* x, int* y, int* w, int* h);
IMAGE (STDCALL * CreateImage)(int width, int height, const RGBA* data);
IMAGE (STDCALL * CloneImage)(IMAGE image);
IMAGE (STDCALL * GrabImage)(int x, int y, int width, int height);
void  (STDCALL * DestroyImage)(IMAGE image);
void  (STDCALL * BlitImage)(IMAGE image, int x, int y, CImage32::BlendMode blendmode);
void  (STDCALL * BlitImageMask)(IMAGE image, int x, int y, CImage32::BlendMode blendmode, RGBA mask, CImage32::BlendMode mask_blendmode);
void  (STDCALL * TransformBlitImage)(IMAGE image, int x[4], int y[4], CImage32::BlendMode blendmode);
void  (STDCALL * TransformBlitImageMask)(IMAGE image, int x[4], int y[4], CImage32::BlendMode blendmode, RGBA mask, CImage32::BlendMode mask_blendmode);
int   (STDCALL * GetImageWidth)(IMAGE image);
int   (STDCALL * GetImageHeight)(IMAGE image);
RGBA* (STDCALL * LockImage)(IMAGE image);
void  (STDCALL * UnlockImage)(IMAGE image, bool pixels_changed);
void  (STDCALL * DirectBlit)(int x, int y, int w, int h, RGBA* pixels);
void  (STDCALL * DirectTransformBlit)(int x[4], int y[4], int w, int h, RGBA* pixels);
void  (STDCALL * DirectGrab)(int x, int y, int w, int h, RGBA* pixels);
void  (STDCALL * DrawPoint)(int x, int y, RGBA color);
void  (STDCALL * DrawPointSeries)(VECTOR_INT** points, int length, RGBA color);
void  (STDCALL * DrawLine)(int x[2], int y[2], RGBA color);
void  (STDCALL * DrawGradientLine)(int x[2], int y[2], RGBA color[2]);
void  (STDCALL * DrawLineSeries)(VECTOR_INT** points, int length, RGBA color, int type);
void  (STDCALL * DrawBezierCurve)(int x[4], int y[4], double step, RGBA color, int cubic);
void  (STDCALL * DrawTriangle)(int x[3], int y[3], RGBA color);
void  (STDCALL * DrawGradientTriangle)(int x[3], int y[3], RGBA color[3]);
void  (STDCALL * DrawPolygon)(VECTOR_INT** points, int length, int invert, RGBA color);
void  (STDCALL * DrawOutlinedRectangle)(int x, int y, int w, int h, int size, RGBA color);
void  (STDCALL * DrawRectangle)(int x, int y, int w, int h, RGBA color);
void  (STDCALL * DrawGradientRectangle)(int x, int y, int w, int h, RGBA color[4]);
void  (STDCALL * DrawOutlinedComplex)(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, RGBA color, int antialias);
void  (STDCALL * DrawFilledComplex)(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, float angle, float frac_size, int fill_empty, RGBA colors[2]);
void  (STDCALL * DrawGradientComplex)(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, float angle, float frac_size, int fill_empty, RGBA colors[3]);
void  (STDCALL * DrawOutlinedEllipse)(int x, int y, int rx, int ry, RGBA color);
void  (STDCALL * DrawFilledEllipse)(int x, int y, int rx, int ry, RGBA color);
void  (STDCALL * DrawOutlinedCircle)(int x, int y, int r, RGBA color, int antialias);
void  (STDCALL * DrawFilledCircle)(int x, int y, int r, RGBA color, int antialias);
void  (STDCALL * DrawGradientCircle)(int x, int y, int r, RGBA color[2], int antialias);

////////////////////////////////////////////////////////////////////////////////
// helps eliminate warnings
template<typename T, typename U>
void assign(T& dest, U src)
{
    dest = (T&)src;
}

bool InitVideo(SPHERECONFIG* config)
{

    // load the video driver
    std::string driver_path = GetSphereDirectory() + "/system/video/" + config->videodriver;
    GraphicsDriver = dlopen(driver_path.c_str(), RTLD_LAZY);

    if (GraphicsDriver == NULL)
    {
        fprintf(stderr, "Could not load '%s':\n%s\n", config->videodriver.c_str(), dlerror());
        return false;
    }

    // get addresses of all of the graphics functions
    assign(_FlipScreen,            dlsym(GraphicsDriver, "FlipScreen"));
    assign(SetClippingRectangle,   dlsym(GraphicsDriver, "SetClippingRectangle"));
    assign(GetClippingRectangle,   dlsym(GraphicsDriver, "GetClippingRectangle"));
    assign(CreateImage,            dlsym(GraphicsDriver, "CreateImage"));
    assign(CloneImage,             dlsym(GraphicsDriver, "CloneImage"));
    assign(GrabImage,              dlsym(GraphicsDriver, "GrabImage"));
    assign(DestroyImage,           dlsym(GraphicsDriver, "DestroyImage"));
    assign(BlitImage,              dlsym(GraphicsDriver, "BlitImage"));
    assign(BlitImageMask,          dlsym(GraphicsDriver, "BlitImageMask"));
    assign(TransformBlitImage,     dlsym(GraphicsDriver, "TransformBlitImage"));
    assign(TransformBlitImageMask, dlsym(GraphicsDriver, "TransformBlitImageMask"));
    assign(GetImageWidth,          dlsym(GraphicsDriver, "GetImageWidth"));
    assign(GetImageHeight,         dlsym(GraphicsDriver, "GetImageHeight"));
    assign(LockImage,              dlsym(GraphicsDriver, "LockImage"));
    assign(UnlockImage,            dlsym(GraphicsDriver, "UnlockImage"));
    assign(DirectBlit,             dlsym(GraphicsDriver, "DirectBlit"));
    assign(DirectTransformBlit,    dlsym(GraphicsDriver, "DirectTransformBlit"));
    assign(DirectGrab,             dlsym(GraphicsDriver, "DirectGrab"));
    assign(DrawPoint,              dlsym(GraphicsDriver, "DrawPoint"));
    assign(DrawPointSeries,        dlsym(GraphicsDriver, "DrawPointSeries"));
    assign(DrawLine,               dlsym(GraphicsDriver, "DrawLine"));
    assign(DrawGradientLine,       dlsym(GraphicsDriver, "DrawGradientLine"));
    assign(DrawLineSeries,         dlsym(GraphicsDriver, "DrawLineSeries"));
    assign(DrawBezierCurve,        dlsym(GraphicsDriver, "DrawBezierCurve"));
    assign(DrawTriangle,           dlsym(GraphicsDriver, "DrawTriangle"));
    assign(DrawGradientTriangle,   dlsym(GraphicsDriver, "DrawGradientTriangle"));
    assign(DrawPolygon,            dlsym(GraphicsDriver, "DrawPolygon"));
    assign(DrawOutlinedRectangle,  dlsym(GraphicsDriver, "DrawOutlinedRectangle"));
    assign(DrawRectangle,          dlsym(GraphicsDriver, "DrawRectangle"));
    assign(DrawGradientRectangle,  dlsym(GraphicsDriver, "DrawGradientRectangle"));
    assign(DrawOutlinedComplex,    dlsym(GraphicsDriver, "DrawOutlinedComplex"));
    assign(DrawFilledComplex,      dlsym(GraphicsDriver, "DrawFilledComplex"));
    assign(DrawGradientComplex,    dlsym(GraphicsDriver, "DrawGradientComplex"));
    assign(DrawOutlinedEllipse,    dlsym(GraphicsDriver, "DrawOutlinedEllipse"));
    assign(DrawFilledEllipse,      dlsym(GraphicsDriver, "DrawFilledEllipse"));
    assign(DrawOutlinedCircle,     dlsym(GraphicsDriver, "DrawOutlinedCircle"));
    assign(DrawFilledCircle,       dlsym(GraphicsDriver, "DrawFilledCircle"));
    assign(DrawGradientCircle,     dlsym(GraphicsDriver, "DrawGradientCircle"));

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
        dlclose(GraphicsDriver);
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
void CloseVideo()
{
    if (GraphicsDriver)
    {
        void (STDCALL * close_video)();

        close_video = (void (STDCALL *)())dlsym(GraphicsDriver, "CloseVideo");

        if (close_video != NULL)
            close_video();

        dlclose(GraphicsDriver);
    }
}

////////////////////////////////////////////////////////////////////////////////
void SetFPSFont (SFONT* font)
{
    FPSFont = font;
}

////////////////////////////////////////////////////////////////////////////////
void ToggleFPS ()
{
    FPSDisplayed = !FPSDisplayed;
}

////////////////////////////////////////////////////////////////////////////////
bool SetWindowTitle(const char* text)
{
    bool (STDCALL * set_window_title)(const char* text);

    set_window_title = (bool (STDCALL *)(const char*))dlsym(GraphicsDriver, "SetWindowTitle");

    if (set_window_title == NULL)
        return false;

    return set_window_title(text);
}

////////////////////////////////////////////////////////////////////////////////
void ToggleFullScreen()
{
    bool (STDCALL * toggle_fullscreen)();

    toggle_fullscreen = (bool (STDCALL *)())dlsym(GraphicsDriver, "ToggleFullScreen");

    if (toggle_fullscreen != NULL)
        toggle_fullscreen();

}

////////////////////////////////////////////////////////////////////////////////
bool SwitchResolution (int w, int h)
{
    ScreenWidth  = w;
    ScreenHeight = h;

    bool (STDCALL * init_video)(int w, int h, std::string sphere_dir);

    init_video = (bool (STDCALL *)(int, int, std::string))dlsym(GraphicsDriver, "InitVideo");

    if (init_video == NULL)
        return false;

    return init_video(w, h, GetSphereDirectory());
}

////////////////////////////////////////////////////////////////////////////////
int GetScreenWidth()
{
    return ScreenWidth;
}

////////////////////////////////////////////////////////////////////////////////
int GetScreenHeight()
{
    return ScreenHeight;
}

////////////////////////////////////////////////////////////////////////////////
void FlipScreen ()
{
    static bool initialized = false;
    static int LastUpdate;
    static int FPS;
    static int CurrentFrames;

    if (!initialized)
    {
        LastUpdate    = GetTime();
        FPS           = 0;
        CurrentFrames = 0;
        initialized   = true;
    }

    if (FPSFont && FPSDisplayed)
    {
        if (GetTime() > LastUpdate + 1000)
        {
            LastUpdate    = GetTime();
            FPS           = CurrentFrames;
            CurrentFrames = 0;
        }

        char fps[80];
        sprintf(fps, "FPS: %d", FPS);
        FPSFont->DrawString(0, 0, fps, CreateRGBA(255, 255, 255, 255));
    }

    CurrentFrames++;
    static int NumFlips;

    if (NumFlips++ % 8 == 0)
        UpdateSystem();

    if (ShouldTakeScreenshot)
    {
        ShouldTakeScreenshot = false;
        TakeScreenshot();
    }

    _FlipScreen();

}

////////////////////////////////////////////////////////////////////////////////
