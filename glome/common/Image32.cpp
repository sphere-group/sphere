#include <algorithm>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <png.h>
#if defined(_WIN32)
#include <jpegwrap.h>
#elif defined(unix)
#include "../jpegwrap/jpegwrap.h"
#endif
#include "Image32.hpp"
#include "Filters.hpp"
#include "packed.hpp"
#include "primitives.hpp"
#include "strcmp_ci.hpp"


// INLINE HELPER FUNCTIONS

////////////////////////////////////////////////////////////////////////////////

inline double RotateX(double x, double y, double radians)
{
  return ((x * cos(radians)) - (y * sin(radians)));
}

////////////////////////////////////////////////////////////////////////////////

inline double RotateY(double x, double y, double radians)
{
  return ((x * sin(radians)) + (y * cos(radians)));
}

////////////////////////////////////////////////////////////////////////////////

inline int sgn(int i)
{
  return (i < 0 ? -1 : (i > 0 ? 1 : 0));
}
           
////////////////////////////////////////////////////////////////////////////////


// needed for PNG read and write callbacks
#ifdef _MSC_VER
#define PNG_CDECL __cdecl
#else
#define PNG_CDECL
#endif                      


// VC++ fux0rs with std::min and std::max
template<typename T>
inline T std_min(T a, T b) {
  return (a < b ? a : b);
}

template<typename T>
inline T std_max(T a, T b) {
  return (a < b ? b : a);
}



// CImage32 Method Definitions


////////////////////////////////////////////////////////////////////////////////

CImage32::CImage32(int width, int height, const RGBA* pixels)
: m_Width(width)
, m_Height(height)
, m_Pixels(new RGBA[width * height])
, m_BlendMode(BLEND)
{
  if (pixels == NULL) {
    memset(m_Pixels, 0, width * height * sizeof(RGBA));
  } else {
    memcpy(m_Pixels, pixels, width * height * sizeof(RGBA));
  }
}

////////////////////////////////////////////////////////////////////////////////

CImage32::CImage32(const CImage32& image)
{
  m_Width  = image.m_Width;
  m_Height = image.m_Height;
  m_Pixels = new RGBA[m_Width * m_Height];
  memcpy(m_Pixels, image.m_Pixels, m_Width * m_Height * sizeof(RGBA));
  m_BlendMode = image.m_BlendMode;
}

////////////////////////////////////////////////////////////////////////////////

CImage32&
CImage32::operator=(const CImage32& image)
{
  if (this != &image) {
    // don't reallocate pixels if number of bytes is the same
    if (m_Width * m_Height != image.m_Width * image.m_Height) {
      delete[] m_Pixels;
      m_Pixels = new RGBA[image.m_Width * image.m_Height];
    }

    // copy members over
    m_Width  = image.m_Width;
    m_Height = image.m_Height;
    memcpy(m_Pixels, image.m_Pixels, m_Width * m_Height * sizeof(RGBA));
    m_BlendMode = image.m_BlendMode;
  }

  return *this;
}

////////////////////////////////////////////////////////////////////////////////

bool
CImage32::operator==(const CImage32& rhs) const
{
  return (
    m_Width == rhs.m_Width &&
    m_Height == rhs.m_Height &&
    memcmp(m_Pixels, rhs.m_Pixels, m_Width * m_Height * sizeof(RGBA)) == 0
  );
}

////////////////////////////////////////////////////////////////////////////////

void
CImage32::Create(int width, int height)
{
  delete[] m_Pixels;

  m_Width  = width;
  m_Height = height;
  m_Pixels = new RGBA[width * height];
  for (int i = 0; i < width * height; i++)
    m_Pixels[i] = CreateRGBA(0, 0, 0, 255);
}

////////////////////////////////////////////////////////////////////////////////

void
CImage32::Clear()
{
  for (int i = 0; i < m_Width * m_Height; i++)
    m_Pixels[i] = CreateRGBA(0, 0, 0, 255);
}

////////////////////////////////////////////////////////////////////////////////

bool
CImage32::Load(const char* filename, IFileSystem& fs)
{
  int old_width = m_Width;
  int old_height = m_Height;
  RGBA* old_pixels = m_Pixels;
  m_Width  = 0;
  m_Height = 0;
  m_Pixels = NULL;

  bool result;
  if (strcmp_ci(filename + strlen(filename) - 4, ".png") == 0)
    result = Import_PNG(filename, fs);
  else if (strcmp_ci(filename + strlen(filename) - 5, ".jpeg") == 0 ||
           strcmp_ci(filename + strlen(filename) - 4, ".jpg") == 0 ||
           strcmp_ci(filename + strlen(filename) - 4, ".jpe") == 0)
    result = Import_JPEG(filename, fs);
  else if (strcmp_ci(filename + strlen(filename) - 4, ".pcx") == 0)
    result = Import_PCX(filename, fs);
  else if (strcmp_ci(filename + strlen(filename) - 4, ".bmp") == 0)
    result = Import_BMP(filename, fs);
  else
    result = false;

  if (result == false)
  {
    m_Width = old_width;
    m_Height = old_height;
    m_Pixels = old_pixels;
  }
  else
    delete[] old_pixels;

  return result;
}

