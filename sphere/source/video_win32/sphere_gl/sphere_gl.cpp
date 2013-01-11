#include <string>

// we need to define it before including windows.h to suppress the min/max macros
#define NOMINMAX

#include <windows.h>
#include <GL/gl.h>
#include <math.h>

#include "../../common/Image32.hpp"
#include "../../common/rgb.hpp"
#include "../../common/VectorStructs.hpp"
#include "../common/win32x.hpp"
#include "resource.h"


#ifndef CDS_FULLSCREEN    // CDS_FULLSCREEN is not defined by some compilers
#define CDS_FULLSCREEN 4
#endif

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

#ifndef GL_FUNC_ADD_EXT
#define GL_FUNC_ADD_EXT 0x8006
#endif

#ifndef GL_FUNC_SUBTRACT_EXT
#define GL_FUNC_SUBTRACT_EXT 0x800A
#endif

#ifndef GL_FUNC_REVERSE_SUBTRACT_EXT
#define GL_FUNC_REVERSE_SUBTRACT_EXT 0x800B
#endif


#define EXPORT(ret) extern "C" ret __stdcall
#define SCALE()  (DriverConfig.scale ? 2 : 1)
#define FILTER() (SCALE() == 1 ? GL_NEAREST : (DriverConfig.bilinear != 0 ? GL_LINEAR : GL_NEAREST))


typedef struct tagIMAGE
{
    enum
    {
        EMPTY,
        TILE,
        NORMAL
    };

    int     width;
    int     height;

    RGBA*   pixels;

    GLuint  texture;
    GLfloat tex_width;
    GLfloat tex_height;

    int     special;  // optimization flags

    int toggle_counter;

} * IMAGE;

struct DRIVERINFO
{
    const char* name;
    const char* author;
    const char* date;
    const char* version;
    const char* description;
};

struct DRIVERCONFIG
{
    int      bitdepth;
    bool     scale;
    bool     fullscreen;
    bool     bilinear;
    bool     vsync;
};


// extension function pointers
void (APIENTRY* glBlendEquationEXT)(GLenum);
BOOL (APIENTRY* wglSwapIntervalEXT)(int interval);
int  (APIENTRY* wglGetSwapIntervalEXT)();


// FUNCTION PROTOTYPES //
EXPORT(void)  CloseVideoDriver();
EXPORT(RGBA*) LockImage(IMAGE image);
EXPORT(void)  UnlockImage(IMAGE image, bool pixels_changed);
EXPORT(void)  DirectGrab(int x, int y, int w, int h, RGBA* pixels);

static BOOL CALLBACK ConfigureDriverDialogProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam);
static void UpdateButtonStates(HWND dialog);

static DRIVERCONFIG DriverConfig;
static HINSTANCE    DriverInstance;

static int ScreenWidth;
static int ScreenHeight;

static bool fullscreen;
static int toggle_counter; // needed to recreate textures after losing the GL context

static GLint MaxTexSize; // width or height
static HWND  SphereWindow;
static DWORD WindowStyle;   // } only use in fullscreen
static DWORD WindowStyleEx; // }
static HDC   MainDC;
static HGLRC MainRC;

