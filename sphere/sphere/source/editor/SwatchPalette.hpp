#ifndef SWATCH_PALETTE_HPP
#define SWATCH_PALETTE_HPP
#include "PaletteWindow.hpp"
#include "TilesetView.hpp"
#include "SwatchServer.hpp"
class CMainWindow;
class ISwatchPaletteHandler
{
public:
  virtual void SP_ColorSelected(RGBA color) = 0;
};
class CSwatchPalette : public CPaletteWindow
{
public:
  CSwatchPalette(CDocumentWindow* owner, ISwatchPaletteHandler* handler);
  void Destroy();
  
  void SetColor(RGBA color);
private:
  afx_msg void OnSize(UINT type, int cx, int cy);
  afx_msg void OnPaint();
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnFileLoad();
  afx_msg void OnFileSave();
  afx_msg void OnDefaultDOS();
  afx_msg void OnDefaultVERGE();
  afx_msg void OnDefaultPlasma();
  afx_msg void OnDefaultRGB332();
  afx_msg void OnDefaultVisibone2();
  afx_msg void OnDefaultHsl256();
  afx_msg void OnInsertColorBefore();
  afx_msg void OnInsertColorAfter();
  afx_msg void OnReplaceColor();
  afx_msg void OnDeleteColor();
  void UpdateScrollBar();
  int  GetPageSize();
  int  GetNumRows();
private:
  ISwatchPaletteHandler* m_Handler;
  RGBA m_Color;
  int m_TopRow;
  int m_SelectedColor;
  int m_RightClickColor;
  friend CMainWindow;
  DECLARE_MESSAGE_MAP()
};
#endif
