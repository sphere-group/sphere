#ifndef WIN32_VIDEO_H
#define WIN32_VIDEO_H

#include "../../common/Image32.hpp"
#include "../../common/rgb.hpp"
#include "../../common/VectorStructs.hpp"

class SFONT; // for drawing FPS
typedef struct IMAGEimp* IMAGE;

extern void SetFPSFont(SFONT* font);
extern bool SwitchResolution(int x, int y);
extern int  GetScreenWidth();
extern int  GetScreenHeight();
extern bool ToggleFullScreen();
extern void FlipScreen();

extern void  (__stdcall * SetClippingRectangle)(int x, int y, int w, int h);
extern void  (__stdcall * GetClippingRectangle)(int* x, int* y, int* w, int* h);
extern IMAGE (__stdcall * CreateImage)(int width, int height, const RGBA* pixels);
extern IMAGE (__stdcall * CloneImage)(IMAGE image);
extern IMAGE (__stdcall * GrabImage)(int x, int y, int width, int height);
extern void  (__stdcall * DestroyImage)(IMAGE image);
extern void  (__stdcall * BlitImage)(IMAGE image, int x, int y, CImage32::BlendMode blendmode);
extern void  (__stdcall * BlitImageMask)(IMAGE image, int x, int y, CImage32::BlendMode blendmode, RGBA mask, CImage32::BlendMode mask_blendmode);
extern void  (__stdcall * TransformBlitImage)(IMAGE image, int x[4], int y[4], CImage32::BlendMode blendmode);
extern void  (__stdcall * TransformBlitImageMask)(IMAGE image, int x[4], int y[4], CImage32::BlendMode blendmode, RGBA mask, CImage32::BlendMode mask_blendmode);
extern int   (__stdcall * GetImageWidth)(IMAGE image);
extern int   (__stdcall * GetImageHeight)(IMAGE image);
extern RGBA* (__stdcall * LockImage)(IMAGE image);
extern void  (__stdcall * UnlockImage)(IMAGE image, bool pixels_changed);
extern void  (__stdcall * DirectBlit)(int x, int y, int w, int h, RGBA* pixels);
extern void  (__stdcall * DirectTransformBlit)(int x[4], int y[4], int w, int h, RGBA* pixels);
extern void  (__stdcall * DirectGrab)(int x, int y, int w, int h, RGBA* pixels);
extern void  (__stdcall * DrawPoint)(int x, int y, RGBA color);
extern void  (__stdcall * DrawPointSeries)(VECTOR_INT** points, int length, RGBA color);
extern void  (__stdcall * DrawLine)(int x[2], int y[2], RGBA color);
extern void  (__stdcall * DrawGradientLine)(int x[2], int y[2], RGBA color[2]);
extern void  (__stdcall * DrawLineSeries)(VECTOR_INT** points, int length, RGBA color, int type);
extern void  (__stdcall * DrawBezierCurve)(int x[4], int y[4], double step, RGBA color, int cubic);
extern void  (__stdcall * DrawTriangle)(int x[3], int y[3], RGBA color);
extern void  (__stdcall * DrawGradientTriangle)(int x[3], int y[3], RGBA color[3]);
extern void  (__stdcall * DrawPolygon)(VECTOR_INT** points, int length, int invert, RGBA color);
extern void  (__stdcall * DrawOutlinedRectangle)(int x, int y, int w, int h, int size, RGBA color);
extern void  (__stdcall * DrawRectangle)(int x, int y, int w, int h, RGBA color);
extern void  (__stdcall * DrawGradientRectangle)(int x, int y, int w, int h, RGBA color[4]);
extern void  (__stdcall * DrawOutlinedComplex)(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, RGBA color, int antialias);
extern void  (__stdcall * DrawFilledComplex)(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, float angle, float frac_size, int fill_empty, RGBA colors[2]);
extern void  (__stdcall * DrawGradientComplex)(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, float angle, float frac_size, int fill_empty, RGBA colors[3]);
extern void  (__stdcall * DrawOutlinedEllipse)(int x, int y, int rx, int ry, RGBA color);
extern void  (__stdcall * DrawFilledEllipse)(int x, int y, int rx, int ry, RGBA color);
extern void  (__stdcall * DrawOutlinedCircle)(int x, int y, int r, RGBA color, int antialias);
extern void  (__stdcall * DrawFilledCircle)(int x, int y, int r, RGBA color, int antialias);
extern void  (__stdcall * DrawGradientCircle)(int x, int y, int r, RGBA color[2], int antialias);

extern bool SetWindowTitle(const char* text);


#endif
