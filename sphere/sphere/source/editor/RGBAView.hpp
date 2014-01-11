#ifndef RGBA_VIEW_HPP
#define RGBA_VIEW_HPP
#include "ColorView.hpp"
#include "AlphaView.hpp"
class CRGBAView : public IColorViewHandler, public IAlphaViewHandler
{
public:
  CRGBAView();
  BOOL Create(CWnd* parent, int x, int y, int w, int h, RGBA color);
  RGBA GetColor(int index = 0);
  void SetNumColors(int num_colors);
private:
  virtual void CV_ColorChanged(int index, RGB color);
  virtual void AV_AlphaChanged(byte alpha);
private:
  CColorView m_ColorView;
  CAlphaView m_AlphaView;
  RGBA m_Colors[2];
};
#endif