////////////////////////////////////////////////////////////////////////////////

bool
CImage32::Save(const char* filename, IFileSystem& fs) const
{
  if (strcmp_ci(filename + strlen(filename) - 4, ".png") == 0)
    return Export_PNG(filename, fs);
  else
    return false;
}

////////////////////////////////////////////////////////////////////////////////

void
CImage32::SetBlendMode(BlendMode mode)
{
  m_BlendMode = mode;
}

////////////////////////////////////////////////////////////////////////////////

CImage32::BlendMode
CImage32::GetBlendMode() const
{
  return m_BlendMode;
}

////////////////////////////////////////////////////////////////////////////////

void
CImage32::Resize(int width, int height)
{
  RGBA* new_pixels = new RGBA[width * height];
  for (int ix = 0; ix < width; ix++)
    for (int iy = 0; iy < height; iy++)
    {
      if (ix < m_Width && iy < m_Height)
        new_pixels[iy * width + ix] = m_Pixels[iy * m_Width + ix];
      else
        new_pixels[iy * width + ix] = CreateRGBA(0, 0, 0, 255);
    }

  m_Width  = width;
  m_Height = height;
  delete[] m_Pixels;
  m_Pixels = new_pixels;
}

////////////////////////////////////////////////////////////////////////////////

void
CImage32::Rescale(int width, int height)
{
  RGBA* NewPixels = new RGBA[width * height];
  if (NewPixels == NULL)
    return;

  double HorzAspectRatio, VertAspectRatio;
  int x,y;
  double ix, iy;
  
  HorzAspectRatio = (double)width / (double)m_Width;   // (dstWidth / srcWidth) * 100
  VertAspectRatio = (double)height / (double)m_Height; // (dstHeight / srcHeight) * 100

  // floating point, should be faster than my crappy fixed-point...
  for (y=0; y<height; y++)
    for (x=0; x<width; x++)
    {
      ix = x / HorzAspectRatio;
      iy = y / VertAspectRatio;

      if ((ix >= 0) && (ix < m_Width))
        if ((iy >= 0) && (iy < m_Height))
          NewPixels[(y * width) + x] = m_Pixels[((int)iy * m_Width) + (int)ix];
    }

  m_Width  = width;
  m_Height = height;
  delete[] m_Pixels;
  m_Pixels = NewPixels;
}

////////////////////////////////////////////////////////////////////////////////

void
CImage32::FlipHorizontal()
{
  FlipHorizontally(m_Width, m_Height, m_Pixels);
}

////////////////////////////////////////////////////////////////////////////////

void
CImage32::FlipVertical()
{
  FlipVertically(m_Width, m_Height, m_Pixels);
}

////////////////////////////////////////////////////////////////////////////////

void
CImage32::Translate(int dx, int dy)
{
  ::Translate(m_Width, m_Height, m_Pixels, dx, dy);
}

////////////////////////////////////////////////////////////////////////////////

void
CImage32::Rotate(double radians, bool autoSize)
{
  double ix, iy;
  double tx, ty;
  int width, height;
  double xOff, yOff;

  if (!autoSize)
  {
    width = m_Width;
    height = m_Height;
    xOff = 0;
    yOff = 0;
  }
  else
  {
    double xA, xB, xC, xD, yA, yB, yC, yD;
    double xNOff, yNOff;

    // probably the slowest bit in the entire routine...
    // finds the largest and smallest point and use that. 
    // TopLeft, BotRight, TopRight, BotLeft
    xOff = xNOff = 0;
    yOff = yNOff = 0;
    xA = RotateX(0, 0, radians);
    yA = RotateY(0, 0, radians);
    xB = RotateX(m_Width, m_Height, radians);
    yB = RotateY(m_Width, m_Height, radians);
    xC = RotateX(m_Width, 0, radians);
    yC = RotateY(m_Width, 0, radians);
    xD = RotateX(0, m_Height, radians);
    yD = RotateY(0, m_Height, radians);
    xOff = std_max(xOff, xA); xNOff = std_min(xNOff, xA);
    xOff = std_max(xOff, xB); xNOff = std_min(xNOff, xB);
    xOff = std_max(xOff, xC); xNOff = std_min(xNOff, xC);
    xOff = std_max(xOff, xD); xNOff = std_min(xNOff, xD);
    yOff = std_max(yOff, yA); yNOff = std_min(yNOff, yA);
    yOff = std_max(yOff, yB); yNOff = std_min(yNOff, yB);
    yOff = std_max(yOff, yC); yNOff = std_min(yNOff, yC);
    yOff = std_max(yOff, yD); yNOff = std_min(yNOff, yD);
    
    xOff = (xOff - xNOff) - m_Width;
    yOff = (yOff - yNOff) - m_Height;

    width  = m_Width + (int)xOff;
    height = m_Height + (int)yOff;
    xOff /= 2;
    yOff /= 2;
  }

  RGBA* NewPixels = new RGBA[width * height];
  if (NewPixels == NULL) return;
  memset(NewPixels, 0, width * height * sizeof(RGBA));

  double sine = sin(radians);
  double cosi = cos(radians);

  for (int y=0; y<height; y++)
    for (int x=0; x<width; x++)
    {
      // realigns the rotating axis to 0,0 (of a graphical point of view)
      tx = x - (m_Width/2) - xOff;
      ty = y - (m_Height/2) - yOff;
      
      ix = (cosi * tx) - (sine * ty);
      iy = (sine * tx) + (cosi * ty);

      ix += m_Width/2; iy += m_Height/2;

      // autoclip. Note: the iy & ix info is supposed to be used on m_Pixels!
      if ((ix>=0) && (ix<m_Width))
        if ((iy>=0) && (iy<m_Height))
          NewPixels[(y * width) + x] = m_Pixels[((int)iy * m_Width) + (int)ix];
    }


  // finish up
  if (autoSize)
  {
    m_Width = width;
    m_Height = height;
  }

  delete[] m_Pixels;
  m_Pixels = NewPixels;
}

