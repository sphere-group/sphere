#ifndef WINDOWSTYLE_PREVIEW_PALETTE_HPP
#define WINDOWSTYLE_PREVIEW_PALETTE_HPP
#include "PaletteWindow.hpp"
#include "DIBSection.hpp"
#include "../common/WindowStyle.hpp"
#include "Zoomer.hpp"
class CDocumentWindow;
class CWindowStylePreviewPalette : public CPaletteWindow
{
public:
  CWindowStylePreviewPalette(CDocumentWindow* owner, sWindowStyle* windowstyle);
  virtual void Destroy();
	void OnWindowStyleChanged();
private:
	void OnZoom(double zoom);
  void DrawBitmap(CPaintDC& dc, int bitmap, int x, int y, int mode);
  void DrawCorner(CPaintDC& dc, int bitmap, int x, int y);
  void DrawBackground(CPaintDC& dc, int bitmap, int x, int y);
  void DrawEdge(CPaintDC& dc, int bitmap, int x, int y);
private:
  afx_msg void OnPaint();
  afx_msg void OnRButtonUp(UINT flags, CPoint point);
	afx_msg void OnTimer(UINT event);
  
  afx_msg void OnZoomIn();
  afx_msg void OnZoomOut();
private:
  sWindowStyle* m_WindowStyle;
	Zoomer m_ZoomFactor;
  CDIBSection* m_BlitImage;
  DECLARE_MESSAGE_MAP()
};
#endif
