#include "Image.hpp"

namespace sde {
  
  void Point::Point(u16 _x=0, u16 _y=0) { x = _x; y = _y; }

  class Image 
  {
  public:
    bool Image(u16 size_x, u16 size_y, RGBA* buffer) 
    {
      m_x = size_x;
      m_y = size_y;
      m_buffer = buffer;
    }

    inline void SetPixel(Point p, RGBA color)
    {
      Blend3(m_buffer[(p.y*m_x)+p.x], color, color.alpha);
    }

    inline RGBA GetPixel(Point p) 
    {
      return m_buffer[(p.y*m_x)+p.x];
    }

    inline RGBA* GetPixels()
    {
      return m_buffer;
    }

  protected:
    u16 m_x, m_y;
    RGBA* m_buffer;
  };

}
