#ifndef MAC_VIDEO_H
#define MAC_VIDEO_H

#include "../../common/platform.h"
#include "../../common/Image32.hpp"
#include "../../common/rgb.hpp"
#include "../../common/VectorStructs.hpp"
#include "mac_sphere_config.h"
#include "mac_filesystem.h"

class SFONT; // for drawing FPS
typedef struct IMAGEimp* IMAGE;

void ToggleFPS();

extern void SetFPSFont(SFONT* font);
extern bool SwitchResolution(int x, int y);
extern int  GetScreenWidth();
extern int  GetScreenHeight();
extern void ToggleFullScreen();
extern void FlipScreen();

extern void  (SPHERE_STDCALL * SetClippingRectangle)(int x, int y, int w, int h);
extern void  (SPHERE_STDCALL * GetClippingRectangle)(int* x, int* y, int* w, int* h);
extern IMAGE (SPHERE_STDCALL * CreateImage)(int width, int height, const RGBA* pixels);
extern IMAGE (SPHERE_STDCALL * CloneImage)(IMAGE image);
extern IMAGE (SPHERE_STDCALL * GrabImage)(int x, int y, int width, int height);
extern void  (SPHERE_STDCALL * DestroyImage)(IMAGE image);
extern void  (SPHERE_STDCALL * BlitImage)(IMAGE image, int x, int y, CImage32::BlendMode blendmode);
extern void  (SPHERE_STDCALL * BlitImageMask)(IMAGE image, int x, int y, CImage32::BlendMode blendmode, RGBA mask, CImage32::BlendMode mask_blendmode);
extern void  (SPHERE_STDCALL * TransformBlitImage)(IMAGE image, int x[4], int y[4], CImage32::BlendMode blendmode);
extern void  (SPHERE_STDCALL * TransformBlitImageMask)(IMAGE image, int x[4], int y[4], CImage32::BlendMode blendmode, RGBA mask, CImage32::BlendMode mask_blendmode);
extern int   (SPHERE_STDCALL * GetImageWidth)(IMAGE image);
extern int   (SPHERE_STDCALL * GetImageHeight)(IMAGE image);
extern RGBA* (SPHERE_STDCALL * LockImage)(IMAGE image);
extern void  (SPHERE_STDCALL * UnlockImage)(IMAGE image, bool pixels_changed);
extern void  (SPHERE_STDCALL * DirectBlit)(int x, int y, int w, int h, RGBA* pixels);
extern void  (SPHERE_STDCALL * DirectTransformBlit)(int x[4], int y[4], int w, int h, RGBA* pixels);
extern void  (SPHERE_STDCALL * DirectGrab)(int x, int y, int w, int h, RGBA* pixels);
extern void  (SPHERE_STDCALL * DrawPoint)(int x, int y, RGBA color);
extern void  (SPHERE_STDCALL * DrawPointSeries)(VECTOR_INT** points, int length, RGBA color);
extern void  (SPHERE_STDCALL * DrawLine)(int x[2], int y[2], RGBA color);
extern void  (SPHERE_STDCALL * DrawGradientLine)(int x[2], int y[2], RGBA color[2]);
extern void  (SPHERE_STDCALL * DrawLineSeries)(VECTOR_INT** points, int length, RGBA color, int type);
extern void  (SPHERE_STDCALL * DrawBezierCurve)(int x[4], int y[4], double step, RGBA color, int cubic);
extern void  (SPHERE_STDCALL * DrawTriangle)(int x[3], int y[3], RGBA color);
extern void  (SPHERE_STDCALL * DrawGradientTriangle)(int x[3], int y[3], RGBA color[3]);
extern void  (SPHERE_STDCALL * DrawPolygon)(VECTOR_INT** points, int length, int invert, RGBA color);
extern void  (SPHERE_STDCALL * DrawOutlinedRectangle)(int x, int y, int w, int h, int size, RGBA color);
extern void  (SPHERE_STDCALL * DrawRectangle)(int x, int y, int w, int h, RGBA color);
extern void  (SPHERE_STDCALL * DrawGradientRectangle)(int x, int y, int w, int h, RGBA color[4]);
extern void  (SPHERE_STDCALL * DrawOutlinedComplex)(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, RGBA color, int antialias);
extern void  (SPHERE_STDCALL * DrawFilledComplex)(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, float angle, float frac_size, int fill_empty, RGBA colors[2]);
extern void  (SPHERE_STDCALL * DrawGradientComplex)(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, float angle, float frac_size, int fill_empty, RGBA colors[3]);
extern void  (SPHERE_STDCALL * DrawOutlinedEllipse)(int x, int y, int rx, int ry, RGBA color);
extern void  (SPHERE_STDCALL * DrawFilledEllipse)(int x, int y, int rx, int ry, RGBA color);
extern void  (SPHERE_STDCALL * DrawOutlinedCircle)(int x, int y, int r, RGBA color, int antialias);
extern void  (SPHERE_STDCALL * DrawFilledCircle)(int x, int y, int r, RGBA color, int antialias);
extern void  (SPHERE_STDCALL * DrawGradientCircle)(int x, int y, int r, RGBA color[2], int antialias);

extern bool SetWindowTitle(const char* text);


#endif /* UNIX_VIDEO_H */