////////////////////////////////////////////////////////////////////////////////

void
CImage32::RotateCW()
{
  // only works on square images
  if (m_Width != m_Height)
    return;

  ::RotateCW(m_Width, m_Pixels);
}

////////////////////////////////////////////////////////////////////////////////

void
CImage32::RotateCCW()
{
  // only works on square images
  if (m_Width != m_Height)
    return;

  ::RotateCCW(m_Width, m_Pixels);
}

////////////////////////////////////////////////////////////////////////////////

inline void copyRGBA(RGBA& dest, RGBA src)
{
  dest = src;
}

inline void blendRGBA(RGBA& dest, RGBA src)
{
  Blend3(dest, src, src.alpha);
  dest.alpha = 255;
}

inline void copyRGB(RGBA& dest, RGBA src)
{
  dest.red   = src.red;
  dest.green = src.green;
  dest.blue  = src.blue;
}

inline void copyAlpha(RGBA& dest, RGBA src)
{
  dest.alpha = src.alpha;
}

////////////////////////////////////////////////////////////////////////////////

class constant_color
{
public:
  constant_color(RGBA color) : m_color(color) { }
  RGBA operator()(int i, int range) { return m_color; }
private:
  RGBA m_color;
};

struct clipper {
  int left;
  int top;
  int right;
  int bottom;
};

void
CImage32::SetPixel(int x, int y, RGBA color)
{
  clipper clip = { 0, 0, m_Width - 1, m_Height - 1 };
  switch (m_BlendMode) {
    case REPLACE:    primitives::Point(m_Pixels, m_Width, x, y, color, clip, copyRGBA);  break;
    case BLEND:      primitives::Point(m_Pixels, m_Width, x, y, color, clip, blendRGBA); break;
    case RGB_ONLY:   primitives::Point(m_Pixels, m_Width, x, y, color, clip, copyRGB);   break;
    case ALPHA_ONLY: primitives::Point(m_Pixels, m_Width, x, y, color, clip, copyAlpha); break;
  }
}

////////////////////////////////////////////////////////////////////////////////

void
CImage32::SetAlpha(int alpha)
{
  for (int i = 0; i < m_Width * m_Height; i++) {
    m_Pixels[i].alpha = alpha;
  }
}

////////////////////////////////////////////////////////////////////////////////

void
CImage32::SetColorAlpha(RGB color, int alpha)
{
  for (int i = 0; i < m_Width * m_Height; i++) {
    RGBA& p = m_Pixels[i];
    if (p.red == color.red &&
        p.green == color.green &&
        p.blue == color.blue) {
      p.alpha = alpha;
    }
  }
}


////////////////////////////////////////////////////////////////////////////////

void
CImage32::Line(int x1, int y1, int x2, int y2, RGBA color)
{
  clipper clip = { 0, 0, m_Width - 1, m_Height - 1 };
  switch (m_BlendMode) {
    case REPLACE:    primitives::Line(m_Pixels, m_Width, x1, y1, x2, y2, constant_color(color), clip, copyRGBA);  break;
    case BLEND:      primitives::Line(m_Pixels, m_Width, x1, y1, x2, y2, constant_color(color), clip, blendRGBA); break;
    case RGB_ONLY:   primitives::Line(m_Pixels, m_Width, x1, y1, x2, y2, constant_color(color), clip, copyRGB);   break;
    case ALPHA_ONLY: primitives::Line(m_Pixels, m_Width, x1, y1, x2, y2, constant_color(color), clip, copyAlpha); break;
  }
}

////////////////////////////////////////////////////////////////////////////////

void
CImage32::Circle(int x, int y, int r, RGBA color)
{
  // use C segments to draw the circle
  const int C = 20;

  const double pi_2 = acos(-1) * 2;
  for (int i = 0; i < C; i++) {
    int j = (i + 1) % C;
    int x1 = x + r * sin(pi_2 * i / C);
    int y1 = y + r * cos(pi_2 * i / C);
    int x2 = x + r * sin(pi_2 * j / C);
    int y2 = y + r * cos(pi_2 * j / C);
    Line(x1, y1, x2, y2, color);
  }
}

