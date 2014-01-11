#define DIRECTDRAW_VERSION 0x0300

// we need to define it before including windows.h to suppress the min/max macros
#define NOMINMAX

#include <windows.h>
#include <ddraw.h>
#include <stdio.h>

#include "2xSaI.h"
#include "hq2x.h"
#include "scale.h"

#include "../../common/Image32.hpp"
#include "../../common/rgb.hpp"
#include "../../common/primitives.hpp"
#include "../common/win32x.hpp"
#include "../common/video.hpp"
#include "resource.h"


typedef struct _IMAGE
{
    int width;
    int height;

    word* rgb;
    byte* alpha;

    void (*blit_routine)(_IMAGE* image, int x, int y);

    RGBA* locked_pixels;

    int pixel_format;

} *IMAGE;

enum SCALING_FILTER
{
    I_NONE = 0,
    I_SCALE2X,
    I_EAGLE,
    I_HQ2X,
    I_2XSAI,
    I_SUPER_2XSAI,
    I_SUPER_EAGLE,
};

typedef struct _screen_border
{
    int top;
    int bottom;
    int left;
    int right;

} screen_border;

class VideoConfiguration
{
    public:

        VideoConfiguration() {};

        int GetExWidth(int width)
        {
            return width + border.left + border.right;
        }

        int GetExHeight(int height)
        {
            return height + border.top + border.bottom;
        }

        int GetOffset(int pitch)
        {
            return pitch * border.top + border.left;
        }

        bool fullscreen;
        bool vsync;

        bool scale;
        int  filter;

        screen_border border;

};

// FUNCTION PROTOTYPES //

EXPORT(RGBA*) LockImage(IMAGE image);
EXPORT(void)  UnlockImage(IMAGE image, bool pixels_changed);

static void LoadConfiguration();

static bool InitFullscreen();
static bool SetDisplayMode();
static bool CreateSurfaces();
static bool InitWindowed();

static void CloseFullscreen();
static void CloseWindowed();

static void Scale(word* dst, int dst_pitch);

static bool FillImagePixels(IMAGE image, RGBA* pixels);
static void OptimizeBlitRoutine(IMAGE image);

static void NullBlit(IMAGE image, int x, int y);
static void TileBlit(IMAGE image, int x, int y);
static void SpriteBlit(IMAGE image, int x, int y);
static void NormalBlit(IMAGE image, int x, int y);
static void AddBlit(IMAGE image, int x, int y);
static void SubtractBlit(IMAGE image, int x, int y);
static void MultiplyBlit(IMAGE image, int x, int y);


// INLINE FUNCTIONS //

// like 255 - a only faster
inline byte InvertAlpha(byte a)
{
    return a ^ 0xFF;
}

inline word PackPixel565(RGBA pixel)
{
    return (word)(((pixel.red   >> 3) << 11) +
                  ((pixel.green >> 2) <<  5) +
                  ((pixel.blue  >> 3) <<  0));
}

inline RGBA UnpackPixel565(word pixel)
{
    RGBA rgba = {
                    ((pixel & 0xF800) >> 11) << 3, // 11111 000000 00000
                    ((pixel & 0x07E0) >>  5) << 2, // 00000 111111 00000
                    ((pixel & 0x001F) >>  0) << 3, // 00000 000000 11111
                };
    return rgba;
}

inline word PackPixel555(RGBA pixel)
{
    return (word)(
               ((pixel.red   >> 3) << 10) +
               ((pixel.green >> 3) << 5) +
               ((pixel.blue  >> 3) << 0));
}

inline RGBA UnpackPixel555(word pixel)
{
    RGBA rgba = {
                    ((pixel & 0x7C00) >> 10) << 3, // 0 11111 00000 00000
                    ((pixel & 0x03E0) >>  5) << 3, // 0 00000 11111 00000
                    ((pixel & 0x001F) >>  0) << 3, // 0 00000 00000 11111
                };
    return rgba;
}

// GLOBAL VARIABLES

static enum
{
    RGB565,
    RGB555,

} PixelFormat;


static VideoConfiguration Config;

static HWND  SphereWindow        = NULL;
static word* ScreenBuffer        = NULL;
static word* ScreenBufferSection = NULL;

static int   ScreenBufferWidth  = 0;
static int   ScreenBufferHeight = 0;
static int   ScreenScaleFactor  = 1;

static LONG OldWindowStyle;
static LONG OldWindowStyleEx;

// fullscreen output
static LPDIRECTDRAW        dd;
static LPDIRECTDRAWSURFACE ddPrimary;
static LPDIRECTDRAWSURFACE ddSecondary;

// windowed output
static HDC     RenderDC;
static HBITMAP RenderBitmap;
static word*   RenderBuffer;


////////////////////////////////////////////////////////////////////////////////
EXPORT(void) GetDriverInfo(DRIVERINFO* driverinfo)
{
    driverinfo->name   = "Standard16";
    driverinfo->author = "Chad Austin, Anatoli Steinmark";
    driverinfo->date   = __DATE__;
    driverinfo->version = "v1.3";
    driverinfo->description = "16-bit Software Sphere Video Driver";
}

////////////////////////////////////////////////////////////////////////////////
void LoadConfiguration()
{
    char config_file_name[MAX_PATH];
    GetDriverConfigFile(config_file_name);

    // load the fields from the file
    Config.fullscreen = GetPrivateProfileInt("standard16", "Fullscreen", 0, config_file_name) != 0;
    Config.vsync      = GetPrivateProfileInt("standard16", "VSync",      1, config_file_name) != 0;
    Config.scale      = GetPrivateProfileInt("standard16", "Scale",      0, config_file_name) != 0;
    Config.filter     = GetPrivateProfileInt("standard16", "Filter",     0, config_file_name);

    int vexpand              = GetPrivateProfileInt("standard16", "VExpand",    0, config_file_name);
    int hexpand              = GetPrivateProfileInt("standard16", "HExpand",    0, config_file_name);

    if (vexpand < 0 || vexpand > 1024)
        vexpand = 0;

    if (hexpand < 0 || hexpand > 1024)
        hexpand = 0;

    Config.border.top    = (int)(vexpand / 2);
    Config.border.bottom = (int)(vexpand / 2) + (vexpand % 2);
    Config.border.left   = (int)(hexpand / 2);
    Config.border.right  = (int)(hexpand / 2) + (hexpand % 2);

}

////////////////////////////////////////////////////////////////////////////////
EXPORT(bool) InitVideoDriver(HWND window, int screen_width, int screen_height)
{
    SphereWindow = window;
    ScreenWidth  = screen_width;
    ScreenHeight = screen_height;

    SetClippingRectangle(0, 0, screen_width, screen_height);

    static bool firstcall = true;

    if (firstcall)
    {
        LoadConfiguration();
        ScreenScaleFactor  = Config.scale ? 2 : 1;
        firstcall = false;
    }

    if (Config.fullscreen)
        return InitFullscreen();
    else
        return InitWindowed();

    return false;
}

