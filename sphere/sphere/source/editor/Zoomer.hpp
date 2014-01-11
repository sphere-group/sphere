#ifndef ZOOMER_HPP
#define ZOOMER_HPP

class Zoomer {
private:
  double m_zoom_factor;

public:
  Zoomer() {
    m_zoom_factor = 1.0;
  }

public:
  inline double GetZoomFactor() const {
    return m_zoom_factor;
  }

  void ZoomIn() {
    if (m_zoom_factor < 8.0) {
      m_zoom_factor *= 2;
    }
  }

  void ZoomOut() {
    //if (m_zoom_factor > 1.0/8.0)
    if (m_zoom_factor > 1.0)
    {
      m_zoom_factor /= 2;
    }
  }

  void SetZoomFactor(double zoom_factor) {
    m_zoom_factor = zoom_factor;
  }
};

#endif