////////////////////////////////////////////////////////////////////////////////
extern "C" BOOL WINAPI DllMain(HINSTANCE inst, DWORD, LPVOID)
{
    DriverInstance = inst;
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
void GetConfigFile(char configfile[MAX_PATH])
{
    GetModuleFileName(DriverInstance, configfile, 512);
    if (strrchr(configfile, '\\'))
    {
        *strrchr(configfile, '\\') = 0;
        strcat(configfile, "\\");
    }
    else
    {
        configfile[0] = 0;
    }
    strcat(configfile, "sphere_gl.cfg");
}

////////////////////////////////////////////////////////////////////////////////
void LoadDriverConfig()
{
    char configfile[520];
    GetConfigFile(configfile);

    DriverConfig.fullscreen = (0 != GetPrivateProfileInt("sphere_gl", "Fullscreen",      0, configfile));
    DriverConfig.vsync      = (0 != GetPrivateProfileInt("sphere_gl", "VSync",           1, configfile));
    DriverConfig.scale      = (0 != GetPrivateProfileInt("sphere_gl", "Scale",           0, configfile));
    DriverConfig.bilinear   = (0 != GetPrivateProfileInt("sphere_gl", "BilinearFilter",  0, configfile));
    DriverConfig.bitdepth   =       GetPrivateProfileInt("sphere_gl", "BitDepth",       32, configfile);
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) GetDriverInfo(DRIVERINFO* driverinfo)
{
    driverinfo->name        = "Sphere GL";
    driverinfo->author      = "Jamie Gennis, Kisai, Chad Austin";
    driverinfo->date        = __DATE__;
    driverinfo->version     = "v1.3";
    driverinfo->description = "Hardware Accelerated OpenGL Sphere Video Driver";
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(bool) InitVideoDriver(HWND window, int screen_width, int screen_height)
{

    SphereWindow = window;
    ScreenWidth  = screen_width;
    ScreenHeight = screen_height;

    const char* error_msg = NULL;

    static bool firstcall = true;

    if (firstcall)
    {
        LoadDriverConfig();
        fullscreen = DriverConfig.fullscreen;
        firstcall  = false;
        toggle_counter = 0;
    }

    int PixelFormat;
    PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
        1,                              // version number
        PFD_DRAW_TO_WINDOW |            // support window
        PFD_SUPPORT_OPENGL |            // support OpenGL
        PFD_DOUBLEBUFFER,               // double buffered
        PFD_TYPE_RGBA,                  // RGBA type
        DriverConfig.bitdepth,          // color depth
        0, 0, 0, 0, 0, 0,               // color bits
        0,                              // alpha buffer
        0,                              // shift bit
        0,                              // accumulation buffer
        0, 0, 0, 0,                     // accum bits
        0,                              // z-buffer
        0,                              // stencil buffer
        0,                              // auxiliary buffer
        PFD_MAIN_PLANE,                 // main layer
        0,                              // reserved
        0, 0, 0                         // layer masks ignored
    };

    if (!fullscreen)
    {

        CenterWindow(SphereWindow, ScreenWidth * SCALE(), ScreenHeight * SCALE());
    }
    else
    {
        // set fullscreen mode
        DEVMODE dm;
        memset(&dm, 0, sizeof(dm));

        dm.dmSize       = sizeof(dm);
        dm.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
        dm.dmBitsPerPel = DriverConfig.bitdepth;
        dm.dmPelsWidth  = ScreenWidth  * SCALE();
        dm.dmPelsHeight = ScreenHeight * SCALE();

        WindowStyle   = GetWindowLong(SphereWindow, GWL_STYLE);
        WindowStyleEx = GetWindowLong(SphereWindow, GWL_EXSTYLE);

        SetWindowLong(SphereWindow, GWL_STYLE, WS_POPUP | WS_CLIPSIBLINGS);
        SetWindowLong(SphereWindow, GWL_EXSTYLE, 0);

        if (ChangeDisplaySettings(&dm, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
        {
            error_msg = "Unable to set fullscreen mode";
            goto error;
        }

        if (1)
        {

            DEVMODE dm;
            memset(&dm, 0, sizeof(dm));
            EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);
            if (dm.dmBitsPerPel != DriverConfig.bitdepth)
            {

                error_msg = "Unable to set bits per pixel, try a different setting";
                goto error;
            }
        }

        // Set up window
        SetWindowPos(SphereWindow, HWND_TOPMOST, 0, 0, ScreenWidth * SCALE(), ScreenHeight * SCALE(), SWP_SHOWWINDOW);
    }

    // Get the DC of the window
    MainDC = GetDC(SphereWindow);
    if (!MainDC)
    {

        MessageBox(SphereWindow, "Error getting window DC.", "Video Error", MB_ICONERROR);
        return false;
    }

    // Set the pfd
    PixelFormat = ChoosePixelFormat(MainDC, &pfd);
    if (!SetPixelFormat(MainDC, PixelFormat, &pfd))
    {

        error_msg = "Error setting the pixel format";
        goto error;
    }

    // Create Render Context
    MainRC = wglCreateContext(MainDC);
    if (!MainRC)
    {

        error_msg = "Error creating render context";
        goto error;
    }

    // Make context current
    if (!wglMakeCurrent(MainDC, MainRC))
    {

        error_msg = "Unable to make render context current";
        goto error;
    }

    // try to get the swap control extension
    if (strstr((const char*)glGetString(GL_EXTENSIONS), "WGL_EXT_swap_control"))
    {
        wglSwapIntervalEXT    = (BOOL (__stdcall*)(int))wglGetProcAddress("wglSwapIntervalEXT");
        wglGetSwapIntervalEXT = (int (__stdcall*)())wglGetProcAddress("wglGetSwapIntervalEXT");
        if (fullscreen && DriverConfig.vsync)
        {
            wglSwapIntervalEXT(1);
        }
        else
        {
            wglSwapIntervalEXT(0);
        }
    }

    // try to get glBlendEquationEXT
    if (strstr((const char*)glGetString(GL_EXTENSIONS), "GL_EXT_blend_minmax"))
    {
        glBlendEquationEXT = (void (APIENTRY*)(GLenum))wglGetProcAddress("glBlendEquationEXT");
    }
    else
    {
        error_msg = "GL_EXT_blend_minmax extension not available";
        goto error;
    }

    // make sure GL_EXT_blend_subtract is also available
    if (!strstr((const char*)glGetString(GL_EXTENSIONS), "GL_EXT_blend_subtract"))
    {
        error_msg = "GL_EXT_blend_subtract extension not available";
        goto error;
    }

    // view initialization
    glViewport(0, 0, ScreenWidth * SCALE(), ScreenHeight * SCALE());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, ScreenWidth, ScreenHeight, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.375, 0.375, 0.0);

    // we don't need the depth buffer
    glDisable(GL_DEPTH_TEST);

    // set the clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // render initialization
    glEnable(GL_SCISSOR_TEST);
    glScissor(0, 0, ScreenWidth * SCALE(), ScreenHeight * SCALE());
    glShadeModel(GL_SMOOTH);

    //blending initialization
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPointSize(FILTER() == GL_LINEAR ? 2.0f : (SCALE() == 1 ? 1.0f : 2.0f));
    glLineWidth(FILTER() == GL_LINEAR ? 2.0f : (SCALE() == 1 ? 1.0f : 2.0f));

    if (FILTER() == GL_LINEAR)
    {
        glHint(GL_POINT_SMOOTH_HINT,   GL_NICEST);
        glHint(GL_LINE_SMOOTH_HINT,    GL_NICEST);
        glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

        glEnable(GL_POINT_SMOOTH);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_POLYGON_SMOOTH);
    }
    else
    {
        glDisable(GL_POINT_SMOOTH);
        glDisable(GL_LINE_SMOOTH);
        glDisable(GL_POLYGON_SMOOTH);
    }

    // get max texture size
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &MaxTexSize);

    return true;