////////////////////////////////////////////////////////////////////////////////
bool InitFullscreen()
{
    // initialize the screen buffer variables and allocate a blitting buffer
    ScreenBufferWidth   = Config.GetExWidth(ScreenWidth);
    ScreenBufferHeight  = Config.GetExHeight(ScreenHeight);
    ScreenBuffer        = new word[ScreenBufferWidth * ScreenBufferHeight];
    ScreenBufferSection = ScreenBuffer + Config.GetOffset(ScreenBufferWidth);

    if (ScreenBuffer == NULL)
        return false;

    memset((byte*)ScreenBuffer, 0, ScreenBufferWidth * ScreenBufferHeight * 2);


    HRESULT ddrval;
    bool    retval;

    // store old window styles
    OldWindowStyle   = GetWindowLong(SphereWindow, GWL_STYLE);
    OldWindowStyleEx = GetWindowLong(SphereWindow, GWL_EXSTYLE);

    SetWindowLong(SphereWindow, GWL_STYLE, WS_POPUP);
    SetWindowLong(SphereWindow, GWL_EXSTYLE, 0);

    // create DirectDraw object
    ddrval = DirectDrawCreate(NULL, &dd, NULL);
    if (ddrval != DD_OK)
    {
        MessageBox(SphereWindow, "DirectDrawCreate() failed", "standard16", MB_OK);
        return false;
    }

    // set application behavior
    ddrval = dd->SetCooperativeLevel(SphereWindow, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
    if (ddrval != DD_OK)
    {
        dd->Release();
        MessageBox(SphereWindow, "SetCooperativeLevel() failed", "standard16", MB_OK);
        return false;
    }

    // set display mode
    retval = SetDisplayMode();
    if (retval == false)
    {
        dd->Release();
        MessageBox(SphereWindow, "SetDisplayMode() failed", "standard16", MB_OK);
        return false;
    }

    // create surfaces
    retval = CreateSurfaces();
    if (retval == false)
    {
        dd->Release();
        MessageBox(SphereWindow, "CreateSurfaces() failed", "standard16", MB_OK);
        return false;
    }

    ShowCursor(FALSE);

    SetWindowPos(SphereWindow, HWND_TOPMOST, 0, 0,
                 ScreenBufferWidth  * ScreenScaleFactor,
                 ScreenBufferHeight * ScreenScaleFactor,
                 SWP_SHOWWINDOW);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(bool) ToggleFullScreen()
{
    int x, y, w, h;
    GetClippingRectangle(&x, &y, &w, &h);

    // if we have a screen size, close the old driver
    if (ScreenWidth != 0 || ScreenHeight != 0)
    {

        if (Config.fullscreen)
        {
            CloseFullscreen();
        }
        else
        {
            CloseWindowed();
        }
    }

    Config.fullscreen = !Config.fullscreen;

    if (InitVideoDriver(SphereWindow, ScreenWidth, ScreenHeight) == true)
    {
        SetClippingRectangle(x, y, w, h);
        return true;
    }
    else
    {

        // switching failed, try to revert to what it was
        Config.fullscreen = !Config.fullscreen;
        if (InitVideoDriver(SphereWindow, ScreenWidth, ScreenHeight) == true)
        {
            SetClippingRectangle(x, y, w, h);
            return true;
        }
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
bool SetDisplayMode()
{
    HRESULT ddrval;

    ddrval = dd->SetDisplayMode(ScreenBufferWidth  * ScreenScaleFactor,
                                ScreenBufferHeight * ScreenScaleFactor,
                                16);

    if (ddrval != DD_OK)
        return false;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool CreateSurfaces()
{
    // define the surface
    DDSURFACEDESC ddsd;
    ddsd.dwSize = sizeof(ddsd);

    if (Config.vsync)
    {
        ddsd.dwFlags           = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
        ddsd.ddsCaps.dwCaps    = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
        ddsd.dwBackBufferCount = 1;
    }
    else
    {
        ddsd.dwFlags        = DDSD_CAPS;
        ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    }

    // create the primary surface
    HRESULT ddrval = dd->CreateSurface(&ddsd, &ddPrimary, NULL);

    if (ddrval != DD_OK)
        return false;

    if (Config.vsync)
    {
        ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
        ddrval = ddPrimary->GetAttachedSurface(&ddsd.ddsCaps, &ddSecondary);
        if (ddrval != DD_OK)
        {
            ddPrimary->Release();
            return false;
        }
    }

    // determine bits per pixel
    DDPIXELFORMAT ddpf;
    ddpf.dwSize = sizeof(ddpf);
    ddpf.dwFlags = DDPF_RGB;
    ddrval = ddPrimary->GetPixelFormat(&ddpf);
    if (ddrval != DD_OK)
    {
        dd->Release();
        return false;
    }

    // 5:6:5 -- F800 07E0 001F
    // 5:5:5 -- 7C00 03E0 001F

    if (ddpf.dwRBitMask == 0xF800)
    {
        PixelFormat = RGB565;
    }
    else if (ddpf.dwRBitMask == 0x7C00)
    {
        PixelFormat = RGB555;
    }
    else
    {
        dd->Release();
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool InitWindowed()
{
    // initialize the screen buffer variables and allocate a blitting buffer
    ScreenBufferWidth   = ScreenWidth;
    ScreenBufferHeight  = ScreenHeight;
    ScreenBuffer        = new word[ScreenBufferWidth * ScreenBufferHeight];
    ScreenBufferSection = ScreenBuffer;

    if (ScreenBuffer == NULL)
        return false;

    memset((byte*)ScreenBuffer, 0, ScreenBufferWidth * ScreenBufferHeight * 2);


    // create the render DC
    RenderDC = CreateCompatibleDC(NULL);
    if (RenderDC == NULL)
        return false;

    // define/create the DIB section
    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));
    BITMAPINFOHEADER& bmih = bmi.bmiHeader;
    bmih.biSize        = sizeof(bmih);
    bmih.biWidth       =  ScreenBufferWidth  * ScreenScaleFactor;
    bmih.biHeight      = -ScreenBufferHeight * ScreenScaleFactor;
    bmih.biPlanes      = 1;
    bmih.biBitCount    = 16;
    bmih.biCompression = BI_RGB;

    RenderBitmap = CreateDIBSection(RenderDC, &bmi, DIB_RGB_COLORS, (void**)&RenderBuffer, NULL, 0);

    if (RenderBitmap == NULL)
    {
        DeleteDC(RenderDC);
        return false;
    }

    SelectObject(RenderDC, RenderBitmap);

    CenterWindow(SphereWindow,
                 ScreenWidth  * ScreenScaleFactor,
                 ScreenHeight * ScreenScaleFactor);

    // we know that 16-bit color DIBs are always 5:5:5
    PixelFormat = RGB555;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) CloseVideoDriver()
{
    if (Config.fullscreen)
        CloseFullscreen();
    else
        CloseWindowed();
}

////////////////////////////////////////////////////////////////////////////////
void CloseFullscreen()
{
    SetWindowLong(SphereWindow, GWL_STYLE, OldWindowStyle);
    SetWindowLong(SphereWindow, GWL_EXSTYLE, OldWindowStyleEx);

    ShowCursor(TRUE);

    if (ScreenBuffer != NULL)
    {
        delete[] ScreenBuffer;
        ScreenBuffer        = NULL;
        ScreenBufferSection = NULL;
    }

    if (dd != NULL)
    {
        dd->Release();
        dd = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////
void CloseWindowed()
{
    DeleteDC(RenderDC);
    RenderDC = NULL;

    DeleteObject(RenderBitmap);
    RenderBitmap = NULL;

    if (ScreenBuffer != NULL)
    {
        delete[] ScreenBuffer;
        ScreenBuffer        = NULL;
        ScreenBufferSection = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) FlipScreen()
{

    if (Config.fullscreen)
    {
        LPDIRECTDRAWSURFACE surface;

        if (Config.vsync)
            surface = ddSecondary;
        else
            surface = ddPrimary;

        // lock the surface
        DDSURFACEDESC ddsd;
        ddsd.dwSize = sizeof(ddsd);
        HRESULT ddrval = surface->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);

        // if the surface was lost, restore it
        if (ddrval == DDERR_SURFACELOST)
        {
            surface->Restore();
            if (surface == ddSecondary)
                ddPrimary->Restore();

            // attempt to lock again
            ddrval = surface->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
            if (ddrval != DD_OK)
            {
                Sleep(100);
                return;
            }
        }

        int dst_pitch = ddsd.lPitch / 2;

        if (Config.scale)
        {
            Scale((word*)ddsd.lpSurface, dst_pitch);
        }
        else
        {
            word* dst = (word*)ddsd.lpSurface;
            word* src = ScreenBuffer;

            for (int i = 0; i < ScreenBufferHeight; i++)
            {
                memcpy(dst, src, ScreenBufferWidth * 2);
                dst += dst_pitch;
                src += ScreenBufferWidth;
            }
        }

        // unlock the surface and do the flip!
        surface->Unlock(NULL);

        if (Config.vsync)
            ddPrimary->Flip(NULL, DDFLIP_WAIT);

    }
    else
    {

        if (Config.scale)
            Scale(RenderBuffer, ScreenBufferWidth * 2);
        else
            memcpy((byte*)RenderBuffer,
                   (byte*)ScreenBuffer,
                   ScreenBufferWidth * ScreenBufferHeight * 2);

        // blit the render buffer to the window
        HDC dc = GetDC(SphereWindow);

        BitBlt(dc, 0, 0,
               ScreenBufferWidth  * ScreenScaleFactor,
               ScreenBufferHeight * ScreenScaleFactor,
               RenderDC, 0, 0, SRCCOPY);

        ReleaseDC(SphereWindow, dc);
    }
}

////////////////////////////////////////////////////////////////////////////////
void Scale(word* dst, int dst_pitch)
{
    if (PixelFormat == RGB565)
    {

        switch (Config.filter)
        {

            case I_NONE:
                DirectScale(dst, dst_pitch, ScreenBuffer, ScreenBufferWidth, ScreenBufferHeight);
                break;

            case I_SCALE2X:
                Scale2x(    dst, dst_pitch, ScreenBuffer, ScreenBufferWidth, ScreenBufferHeight);
                break;

            case I_EAGLE:
                Eagle(      dst, dst_pitch, ScreenBuffer, ScreenBufferWidth, ScreenBufferHeight);
                break;

            case I_HQ2X:
                hq2x(       dst, dst_pitch, ScreenBuffer, ScreenBufferWidth, ScreenBufferHeight, 16);
                break;

            case I_2XSAI:
                _2xSaI(     dst, dst_pitch, ScreenBuffer, ScreenBufferWidth, ScreenBufferHeight, 16);
                break;

            case I_SUPER_2XSAI:
                Super2xSaI( dst, dst_pitch, ScreenBuffer, ScreenBufferWidth, ScreenBufferHeight, 16);
                break;

            case I_SUPER_EAGLE:
                SuperEagle( dst, dst_pitch, ScreenBuffer, ScreenBufferWidth, ScreenBufferHeight, 16);
                break;

        }
    }
    else
    {
        switch (Config.filter)
        {

            case I_NONE:
                DirectScale(dst, dst_pitch, ScreenBuffer, ScreenBufferWidth, ScreenBufferHeight);
                break;

            case I_SCALE2X:
                Scale2x(    dst, dst_pitch, ScreenBuffer, ScreenBufferWidth, ScreenBufferHeight);
                break;

            case I_EAGLE:
                Eagle(      dst, dst_pitch, ScreenBuffer, ScreenBufferWidth, ScreenBufferHeight);
                break;

            case I_HQ2X:
                hq2x(       dst, dst_pitch, ScreenBuffer, ScreenBufferWidth, ScreenBufferHeight, 15);
                break;

            case I_2XSAI:
                _2xSaI(     dst, dst_pitch, ScreenBuffer, ScreenBufferWidth, ScreenBufferHeight, 15);
                break;

            case I_SUPER_2XSAI:
                Super2xSaI( dst, dst_pitch, ScreenBuffer, ScreenBufferWidth, ScreenBufferHeight, 15);
                break;

            case I_SUPER_EAGLE:
                SuperEagle( dst, dst_pitch, ScreenBuffer, ScreenBufferWidth, ScreenBufferHeight, 15);
                break;

        }
    }
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DestroyImage(IMAGE image)
{
    if (image)
    {
        if (image->rgb)
            delete[] image->rgb;

        if (image->alpha)
            delete[] image->alpha;

        if (image->locked_pixels)
            delete[] image->locked_pixels;

        delete image;
    }
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(IMAGE) CreateImage(int width, int height, RGBA* pixels)
{
    IMAGE image = new _IMAGE;

    if (!image)
        return NULL;

    image->width  = width;
    image->height = height;
    image->locked_pixels = NULL;
    image->pixel_format = PixelFormat;

    if (!FillImagePixels(image, pixels))
    {
        DestroyImage(image);
        return NULL;
    }

    OptimizeBlitRoutine(image);

    return image;
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(IMAGE) CloneImage(IMAGE image)
{
    if (!image)
        return NULL;

    IMAGE clone = CreateImage(image->width, image->height, LockImage(image));
    UnlockImage(image, false);

    return clone;
}

////////////////////////////////////////////////////////////////////////////////
bool FillImagePixels(IMAGE image, RGBA* pixels)
{
    if (!pixels)
        return false;

    const int pixels_total = image->width * image->height;

    image->rgb   = new word[pixels_total];
    image->alpha = new byte[pixels_total];

    if (!image->rgb || !image->alpha)
    {
        if (image->rgb)   delete[] image->rgb;
        if (image->alpha) delete[] image->alpha;
        return false;
    }

    if (PixelFormat == RGB565)
    {
        for (int i = 0; i < pixels_total; i++)
        {
            image->rgb[i]   = PackPixel565(pixels[i]);
            image->alpha[i] = pixels[i].alpha;
        }
    }
    else
    {
        for (int i = 0; i < pixels_total; i++)
        {
            image->rgb[i]   = PackPixel555(pixels[i]);
            image->alpha[i] = pixels[i].alpha;
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
void OptimizeBlitRoutine(IMAGE image)
{
    const int pixels_total = image->width * image->height;

    // null blit
    bool is_empty = true;
    for (int i = 0; i < pixels_total; i++)
    {
        if (image->alpha[i])
        {
            is_empty = false;
            break;
        }
    }
    if (is_empty)
    {
        image->blit_routine = NullBlit;
        return;
    }

    // tile blit
    bool is_tile = true;
    for (int i = 0; i < pixels_total; i++)
    {
        if (image->alpha[i] < 255)
        {
            is_tile = false;
            break;
        }
    }
    if (is_tile)
    {
        image->blit_routine = TileBlit;
        return;
    }

    // sprite blit
    bool is_sprite = true;
    for (int i = 0; i < pixels_total; i++)
    {
        if (image->alpha[i] > 0 && image->alpha[i] < 255)
        {
            is_sprite = false;
            break;
        }
    }
    if (is_sprite)
    {
        image->blit_routine = SpriteBlit;
        return;
    }

    // normal blit
    image->blit_routine = NormalBlit;
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(IMAGE) GrabImage(int x, int y, int width, int height)
{
    if (x          < 0           ||
        y          < 0           ||
        x + width  > ScreenWidth ||
        y + height > ScreenHeight)
    {
        return NULL;
    }

    const int pixels_total = width * height;

    IMAGE image = new _IMAGE;

    if (!image)
        return NULL;

    image->width  = width;
    image->height = height;
    image->locked_pixels = NULL;
    image->blit_routine  = TileBlit;
    image->pixel_format  = PixelFormat;

    image->rgb   = new word[pixels_total];
    image->alpha = new byte[pixels_total];

    if (!image->rgb || !image->alpha)
    {
        if (image->rgb)   delete[] image->rgb;
        if (image->alpha) delete[] image->alpha;
        delete image;
        return NULL;
    }

    for (int iy = 0; iy < height; iy++)
        memcpy(image->rgb + iy * width, ScreenBufferSection + (y + iy) * ScreenBufferWidth + x, width * 2);

    memset(image->alpha, 255, pixels_total);

    return image;
}

////////////////////////////////////////////////////////////////////////////////
/*
 * Makes sure that images always are in the global pixel format
 */
static void validate_pixel_format(IMAGE image)
{
    if (image->pixel_format != PixelFormat)
    {
        RGBA* pixels = LockImage(image);
        FillImagePixels(image, pixels);
        image->pixel_format = PixelFormat;
        UnlockImage(image, false);
    }
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) BlitImage(IMAGE image, int x, int y, CImage32::BlendMode blendmode)
{
    validate_pixel_format(image);

    // don't draw it if it's off the screen
    if (x + image->width  < ClippingRectangle.left  ||
        y + image->height < ClippingRectangle.top   ||
        x                 > ClippingRectangle.right ||
        y                 > ClippingRectangle.bottom)
    {
        return;
    }

    switch (blendmode)
    {
        case CImage32::BLEND:
            image->blit_routine(image, x, y);
            break;

        case CImage32::ADD:
            AddBlit(image, x, y);
            break;

        case CImage32::SUBTRACT:
            SubtractBlit(image, x, y);
            break;

        case CImage32::MULTIPLY:
            MultiplyBlit(image, x, y);
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
template<CImage32::BlendMode blendmode, CImage32::BlendMode mask_blendmode>
class render_pixel_mask_565
{
private:

    RGBA m_Mask;

public:

    render_pixel_mask_565(RGBA mask) : m_Mask(mask)
    {
    }

    void operator()(word& d, const word s, int alpha)
    {
        // Note: Due to the blendmode parameters being template parameters,
        //       the switches will be resolved at the time of compilation.

        RGBA dst = UnpackPixel565(d);
        RGBA src = UnpackPixel565(s);

        // do the masking on the source pixel
        switch (mask_blendmode)
        {
            case CImage32::ADD:
            {
                src.red   = std::min(src.red   + m_Mask.red,   255);
                src.green = std::min(src.green + m_Mask.green, 255);
                src.blue  = std::min(src.blue  + m_Mask.blue,  255);
                alpha     = std::min(alpha     + m_Mask.alpha, 255);
            }
            break;

            case CImage32::SUBTRACT:
            {
                src.red   = std::max(src.red   - m_Mask.red,   0);
                src.green = std::max(src.green - m_Mask.green, 0);
                src.blue  = std::max(src.blue  - m_Mask.blue,  0);
                alpha     = std::max(alpha     - m_Mask.alpha, 0);
            }
            break;

            case CImage32::MULTIPLY:
            {
                src.red   = src.red   * (m_Mask.red   + 1) >> 8;
                src.green = src.green * (m_Mask.green + 1) >> 8;
                src.blue  = src.blue  * (m_Mask.blue  + 1) >> 8;
                alpha     = alpha     * (m_Mask.alpha + 1) >> 8;
            }
        }

        // blit to the destination pixel
        switch (blendmode)
        {
            case CImage32::BLEND:
            {
                int a = 256 - alpha;
                int b = alpha + 1;
                dst.red   = (dst.red   * a + src.red   * b) >> 8;
                dst.green = (dst.green * a + src.green * b) >> 8;
                dst.blue  = (dst.blue  * a + src.blue  * b) >> 8;
            }
            break;

            case CImage32::ADD:
            {
                dst.red   = std::min(dst.red   + src.red,   255);
                dst.green = std::min(dst.green + src.green, 255);
                dst.blue  = std::min(dst.blue  + src.blue,  255);
            }
            break;

            case CImage32::SUBTRACT:
            {
                dst.red   = std::max(dst.red   - src.red,   0);
                dst.green = std::max(dst.green - src.green, 0);
                dst.blue  = std::max(dst.blue  - src.blue,  0);
            }
            break;

            case CImage32::MULTIPLY:
            {
                dst.red   = dst.red   * (src.red   + 1) >> 8;
                dst.green = dst.green * (src.green + 1) >> 8;
                dst.blue  = dst.blue  * (src.blue  + 1) >> 8;
            }
        }

        d = PackPixel565(dst);
    }
};

////////////////////////////////////////////////////////////////////////////////
template<CImage32::BlendMode blendmode, CImage32::BlendMode mask_blendmode>
class render_pixel_mask_555
{
private:

    RGBA m_Mask;

public:

    render_pixel_mask_555(RGBA mask) : m_Mask(mask)
    {
    }

    void operator()(word& d, const word s, int alpha)
    {
        // Note: Due to the blendmode parameters being template parameters,
        //       the switches will be resolved at the time of compilation.

        RGBA dst = UnpackPixel555(d);
        RGBA src = UnpackPixel555(s);

        // do the masking on the source pixel
        switch (mask_blendmode)
        {
            case CImage32::ADD:
            {
                src.red   = std::min(src.red   + m_Mask.red,   255);
                src.green = std::min(src.green + m_Mask.green, 255);
                src.blue  = std::min(src.blue  + m_Mask.blue,  255);
                alpha     = std::min(alpha     + m_Mask.alpha, 255);
            }
            break;

            case CImage32::SUBTRACT:
            {
                src.red   = std::max(src.red   - m_Mask.red,   0);
                src.green = std::max(src.green - m_Mask.green, 0);
                src.blue  = std::max(src.blue  - m_Mask.blue,  0);
                alpha     = std::max(alpha     - m_Mask.alpha, 0);
            }
            break;

            case CImage32::MULTIPLY:
            {
                src.red   = src.red   * (m_Mask.red   + 1) >> 8;
                src.green = src.green * (m_Mask.green + 1) >> 8;
                src.blue  = src.blue  * (m_Mask.blue  + 1) >> 8;
                alpha     = alpha     * (m_Mask.alpha + 1) >> 8;
            }
        }

        // blit to the destination pixel
        switch (blendmode)
        {
            case CImage32::BLEND:
            {
                int a = 256 - alpha;
                int b = alpha + 1;
                dst.red   = (dst.red   * a + src.red   * b) >> 8;
                dst.green = (dst.green * a + src.green * b) >> 8;
                dst.blue  = (dst.blue  * a + src.blue  * b) >> 8;
            }
            break;

            case CImage32::ADD:
            {
                dst.red   = std::min(dst.red   + src.red,   255);
                dst.green = std::min(dst.green + src.green, 255);
                dst.blue  = std::min(dst.blue  + src.blue,  255);
            }
            break;

            case CImage32::SUBTRACT:
            {
                dst.red   = std::max(dst.red   - src.red,   0);
                dst.green = std::max(dst.green - src.green, 0);
                dst.blue  = std::max(dst.blue  - src.blue,  0);
            }
            break;

            case CImage32::MULTIPLY:
            {
                dst.red   = dst.red   * (src.red   + 1) >> 8;
                dst.green = dst.green * (src.green + 1) >> 8;
                dst.blue  = dst.blue  * (src.blue  + 1) >> 8;
            }
        }

        d = PackPixel555(dst);
    }
};

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) BlitImageMask(IMAGE image, int x, int y, CImage32::BlendMode blendmode,
                           RGBA mask, CImage32::BlendMode mask_blendmode)
{
    validate_pixel_format(image);

    // use preprocessor to save us typing
    #define FUNC    primitives::Blit
    #define PARAMS  ScreenBufferSection, \
                    ScreenBufferWidth,   \
                    x,                   \
                    y,                   \
                    image->rgb,          \
                    image->alpha,        \
                    image->width,        \
                    image->height,       \
                    ClippingRectangle

    if (PixelFormat == RGB565)
    {
        switch (blendmode)
        {
            case CImage32::BLEND:
            switch (mask_blendmode)
            {
                case CImage32::ADD:
                    FUNC(PARAMS, render_pixel_mask_565<CImage32::BLEND, CImage32::ADD>(mask));
                    break;
                case CImage32::SUBTRACT:
                    FUNC(PARAMS, render_pixel_mask_565<CImage32::BLEND, CImage32::SUBTRACT>(mask));
                    break;
                case CImage32::MULTIPLY:
                    FUNC(PARAMS, render_pixel_mask_565<CImage32::BLEND, CImage32::MULTIPLY>(mask));
            }
            break;

            case CImage32::ADD:
            switch (mask_blendmode)
            {
                case CImage32::ADD:
                    FUNC(PARAMS, render_pixel_mask_565<CImage32::ADD, CImage32::ADD>(mask));
                    break;
                case CImage32::SUBTRACT:
                    FUNC(PARAMS, render_pixel_mask_565<CImage32::ADD, CImage32::SUBTRACT>(mask));
                    break;
                case CImage32::MULTIPLY:
                    FUNC(PARAMS, render_pixel_mask_565<CImage32::ADD, CImage32::MULTIPLY>(mask));
            }
            break;

            case CImage32::SUBTRACT:
            switch (mask_blendmode)
            {
                case CImage32::ADD:
                    FUNC(PARAMS, render_pixel_mask_565<CImage32::SUBTRACT, CImage32::ADD>(mask));
                    break;
                case CImage32::SUBTRACT:
                    FUNC(PARAMS, render_pixel_mask_565<CImage32::SUBTRACT, CImage32::SUBTRACT>(mask));
                    break;
                case CImage32::MULTIPLY:
                    FUNC(PARAMS, render_pixel_mask_565<CImage32::SUBTRACT, CImage32::MULTIPLY>(mask));
            }
            break;

            case CImage32::MULTIPLY:
            switch (mask_blendmode)
            {
                case CImage32::ADD:
                    FUNC(PARAMS, render_pixel_mask_565<CImage32::MULTIPLY, CImage32::ADD>(mask));
                    break;
                case CImage32::SUBTRACT:
                    FUNC(PARAMS, render_pixel_mask_565<CImage32::MULTIPLY, CImage32::SUBTRACT>(mask));
                    break;
                case CImage32::MULTIPLY:
                    FUNC(PARAMS, render_pixel_mask_565<CImage32::MULTIPLY, CImage32::MULTIPLY>(mask));
            }
        }
    }
    else
    {
        switch (blendmode)
        {
            case CImage32::BLEND:
            switch (mask_blendmode)
            {
                case CImage32::ADD:
                    FUNC(PARAMS, render_pixel_mask_555<CImage32::BLEND, CImage32::ADD>(mask));
                    break;
                case CImage32::SUBTRACT:
                    FUNC(PARAMS, render_pixel_mask_555<CImage32::BLEND, CImage32::SUBTRACT>(mask));
                    break;
                case CImage32::MULTIPLY:
                    FUNC(PARAMS, render_pixel_mask_555<CImage32::BLEND, CImage32::MULTIPLY>(mask));
            }
            break;

            case CImage32::ADD:
            switch (mask_blendmode)
            {
                case CImage32::ADD:
                    FUNC(PARAMS, render_pixel_mask_555<CImage32::ADD, CImage32::ADD>(mask));
                    break;
                case CImage32::SUBTRACT:
                    FUNC(PARAMS, render_pixel_mask_555<CImage32::ADD, CImage32::SUBTRACT>(mask));
                    break;
                case CImage32::MULTIPLY:
                    FUNC(PARAMS, render_pixel_mask_555<CImage32::ADD, CImage32::MULTIPLY>(mask));
            }
            break;

            case CImage32::SUBTRACT:
            switch (mask_blendmode)
            {
                case CImage32::ADD:
                    FUNC(PARAMS, render_pixel_mask_555<CImage32::SUBTRACT, CImage32::ADD>(mask));
                    break;
                case CImage32::SUBTRACT:
                    FUNC(PARAMS, render_pixel_mask_555<CImage32::SUBTRACT, CImage32::SUBTRACT>(mask));
                    break;
                case CImage32::MULTIPLY:
                    FUNC(PARAMS, render_pixel_mask_555<CImage32::SUBTRACT, CImage32::MULTIPLY>(mask));
            }
            break;

            case CImage32::MULTIPLY:
            switch (mask_blendmode)
            {
                case CImage32::ADD:
                    FUNC(PARAMS, render_pixel_mask_555<CImage32::MULTIPLY, CImage32::ADD>(mask));
                    break;
                case CImage32::SUBTRACT:
                    FUNC(PARAMS, render_pixel_mask_555<CImage32::MULTIPLY, CImage32::SUBTRACT>(mask));
                    break;
                case CImage32::MULTIPLY:
                    FUNC(PARAMS, render_pixel_mask_555<CImage32::MULTIPLY, CImage32::MULTIPLY>(mask));
            }
        }
    }

    #undef FUNC
    #undef PARAMS
}

////////////////////////////////////////////////////////////////////////////////
inline void Blend565(word& d, const word s, int alpha)
{
    RGBA dst = UnpackPixel565(d);
    RGBA src = UnpackPixel565(s);
    int a = 256 - alpha;
    int b = alpha + 1;
    dst.red   = (dst.red   * a + src.red   * b) >> 8;
    dst.green = (dst.green * a + src.green * b) >> 8;
    dst.blue  = (dst.blue  * a + src.blue  * b) >> 8;
    d = PackPixel565(dst);
}

inline void Add565(word& d, const word s, int alpha)
{
    RGBA dst = UnpackPixel565(d);
    RGBA src = UnpackPixel565(s);
    dst.red   = std::min(dst.red   + src.red,   255);
    dst.green = std::min(dst.green + src.green, 255);
    dst.blue  = std::min(dst.blue  + src.blue,  255);
    d = PackPixel565(dst);
}

inline void Subtract565(word& d, const word s, int alpha)
{
    RGBA dst = UnpackPixel565(d);
    RGBA src = UnpackPixel565(s);
    dst.red   = std::max(dst.red   - src.red,   0);
    dst.green = std::max(dst.green - src.green, 0);
    dst.blue  = std::max(dst.blue  - src.blue,  0);
    d = PackPixel565(dst);
}

inline void Multiply565(word& d, const word s, int alpha)
{
    RGBA dst = UnpackPixel565(d);
    RGBA src = UnpackPixel565(s);
    dst.red   = dst.red   * (src.red   + 1) >> 8;
    dst.green = dst.green * (src.green + 1) >> 8;
    dst.blue  = dst.blue  * (src.blue  + 1) >> 8;
    d = PackPixel565(dst);
}

////////////////////////////////////////////////////////////////////////////////
inline void Blend555(word& d, const word s, int alpha)
{
    RGBA dst = UnpackPixel555(d);
    RGBA src = UnpackPixel555(s);
    int a = 256 - alpha;
    int b = alpha + 1;
    dst.red   = (dst.red   * a + src.red   * b) >> 8;
    dst.green = (dst.green * a + src.green * b) >> 8;
    dst.blue  = (dst.blue  * a + src.blue  * b) >> 8;
    d = PackPixel555(dst);
}

inline void Add555(word& d, const word s, int alpha)
{
    RGBA dst = UnpackPixel555(d);
    RGBA src = UnpackPixel555(s);
    dst.red   = std::min(dst.red   + src.red,   255);
    dst.green = std::min(dst.green + src.green, 255);
    dst.blue  = std::min(dst.blue  + src.blue,  255);
    d = PackPixel555(dst);
}

inline void Subtract555(word& d, const word s, int alpha)
{
    RGBA dst = UnpackPixel555(d);
    RGBA src = UnpackPixel555(s);
    dst.red   = std::max(dst.red   - src.red,   0);
    dst.green = std::max(dst.green - src.green, 0);
    dst.blue  = std::max(dst.blue  - src.blue,  0);
    d = PackPixel555(dst);
}

inline void Multiply555(word& d, const word s, int alpha)
{
    RGBA dst = UnpackPixel555(d);
    RGBA src = UnpackPixel555(s);
    dst.red   = dst.red   * (src.red   + 1) >> 8;
    dst.green = dst.green * (src.green + 1) >> 8;
    dst.blue  = dst.blue  * (src.blue  + 1) >> 8;
    d = PackPixel555(dst);
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) TransformBlitImage(IMAGE image, int x[4], int y[4], CImage32::BlendMode blendmode)
{
    validate_pixel_format(image);

    // fallback onto BlitImage if possible
    if (x[0] == x[3] && x[1] == x[2] && y[0] == y[1] && y[2] == y[3])
    {
        int dw = x[2] - x[0] + 1;
        int dh = y[2] - y[0] + 1;
        if (dw == image->width && dh == image->height)
        {
            BlitImage(image, x[0], y[0], blendmode);
            return;
        }
    }

    // use preprocessor to save us typing
    #define FUNC    primitives::TexturedQuad
    #define PARAMS  ScreenBufferSection, \
                    ScreenBufferWidth,   \
                    x,                   \
                    y,                   \
                    image->rgb,          \
                    image->alpha,        \
                    image->width,        \
                    image->height,       \
                    ClippingRectangle

    if (PixelFormat == RGB565)
    {
        switch (blendmode)
        {
            case CImage32::BLEND:    FUNC(PARAMS, Blend565);    break;
            case CImage32::ADD:      FUNC(PARAMS, Add565);      break;
            case CImage32::SUBTRACT: FUNC(PARAMS, Subtract565); break;
            case CImage32::MULTIPLY: FUNC(PARAMS, Multiply565); break;
        }
    }
    else
    {
        switch (blendmode)
        {
            case CImage32::BLEND:    FUNC(PARAMS, Blend555);    break;
            case CImage32::ADD:      FUNC(PARAMS, Add555);      break;
            case CImage32::SUBTRACT: FUNC(PARAMS, Subtract555); break;
            case CImage32::MULTIPLY: FUNC(PARAMS, Multiply555); break;
        }
    }

    #undef FUNC
    #undef PARAMS
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) TransformBlitImageMask(IMAGE image, int x[4], int y[4], CImage32::BlendMode blendmode,
                                    RGBA mask, CImage32::BlendMode mask_blendmode)
{
    validate_pixel_format(image);

    // fallback onto BlitImageMask if possible
    if (x[0] == x[3] && x[1] == x[2] && y[0] == y[1] && y[2] == y[3])
    {
        int dw = x[2] - x[0] + 1;
        int dh = y[2] - y[0] + 1;
        if (dw == image->width && dh == image->height)
        {
            BlitImageMask(image, x[0], y[0], blendmode, mask, mask_blendmode);
            return;
        }
    }

    // use preprocessor to save us typing
    #define FUNC    primitives::TexturedQuad
    #define PARAMS  ScreenBufferSection, \
                    ScreenBufferWidth,   \
                    x,                   \
                    y,                   \
                    image->rgb,          \
                    image->alpha,        \
                    image->width,        \
                    image->height,       \
                    ClippingRectangle

    if (PixelFormat == RGB565)
    {
        switch (blendmode)
        {
            case CImage32::BLEND:
            switch (mask_blendmode)
            {
                case CImage32::ADD:
                    FUNC(PARAMS, render_pixel_mask_565<CImage32::BLEND, CImage32::ADD>(mask));
                    break;
                case CImage32::SUBTRACT:
                    FUNC(PARAMS, render_pixel_mask_565<CImage32::BLEND, CImage32::SUBTRACT>(mask));
                    break;
                case CImage32::MULTIPLY:
                    FUNC(PARAMS, render_pixel_mask_565<CImage32::BLEND, CImage32::MULTIPLY>(mask));
            }
            break;

            case CImage32::ADD:
            switch (mask_blendmode)
            {
                case CImage32::ADD:
                    FUNC(PARAMS, render_pixel_mask_565<CImage32::ADD, CImage32::ADD>(mask));
                    break;
                case CImage32::SUBTRACT:
                    FUNC(PARAMS, render_pixel_mask_565<CImage32::ADD, CImage32::SUBTRACT>(mask));
                    break;
                case CImage32::MULTIPLY:
                    FUNC(PARAMS, render_pixel_mask_565<CImage32::ADD, CImage32::MULTIPLY>(mask));
            }
            break;

            case CImage32::SUBTRACT:
            switch (mask_blendmode)
            {
                case CImage32::ADD:
                    FUNC(PARAMS, render_pixel_mask_565<CImage32::SUBTRACT, CImage32::ADD>(mask));
                    break;
                case CImage32::SUBTRACT:
                    FUNC(PARAMS, render_pixel_mask_565<CImage32::SUBTRACT, CImage32::SUBTRACT>(mask));
                    break;
                case CImage32::MULTIPLY:
                    FUNC(PARAMS, render_pixel_mask_565<CImage32::SUBTRACT, CImage32::MULTIPLY>(mask));
            }
            break;

            case CImage32::MULTIPLY:
            switch (mask_blendmode)
            {
                case CImage32::ADD:
                    FUNC(PARAMS, render_pixel_mask_565<CImage32::MULTIPLY, CImage32::ADD>(mask));
                    break;
                case CImage32::SUBTRACT:
                    FUNC(PARAMS, render_pixel_mask_565<CImage32::MULTIPLY, CImage32::SUBTRACT>(mask));
                    break;
                case CImage32::MULTIPLY:
                    FUNC(PARAMS, render_pixel_mask_565<CImage32::MULTIPLY, CImage32::MULTIPLY>(mask));
            }
        }
    }
    else
    {
        switch (blendmode)
        {
            case CImage32::BLEND:
            switch (mask_blendmode)
            {
                case CImage32::ADD:
                    FUNC(PARAMS, render_pixel_mask_555<CImage32::BLEND, CImage32::ADD>(mask));
                    break;
                case CImage32::SUBTRACT:
                    FUNC(PARAMS, render_pixel_mask_555<CImage32::BLEND, CImage32::SUBTRACT>(mask));
                    break;
                case CImage32::MULTIPLY:
                    FUNC(PARAMS, render_pixel_mask_555<CImage32::BLEND, CImage32::MULTIPLY>(mask));
            }
            break;

            case CImage32::ADD:
            switch (mask_blendmode)
            {
                case CImage32::ADD:
                    FUNC(PARAMS, render_pixel_mask_555<CImage32::ADD, CImage32::ADD>(mask));
                    break;
                case CImage32::SUBTRACT:
                    FUNC(PARAMS, render_pixel_mask_555<CImage32::ADD, CImage32::SUBTRACT>(mask));
                    break;
                case CImage32::MULTIPLY:
                    FUNC(PARAMS, render_pixel_mask_555<CImage32::ADD, CImage32::MULTIPLY>(mask));
            }
            break;

            case CImage32::SUBTRACT:
            switch (mask_blendmode)
            {
                case CImage32::ADD:
                    FUNC(PARAMS, render_pixel_mask_555<CImage32::SUBTRACT, CImage32::ADD>(mask));
                    break;
                case CImage32::SUBTRACT:
                    FUNC(PARAMS, render_pixel_mask_555<CImage32::SUBTRACT, CImage32::SUBTRACT>(mask));
                    break;
                case CImage32::MULTIPLY:
                    FUNC(PARAMS, render_pixel_mask_555<CImage32::SUBTRACT, CImage32::MULTIPLY>(mask));
            }
            break;

            case CImage32::MULTIPLY:
            switch (mask_blendmode)
            {
                case CImage32::ADD:
                    FUNC(PARAMS, render_pixel_mask_555<CImage32::MULTIPLY, CImage32::ADD>(mask));
                    break;
                case CImage32::SUBTRACT:
                    FUNC(PARAMS, render_pixel_mask_555<CImage32::MULTIPLY, CImage32::SUBTRACT>(mask));
                    break;
                case CImage32::MULTIPLY:
                    FUNC(PARAMS, render_pixel_mask_555<CImage32::MULTIPLY, CImage32::MULTIPLY>(mask));
            }
        }
    }

    #undef FUNC
    #undef PARAMS

}

////////////////////////////////////////////////////////////////////////////////
void NullBlit(IMAGE image, int x, int y)
{
}

////////////////////////////////////////////////////////////////////////////////
void TileBlit(IMAGE image, int x, int y)
{
    calculate_clipping_metrics(image->width, image->height);

    word* dest  = (word*)ScreenBufferSection + (y + image_offset_y) * ScreenBufferWidth  + image_offset_x + x;
    word* src   = (word*)image->rgb          +       image_offset_y * image->width       + image_offset_x;

    int iy = image_blit_height;
    while (iy-- > 0)
    {

        memcpy(dest, src, image_blit_width * sizeof(word));
        dest += ScreenBufferWidth;
        src  += image->width;

    }
}

////////////////////////////////////////////////////////////////////////////////
void SpriteBlit(IMAGE image, int x, int y)
{
    calculate_clipping_metrics(image->width, image->height);

    word* dst   = (word*)ScreenBufferSection + (y + image_offset_y) * ScreenBufferWidth  + image_offset_x + x;
    word* src   = (word*)image->rgb          +      image_offset_y  * image->width       + image_offset_x;
    byte* alpha = image->alpha               +      image_offset_y  * image->width       + image_offset_x;

    int dst_inc = ScreenBufferWidth  - image_blit_width;
    int src_inc = image->width       - image_blit_width;

    int iy = image_blit_height;
    int ix;
    while (iy-- > 0)
    {
        ix = image_blit_width;
        while (ix-- > 0)
        {
            if (*alpha)
                *dst = *src;

            ++dst;
            ++src;
            ++alpha;
        }

        dst   += dst_inc;
        src   += src_inc;
        alpha += src_inc;
    }
}

////////////////////////////////////////////////////////////////////////////////
void NormalBlit(IMAGE image, int x, int y)
{
    calculate_clipping_metrics(image->width, image->height);

    word result;

    word* dst   = (word*)ScreenBufferSection + (y + image_offset_y) * ScreenBufferWidth + image_offset_x + x;
    word* src   = (word*)image->rgb          +      image_offset_y  * image->width      + image_offset_x;
    byte* alpha = image->alpha               +      image_offset_y  * image->width      + image_offset_x;

    int dst_inc = ScreenBufferWidth - image_blit_width;
    int src_inc = image->width      - image_blit_width;

    int iy = image_blit_height;
    int ix;

    if (PixelFormat == RGB565)
    {
        while (iy-- > 0)
        {
            ix = image_blit_width;
            while (ix-- > 0)
            {
                int a = 256 - *alpha;
                int b = *alpha + 1;

                result  = (((*dst & 0x001F) * a + (*src & 0x001F) * b) >> 8);
                result |= (((*dst & 0x07E0) * a + (*src & 0x07E0) * b) >> 8) & 0x07E0;
                result |= (((*dst & 0xF800) * a + (*src & 0xF800) * b) >> 8) & 0xF800;

                *dst = result;
                ++dst;
                ++src;
                ++alpha;
            }

            dst   += dst_inc;
            src   += src_inc;
            alpha += src_inc;
        }
    }
    else
    {
        while (iy-- > 0)
        {
            ix = image_blit_width;
            while (ix-- > 0)
            {
                int a = 256 - *alpha;
                int b = *alpha + 1;

                result  = (((*dst & 0x001F) * a + (*src & 0x001F) * b) >> 8);
                result |= (((*dst & 0x03E0) * a + (*src & 0x03E0) * b) >> 8) & 0x03E0;
                result |= (((*dst & 0x7C00) * a + (*src & 0x7C00) * b) >> 8) & 0x7C00;

                *dst = result;
                ++dst;
                ++src;
                ++alpha;
            }

            dst   += dst_inc;
            src   += src_inc;
            alpha += src_inc;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void AddBlit(IMAGE image, int x, int y)
{
    calculate_clipping_metrics(image->width, image->height);

    word* dst = (word*)ScreenBufferSection + (y + image_offset_y) * ScreenBufferWidth + image_offset_x + x;
    word* src = (word*)image->rgb          +      image_offset_y  * image->width      + image_offset_x;

    int dst_inc = ScreenBufferWidth - image_blit_width;
    int src_inc = image->width      - image_blit_width;

    word result;

    int iy = image_blit_height;
    if (PixelFormat == RGB565)
    {
        while (iy-- > 0)
        {
            int ix = image_blit_width;
            while (ix-- > 0)
            {
                result  = std::min( (*dst & 0x001F)        +  (*src & 0x001F),        31);
                result |= std::min(((*dst & 0x07E0) >> 5)  + ((*src & 0x07E0) >> 5),  63) << 5;
                result |= std::min(((*dst & 0xF800) >> 11) + ((*src & 0xF800) >> 11), 31) << 11;

                *dst = result;

                ++dst;
                ++src;
            }

            dst += dst_inc;
            src += src_inc;
        }
    }
    else
    {
        while (iy-- > 0)
        {
            int ix = image_blit_width;
            while (ix-- > 0)
            {
                result  = std::min( (*dst & 0x001F)        +  (*src & 0x001F),        31);
                result |= std::min(((*dst & 0x03E0) >> 5)  + ((*src & 0x03E0) >> 5),  31) << 5;
                result |= std::min(((*dst & 0x7C00) >> 10) + ((*src & 0x7C00) >> 10), 31) << 10;

                *dst = result;

                ++dst;
                ++src;
            }

            dst += dst_inc;
            src += src_inc;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void SubtractBlit(IMAGE image, int x, int y)
{
    calculate_clipping_metrics(image->width, image->height);

    word* dst = (word*)ScreenBufferSection + (y + image_offset_y) * ScreenBufferWidth + image_offset_x + x;
    word* src = (word*)image->rgb          +      image_offset_y  * image->width      + image_offset_x;

    int dst_inc = ScreenBufferWidth - image_blit_width;
    int src_inc = image->width      - image_blit_width;

    word result;

    int iy = image_blit_height;
    if (PixelFormat == RGB565)
    {
        while (iy-- > 0)
        {
            int ix = image_blit_width;
            while (ix-- > 0)
            {
                result  = std::max( (*dst & 0x001F)        -  (*src & 0x001F),        0);
                result |= std::max(((*dst & 0x07E0) >> 5)  - ((*src & 0x07E0) >> 5),  0) << 5;
                result |= std::max(((*dst & 0xF800) >> 11) - ((*src & 0xF800) >> 11), 0) << 11;

                *dst = result;

                ++dst;
                ++src;
            }

            dst += dst_inc;
            src += src_inc;
        }
    }
    else
    {
        while (iy-- > 0)
        {
            int ix = image_blit_width;
            while (ix-- > 0)
            {
                result  = std::max( (*dst & 0x001F)        -  (*src & 0x001F),        0);
                result |= std::max(((*dst & 0x03E0) >> 5)  - ((*src & 0x03E0) >> 5),  0) << 5;
                result |= std::max(((*dst & 0x7C00) >> 10) - ((*src & 0x7C00) >> 10), 0) << 10;

                *dst = result;

                ++dst;
                ++src;
            }

            dst += dst_inc;
            src += src_inc;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void MultiplyBlit(IMAGE image, int x, int y)
{
    calculate_clipping_metrics(image->width, image->height);

    word* dst = (word*)ScreenBufferSection + (y + image_offset_y) * ScreenBufferWidth + image_offset_x + x;
    word* src = (word*)image->rgb          +      image_offset_y  * image->width      + image_offset_x;

    int dst_inc = ScreenBufferWidth - image_blit_width;
    int src_inc = image->width      - image_blit_width;

    word result;

    int iy = image_blit_height;
    if (PixelFormat == RGB565)
    {
        while (iy-- > 0)
        {
            int ix = image_blit_width;
            while (ix-- > 0)
            {
                result  =   (*dst & 0x001F)        *  ((*src & 0x001F)        + 1) >> 5;
                result |= (((*dst & 0x07E0) >> 5)  * (((*src & 0x07E0) >> 5)  + 1) >> 6) << 5;
                result |= (((*dst & 0xF800) >> 11) * (((*src & 0xF800) >> 11) + 1) >> 5) << 11;

                *dst = result;

                ++dst;
                ++src;
            }

            dst += dst_inc;
            src += src_inc;
        }
    }
    else
    {
        while (iy-- > 0)
        {
            int ix = image_blit_width;
            while (ix-- > 0)
            {
                result  =   (*dst & 0x001F)        *  ((*src & 0x001F)        + 1) >> 5;
                result |= (((*dst & 0x03E0) >> 5)  * (((*src & 0x03E0) >> 5)  + 1) >> 5) << 5;
                result |= (((*dst & 0x7C00) >> 10) * (((*src & 0x7C00) >> 10) + 1) >> 5) << 10;

                *dst = result;

                ++dst;
                ++src;
            }

            dst += dst_inc;
            src += src_inc;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(int) GetImageWidth(IMAGE image)
{
    return image->width;
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(int) GetImageHeight(IMAGE image)
{
    return image->height;
}

////////////////////////////////////////////////////////////////////////////////
/**
 * ClipByte clamps an integer value to the range 0-255 without
 * any branch operations.
 */

inline int ClipByte(int value)
{
    int iClipped;
    value = (value & (-(int)!(value < 0)));
    iClipped = -(int)(value > 255);
    return (255 & iClipped) | (value & ~iClipped);
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(RGBA*) LockImage(IMAGE image)
{
    if (!image)
        return NULL;

    const int pixels_total = image->width * image->height;

    if (image->locked_pixels)
        delete[] image->locked_pixels;

    image->locked_pixels = new RGBA[image->width * image->height];

    if (!image->locked_pixels)
        return NULL;

    if (image->pixel_format == RGB565)
    {
        for (int i = 0; i < pixels_total; i++)
        {
            image->locked_pixels[i] = UnpackPixel565(image->rgb[i]);
            image->locked_pixels[i].alpha = image->alpha[i];
        }
    }
    else
    {
        for (int i = 0; i < pixels_total; i++)
        {
            image->locked_pixels[i] = UnpackPixel555(image->rgb[i]);
            image->locked_pixels[i].alpha = image->alpha[i];
        }
    }

    return image->locked_pixels;
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) UnlockImage(IMAGE image, bool pixels_changed)
{
    if (!image || !image->locked_pixels)
        return;

    if (pixels_changed)
    {
        delete[] image->rgb;
        delete[] image->alpha;
        FillImagePixels(image, image->locked_pixels);
        OptimizeBlitRoutine(image);
    }

    delete[] image->locked_pixels;
    image->locked_pixels = NULL;
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DirectBlit(int x, int y, int w, int h, RGBA* pixels)
{
    calculate_clipping_metrics(w, h);

    word* dst = ScreenBufferSection + (y + image_offset_y) * ScreenBufferWidth + image_offset_x + x;
    RGBA* src = pixels              +      image_offset_y  * w                 + image_offset_x;

    int dst_inc = ScreenBufferWidth - image_blit_width;
    int src_inc = w                 - image_blit_width;

    RGBA temp;
    word result;

    int iy = image_blit_height;
    int ix;

    if (PixelFormat == RGB565)
    {
        while (iy-- > 0)
        {
            ix = image_blit_width;

            while (ix-- > 0)
            {
                temp.red   = src->red   * (src->alpha + 1) >> 8;
                temp.green = src->green * (src->alpha + 1) >> 8;
                temp.blue  = src->blue  * (src->alpha + 1) >> 8;

                int a = 256 - src->alpha;

                result  =  ((*dst & 0x001F) * a >> 8) +  (temp.blue  >> 3);
                result |= (((*dst & 0x07E0) * a >> 8) + ((temp.green >> 2) <<  5)) & 0x07E0;
                result |= (((*dst & 0xF800) * a >> 8) + ((temp.red   >> 3) << 11)) & 0xF800;

                *dst = result;
                ++dst;
                ++src;
            }

            dst += dst_inc;
            src += src_inc;
        }
    }
    else
    {
        while (iy-- > 0)
        {
            ix = image_blit_width;

            while (ix-- > 0)
            {

                temp.red   = src->red   * (src->alpha + 1) >> 8;
                temp.green = src->green * (src->alpha + 1) >> 8;
                temp.blue  = src->blue  * (src->alpha + 1) >> 8;

                int a = 256 - src->alpha;

                result  =  ((*dst & 0x001F) * a >> 8) +  (temp.blue  >> 3);
                result |= (((*dst & 0x03E0) * a >> 8) + ((temp.green >> 3) <<  5)) & 0x03E0;
                result |= (((*dst & 0x7C00) * a >> 8) + ((temp.red   >> 3) << 10)) & 0x7C00;

                *dst = result;
                ++dst;
                ++src;
            }

            dst += dst_inc;
            src += src_inc;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
inline void BlendRGBAto565(word& d, const RGBA& s, const RGBA& alpha)
{
    int  a;
    RGBA temp;
    word result;

    temp.red   = s.red   * (alpha.alpha + 1) >> 8;
    temp.green = s.green * (alpha.alpha + 1) >> 8;
    temp.blue  = s.blue  * (alpha.alpha + 1) >> 8;

    a = 256 - alpha.alpha;

    result  =  ((d & 0x001F) * a >> 8) + ( temp.blue  >> 3);
    result |= (((d & 0x07E0) * a >> 8) + ((temp.green >> 2) <<  5)) & 0x07E0;
    result |= (((d & 0xF800) * a >> 8) + ((temp.red   >> 3) << 11)) & 0xF800;

    d = result;
}

inline void BlendRGBAto555(word& d, const RGBA& s, const RGBA& alpha)
{
    int  a;
    RGBA temp;
    word result;

    temp.red   = s.red   * (alpha.alpha + 1) >> 8;
    temp.green = s.green * (alpha.alpha + 1) >> 8;
    temp.blue  = s.blue  * (alpha.alpha + 1) >> 8;

    a = 256 - alpha.alpha;

    result  =  ((d & 0x001F) * a >> 8) + ( temp.blue  >> 3);
    result |= (((d & 0x03E0) * a >> 8) + ((temp.green >> 3) <<  5)) & 0x03E0;
    result |= (((d & 0x7C00) * a >> 8) + ((temp.red   >> 3) << 10)) & 0x7C00;

    d = result;
}

EXPORT(void) DirectTransformBlit(int x[4], int y[4], int w, int h, RGBA* pixels)
{
    if (PixelFormat == RGB565)
    {
        primitives::TexturedQuad(ScreenBufferSection,
                                 ScreenBufferWidth,
                                 x,
                                 y,
                                 pixels,
                                 pixels,
                                 w,
                                 h,
                                 ClippingRectangle,
                                 BlendRGBAto565);
    }
    else
    {
        primitives::TexturedQuad(ScreenBufferSection,
                                 ScreenBufferWidth,
                                 x,
                                 y,
                                 pixels,
                                 pixels,
                                 w,
                                 h,
                                 ClippingRectangle,
                                 BlendRGBAto555);
    }
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DirectGrab(int x, int y, int w, int h, RGBA* pixels)
{
    if (x     < 0           ||
        y     < 0           ||
        x + w > ScreenWidth ||
        y + h > ScreenHeight)
    {
        return;
    }

    word* Screen = ScreenBufferSection + y * ScreenBufferWidth + x;

    int scr_inc = ScreenBufferWidth - w;

    int iy = h;
    int ix;

    if (PixelFormat == RGB565)
    {
        while (iy-- > 0)
        {
            ix = w;

            while (ix-- > 0)
            {
                pixels->red   = ((*Screen & 0xF800) >> 11) << 3;
                pixels->green = ((*Screen & 0x07E0) >>  5) << 2;
                pixels->blue  = ((*Screen & 0x001F) >>  0) << 3;
                pixels->alpha = 255;

                ++Screen;
                ++pixels;
            }

            Screen += scr_inc;
        }
    }
    else
    {
        while (iy-- > 0)
        {
            ix = w;

            while (ix-- > 0)
            {
                pixels->red   = ((*Screen & 0x7C00) >> 10) << 3;
                pixels->green = ((*Screen & 0x03E0) >>  5) << 3;
                pixels->blue  = ((*Screen & 0x001F) >>  0) << 3;
                pixels->alpha = 255;

                ++Screen;
                ++pixels;
            }

            Screen += scr_inc;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
class constant_color
{
    public:

        constant_color(RGBA color) : m_color(color) {};

        RGBA operator()(int i, int range)
        {
            return m_color;
        }

    private:

        RGBA m_color;

};

class gradient_color
{
    public:

        gradient_color(RGBA color1, RGBA color2)
                : m_color1(color1)
                , m_red_range(  color2.red   - color1.red)
                , m_green_range(color2.green - color1.green)
                , m_blue_range( color2.blue  - color1.blue)
                , m_alpha_range(color2.alpha - color1.alpha) {};

        RGBA operator()(int i, int range)
        {
            if (range == 0)
                return m_color1;

            RGBA color;

            color.red   = m_color1.red   + (m_red_range   * i / range);
            color.green = m_color1.green + (m_green_range * i / range);
            color.blue  = m_color1.blue  + (m_blue_range  * i / range);
            color.alpha = m_color1.alpha + (m_alpha_range * i / range);

            return color;
        }

    private:

        RGBA m_color1;
        int  m_red_range;
        int  m_green_range;
        int  m_blue_range;
        int  m_alpha_range;

};

inline void blend565(word& dest, RGBA source)
{
    RGBA out = UnpackPixel565(dest);
    out.red   = (source.red   * source.alpha + out.red   * (256 - source.alpha)) / 256;
    out.green = (source.green * source.alpha + out.green * (256 - source.alpha)) / 256;
    out.blue  = (source.blue  * source.alpha + out.blue  * (256 - source.alpha)) / 256;
    dest = PackPixel565(out);
}

inline void blend555(word& dest, RGBA source)
{
    RGBA out = UnpackPixel555(dest);
    out.red   = (source.red   * source.alpha + out.red   * (256 - source.alpha)) / 256;
    out.green = (source.green * source.alpha + out.green * (256 - source.alpha)) / 256;
    out.blue  = (source.blue  * source.alpha + out.blue  * (256 - source.alpha)) / 256;
    dest = PackPixel555(out);
}

inline void copyWord(word& dest, word source)
{
    dest = source;
}


EXPORT(void) DrawPoint(int x, int y, RGBA color)
{
    if (PixelFormat == RGB565)
    {
        primitives::Point(ScreenBufferSection, ScreenBufferWidth, x, y, color, ClippingRectangle, blend565);
    }
    else
    {
        primitives::Point(ScreenBufferSection, ScreenBufferWidth, x, y, color, ClippingRectangle, blend555);
    }
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawPointSeries(VECTOR_INT** points, int length, RGBA color)
{
    if (PixelFormat == RGB565)
    {
        primitives::PointSeries(ScreenBufferSection, ScreenBufferWidth, points, length, color, ClippingRectangle, blend565);
    }
    else
    {
        primitives::PointSeries(ScreenBufferSection, ScreenBufferWidth, points, length, color, ClippingRectangle, blend555);
    }
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawLine(int x[2], int y[2], RGBA color)
{
    if (PixelFormat == RGB565)
    {
        primitives::Line(ScreenBufferSection, ScreenBufferWidth, x[0], y[0], x[1], y[1], constant_color(color), ClippingRectangle, blend565);
    }
    else
    {
        primitives::Line(ScreenBufferSection, ScreenBufferWidth, x[0], y[0], x[1], y[1], constant_color(color), ClippingRectangle, blend555);
    }
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawGradientLine(int x[2], int y[2], RGBA colors[2])
{
    if (PixelFormat == RGB565)
    {
        primitives::Line(ScreenBufferSection, ScreenBufferWidth, x[0], y[0], x[1], y[1], gradient_color(colors[0], colors[1]), ClippingRectangle, blend565);
    }
    else
    {
        primitives::Line(ScreenBufferSection, ScreenBufferWidth, x[0], y[0], x[1], y[1], gradient_color(colors[0], colors[1]), ClippingRectangle, blend555);
    }
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawLineSeries(VECTOR_INT** points, int length, RGBA color, int type)
{
    if (PixelFormat == RGB565)
    {
        primitives::LineSeries(ScreenBufferSection, ScreenBufferWidth, points, length, color, type, ClippingRectangle, blend565);
    }
    else
    {
        primitives::LineSeries(ScreenBufferSection, ScreenBufferWidth, points, length, color, type, ClippingRectangle, blend555);
    }
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawBezierCurve(int x[4], int y[4], double step, RGBA color, int cubic)
{
    if (PixelFormat == RGB565)
    {
        primitives::BezierCurve(ScreenBufferSection, ScreenBufferWidth, x, y, step, color, cubic, ClippingRectangle, blend565);
    }
    else
    {
        primitives::BezierCurve(ScreenBufferSection, ScreenBufferWidth, x, y, step, color, cubic, ClippingRectangle, blend555);
    }
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawTriangle(int x[3], int y[3], RGBA color)
{
    if (PixelFormat == RGB565)
    {
        primitives::Triangle(ScreenBufferSection, ScreenBufferWidth, x, y, color, ClippingRectangle, blend565);
    }
    else
    {
        primitives::Triangle(ScreenBufferSection, ScreenBufferWidth, x, y, color, ClippingRectangle, blend555);
    }
}

////////////////////////////////////////////////////////////////////////////////
inline RGBA interpolateRGBA(RGBA a, RGBA b, int i, int range)
{
    if (range == 0)
    {
        return a;
    }

    RGBA result = {
                      (a.red   * (range - i) + b.red   * i) / range,
                      (a.green * (range - i) + b.green * i) / range,
                      (a.blue  * (range - i) + b.blue  * i) / range,
                      (a.alpha * (range - i) + b.alpha * i) / range,
                  };
    return result;
}

EXPORT(void) DrawGradientTriangle(int x[3], int y[3], RGBA colors[3])
{
    if (PixelFormat == RGB565)
    {
        primitives::GradientTriangle(ScreenBufferSection, ScreenBufferWidth, x, y, colors, ClippingRectangle, blend565, interpolateRGBA);
    }
    else
    {
        primitives::GradientTriangle(ScreenBufferSection, ScreenBufferWidth, x, y, colors, ClippingRectangle, blend555, interpolateRGBA);
    }
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawPolygon(VECTOR_INT** points, int length, int invert, RGBA color)
{
    if (PixelFormat == RGB565)
    {
        primitives::Polygon(ScreenBufferSection, ScreenBufferWidth, points, length, invert, color, ClippingRectangle, blend565);
    }
    else
    {
        primitives::Polygon(ScreenBufferSection, ScreenBufferWidth, points, length, invert, color, ClippingRectangle, blend555);
    }
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawOutlinedRectangle(int x, int y, int w, int h, int size, RGBA color)
{
    if (color.alpha == 0)
    {          // no mask

        return;

    }
    else if (color.alpha == 255)
    { // full mask

        if (PixelFormat == RGB565)
        {
            word c = PackPixel565(color);
            primitives::OutlinedRectangle(ScreenBufferSection, ScreenBufferWidth, x, y, w, h, size, c, ClippingRectangle, copyWord);
        }
        else
        {
            word c = PackPixel555(color);
            primitives::OutlinedRectangle(ScreenBufferSection, ScreenBufferWidth, x, y, w, h, size, c, ClippingRectangle, copyWord);
        }

    }
    else
    {

        if (PixelFormat == RGB565)
        {
            primitives::OutlinedRectangle(ScreenBufferSection, ScreenBufferWidth, x, y, w, h, size, color, ClippingRectangle, blend565);
        }
        else
        {
            primitives::OutlinedRectangle(ScreenBufferSection, ScreenBufferWidth, x, y, w, h, size, color, ClippingRectangle, blend555);
        }

    }
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawRectangle(int x, int y, int w, int h, RGBA color)
{
    if (color.alpha == 0)
    {          // no mask

        return;

    }
    else if (color.alpha == 255)
    { // full mask

        if (PixelFormat == RGB565)
        {
            word c = PackPixel565(color);
            primitives::Rectangle(ScreenBufferSection, ScreenBufferWidth, x, y, w, h, c, ClippingRectangle, copyWord);
        }
        else
        {
            word c = PackPixel555(color);
            primitives::Rectangle(ScreenBufferSection, ScreenBufferWidth, x, y, w, h, c, ClippingRectangle, copyWord);
        }

    }
    else
    {

        if (PixelFormat == RGB565)
        {
            primitives::Rectangle(ScreenBufferSection, ScreenBufferWidth, x, y, w, h, color, ClippingRectangle, blend565);
        }
        else
        {
            primitives::Rectangle(ScreenBufferSection, ScreenBufferWidth, x, y, w, h, color, ClippingRectangle, blend555);
        }

    }
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawGradientRectangle(int x, int y, int w, int h, RGBA colors[4])
{
    if (PixelFormat == RGB565)
    {
        primitives::GradientRectangle(ScreenBufferSection, ScreenBufferWidth, x, y, w, h, colors, ClippingRectangle, blend565, interpolateRGBA);
    }
    else
    {
        primitives::GradientRectangle(ScreenBufferSection, ScreenBufferWidth, x, y, w, h, colors, ClippingRectangle, blend555, interpolateRGBA);
    }
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawOutlinedComplex(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, RGBA color, int antialias)
{
    if (color.alpha == 0)
    {          // no mask

        return;

    }
    else
    {
        if (PixelFormat == RGB565)
        {
            primitives::OutlinedComplex(ScreenBufferSection, ScreenBufferWidth, r_x, r_y, r_w, r_h, circ_x, circ_y, circ_r, color, antialias, ClippingRectangle, blend565);
        }
        else
        {
            primitives::OutlinedComplex(ScreenBufferSection, ScreenBufferWidth, r_x, r_y, r_w, r_h, circ_x, circ_y, circ_r, color, antialias, ClippingRectangle, blend555);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawFilledComplex(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, float angle, float frac_size, int fill_empty, RGBA colors[2])
{
    if (colors[0].alpha == 0 && colors[1].alpha == 0)
    {          // no mask

        return;

    }
    else
    {
        if (PixelFormat == RGB565)
        {
            primitives::FilledComplex(ScreenBufferSection, ScreenBufferWidth, r_x, r_y, r_w, r_h, circ_x, circ_y, circ_r, angle, frac_size, fill_empty, colors, ClippingRectangle, blend565);
        }
        else
        {
            primitives::FilledComplex(ScreenBufferSection, ScreenBufferWidth, r_x, r_y, r_w, r_h, circ_x, circ_y, circ_r, angle, frac_size, fill_empty, colors, ClippingRectangle, blend555);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawGradientComplex(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, float angle, float frac_size, int fill_empty, RGBA colors[3])
{
    if (colors[0].alpha == 0 && colors[1].alpha == 0 && colors[2].alpha == 0)
    {          // no mask

        return;

    }
    else
    {
        if (PixelFormat == RGB565)
        {
            primitives::GradientComplex(ScreenBufferSection, ScreenBufferWidth, r_x, r_y, r_w, r_h, circ_x, circ_y, circ_r, angle, frac_size, fill_empty, colors, ClippingRectangle, blend565);
        }
        else
        {
            primitives::GradientComplex(ScreenBufferSection, ScreenBufferWidth, r_x, r_y, r_w, r_h, circ_x, circ_y, circ_r, angle, frac_size, fill_empty, colors, ClippingRectangle, blend555);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawOutlinedEllipse(int x, int y, int rx, int ry, RGBA color)
{
    if (color.alpha == 0)
    {          // no mask

        return;

    }
    else
    {
        if (PixelFormat == RGB565)
        {
            primitives::OutlinedEllipse(ScreenBufferSection, ScreenBufferWidth, x, y, rx, ry, color, ClippingRectangle, blend565);
        }
        else
        {
            primitives::OutlinedEllipse(ScreenBufferSection, ScreenBufferWidth, x, y, rx, ry, color, ClippingRectangle, blend555);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawFilledEllipse(int x, int y, int rx, int ry, RGBA color)
{
    if (color.alpha == 0)
    {          // no mask

        return;

    }
    else
    {
        if (PixelFormat == RGB565)
        {
            primitives::FilledEllipse(ScreenBufferSection, ScreenBufferWidth, x, y, rx, ry, color, ClippingRectangle, blend565);
        }
        else
        {
            primitives::FilledEllipse(ScreenBufferSection, ScreenBufferWidth, x, y, rx, ry, color, ClippingRectangle, blend555);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawOutlinedCircle(int x, int y, int r, RGBA color, int antialias)
{
    if (color.alpha == 0)
    {          // no mask

        return;

    }
    else
    {
        if (PixelFormat == RGB565)
        {
            primitives::OutlinedCircle(ScreenBufferSection, ScreenBufferWidth, x, y, r, color, antialias, ClippingRectangle, blend565);
        }
        else
        {
            primitives::OutlinedCircle(ScreenBufferSection, ScreenBufferWidth, x, y, r, color, antialias, ClippingRectangle, blend555);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawFilledCircle(int x, int y, int r, RGBA color, int antialias)
{
    if (color.alpha == 0)
    {          // no mask

        return;

    }
    else
    {
        if (PixelFormat == RGB565)
        {
            primitives::FilledCircle(ScreenBufferSection, ScreenBufferWidth, x, y, r, color, antialias, ClippingRectangle, blend565);
        }
        else
        {
            primitives::FilledCircle(ScreenBufferSection, ScreenBufferWidth, x, y, r, color, antialias, ClippingRectangle, blend555);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawGradientCircle(int x, int y, int r, RGBA colors[2], int antialias)
{
    if (colors[0].alpha == 0 && colors[1].alpha == 0)
    {          // no mask

        return;

    }
    else
    {
        if (PixelFormat == RGB565)
        {
            primitives::GradientCircle(ScreenBufferSection, ScreenBufferWidth, x, y, r, colors, antialias, ClippingRectangle, blend565);
        }
        else
        {
            primitives::GradientCircle(ScreenBufferSection, ScreenBufferWidth, x, y, r, colors, antialias, ClippingRectangle, blend555);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
