#ifndef COLOR_VIEW_HPP
#define COLOR_VIEW_HPP
#include <afxwin.h>
#include "../common/rgb.hpp"
class IColorViewHandler
{
public:
  virtual void CV_ColorChanged(int index, RGB color) = 0;
};
class CColorView : public CWnd
{
public:
  CColorView();
  ~CColorView();
  BOOL Create(IColorViewHandler* handler, CWnd* parent, RGB color_a, RGB color_b);
  BOOL Create(IColorViewHandler* handler, CWnd* parent);
  void SetColor(int index, RGB color);
  RGB  GetColor(int index = 0) const;
  void SetNumColors(int num_colors);
private:
  afx_msg void OnPaint();
  afx_msg void OnLButtonDown(UINT flags, CPoint point);
private:
  IColorViewHandler* m_Handler;
  RGB m_ColorA;
  RGB m_ColorB;
  int m_NumColors;
  DECLARE_MESSAGE_MAP()
};
#endif