////////////////////////////////////////////////////////////////////////////////

void
CImage32::Rectangle(int x1, int y1, int x2, int y2, RGBA color)
{
  // make sure x1 < x2 and y1 < y2 so we can get good w and h values
  if (x1 > x2) {
    std::swap(x1, x2);
  }
  if (y1 > y2) {
    std::swap(y1, y2);
  }
  int w = x2 - x1 + 1;
  int h = y2 - y1 + 1;

  clipper clip = { 0, 0, m_Width - 1, m_Height - 1 };
  switch (m_BlendMode) {
    case REPLACE:    primitives::Rectangle(m_Pixels, m_Width, x1, y1, w, h, color, clip, copyRGBA);  break;
    case BLEND:      primitives::Rectangle(m_Pixels, m_Width, x1, y1, w, h, color, clip, blendRGBA); break;
    case RGB_ONLY:   primitives::Rectangle(m_Pixels, m_Width, x1, y1, w, h, color, clip, copyRGB);   break;
    case ALPHA_ONLY: primitives::Rectangle(m_Pixels, m_Width, x1, y1, w, h, color, clip, copyAlpha); break;
  }
}

////////////////////////////////////////////////////////////////////////////////

inline void renderer(RGBA& dest, RGBA src, RGBA alpha)
{
  Blend3(dest, src, alpha.alpha);
}

void
CImage32::BlitImage(CImage32& image, int x, int y)
{
  clipper clip = {
    0, 0, m_Width - 1, m_Height - 1
  };

  primitives::Blit(
    m_Pixels,
    m_Width,
    x,
    y,
    image.GetPixels(),
    image.GetPixels(),
    image.GetWidth(),
    image.GetHeight(),
    clip,
    renderer
  );
}

////////////////////////////////////////////////////////////////////////////////

static void PNG_CDECL PNG_read_function(png_structp png_ptr, png_bytep data, png_size_t length)
{
  IFile* file = (IFile*)png_get_io_ptr(png_ptr);
  file->Read(data, length);
}

////////////////////////////////////////////////////////////////////////////////

static void PNG_CDECL PNG_write_function(png_structp png_ptr, png_bytep data, png_size_t length)
{
  IFile* file = (IFile*)png_get_io_ptr(png_ptr);
  file->Write(data, length);
}

////////////////////////////////////////////////////////////////////////////////

static void PNG_CDECL PNG_flush_function(png_structp png_ptr)
{
  // assume that IFiles automatically flush
}

////////////////////////////////////////////////////////////////////////////////

