#ifndef IMAGE32_HPP
#define IMAGE32_HPP

#include <stdio.h>
#include "../common/rgb.hpp"
#include "../common/VectorStructs.hpp"
#include "DefaultFileSystem.hpp"
#include "ColorMatrix.hpp"

struct clipper
{
    int left;
    int top;
    int right;
    int bottom;
};

class CImage32
{
public:
    enum BlendMode {
        BLEND = 0,
        REPLACE,
        RGB_ONLY,
        ALPHA_ONLY,
        ADD,
        SUBTRACT,
        MULTIPLY,
        AVERAGE,
        INVERT,
		NUM_BLENDS
    };

public:
    CImage32();
    CImage32(int width, int height, const RGBA* pixels = NULL);
    CImage32(int width, int height, RGBA pixel);
    CImage32(const CImage32& image);
    ~CImage32();

    CImage32& operator=(const CImage32& image);
    bool operator==(const CImage32& rhs) const;

    bool Create(int width, int height);
    bool Load(const char* filename, IFileSystem& fs = g_DefaultFileSystem);
    bool Save(const char* filename, IFileSystem& fs = g_DefaultFileSystem) const;

    void      SetBlendMode(BlendMode mode);
    BlendMode GetBlendMode() const;

    void ApplyColorFX(int x1, int y1, int w, int h, const CColorMatrix &c);
    void ApplyColorFX4(int x1, int y1, int w, int h, const CColorMatrix &c1, const CColorMatrix &c2, const CColorMatrix &c3, const CColorMatrix &c4);

    void ApplyLookup(int x, int y, int w, int h, unsigned char rlut[256], unsigned char glut[256], unsigned char blut[256], unsigned char alut[256]);
    void Clear();
    void Resize(int width, int height);
    void Rescale(int width, int height);
    void Resample(int width, int height, bool weighted = false);

    void AdjustBorders(int top, int right, int bottom, int left);
    int         GetWidth() const;
    int         GetHeight() const;
    RGBA        GetPixel(int x, int y) const;
    RGBA*       GetPixels();
    const RGBA* GetPixels() const;

    void FlipHorizontal();
    void FlipVertical();
    void Translate(int dx, int dy);
    void Rotate(double radians, bool autoSize);
    void RotateCW();
    void RotateCCW();

    void SetPixel(int x, int y, RGBA color, clipper clip);
    void SetPixel(int x, int y, RGBA color);
    void SetAlpha(int alpha, bool all);
    void SetColorAlpha(RGB color, int alpha);
    void SetColorAlpha(int x, int y, int w, int h, RGB color, int alpha);
    void ReplaceColor(RGBA oldColor, RGBA newColor);
	bool FindColor(RGBA aColor, int &x, int &y);
	void FloodFill(int x, int y, RGBA color);
	void SeedFill_4(int x, int y, RGBA new_color, clipper clip);

    void PointSeries(VECTOR_INT** points, int length, RGBA color);
    void Line(int x1, int y1, int x2, int y2, RGBA color);
    void Line(int x1, int y1, int x2, int y2, RGBA color, clipper clip);
    void LineSeries(VECTOR_INT** points, int length, RGBA color, int type);
    void BezierCurve(int x[4], int y[4], double step, RGBA color, int cubic);
    void OutlinedRectangle(int x, int y, int w, int h, int size, RGBA color);
    void OutlinedRectangle(int x, int y, int w, int h, int size, RGBA color, clipper clip);
    void Rectangle(int x1, int y1, int x2, int y2, RGBA color);
    void Rectangle(int x1, int y1, int x2, int y2, RGBA color, clipper clip);
    void Triangle(int x1, int y1, int x2, int y2, int x3, int y3, RGBA color);
    void Polygon(VECTOR_INT** points, int length, int invert, RGBA color);
    void OutlinedEllipse(int x, int y, int rx, int ry, RGBA color);
    void OutlinedEllipse(int x, int y, int rx, int ry, RGBA color, clipper clip);
    void FilledEllipse(  int x, int y, int rx, int ry, RGBA color);
    void FilledEllipse(  int x, int y, int rx, int ry, RGBA color, clipper clip);
    void OutlinedCircle(int x, int y, int radius, RGBA color, int antialias);
    void OutlinedCircle(int x, int y, int radius, RGBA color, int antialias, clipper clip);
    void FilledCircle(  int x, int y, int radius, RGBA color, int antialias);
    void FilledCircle(  int x, int y, int radius, RGBA color, int antialias, clipper clip);
    void GradientCircle(int x, int y, int radius, RGBA c[2],  int antialias);

    void GradientLine(int x1, int y1, int x2, int y2, RGBA c[2]);
    void GradientRectangle(int x, int y, int w, int h, RGBA c[4]);
    void GradientTriangle(int x1, int y1, int x2, int y2, int x3, int y3, RGBA c[3]);

    void BlitImage(CImage32& image, int x, int y);
    void BlitImageMask(CImage32& image, int x, int y, RGBA mask, int mask_bmode = 6);
    void TransformBlitImage(CImage32& image, int x[4], int y[4]);
    void TransformBlitImageMask(CImage32& image, int x[4], int y[4], RGBA mask, int mask_bmode = 6);

private:
    int   m_Width;
    int   m_Height;
    RGBA* m_Pixels;

    BlendMode m_BlendMode;
};

inline
CImage32::CImage32()
        : m_Width(0)
        , m_Height(0)
        , m_Pixels(NULL)
        , m_BlendMode(BLEND)
{}

inline
CImage32::~CImage32()
{
    delete[] m_Pixels;
    m_Width = 0;
    m_Height = 0;
}

inline int
CImage32::GetWidth() const
{
    return m_Width;
}

inline int
CImage32::GetHeight() const
{
    return m_Height;
}

inline RGBA
CImage32::GetPixel(int x, int y) const
{
    return m_Pixels[y * m_Width + x];
}

inline RGBA*
CImage32::GetPixels()
{
    return m_Pixels;
}

inline const RGBA*
CImage32::GetPixels() const
{
    return m_Pixels;
}

#endif