error:

    if (fullscreen)
    {

        ChangeDisplaySettings(NULL, 0);
        SetWindowLong(SphereWindow, GWL_STYLE, WindowStyle);
        SetWindowLong(SphereWindow, GWL_EXSTYLE, WindowStyleEx);
    }

    MessageBox(SphereWindow, error_msg, "Video Error", MB_ICONERROR);

    return false;
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) CloseVideoDriver()
{

    if (SphereWindow != 0)
    {
        if (MainDC != 0)
        {
            wglMakeCurrent (MainDC, 0);

            if (MainRC != 0)
            {
                wglDeleteContext (MainRC);
                MainRC = 0;

            }

            ReleaseDC (SphereWindow, MainDC);
            MainDC = 0;
        }
    }

    // reset screen resolution
    if (fullscreen)
    {

        ChangeDisplaySettings(NULL, 0);
        SetWindowLong(SphereWindow, GWL_STYLE, WindowStyle);
        SetWindowLong(SphereWindow, GWL_EXSTYLE, WindowStyleEx);
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Switches from fullscreen to windowed or vice-versa,
 * updates the fullscreen flag as needed
 * returns whether the engine should *not* be shutdown
 */
EXPORT(bool) ToggleFullScreen()
{
    // NOTE: fullscreen toggling will lose the GL context,
    //       so we need to recreate all textures afterwards

    extern void __stdcall GetClippingRectangle(int*, int*, int*, int*);
    extern void __stdcall SetClippingRectangle(int, int, int, int);

    // increase the toggle counter to state that textures need to be recreated
    toggle_counter++;

    int x, y, w, h;
    GetClippingRectangle(&x, &y, &w, &h);

    // if we haven't set a screen size, don't close the old driver
    if (ScreenWidth != 0 || ScreenHeight != 0)
        CloseVideoDriver();

    fullscreen = !fullscreen;

    // attempt to switch
    if (InitVideoDriver(SphereWindow, ScreenWidth, ScreenHeight))
    {
        SetClippingRectangle(x, y, w, h);
        return true;
    }
    else
    {
        fullscreen = !fullscreen;

        // attempt to switch back since the switch failed
        if (InitVideoDriver(SphereWindow, ScreenWidth, ScreenHeight))
        {
            SetClippingRectangle(x, y, w, h);
            return true;
        }
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) FlipScreen()
{
    SwapBuffers(MainDC);
    glClear(GL_COLOR_BUFFER_BIT);
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) SetClippingRectangle(int x, int y, int w, int h)
{
    glScissor(x * SCALE(), (ScreenHeight - y - h) * SCALE(), w * SCALE(), h * SCALE());
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) GetClippingRectangle(int* x, int* y, int* w, int* h)
{
    GLint cliprect[4];
    glGetIntegerv(GL_SCISSOR_BOX, cliprect);
    *x = (cliprect[0]) / SCALE();
    *y = (cliprect[1] - ScreenHeight * SCALE() + cliprect[3]) / SCALE();
    *w = (cliprect[2]) / SCALE();
    *h = (cliprect[3]) / SCALE();
}

////////////////////////////////////////////////////////////////////////////////
static bool CreateTexture(IMAGE image)
{
    // figure out if image needs to be scaled
    double log2_width  = log10((double)image->width)  / log10((double)2);
    double log2_height = log10((double)image->height) / log10((double)2);
    int new_width = image->width;
    int new_height = image->height;

    // if they're not integers, calculate a good texture width
    if (log2_width != floor(log2_width))
    {
        new_width = 1 << (int)ceil(log2_width);
    }

    // if they're not integers, calculate a good texture height
    if (log2_height != floor(log2_height))
    {
        new_height = 1 << (int)ceil(log2_height);
    }

    RGBA* new_pixels = image->pixels;
    if (new_width != image->width || new_height != image->height)
    {

        // copy the old pixels into the new buffer
        new_pixels = new RGBA[new_width * new_height];

        if (!new_pixels)
            return false;
        for (int i = 0; i < image->height; i++)
        {
            memcpy(new_pixels + i * new_width,
                   image->pixels + i * image->width,
                   image->width * sizeof(RGBA));
        }
    }

    // now make sure texture is, at max, MaxTexSize by MaxTexSize
    while (new_width > MaxTexSize)
    {
        new_width /= 2;

        // allocate a new texture buffer
        RGBA* old_pixels = new_pixels;
        new_pixels = new RGBA[new_width * new_height];

        if (!new_pixels)
            return false;

        RGBA* p = new_pixels;
        RGBA* o = old_pixels;

        for (int iy = 0; iy < new_height; iy++)
        {
            for (int ix = 0; ix < new_width; ix++)
            {
                p[ix].red   = (o[ix * 2].red   + o[ix * 2 + 1].red)   / 2;
                p[ix].green = (o[ix * 2].green + o[ix * 2 + 1].green) / 2;
                p[ix].blue  = (o[ix * 2].blue  + o[ix * 2 + 1].blue)  / 2;
                p[ix].alpha = (o[ix * 2].alpha + o[ix * 2 + 1].alpha) / 2;
            }

            p += new_width;
            o += new_width * 2;
        }

        if (old_pixels != image->pixels)
        {
            delete[] old_pixels;
        }
    }

    while (new_height > MaxTexSize)
    {
        new_height /= 2;

        // allocate a new texture buffer
        RGBA* old_pixels = new_pixels;
        new_pixels = new RGBA[new_width * new_height];

        if (!new_pixels)
            return false;

        RGBA* p = new_pixels;
        RGBA* o = old_pixels;

        for (int ix = 0; ix < new_width; ix++)
        {
            for (int iy = 0; iy < new_height; iy++)
            {
                p[iy * new_width].red   = (o[(iy * 2) * new_width].red   + o[(iy * 2 + 1) * new_width].red)   / 2;
                p[iy * new_width].green = (o[(iy * 2) * new_width].green + o[(iy * 2 + 1) * new_width].green) / 2;
                p[iy * new_width].blue  = (o[(iy * 2) * new_width].blue  + o[(iy * 2 + 1) * new_width].blue)  / 2;
                p[iy * new_width].alpha = (o[(iy * 2) * new_width].alpha + o[(iy * 2 + 1) * new_width].alpha) / 2;
            }

            p++;
            o++;
        }

        if (old_pixels != image->pixels)
        {
            delete[] old_pixels;
        }

    }

    // minor correction factor
    float correction_x = (DriverConfig.scale && DriverConfig.bilinear ? 0.5f / (float)new_width  : 0.0f);
    float correction_y = (DriverConfig.scale && DriverConfig.bilinear ? 0.5f / (float)new_height : 0.0f);
    image->tex_width  = (GLfloat)image->width  / new_width  - correction_x;
    image->tex_height = (GLfloat)image->height / new_height - correction_y;

    glGenTextures(1, &image->texture);
    glBindTexture(GL_TEXTURE_2D, image->texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (float)FILTER());
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (float)FILTER());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, new_width, new_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, new_pixels);

    if (new_pixels != image->pixels)
    {
        delete[] new_pixels;
    }

    image->toggle_counter = toggle_counter;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
static int AnalyzePixels(int width, int height, RGBA* pixels)
{
    bool is_empty = true;
    for (int i = 0; i < width * height; i++)
    {
        if (pixels[i].alpha)
        {
            is_empty = false;
            break;
        }
    }
    if (is_empty)
    {
        return tagIMAGE::EMPTY;
    }

    // no alpha data (tile)
    bool is_tile = true;
    for (int i = 0; i < width * height; i++)
    {
        if (pixels[i].alpha < 255)
        {
            is_tile = false;
            break;
        }
    }
    if (is_tile)
    {
        return tagIMAGE::TILE;
    }

    // normal image
    return tagIMAGE::NORMAL;
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(IMAGE) CreateImage(int width, int height, RGBA* pixels)
{
    // put default values in image object
    IMAGE image = new tagIMAGE;

    if (!image)
        return NULL;

    image->width  = width;
    image->height = height;
    image->pixels = new RGBA[width * height];

    if (!pixels)
    {
        delete image;
        return NULL;
    }

    memcpy(image->pixels, pixels, width * height * sizeof(RGBA));

    if (!CreateTexture(image))
    {
        delete image;
        delete[] image->pixels;
        return NULL;
    }

    image->special = AnalyzePixels(width, height, pixels);

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
EXPORT(IMAGE) GrabImage(int x, int y, int width, int height)
{
    RGBA* pixels = new RGBA[width * height];

    if (!pixels)
        return NULL;

    DirectGrab(x, y, width, height, pixels);
    IMAGE result = CreateImage(width, height, pixels);

    delete[] pixels;

    return result;
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DestroyImage(IMAGE image)
{
    glDeleteTextures(1, &image->texture);
    delete[] image->pixels;
    delete image;
}

////////////////////////////////////////////////////////////////////////////////
static void
set_blendmode(CImage32::BlendMode blendmode)
{
    switch (blendmode)
    {
        case CImage32::BLEND:
            glBlendEquationEXT(GL_FUNC_ADD_EXT);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;

        case CImage32::ADD:
            glBlendEquationEXT(GL_FUNC_ADD_EXT);
            glBlendFunc(GL_ONE, GL_ONE);
            break;

        case CImage32::SUBTRACT:
            glBlendEquationEXT(GL_FUNC_REVERSE_SUBTRACT_EXT);
            glBlendFunc(GL_ONE, GL_ONE);
            break;

        case CImage32::MULTIPLY:
            glBlendEquationEXT(GL_FUNC_ADD_EXT);
            glBlendFunc(GL_DST_COLOR, GL_ZERO);
            break;

        default:
            glBlendEquationEXT(GL_FUNC_ADD_EXT);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
static void
blit_image(IMAGE image, int x[4], int y[4], RGBA color)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, image->texture);

    glColor4ub(color.red, color.green, color.blue, color.alpha);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2i(x[0], y[0]);
    glTexCoord2f(image->tex_width, 0.0f);
    glVertex2i(x[1], y[1]);
    glTexCoord2f(image->tex_width, image->tex_height);
    glVertex2i(x[2], y[2]);
    glTexCoord2f(0.0f, image->tex_height);
    glVertex2i(x[3], y[3]);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

////////////////////////////////////////////////////////////////////////////////
static void
blit_image(IMAGE image, int x[4], int y[4])
{
    RGBA color = {255, 255, 255, 255};
    blit_image(image, x, y, color);
}

////////////////////////////////////////////////////////////////////////////////
static IMAGE
create_masked_image_add(IMAGE image, RGBA mask)
{
    const int num_pixels = image->width * image->height;
    RGBA* pixels = new RGBA[num_pixels];

    if (!pixels)
        return NULL;

    for (int i = 0; i < num_pixels; ++i)
    {
        pixels[i].red   = std::min(image->pixels[i].red   + mask.red,   255);
        pixels[i].green = std::min(image->pixels[i].green + mask.green, 255);
        pixels[i].blue  = std::min(image->pixels[i].blue  + mask.blue,  255);
        pixels[i].alpha = std::min(image->pixels[i].alpha + mask.alpha, 255);
    }

    IMAGE masked_image = CreateImage(image->width, image->height, pixels);
    delete[] pixels;
    return masked_image;
}

////////////////////////////////////////////////////////////////////////////////
static IMAGE
create_masked_image_subtract(IMAGE image, RGBA mask)
{
    const int num_pixels = image->width * image->height;
    RGBA* pixels = new RGBA[num_pixels];

    if (!pixels)
        return NULL;

    for (int i = 0; i < num_pixels; ++i)
    {
        pixels[i].red   = std::max(image->pixels[i].red   - mask.red,   0);
        pixels[i].green = std::max(image->pixels[i].green - mask.green, 0);
        pixels[i].blue  = std::max(image->pixels[i].blue  - mask.blue,  0);
        pixels[i].alpha = std::max(image->pixels[i].alpha - mask.alpha, 0);
    }

    IMAGE masked_image = CreateImage(image->width, image->height, pixels);
    delete[] pixels;
    return masked_image;
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) BlitImage(IMAGE image, int x, int y, CImage32::BlendMode blendmode)
{
    if (image->toggle_counter != toggle_counter)
        CreateTexture(image);

    if (image->special == tagIMAGE::EMPTY)
        return;

    int tx[4] = {x, x + image->width, x + image->width, x};
    int ty[4] = {y, y, y + image->height, y + image->height};

    set_blendmode(blendmode);
    blit_image(image, tx, ty);

}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) BlitImageMask(IMAGE image, int x, int y, CImage32::BlendMode blendmode,
                           RGBA mask, CImage32::BlendMode mask_blendmode)
{
    if (image->toggle_counter != toggle_counter)
        CreateTexture(image);

    if (image->special == tagIMAGE::EMPTY)
        return;

    int tx[4] = {x, x + image->width, x + image->width, x};
    int ty[4] = {y, y, y + image->height, y + image->height};

    set_blendmode(blendmode);

    switch (mask_blendmode)
    {
        case CImage32::ADD:
        {
            IMAGE masked_image = create_masked_image_add(image, mask);
            if (masked_image)
            {
                blit_image(masked_image, tx, ty);
                DestroyImage(masked_image);
            }
        }
        break;

        case CImage32::SUBTRACT:
        {
            IMAGE masked_image = create_masked_image_subtract(image, mask);
            if (masked_image)
            {
                blit_image(masked_image, tx, ty);
                DestroyImage(masked_image);
            }
        }
        break;

        case CImage32::MULTIPLY:
        {
            blit_image(image, tx, ty, mask);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) TransformBlitImage(IMAGE image, int x[4], int y[4], CImage32::BlendMode blendmode)
{
    if (image->toggle_counter != toggle_counter)
        CreateTexture(image);

    if (image->special == tagIMAGE::EMPTY)
        return;

    set_blendmode(blendmode);
    blit_image(image, x, y);
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) TransformBlitImageMask(IMAGE image, int x[4], int y[4], CImage32::BlendMode blendmode,
                                    RGBA mask, CImage32::BlendMode mask_blendmode)
{
    if (image->toggle_counter != toggle_counter)
        CreateTexture(image);

    if (image->special == tagIMAGE::EMPTY)
        return;

    set_blendmode(blendmode);

    switch (mask_blendmode)
    {
        case CImage32::ADD:
        {
            IMAGE masked_image = create_masked_image_add(image, mask);
            if (masked_image)
            {
                blit_image(masked_image, x, y);
                DestroyImage(masked_image);
            }
        }
        break;

        case CImage32::SUBTRACT:
        {
            IMAGE masked_image = create_masked_image_subtract(image, mask);
            if (masked_image)
            {
                blit_image(masked_image, x, y);
                DestroyImage(masked_image);
            }
        }
        break;

        case CImage32::MULTIPLY:
        {
            blit_image(image, x, y, mask);
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
EXPORT(RGBA*) LockImage(IMAGE image)
{
    return image->pixels;
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) UnlockImage(IMAGE image, bool pixels_changed)
{

    if (pixels_changed)
    {

        glDeleteTextures(1, &image->texture);
        CreateTexture(image);
        image->special = AnalyzePixels(image->width, image->height, image->pixels);

    }
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DirectBlit(int x, int y, int w, int h, RGBA* pixels)
{
    IMAGE i = CreateImage(w, h, pixels);

    if (i)
    {
        BlitImage(i, x, y, CImage32::BLEND);
        DestroyImage(i);

    }
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DirectTransformBlit(int x[4], int y[4], int w, int h, RGBA* pixels)
{
    IMAGE i = CreateImage(w, h, pixels);

    if (i)
    {
        TransformBlitImage(i, x, y, CImage32::BLEND);
        DestroyImage(i);

    }
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DirectGrab(int x, int y, int w, int h, RGBA* pixels)
{
    if (x < 0 || y < 0 || x + w > ScreenWidth || y + h > ScreenHeight)
    {
        return;
    }

    if (SCALE() == 2)
    {

        // manually scale the framebuffer down
        RGBA* new_pixels = new RGBA[4 * w * h];

        if (!new_pixels)
            return;

        glReadPixels(x * 2, (ScreenHeight - y - h) * 2, w * 2, h * 2, GL_RGBA, GL_UNSIGNED_BYTE, new_pixels);

        for (int i = 0; i < w; i++)
        {
            for (int j = 0; j < h; j++)
            {
                pixels[j * w + i] = new_pixels[(j * 2) * (w * 2) + i * 2];
            }
        }

        delete[] new_pixels;
    }
    else
    {
        glReadPixels(x, ScreenHeight - y - h, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    }

    // now invert the rows
    RGBA* row = new RGBA[w];

    if (!row)
        return;

    RGBA* top = pixels;
    RGBA* bot = pixels + w * (h - 1);

    while (top < bot)
    {
        memcpy(row, top, w * sizeof(RGBA));
        memcpy(top, bot, w * sizeof(RGBA));
        memcpy(bot, row, w * sizeof(RGBA));

        top += w;
        bot -= w;
    }

    delete[] row;
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawPoint(int x, int y, RGBA color)
{
    glBegin(GL_POINTS);

    glColor4ubv((GLubyte*)&color);
    glVertex2i(x, y);

    glEnd();
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawPointSeries(VECTOR_INT** points, int length, RGBA color)
{
    if (color.alpha == 0)
        return;

    glColor4ubv((GLubyte*)&color);

    glBegin(GL_POINTS);

    for (int i = 0; i < length; ++i)
        glVertex2i(points[i]->x, points[i]->y);

    glEnd();
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawLine(int x[2], int y[2], RGBA color)
{
    glColor4ub(color.red, color.green, color.blue, color.alpha);

    glBegin(GL_LINES);

    glVertex2i(x[0], y[0]);
    glVertex2i(x[1], y[1]);

    glEnd();
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawGradientLine(int x[2], int y[2], RGBA colors[2])
{
    glBegin(GL_LINES);

    glColor4ubv((GLubyte*)(colors + 0));
    glVertex2i(x[0], y[0]);

    glColor4ubv((GLubyte*)(colors + 1));
    glVertex2i(x[1], y[1]);

    glEnd();
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawLineSeries(VECTOR_INT** points, int length, RGBA color, int type)
{
    if (color.alpha == 0)
        return;

    glColor4ubv((GLubyte*)&color);

    if (type == 0)
        glBegin(GL_LINES);
    else if (type == 1)
        glBegin(GL_LINE_STRIP);
    else
        glBegin(GL_LINE_LOOP);

    for (int i = 0; i < length; ++i)
        glVertex2i(points[i]->x, points[i]->y);

    glEnd();
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawBezierCurve(int x[4], int y[4], double step, RGBA color, int cubic)
{
    if (color.alpha == 0)
        return;

    // make sure step is in a valid range
    if (step <= 0)
        step = 0.001;
    else if (step > 1.0)
        step = 1.0;

    int new_x, new_y, old_x, old_y;
    double b;

    glColor4ubv((GLubyte*)&color);

    glBegin(GL_POINTS);

    for (double a = 1.0; a >= 0; a -= step)
    {
        b = 1.0 - a;
        if (cubic)
        {               // calculate the cubic Bezier curve
            new_x = (int)(x[0]*a*a*a + x[1]*3*a*a*b + x[3]*3*a*b*b + x[2]*b*b*b);
            new_y = (int)(y[0]*a*a*a + y[1]*3*a*a*b + y[3]*3*a*b*b + y[2]*b*b*b);
        }
        else
        {               // calculate the quadratic Bezier curve
            new_x = (int)(x[0]*a*a + x[1]*2*a*b + x[2]*b*b);
            new_y = (int)(y[0]*a*a + y[1]*2*a*b + y[2]*b*b);
        }
        if (a != 1.0)
        {
            if (new_x != old_x || new_y != old_y)
                glVertex2i(new_x, new_y);

        }
        else
        {
            glVertex2i(new_x, new_y);
        }
        old_x = new_x;
        old_y = new_y;
    }

    glEnd();
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawTriangle(int x[3], int y[3], RGBA color)
{
    glColor4ubv((GLubyte*)&color);

    glBegin(GL_TRIANGLES);

    glVertex2i(x[0], y[0]);
    glVertex2i(x[1], y[1]);
    glVertex2i(x[2], y[2]);

    glEnd();
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawGradientTriangle(int x[3], int y[3], RGBA colors[3])
{
    glBegin(GL_TRIANGLES);

    glColor4ubv((GLubyte*)(colors + 0));
    glVertex2i(x[0], y[0]);

    glColor4ubv((GLubyte*)(colors + 1));
    glVertex2i(x[1], y[1]);

    glColor4ubv((GLubyte*)(colors + 2));
    glVertex2i(x[2], y[2]);

    glEnd();
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawPolygon(VECTOR_INT** points, int length, int invert, RGBA color)
{
    if (color.alpha == 0)
        return;

    // find polygon's bounds
    int i, bound_x1 = points[0]->x, bound_x2 = points[0]->x, bound_y1 = points[0]->y, bound_y2 = points[0]->y;
    for (i = 1; i < length; i++)
    {
        if (points[i]->x > bound_x2) bound_x2 = points[i]->x;
        if (points[i]->x < bound_x1) bound_x1 = points[i]->x;
        if (points[i]->y > bound_y2) bound_y2 = points[i]->y;
        if (points[i]->y < bound_y1) bound_y1 = points[i]->y;
    }

    glColor4ubv((GLubyte*)&color);

    glBegin(GL_POINTS);

    // draw the polygon with the crossing number algorithm
    int point_in, c_x, c_y, j;
    for (c_y = bound_y1; c_y <= bound_y2; c_y++)
    {
        for (c_x = bound_x1; c_x <= bound_x2; c_x++)
        {
            point_in = 0;
            j = length-1;

            for (i = 0; i < length; i++)
            {
                if (points[i]->y <= c_y && points[j]->y > c_y ||
                    points[j]->y <= c_y && points[i]->y > c_y)
                {
                    if (points[i]->x >= c_x && points[j]->x >= c_x)
                    {
                        point_in = !point_in;
                    }
                    else if ((float)(points[i]->x) +
                            ((float)(c_y - points[i]->y) /
                             (float)(points[j]->y - points[i]->y)) *
                             (float)(points[j]->x - points[i]->x) > c_x)
                    {
                        point_in = !point_in;
                    }
                }
                j = i;
            }

            if (invert)
                point_in = !point_in;

            if (point_in)
                glVertex2i(c_x, c_y);
        }
    }

    glEnd();
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawOutlinedRectangle(int x, int y, int w, int h, int size, RGBA color)
{
    if (color.alpha == 0 || size <= 0 || h / 2 < 1)
        return;

    // make sure size is in a valid range
    if (size > h / 2)
        size = h / 2;

    int iy, ty;

    glColor4ubv((GLubyte*)&color);

    glBegin(GL_LINES);

    for (iy = y; iy < y + size; iy++)
    {
        glVertex2i(x,     iy);
        glVertex2i(x + w, iy);

        ty = y + h - 1 - (iy - y);

        glVertex2i(x,     ty);
        glVertex2i(x + w, ty);
    }

    ty = y + h - size;

    for (iy = y + size; iy < ty; iy++)
    {
        glVertex2i(x,            iy);
        glVertex2i(x + size,     iy);
        glVertex2i(x + w - size, iy);
        glVertex2i(x + w,        iy);
    }

    glEnd();
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawRectangle(int x, int y, int w, int h, RGBA color)
{
    if (color.alpha == 0)
        return;

    glColor4ubv((GLubyte*)&color);

    glBegin(GL_QUADS);

    glVertex2i(x,     y);
    glVertex2i(x + w, y);
    glVertex2i(x + w, y + h);
    glVertex2i(x,     y + h);

    glEnd();
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawGradientRectangle(int x, int y, int w, int h, RGBA colors[4])
{
    glBegin(GL_QUADS);

    glColor4ubv((GLubyte*)(colors + 0));
    glVertex2i(x, y);

    glColor4ubv((GLubyte*)(colors + 1));
    glVertex2i(x + w, y);

    glColor4ubv((GLubyte*)(colors + 2));
    glVertex2i(x + w, y + h);

    glColor4ubv((GLubyte*)(colors + 3));
    glVertex2i(x, y + h);

    glEnd();
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawOutlinedComplex(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, RGBA color, int antialias)
{
    int x, y, dist, ca = color.alpha, crr = circ_r * circ_r;
    float fdist, fcr = (float)(circ_r), fca = (float)(ca);

    glBegin(GL_POINTS);

    for (y = r_y; y < r_y + r_h; y++)
    {
        for (x = r_x; x < r_x + r_w; x++)
        {
            dist = abs(circ_x-x)*abs(circ_x-x) + abs(circ_y-y)*abs(circ_y-y);
            if (dist >= crr)
            {
                color.alpha = ca;
                glColor4ub(color.red, color.green, color.blue, color.alpha);
                glVertex2i(x, y);
            }
            else if (antialias)
            {
                fdist = fcr - sqrt((float)(dist));
                if (fdist < 1)
                {
                    color.alpha = (byte)(fca * (1.0 - fdist));
                    glColor4ub(color.red, color.green, color.blue, color.alpha);
                    glVertex2i(x, y);
                }
            }
        }
    }

    glEnd();
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawFilledComplex(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, float angle, float frac_size, int fill_empty, RGBA colors[2])
{
    int x, y, dist, crr = circ_r * circ_r;
    float fang_p;
    const float PI = (float)(3.1415927);

    // make sure frac_size is in a valid range
    if (frac_size < 0 || frac_size >= PI)
        frac_size = 0;

    glBegin(GL_POINTS);

    for (y = r_y; y < r_y + r_h; y++)
    {
        for (x = r_x; x < r_x + r_w; x++)
        {
            // check if point is outside of the circle
            dist = abs(x-circ_x)*abs(x-circ_x) + abs(y-circ_y)*abs(y-circ_y);

            if (dist >= crr)
            {
                glColor4ubv((GLubyte*)(colors + 0));
                glVertex2i(x, y);
            }
            else
            {
                if (frac_size != 0)
                {
                    // check if point is located in fraction
                    fang_p = atan2(float(y-circ_y), float(x-circ_x));

                    if (fang_p < 0)
                        fang_p = PI + (PI + fang_p);

                    fang_p = fabs(angle - fang_p);

                    if (fang_p >= PI)
                        fang_p = 2*PI - fang_p;

                    if (fang_p <= frac_size)
                    {
                        // it is, so draw the point with circle's color
                        glColor4ubv((GLubyte*)(colors + 1));
                        glVertex2i(x, y);
                    }
                    else if (fill_empty)
                    {
                        // it is not, so draw the point with rectangle's color
                        glColor4ubv((GLubyte*)(colors + 0));
                        glVertex2i(x, y);
                    }
                }
                else
                {
                    glColor4ubv((GLubyte*)(colors + 1));
                    glVertex2i(x, y);
                }
            }
        }
    }

    glEnd();
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawGradientComplex(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, float angle, float frac_size, int fill_empty, RGBA colors[3])
{
    int x, y, dist, crr = circ_r * circ_r;
    float fdr = (float)(colors[2].red   - colors[1].red);
    float fdg = (float)(colors[2].green - colors[1].green);
    float fdb = (float)(colors[2].blue  - colors[1].blue);
    float fda = (float)(colors[2].alpha - colors[1].alpha);
    float fang_p, factor, fdist, fr = (float)(circ_r);
    const float PI = (float)(3.1415927), PI_H = PI / 2;

    // make sure frac_size is in a valid range
    if (frac_size < 0 || frac_size >= PI)
        frac_size = 0;

    glBegin(GL_POINTS);

    for (y = r_y; y < r_y + r_h; y++)
    {
        for (x = r_x; x < r_x + r_w; x++)
        {
            // check if point is outside of the circle
            dist = abs(x-circ_x)*abs(x-circ_x) + abs(y-circ_y)*abs(y-circ_y);

            if (dist >= crr)
            {
                glColor4ubv((GLubyte*)(colors + 0));
                glVertex2i(x, y);
            }
            else
            {
                if (frac_size != 0)
                {
                    // check if point is located in fraction
                    fang_p = atan2(float(y-circ_y), float(x-circ_x));

                    if (fang_p < 0)
                        fang_p = PI + (PI + fang_p);

                    fang_p = fabs(angle - fang_p);

                    if (fang_p >= PI)
                        fang_p = 2*PI - fang_p;

                    if (fang_p <= frac_size)
                    {
                        // it is, so draw the point with circle's color
                        fdist = sqrt((float)(dist));
                        factor = sin((float(1) - fdist / fr) * PI_H);

                        colors[1].red   = (byte)(colors[2].red   - fdr * factor);
                        colors[1].green = (byte)(colors[2].green - fdg * factor);
                        colors[1].blue  = (byte)(colors[2].blue  - fdb * factor);
                        colors[1].alpha = (byte)(colors[2].alpha - fda * factor);

                        glColor4ub(colors[1].red, colors[1].green, colors[1].blue, colors[1].alpha);
                        glVertex2i(x, y);
                    }
                    else if (fill_empty)
                    {
                        // it is not, so draw the point with rectangle's color
                        glColor4ub(colors[0].red, colors[0].green, colors[0].blue, colors[0].alpha);
                        glVertex2i(x, y);
                    }
                }
                else
                {
                    fdist = sqrt((float)(dist));
                    factor = sin((float(1) - fdist / fr) * PI_H);

                    colors[1].red   = (byte)(colors[2].red   - fdr * factor);
                    colors[1].green = (byte)(colors[2].green - fdg * factor);
                    colors[1].blue  = (byte)(colors[2].blue  - fdb * factor);
                    colors[1].alpha = (byte)(colors[2].alpha - fda * factor);

                    glColor4ub(colors[1].red, colors[1].green, colors[1].blue, colors[1].alpha);
                    glVertex2i(x, y);
                }
            }
        }
    }

    glEnd();
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawOutlinedEllipse(int xc, int yc, int rx, int ry, RGBA color)
{
    if (color.alpha == 0)
        return;

    int xcm1 = xc - 1;
    int ycm1 = yc - 1;

    // draw ellipse with bresenham's ellipse algorithm
    int x = rx;
    int y = 0;
    int tworx2 = 2 * rx * rx;
    int twory2 = 2 * ry * ry;
    int xchange = ry * ry * (1 - 2 * rx);
    int ychange = rx * rx;
    int error = 0;
    int xstop = twory2 * rx;
    int ystop = 0;

    glColor4ubv((GLubyte*)&color);

    glBegin(GL_POINTS);

    // draw first set of points
    while (xstop >= ystop)
    {
        glVertex2i(xc + x,   yc + y);
        glVertex2i(xcm1 - x, yc + y);
        glVertex2i(xcm1 - x, ycm1 - y);
        glVertex2i(xc + x,   ycm1 - y);

        y++;
        ystop   += tworx2;
        error   += ychange;
        ychange += tworx2;

        if (2 * error + xchange > 0)
        {
            x--;
            xstop -= twory2;
            error += xchange;
            xchange += twory2;
        }
    }

    x = 0;
    y = ry;
    xchange = ry * ry;
    ychange = rx * rx * (1 - 2 * ry);
    error = 0;
    xstop = 0;
    ystop = tworx2 * ry;

    // draw second set of points
    while (xstop <= ystop)
    {
        glVertex2i(xc + x,   yc + y);
        glVertex2i(xcm1 - x, yc + y);
        glVertex2i(xcm1 - x, ycm1 - y);
        glVertex2i(xc + x,   ycm1 - y);

        x++;
        xstop   += twory2;
        error   += xchange;
        xchange += twory2;

        if (2 * error + ychange > 0)
        {
            y--;
            ystop -= tworx2;
            error += ychange;
            ychange += tworx2;
        }
    }
    glEnd();
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawFilledEllipse(int xc, int yc, int rx, int ry, RGBA color)
{
    if (color.alpha == 0)
        return;

    int xcm1 = xc - 1;
    int ycm1 = yc - 1;

    // draw ellipse with bresenham's ellipse algorithm
    int x = rx;
    int y = 0;
    int tworx2 = 2 * rx * rx;
    int twory2 = 2 * ry * ry;
    int xchange = ry * ry * (1 - 2 * rx);
    int ychange = rx * rx;
    int error = 0;
    int xstop = twory2 * rx;
    int ystop = 0;

    glColor4ubv((GLubyte*)&color);

    glBegin(GL_LINES);

    // first set of points
    while (xstop >= ystop)
    {
        glVertex2i(xcm1 - x, ycm1 - y);
        glVertex2i(xc + x,   ycm1 - y);
        glVertex2i(xcm1 - x, yc + y);
        glVertex2i(xc + x,   yc + y);

        y++;
        ystop   += tworx2;
        error   += ychange;
        ychange += tworx2;

        if (2 * error + xchange > 0)
        {
            x--;
            xstop -= twory2;
            error += xchange;
            xchange += twory2;
        }
    }

    x = 0;
    y = ry;
    xchange = ry * ry;
    ychange = rx * rx * (1 - 2 * ry);
    error = 0;
    xstop = 0;
    ystop = tworx2 * ry;

    // second set of points
    while (xstop <= ystop)
    {
        x++;
        xstop   += twory2;
        error   += xchange;
        xchange += twory2;

        if (2 * error + ychange > 0)
        {
            glVertex2i(xcm1 - x, ycm1 - y);
            glVertex2i(xc + x,   ycm1 - y);
            glVertex2i(xcm1 - x, yc + y);
            glVertex2i(xc + x,   yc + y);

            y--;
            ystop -= tworx2;
            error += ychange;
            ychange += tworx2;
        }
    }

    glEnd();
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawOutlinedCircle(int x, int y, int r, RGBA color, int antialias)
{
    if (color.alpha == 0)
        return;

    int ix = 1, iy = r, dist, n, rr = r*r, rr_m2 = (r-2)*(r-2), ca = color.alpha;
    float fr = (float)(r), fca = (float)(ca);
    const float PI_H = (float)(3.1415927 / 2.0);

    glColor4ubv((GLubyte*)&color);

    glBegin(GL_POINTS);

    while (ix <= iy)
    {
        if (antialias == 1)
        {
            n = iy + 1;
            while (--n >= ix)
            {
                dist = ix*ix + n*n;
                if (dist > rr) dist = rr;
                if (dist > rr_m2)
                {
                    color.alpha = (byte)(fca * sin(sin((1.0 - fabs(sqrt((float)(dist)) - fr + 1.0)) * PI_H) * PI_H));
                    glColor4ubv((GLubyte*)&color);

                    glVertex2i(x-1+ix, y-n);
                    glVertex2i(x-ix,   y-n);
                    glVertex2i(x-1+ix, y-1+n);
                    glVertex2i(x-ix,   y-1+n);

                    if (ix != n)
                    {
                        glVertex2i(x-1+n, y-ix);
                        glVertex2i(x-n,   y-ix);
                        glVertex2i(x-1+n, y-1+ix);
                        glVertex2i(x-n,   y-1+ix);
                    }
                }
            }
        }
        else
        {
            glVertex2i(x-1+ix, y-iy);
            glVertex2i(x-ix,   y-iy);
            glVertex2i(x-1+ix, y-1+iy);
            glVertex2i(x-ix,   y-1+iy);

            if (ix != iy)
            {
                glVertex2i(x-1+iy, y-ix);
                glVertex2i(x-iy,   y-ix);
                glVertex2i(x-1+iy, y-1+ix);
                glVertex2i(x-iy,   y-1+ix);
            }
        }

        ix++;
        if (abs(ix*ix + iy*iy - rr) > abs(ix*ix + (iy-1)*(iy-1) - rr)) iy--;
    }

    glEnd();
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawFilledCircle(int x, int y, int r, RGBA color, int antialias)
{
    if (color.alpha == 0)
        return;

    int ix = 1, iy = r, dist, n, rr = r*r, rr_m1 = (r-1)*(r-1), ca = color.alpha;
    float fr = (float)(r), fca = (float)(ca);

    glColor4ub(color.red, color.green, color.blue, color.alpha);

    glBegin(GL_POINTS);

    while (ix <= iy)
    {
        n = iy + 1;
        while (--n >= ix)
        {
            if (antialias)
            {
                dist = ix*ix + n*n;
                if (dist > rr) dist = rr;
                if (dist > rr_m1) {color.alpha = (byte)(fca * (fr - sqrt(float(dist))));}
                else {color.alpha = ca;};
            }
            else {color.alpha = ca;};
            glColor4ub(color.red, color.green, color.blue, color.alpha);

            glVertex2i(x-1+ix, y-n);
            glVertex2i(x-ix,   y-n);
            glVertex2i(x-1+ix, y-1+n);
            glVertex2i(x-ix,   y-1+n);

            if (ix != n)
            {
                glVertex2i(x-1+n, y-ix);
                glVertex2i(x-n,   y-ix);
                glVertex2i(x-1+n, y-1+ix);
                glVertex2i(x-n,   y-1+ix);
            }
        }

        ix++;
        if (abs(ix*ix + iy*iy - rr) > abs(ix*ix + (iy-1)*(iy-1) - rr)) iy--;
    }

    glEnd();
}

////////////////////////////////////////////////////////////////////////////////
EXPORT(void) DrawGradientCircle(int x, int y, int r, RGBA colors[2], int antialias)
{
    if (colors[0].alpha == 0 && colors[1].alpha == 0)
        return;

    int ix = 1, iy = r, n, rr = r*r;
    float fdr = (float)(colors[1].red - colors[0].red);
    float fdg = (float)(colors[1].green - colors[0].green);
    float fdb = (float)(colors[1].blue - colors[0].blue);
    float fda = (float)(colors[1].alpha - colors[0].alpha);
    float dist, factor, fr = (float)(r);
    const float PI_H = (float)(3.1415927 / 2.0);

    glBegin(GL_POINTS);

    while (ix <= iy)
    {
        n = iy + 1;
        while (--n >= ix)
        {
            dist = sqrt((float)(ix*ix + n*n));
            if (dist > r) dist = fr;
            factor = sin((float(1) - dist / fr) * PI_H);
            colors[0].red   = (byte)(colors[1].red   - fdr * factor);
            colors[0].green = (byte)(colors[1].green - fdg * factor);
            colors[0].blue  = (byte)(colors[1].blue  - fdb * factor);
            colors[0].alpha = (byte)(colors[1].alpha - fda * factor);

            if (antialias)
                if (dist > r - 1)
                    colors[0].alpha = (byte)((float)(colors[0].alpha) * (fr - dist));

            glColor4ub(colors[0].red, colors[0].green, colors[0].blue, colors[0].alpha);
            glVertex2i(x-1+ix, y-n);
            glVertex2i(x-ix,   y-n);
            glVertex2i(x-1+ix, y-1+n);
            glVertex2i(x-ix,   y-1+n);

            if (ix != n)
            {
                glVertex2i(x-1+n, y-ix);
                glVertex2i(x-n,   y-ix);
                glVertex2i(x-1+n, y-1+ix);
                glVertex2i(x-n,   y-1+ix);
            }
        }

        ix++;
        if (abs(ix*ix + iy*iy - rr) > abs(ix*ix + (iy-1)*(iy-1) - rr)) iy--;
    }

    glEnd();
}

////////////////////////////////////////////////////////////////////////////////