bool
CImage32::Import_PNG(const char* filename, IFileSystem& fs)
{
  // open file
  IFile* file = fs.Open(filename, IFileSystem::read);
  if (file == NULL)
    return false;

  // verify signature
  byte sig[8];
  file->Read(sig, 8);
  if (png_sig_cmp(sig, 0, 8))
  {
    file->Close();
    return false;
  }

  // read struct
  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr)
  {
    file->Close();
    return false;
  }

  // info struct
  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    file->Close();
    return false;
  }

  // read the image
  png_set_read_fn(png_ptr, file, PNG_read_function);

  png_set_sig_bytes(png_ptr, 8);
  int png_transform = PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_PACKSWAP;
  png_read_png(png_ptr, info_ptr, png_transform, NULL);

  if (png_get_rows(png_ptr, info_ptr) == NULL)
  {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    file->Close();
    return false;
  }

  // initialize CImage32 members
  m_Width = png_get_image_width(png_ptr, info_ptr);
  m_Height = png_get_image_height(png_ptr, info_ptr);
  m_Pixels = new RGBA[m_Width * m_Height];

  // decode based on pixel depth
  int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
  int num_channels = png_get_channels(png_ptr, info_ptr);
  void** row_pointers = (void**)png_get_rows(png_ptr, info_ptr);

  if (bit_depth == 8 && num_channels == 4)
  {
    for (int i = 0; i < m_Height; i++)
    {
      RGBA* row = (RGBA*)(row_pointers[i]);
      for (int j = 0; j < m_Width; j++)
        m_Pixels[i * m_Width + j] = row[j];
    }
  }
  else if (bit_depth == 8 && num_channels == 3)
  {
    for (int i = 0; i < m_Height; i++)
    {
      RGB* row = (RGB*)(row_pointers[i]);
      for (int j = 0; j < m_Width; j++)
      {
        RGBA p = { row[j].red, row[j].green, row[j].blue, 255 };
        m_Pixels[i * m_Width + j] = p;
      }
    }
  }
  else if (bit_depth == 8 && num_channels == 2)
  {
    png_colorp palette;
    int num_palette = 0;
    png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);

    // if there is no palette, use black and white
    if (num_palette == 0)
    {
      for (int i = 0; i < m_Height; i++)
      {
        byte* row = (byte*)(row_pointers[i]);
        for (int j = 0; j < m_Width; j++)
        {
          m_Pixels[i * m_Width + j].red   = row[j * 2];
          m_Pixels[i * m_Width + j].green = row[j * 2];
          m_Pixels[i * m_Width + j].blue  = row[j * 2];
          m_Pixels[i * m_Width + j].alpha = row[j * 2 + 1];
        }
      }
    }
    else // otherwise use the palette
    {
      for (int i = 0; i < m_Height; i++)
      {
        byte* row = (byte*)(row_pointers[i]);
        for (int j = 0; j < m_Width; j++)
        {
          m_Pixels[i * m_Width + j].red   = palette[row[j * 2]].red;
          m_Pixels[i * m_Width + j].green = palette[row[j * 2]].green;
          m_Pixels[i * m_Width + j].blue  = palette[row[j * 2]].blue;
          m_Pixels[i * m_Width + j].alpha = row[j * 2 + 1];
        }
      }
    }
  }
  else if (bit_depth == 8 && num_channels == 1)
  {
    png_colorp palette;
    int num_palette = 0;
    png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);

    png_bytep trans;
    int num_trans = 0;
    png_color_16p trans_values;
    png_get_tRNS(png_ptr, info_ptr, &trans, &num_trans, &trans_values);

    // if there is no palette, use black and white
    if (num_palette == 0)
    {
      for (int i = 0; i < m_Height; i++)
      {
        byte* row = (byte*)(row_pointers[i]);
        for (int j = 0; j < m_Width; j++)
        {
          byte alpha = 255;
          for (int k = 0; k < num_trans; k++) {
            if (trans[k] == row[j]) {
              alpha = 0;
            }
          }
          m_Pixels[i * m_Width + j].red   = row[j];
          m_Pixels[i * m_Width + j].green = row[j];
          m_Pixels[i * m_Width + j].blue  = row[j];
          m_Pixels[i * m_Width + j].alpha = alpha;
        }
      }
    }
    else // otherwise use the palette
    {
      for (int i = 0; i < m_Height; i++)
      {
        byte* row = (byte*)(row_pointers[i]);
        for (int j = 0; j < m_Width; j++)
        {
          byte alpha = 255;
          for (int k = 0; k < num_trans; k++) {
            if (trans[k] == row[j]) {
              alpha = 0;
            }
          }
          m_Pixels[i * m_Width + j].red   = palette[row[j]].red;
          m_Pixels[i * m_Width + j].green = palette[row[j]].green;
          m_Pixels[i * m_Width + j].blue  = palette[row[j]].blue;
          m_Pixels[i * m_Width + j].alpha = alpha;
        }
      }
    }
  }
  else if (bit_depth == 4 && num_channels == 1)
  {
    png_colorp palette;
    int num_palette;
    png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);

    for (int i = 0; i < m_Height; i++)
    {
      RGBA* dst = m_Pixels + i * m_Width;

      byte* row = (byte*)(row_pointers[i]);
      for (int j = 0; j < m_Width / 2; j++)
      {
        byte p1 = *row >> 4;
        byte p2 = *row & 0xF;

        dst->red   = palette[p1].red;
        dst->green = palette[p1].green;
        dst->blue  = palette[p1].blue;
        dst->alpha = 255;
        dst++;

        dst->red   = palette[p2].red;
        dst->green = palette[p2].green;
        dst->blue  = palette[p2].blue;
        dst->alpha = 255;
        dst++;

        row++;
      }

      if (m_Width % 2)
      {
        byte p = *row >> 4;
        dst->red   = palette[p].red;
        dst->green = palette[p].green;
        dst->blue  = palette[p].blue;
        dst->alpha = 255;
        dst++;
      }
    }
  }
  else if (bit_depth == 1 && num_channels == 1)
  {
    png_colorp palette;
    int num_palette;
    png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);

    for (int i = 0; i < m_Height; i++)
    {
      RGBA* dst = m_Pixels + i * m_Width;

      int mask = 1;
      byte* p = (byte*)(row_pointers[i]);

      for (int j = 0; j < m_Width; j++)
      {
        dst->red   = palette[(*p & mask) > 0].red;
        dst->green = palette[(*p & mask) > 0].green;
        dst->blue  = palette[(*p & mask) > 0].blue;
        dst->alpha = 255;
        dst++;

        mask <<= 1;
        if (mask == 256)
        {
          p++;
          mask = 1;
        }
      }

    }
  }
  else
  {
    delete[] m_Pixels;
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    file->Close();
    return false;
  }

  // we're done
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  file->Close();
  return true;
}

////////////////////////////////////////////////////////////////////////////////

static void* JPEG_CALL jpeg_open(JPEG_CONTEXT context, const char* filename)
{
  IFileSystem* fs = (IFileSystem*)JPEG_GetContextPrivate(context);
  return fs->Open(filename, IFileSystem::read);
}

static void JPEG_CALL jpeg_close(void* file)
{
  IFile* f = (IFile*)file;
  f->Close();
}

static int JPEG_CALL jpeg_read(void* file, int bytes, void* buffer)
{
  IFile* f = (IFile*)file;
  return f->Read(buffer, bytes);
}

