#ifndef IMAGE32_HPP
#define IMAGE32_HPP


#include <stdio.h>
#include "../common/rgb.hpp"
#include "DefaultFileSystem.hpp"


class CImage32
{
public:
  enum BlendMode {
    REPLACE,
    BLEND,
    RGB_ONLY,
    ALPHA_ONLY,
  };

public:
  CImage32();
  CImage32(int width, int height, const RGBA* pixels = NULL);
  CImage32(const CImage32& image);
  ~CImage32();

  CImage32& operator=(const CImage32& image);
  bool operator==(const CImage32& rhs) const;

  void Create(int width, int height);
  bool Load(const char* filename, IFileSystem& fs = g_DefaultFileSystem);
  bool Save(const char* filename, IFileSystem& fs = g_DefaultFileSystem) const;
  
  void      SetBlendMode(BlendMode mode);
  BlendMode GetBlendMode() const;

  void Clear();
  void Resize(int width, int height);
  void Rescale(int width, int height);

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
  
  void SetPixel(int x, int y, RGBA color);
  void SetAlpha(int alpha);
  void SetColorAlpha(RGB color, int alpha);
  void Line(int x1, int y1, int x2, int y2, RGBA color);
  void Circle(int x, int y, int r, RGBA color);
  void Rectangle(int x1, int y1, int x2, int y2, RGBA color);

  void BlitImage(CImage32& image, int x, int y);

private:
  // PNG
  bool Import_PNG(const char* filename, IFileSystem& fs);

  // JPEG
  bool Import_JPEG(const char* filename, IFileSystem& fs);
  
  // BMP
  bool Import_BMP(const char* filename, IFileSystem& fs);
  bool BMP_ReadRGB(IFile* file, int imagesize, int bpp, BGRA* Pal);
  bool BMP_ReadRLE8(IFile* file, BGRA* Pal);
  bool BMP_ReadRLE4(IFile* file, int bpp, int NumPal, RGBA* Pal);

  // PCX
  bool Import_PCX(const char* filename, IFileSystem& fs);
  void PCX_ReadScanline(IFile* file, int scansize, byte* scanline);

  bool Export_PNG(const char* filename, IFileSystem& fs) const;

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
{
}


inline
CImage32::~CImage32()
{
  delete[] m_Pixels;
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
