#ifndef UNIX_VIDEO_H
#define UNIX_VIDEO_H

#include "../../common/Image32.hpp"
#include "../../common/rgb.hpp"
#include "../../common/VectorStructs.hpp"
#include "unix_sphere_config.h"
#include "unix_filesystem.h"


#define STDCALL __attribute__((stdcall))


class SFONT; // for drawing FPS
typedef struct IMAGEimp* IMAGE;

void ToggleFPS();

extern void SetFPSFont(SFONT* font);
extern bool SwitchResolution(int x, int y);
extern int  GetScreenWidth();
extern int  GetScreenHeight();
extern void ToggleFullScreen();
extern void FlipScreen();

extern void  (STDCALL * SetClippingRectangle)(int x, int y, int w, int h);
extern void  (STDCALL * GetClippingRectangle)(int* x, int* y, int* w, int* h);
extern IMAGE (STDCALL * CreateImage)(int width, int height, const RGBA* pixels);
extern IMAGE (STDCALL * CloneImage)(IMAGE image);
extern IMAGE (STDCALL * GrabImage)(int x, int y, int width, int height);
extern void  (STDCALL * DestroyImage)(IMAGE image);
extern void  (STDCALL * BlitImage)(IMAGE image, int x, int y, CImage32::BlendMode blendmode);
extern void  (STDCALL * BlitImageMask)(IMAGE image, int x, int y, CImage32::BlendMode blendmode, RGBA mask, CImage32::BlendMode mask_blendmode);
extern void  (STDCALL * TransformBlitImage)(IMAGE image, int x[4], int y[4], CImage32::BlendMode blendmode);
extern void  (STDCALL * TransformBlitImageMask)(IMAGE image, int x[4], int y[4], CImage32::BlendMode blendmode, RGBA mask, CImage32::BlendMode mask_blendmode);
extern int   (STDCALL * GetImageWidth)(IMAGE image);
extern int   (STDCALL * GetImageHeight)(IMAGE image);
extern RGBA* (STDCALL * LockImage)(IMAGE image);
extern void  (STDCALL * UnlockImage)(IMAGE image, bool pixels_changed);
extern void  (STDCALL * DirectBlit)(int x, int y, int w, int h, RGBA* pixels);
extern void  (STDCALL * DirectTransformBlit)(int x[4], int y[4], int w, int h, RGBA* pixels);
extern void  (STDCALL * DirectGrab)(int x, int y, int w, int h, RGBA* pixels);
extern void  (STDCALL * DrawPoint)(int x, int y, RGBA color);
extern void  (STDCALL * DrawPointSeries)(VECTOR_INT** points, int length, RGBA color);
extern void  (STDCALL * DrawLine)(int x[2], int y[2], RGBA color);
extern void  (STDCALL * DrawGradientLine)(int x[2], int y[2], RGBA color[2]);
extern void  (STDCALL * DrawLineSeries)(VECTOR_INT** points, int length, RGBA color, int type);
extern void  (STDCALL * DrawBezierCurve)(int x[4], int y[4], double step, RGBA color, int cubic);
extern void  (STDCALL * DrawTriangle)(int x[3], int y[3], RGBA color);
extern void  (STDCALL * DrawGradientTriangle)(int x[3], int y[3], RGBA color[3]);
extern void  (STDCALL * DrawPolygon)(VECTOR_INT** points, int length, int invert, RGBA color);
extern void  (STDCALL * DrawOutlinedRectangle)(int x, int y, int w, int h, int size, RGBA color);
extern void  (STDCALL * DrawRectangle)(int x, int y, int w, int h, RGBA color);
extern void  (STDCALL * DrawGradientRectangle)(int x, int y, int w, int h, RGBA color[4]);
extern void  (STDCALL * DrawOutlinedComplex)(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, RGBA color, int antialias);
extern void  (STDCALL * DrawFilledComplex)(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, float angle, float frac_size, int fill_empty, RGBA colors[2]);
extern void  (STDCALL * DrawGradientComplex)(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, float angle, float frac_size, int fill_empty, RGBA colors[3]);
extern void  (STDCALL * DrawOutlinedEllipse)(int x, int y, int rx, int ry, RGBA color);
extern void  (STDCALL * DrawFilledEllipse)(int x, int y, int rx, int ry, RGBA color);
extern void  (STDCALL * DrawOutlinedCircle)(int x, int y, int r, RGBA color, int antialias);
extern void  (STDCALL * DrawFilledCircle)(int x, int y, int r, RGBA color, int antialias);
extern void  (STDCALL * DrawGradientCircle)(int x, int y, int r, RGBA color[2], int antialias);

extern bool SetWindowTitle(const char* text);


#endif /* UNIX_VIDEO_H */