bool
CImage32::Import_JPEG(const char* filename, IFileSystem& fs)
{
  // create context
  JPEG_CONTEXT context = JPEG_CreateContext();
  if (context == NULL) {
    return false;
  }

  // assign file callbacks
  JPEG_SetContextPrivate(context, &fs);
  JPEG_SetFileCallbacks(context, jpeg_open, jpeg_close, jpeg_read);

  // load JPEG file
  JPEG_IMAGE image = JPEG_LoadImage(context, filename);
  if (image == NULL) {
    JPEG_DestroyContext(context);
    return false;
  }

  // read image pixels
  m_Width = JPEG_GetImageWidth(image);
  m_Height = JPEG_GetImageHeight(image);
  m_Pixels = new RGBA[m_Width * m_Height];
  memcpy(m_Pixels, JPEG_GetImagePixels(image), m_Width * m_Height * sizeof(RGBA));

  // clean up and go home
  JPEG_DestroyImage(image);
  JPEG_DestroyContext(context);
  return true;
}

////////////////////////////////////////////////////////////////////////////////

#define STRUCT_NAME BMP_HEADER
#include "begin_packed_struct.h"
  word  bfType;
  dword bfSize;
  word  bfReserved1;
  word  bfReserved2;
  dword bfOffBits;

  dword biSize;
  long  biWidth;
  long  biHeight;
  word  biNumPlanes;
  word  biBitsPerPixel;
  dword biCompression;
#include "end_packed_struct.h"

ASSERT_STRUCT_SIZE(BMP_HEADER, 34);

#define BMP_RGB    0
#define BMP_RLE8   1
#define BMP_RLE4   2

////////////////////////////////////////////////////////////////////////////////

inline int ReadByte(IFile* file)
{
  unsigned char byte;
  int numbytes = file->Read(&byte, 1);
  return (numbytes == 0 ? EOF : byte);
}

////////////////////////////////////////////////////////////////////////////////

bool 
CImage32::Import_BMP(const char* filename, IFileSystem& fs)
{
  BMP_HEADER header;
  int   NumPal;
  BGRA* Pal = NULL;
  bool  result = true;

  IFile* file = fs.Open(filename, IFileSystem::read);
  if (file == NULL)
    return false;

  // read the file header
  file->Read(&header.bfType,      sizeof(word));
  file->Read(&header.bfSize,      sizeof(dword));
  file->Read(&header.bfReserved1, sizeof(word));
  file->Read(&header.bfReserved2, sizeof(word));
  file->Read(&header.bfOffBits,   sizeof(dword));

  // if the file ain't 'BM'
  if (header.bfType != 19778)
  {
    file->Close();
    return false;
  }

  file->Read(&header.biSize, sizeof(dword));
  // my guess this is a OS/DIB bmp header
  if (header.biSize < 40)
  {
    word temp;
    // seek to BITMAPFILEHEADER + 4(the biSize of DIBINFOHEADER)
    //fseek(file, 18, SEEK_SET);
    file->Read(&temp, sizeof(word));
    header.biWidth = temp;
    file->Read(&temp, sizeof(word));
    header.biHeight = temp;
    file->Read(&temp, sizeof(word));
    header.biNumPlanes = temp;
    file->Read(&temp, sizeof(word));
    header.biBitsPerPixel = temp;
    header.biCompression = BMP_RGB;
    
    // calculate and grab palette for non-windows bmp
    NumPal = (header.bfOffBits - (header.biSize + 14)) / 3;
    Pal = new BGRA[NumPal];
    file->Seek(header.biSize + 14);

    for (int i=0; i<NumPal; i++)
    {
      int Red,Green,Blue;
      Blue = ReadByte(file); Green = ReadByte(file); Red = ReadByte(file);
      if (Red == EOF || Green == EOF || Blue == EOF)
      {
        file->Close();
        return false;
      }

      Pal[i].red = Red;
      Pal[i].green = Green;
      Pal[i].blue = Blue;
    }
  }
  else // this is a normal windows bmp header
  { 
    file->Read(&header.biWidth, sizeof(long));
    file->Read(&header.biHeight, sizeof(long));
    file->Read(&header.biNumPlanes, sizeof(word));
    file->Read(&header.biBitsPerPixel, sizeof(word));
    file->Read(&header.biCompression, sizeof(dword));
    
    // calculate and grab palette
    NumPal = (header.bfOffBits - (header.biSize + 14)) / 4;    
    Pal = new BGRA[NumPal];
    //byte* fluff = new byte[header.biSize - 20];

    file->Seek(header.biSize + 14);
    //file->Read(fluff, header.biSize - 20);
    file->Read(Pal, 4 * NumPal);
    //delete[] fluff;
  }

  // set up the Image32 now...
  m_Width  = header.biWidth;
  m_Height = header.biHeight;
  m_Pixels = new RGBA[m_Width * m_Height * 4];
  if (m_Pixels == NULL)
  {
    file->Close();
    return false;
  }
  
  // seek to data in case of > 16bit bitmaps
  file->Seek(header.bfOffBits);

  switch(header.biCompression)
  {
    case BMP_RGB: 
      if (!BMP_ReadRGB(file, header.bfSize - header.bfOffBits, header.biBitsPerPixel, Pal))
        result = false;
      break;

    case BMP_RLE8:
      if (!BMP_ReadRLE8(file, Pal))
        result = false;
      break;

    case BMP_RLE4:
      result = false;
      break;

    default: // I don't have that kind of decompression!
      result = false;
  }

  // clean up and exit this
  delete[] Pal;
  file->Close();
  return result;
}

