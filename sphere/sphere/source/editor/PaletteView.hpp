#ifndef PALETTE_VIEW_HPP
#define PALETTE_VIEW_HPP
// 2004-06-24 //
#include <afxwin.h>
#include "../common/rgb.hpp"
class CDIBSection;  // #include "DIBSection.hpp"
class IPaletteViewHandler
{
public:
  virtual void PV_ColorChanged(int index, RGB color) = 0;
};
class CPaletteView : public CWnd
{
public:
  CPaletteView();
  ~CPaletteView();
  BOOL Create(IPaletteViewHandler* pHandler, CWnd* pParentWindow);
  RGB GetColor(int index = 0) const;
private:
  bool UpdateColor(int index, UINT flags, CPoint point);
  void UpdatePalette(int w, int h, int pitch, BGR* pixels);
  afx_msg void OnPaint();
  afx_msg void OnLButtonDown(UINT flags, CPoint point);
  afx_msg void OnLButtonUp(UINT flags, CPoint point);
  afx_msg void OnRButtonDown(UINT flags, CPoint point);
  afx_msg void OnRButtonUp(UINT flags, CPoint point);
  afx_msg void OnMouseMove(UINT flags, CPoint point);
  afx_msg void OnSize(UINT type, int cx, int cy);
private:
  IPaletteViewHandler* m_pHandler;
  CDIBSection* m_pPaletteDIB;
  RGB          m_Colors[2];
  bool         m_bMouseDown[2];
  DECLARE_MESSAGE_MAP()
};
#endif
