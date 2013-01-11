#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <wx.h>
#include <../common/rgb.hpp>

namespace sde {

  class Point {
  public:
    void Point(u16 _x=0, u16 _y=0);
    u16  x, y;
  };

  class Image {
  public:
    bool         Image(u16 size_x, u16 size_y, RGBA* buffer);
    inline void  SetPixel(Point p, RGBA color);
    inline RGBA  GetPixel(Point p);
    inline RGBA* GetPixels();
  };

}

#endif // IMAGE_HPP