////////////////////////////////////////////////////////////////////////////////

bool
CImage32::BMP_ReadRGB(IFile* file, int imagesize, int bpp, BGRA* Pal)
{
  // calculates the number of bytes it has to realign
  int reAlignBytes;
  reAlignBytes = imagesize / m_Height;
  reAlignBytes -= m_Width * (bpp/8);

  for (int j=m_Height-1; j>=0; j--)
  {
    for (int i=0; i<m_Width; i++)
    {
      switch(bpp)
      {
        case 1:
        {
          // cool trick ;)
          static char bit[8];
          static int pos = 0;

          // fill in new info if the number is divisible by 8 :)
          int remainder = pos % 8;
          if (remainder == 0)
          {
            int pixel = ReadByte(file);
            if (pixel == EOF)
              return false;
          
            for (int l=0; l<8; l++)
            {
              bit[l] = pixel & 1;
              pixel >>= 1;
            }
          }

          // now draw!
          if (bit[7 - remainder])
            m_Pixels[m_Width * j + i] = CreateRGBA(255, 255, 255, 255);
          else
            m_Pixels[m_Width * j + i] = CreateRGBA(  0,   0,   0, 255);

          pos++;
        }
        break;
        
        case 8:
        {
          int pixel = ReadByte(file);
          if (pixel == EOF)
            return false;
          m_Pixels[m_Width * j + i].red   = Pal[pixel].red;
          m_Pixels[m_Width * j + i].green = Pal[pixel].green;
          m_Pixels[m_Width * j + i].blue  = Pal[pixel].blue;
          m_Pixels[m_Width * j + i].alpha = 255;
        }
        break;

        case 24:         
        { 
          /* byte Red, Green, Blue;           */
			 int Red, Green, Blue;
          
          Blue = ReadByte(file); Green = ReadByte(file); Red = ReadByte(file);          
          if (Red == EOF || Green == EOF || Blue == EOF)
            return false;
          
          m_Pixels[m_Width * j + i].red   = Red;
          m_Pixels[m_Width * j + i].green = Green;
          m_Pixels[m_Width * j + i].blue  = Blue;
          m_Pixels[m_Width * j + i].alpha = 255;
        }
        break;

        case 32:
        {          
          /* byte Red, Green, Blue;           */
			 int Red, Green, Blue;
          
          Blue = ReadByte(file); Green = ReadByte(file); Red = ReadByte(file);
          ReadByte(file);
          if (Red == EOF || Green == EOF || Blue == EOF)
            return false;
          
          m_Pixels[m_Width * j + i].red   = Red;
          m_Pixels[m_Width * j + i].green = Green;
          m_Pixels[m_Width * j + i].blue  = Blue;
          m_Pixels[m_Width * j + i].alpha = 255;
        }
        break;

        // If I haven't done the resolution yet, it will return false
        default:
          return false;
      }
    }

    // this is to fix the alignment problem
    if (bpp != 1)
    for (int w=0; w<reAlignBytes; w++)
        ReadByte(file);      
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CImage32::BMP_ReadRLE8(IFile* file, BGRA* Pal)
{
  for (int y=m_Height-1; y>=0; )
    for (int x = 0; x<m_Width; )
    {
      int code = ReadByte(file);
      int value = ReadByte(file);
      if (code == EOF || value == EOF)
        return false;

      if (code > 0)
      {
        int i;
        for (i=0; i<code; i++)
        {
          m_Pixels[y * m_Width + x].red   = Pal[value].red;
          m_Pixels[y * m_Width + x].green = Pal[value].green;
          m_Pixels[y * m_Width + x].blue  = Pal[value].blue;
          m_Pixels[y * m_Width + x].alpha = 255;
          x++;
        }
      }
      else
      {
        switch (value)
        {
          // end of line
          case 0:
            x = 0;
            y--;
            break;

          // end of picture
          case 1:
            return true;
            break;

          // displace picture
          case 2:
            {
            int x_offset = ReadByte(file);
            int y_offset = ReadByte(file);
            if (x_offset == EOF || x_offset == EOF)
              return false;

            x += x_offset;
            y -= y_offset;
            }
            break;

          default:
            {
            int i;
            for (i=0; i<value; i++)
            {
              int ccode = ReadByte(file);
              if (ccode == EOF) return false;

              m_Pixels[y * m_Width + x].red   = Pal[ccode].red;
              m_Pixels[y * m_Width + x].green = Pal[ccode].green;
              m_Pixels[y * m_Width + x].blue  = Pal[ccode].blue;
              m_Pixels[y * m_Width + x].alpha = 255;
              x++;
            }

            if (value%2 == 1)
              ReadByte(file);
            }
            break;

        }
      }
    }
  return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CImage32::BMP_ReadRLE4(IFile* file, int bpp, int NumPal, RGBA* Pal)
{
  return false;
}

////////////////////////////////////////////////////////////////////////////////

#define STRUCT_NAME PCX_HEADER
#include "begin_packed_struct.h"
  byte manufacturer;
  byte version;
  byte encoding;
  byte bits_per_pixel;
  word xmin;
  word ymin;
  word xmax;
  word ymax;
  word hdpi;
  word vdpi;
  byte colormap[48];
  byte reserved;
  byte num_planes;
  word bytes_per_line;
  word palette_info;
  word h_screen_size;
  word v_screen_size;
  byte filler[54];
#include "end_packed_struct.h"

ASSERT_STRUCT_SIZE(PCX_HEADER, 128)

////////////////////////////////////////////////////////////////////////////////

bool
CImage32::Import_PCX(const char* filename, IFileSystem& fs)
{
  IFile* file = fs.Open(filename, IFileSystem::read);
  if (file == NULL) {
    return false;
  }

  PCX_HEADER header;
  file->Read(&header, sizeof(header));

  m_Width  = header.xmax - header.xmin + 1;
  m_Height = header.ymax - header.ymin + 1;
  m_Pixels = new RGBA[m_Width * m_Height];

  int   scansize = header.num_planes * header.bytes_per_line;
  byte* scanline = new byte[scansize];

  if (header.num_planes == 1)      // 256 colors
  {
    RGB palette[256];
    byte* image = new byte[m_Width * m_Height];

    for (int iy = 0; iy < m_Height; iy++)
    {
      PCX_ReadScanline(file, scansize, scanline);
      memcpy(image + (iy * m_Width), scanline, m_Width);
    }

    // read palette
    // one byte padding :P
    ReadByte(file);
    file->Read(palette, 3 * 256);
    for (int i=0; i < m_Width * m_Height; i++)
    {
      m_Pixels[i].red   = palette[image[i]].red;
      m_Pixels[i].green = palette[image[i]].green;
      m_Pixels[i].blue  = palette[image[i]].blue;
      m_Pixels[i].alpha = 255;
    }

    delete[] image;
  }
  else if (header.num_planes == 3) // 24-bit color
  {
    for (int iy = 0; iy < m_Height; iy++)
    {
      PCX_ReadScanline(file, scansize, scanline);
      for (int ix = 0; ix < m_Width; ix++)
        m_Pixels[iy * m_Width + ix].red   = scanline[ix + 0 * header.bytes_per_line];
      for (int ix = 0; ix < m_Width; ix++)
        m_Pixels[iy * m_Width + ix].green = scanline[ix + 1 * header.bytes_per_line];
      for (int ix = 0; ix < m_Width; ix++)
      {
        m_Pixels[iy * m_Width + ix].blue  = scanline[ix + 2 * header.bytes_per_line];
        m_Pixels[iy * m_Width + ix].alpha = 255;
      }
    }
  }

  delete[] scanline;
  file->Close();
  return true;
}

////////////////////////////////////////////////////////////////////////////////

void
CImage32::PCX_ReadScanline(IFile* file, int scansize, byte* scanline)
{
  int bytesread = 0;
  while (bytesread < scansize)
  {
    /* byte data = ReadByte(file); */
	 int data = ReadByte(file);
    if (data == EOF)
      return;

    if (data > 192 && data < 256)
    {
      int numbytes = data - 192;
      data = ReadByte(file);
      for (int i = 0; i < numbytes; i++)
      {
        scanline[bytesread] = data;
        bytesread++;
      }
    }
    else
    {
      scanline[bytesread] = data;
      bytesread++;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

bool
CImage32::Export_PNG(const char* filename, IFileSystem& fs) const
{
  IFile* file = fs.Open(filename, IFileSystem::write);
  if (file == NULL)
    return false;

  // create png struct
  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr)
  {
    file->Close();
    return false;
  }

  // create info struct
  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
    png_destroy_write_struct(&png_ptr, NULL);
    file->Close();
    return false;
  }

  // read the image
  png_set_write_fn(png_ptr, file, PNG_write_function, PNG_flush_function);

  png_set_IHDR(png_ptr, info_ptr, m_Width, m_Height, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

  // put the image data into the PNG
  void** rows = (void**)png_malloc(png_ptr, sizeof(void*) * m_Height);
  for (int i = 0; i < m_Height; i++)
  {
    rows[i] = png_malloc(png_ptr, sizeof(RGBA) * m_Width);
    memcpy(rows[i], m_Pixels + i * m_Width, m_Width * sizeof(RGBA));
  }
  png_set_rows(png_ptr, info_ptr, (png_bytepp)rows);
  info_ptr->valid |= PNG_INFO_IDAT;

  png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

  png_destroy_write_struct(&png_ptr, &info_ptr);
  file->Close();
  return true;
}

////////////////////////////////////////////////////////////////////////////////
