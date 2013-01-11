#ifndef FONT_PREVIEW_PALETTE_HPP
#define FONT_PREVIEW_PALETTE_HPP
#include "PaletteWindow.hpp"
#include "DIBSection.hpp"
#include "../common/Font.hpp"
#include "Zoomer.hpp"
class CDocumentWindow;
class CFontPreviewPalette : public CPaletteWindow
{
public:
  CFontPreviewPalette(CDocumentWindow* owner, sFont* font);
  virtual void Destroy();
	void OnCharacterChanged(int character);
  afx_msg void OnSetText();
private:
	void OnZoom(double zoom);
  afx_msg void OnPaint();
  afx_msg void OnRButtonUp(UINT flags, CPoint point);
	afx_msg void OnTimer(UINT event);
  afx_msg void OnZoomIn();
  afx_msg void OnZoomOut();
  void OnSize(UINT type, int cx, int cy);
private:
  sFont* m_Font;
	Zoomer m_ZoomFactor;
  int m_RedrawCharacter;
  bool m_RedrawAll;
  std::string m_Text;
  CDIBSection* m_BlitImage;
  DECLARE_MESSAGE_MAP()
};
#endif
