#ifndef SPRITESET_WINDOW_HPP
#define SPRITESET_WINDOW_HPP
#include "SaveableDocumentWindow.hpp"
#include "SpritesetView.hpp"
#include "ImageView.hpp"
#include "PaletteView.hpp"
#include "ColorView.hpp"
#include "AlphaView.hpp"
#include "SpriteBaseView.hpp"
#include "SpritesetImagesPalette.hpp"
#include "SpritesetAnimationPalette.hpp"
#include "../common/Spriteset.hpp"
class CSpritesetWindow
  : public CSaveableDocumentWindow
  , private ISpritesetViewHandler
  , private IImageViewHandler
  , private IPaletteViewHandler
  , private IColorViewHandler
  , private IAlphaViewHandler
  , private ISpriteBaseViewHandler
  , private ISpritesetImagesPaletteHandler
{
#ifdef USE_SIZECBAR
	DECLARE_DYNAMIC(CSpritesetWindow)
#endif
public:
  CSpritesetWindow(const char* filename = NULL);
private:
  void OnToolChanged(UINT id, int tool_index);
  BOOL IsToolAvailable(UINT id);
  afx_msg void OnKeyDown(UINT vk, UINT repeat, UINT flags);
  afx_msg void OnKeyUp(UINT vk, UINT repeat, UINT flags);
  afx_msg void OnTimer(UINT event);
private:
  void Create();
  void TabChanged(int tab);
  void ShowFramesTab(int show);
  void ShowEditTab(int show);
  void ShowBaseTab(int show);
  void UpdateImageView();
  afx_msg void OnDestroy();
  afx_msg void OnSize(UINT type, int cx, int cy);
  afx_msg void OnZoom1x();
  afx_msg void OnZoom2x();
  afx_msg void OnZoom4x();
  afx_msg void OnZoom8x();
  afx_msg void OnZoomIn();
  afx_msg void OnZoomOut();
  afx_msg void OnResize();
  afx_msg void OnRescale();
  afx_msg void OnResample();
  afx_msg void OnFillDelay();
  afx_msg void OnFrameProperties();
  afx_msg void OnExportAsImage();
  afx_msg void OnUpdateZoom1x(CCmdUI* cmdui);
  afx_msg void OnUpdateZoom2x(CCmdUI* cmdui);
  afx_msg void OnUpdateZoom4x(CCmdUI* cmdui);
  afx_msg void OnUpdateZoom8x(CCmdUI* cmdui);
  afx_msg void OnCopy();
  afx_msg void OnPaste();
  afx_msg void OnUndo();
  afx_msg void OnRedo();
  //afx_msg void OnUpdateZoomIn(CCmdUI* cmdui);
  //afx_msg void OnUpdateZoomOut(CCmdUI* cmdui);
  afx_msg void OnTabChanged(NMHDR* ns, LRESULT* result);
  afx_msg void OnFramesTab();
  afx_msg void OnEditTab();
  afx_msg void OnBaseTab();
  afx_msg void OnUpdateFramesTab(CCmdUI* cmdui);
  afx_msg void OnUpdateEditTab(CCmdUI* cmdui);
  afx_msg void OnUpdateBaseTab(CCmdUI* cmdui);
  virtual bool GetSavePath(char* path);
  virtual bool SaveDocument(const char* path);
  virtual void SV_CurrentFrameChanged(int direction, int frame);
  virtual void SV_EditFrame();
  virtual void SV_SpritesetModified();
  virtual void SV_CopyCurrentFrame();
  virtual void SV_PasteCurrentFrame();
	virtual void SV_ZoomFactorChanged(double zoom);
  virtual void IV_ImageChanged();
  virtual void IV_ColorChanged(RGBA color);
  virtual void PV_ColorChanged(int index, RGB color);
  virtual void CV_ColorChanged(int index, RGB color);
  virtual void AV_AlphaChanged(byte alpha);
  virtual void SBV_SpritesetModified();
  virtual void SP_ColorSelected(RGBA color);
  virtual void SIP_IndexChanged(int index);
  virtual void SIP_SpritesetModified();
private:
  sSpriteset m_Spriteset;
  sTileset m_Tile;
  int m_CurrentDirection;
  int m_CurrentFrame;
  bool m_Created;
  CTabCtrl    m_TabControl;
  // views
  CSpritesetView  m_SpritesetView;
  CImageView      m_ImageView;
  CPaletteView    m_PaletteView;
  CColorView      m_ColorView;
  CAlphaView      m_AlphaView;
  CSpriteBaseView m_SpriteBaseView;
  // palette
  CSpritesetImagesPalette* m_ImagesPalette;
	CSpritesetAnimationPalette* m_AnimationPalette;
  UINT m_Timer;
  DECLARE_MESSAGE_MAP()
};
#endif
