#ifndef SPRITESET_IMAGES_PALETTE_HPP
#define SPRITESET_IMAGES_PALETTE_HPP
#include "PaletteWindow.hpp"
#include "DIBSection.hpp"
#include "../common/Spriteset.hpp"
#include "Zoomer.hpp"
class CDocumentWindow;
struct ISpritesetImagesPaletteHandler
{
  virtual void SIP_IndexChanged(int index) = 0;
  virtual void SIP_SpritesetModified() = 0;
};
class CSpritesetImagesPalette : public CPaletteWindow
{
public:
  CSpritesetImagesPalette(CDocumentWindow* owner, ISpritesetImagesPaletteHandler* handler, sSpriteset* spriteset);

  virtual void Destroy();
  void SetCurrentImage(int image);
  void SpritesetResized();
private:
  int GetPageSize();
  int GetNumRows();
  void UpdateScrollBar();
private:
  afx_msg void OnSize(UINT type, int cx, int cy);
  afx_msg void OnPaint();
  afx_msg void OnLButtonDown(UINT flags, CPoint point);
  afx_msg void OnRButtonUp(UINT flags, CPoint point);
  afx_msg void OnVScroll(UINT code, UINT pos, CScrollBar* scroll_bar);
  afx_msg void OnMoveBack();
  afx_msg void OnMoveForward();

  afx_msg void OnInsertImage();
  afx_msg void OnAppendImage();
  afx_msg void OnRemoveImage();
  afx_msg void OnInsertImages();
  afx_msg void OnAppendImages();
  afx_msg void OnRemoveImages();
  afx_msg void OnReplacePaletteFromImageHorizontal();
  afx_msg void OnReplacePaletteFromImageVertical();
  afx_msg void OnReplacePaletteFromImageFixed();
  afx_msg void OnExportPaletteToImageHorizontal();
  afx_msg void OnExportPaletteToImageVertical();
  afx_msg void OnExportPaletteToImageFixed();
  afx_msg void OnReplaceFromImageSingle();
  afx_msg void OnExportToImageSingle();

  void OnSwap(int new_index);
  void OnZoom(double zoom);

  afx_msg void OnZoomIn();
  afx_msg void OnZoomOut();
  afx_msg void OnZoom1X();
  afx_msg void OnZoom2X();
  afx_msg void OnZoom4X();
  afx_msg void OnZoom8X();
private:
  ISpritesetImagesPaletteHandler* m_Handler;
  sSpriteset* m_Spriteset;
  int m_TopRow;
  Zoomer m_ZoomFactor;
  int m_SelectedImage;
  CDIBSection* m_BlitImage;
  DECLARE_MESSAGE_MAP()
};
#endif
