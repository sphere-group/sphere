#ifndef FONT_WINDOW_HPP
#define FONT_WINDOW_HPP
#include "SaveableDocumentWindow.hpp"
#include "ImageView.hpp"
#include "PaletteView.hpp"
#include "ColorView.hpp"
#include "AlphaView.hpp"
#include "../common/Font.hpp"
#include "FontPreviewPalette.hpp"
class CFontWindow
: public CSaveableDocumentWindow
, private IImageViewHandler
, private IPaletteViewHandler
, private IColorViewHandler
, private IAlphaViewHandler
{
#ifdef USE_SIZECBAR
	DECLARE_DYNAMIC(CFontWindow)
#endif
public:
  CFontWindow(const char* font = NULL);
  ~CFontWindow();
private:
  void OnToolChanged(UINT id, int tool_index);
  BOOL IsToolAvailable(UINT id);
private:
  void Create();
  void UpdateWindowTitle();
  void SetImage();
  afx_msg void OnChar(UINT c, UINT repeat, UINT flags);
  afx_msg void OnKeyDown(UINT vk, UINT repeat, UINT flags);
  afx_msg void OnSize(UINT type, int cx, int cy);
  afx_msg void OnHScroll(UINT sbcode, UINT pos, CScrollBar* scroll_bar);
  afx_msg void OnFontResize();
  //afx_msg void OnFontResizeAll();
  afx_msg void OnFontSimplify();
  afx_msg void OnFontMakeColorTransparent();
  afx_msg void OnFontGenerateGradient();
  afx_msg void OnFontExportToImage();
  afx_msg void OnFontAdjustBorders();
  afx_msg void OnCopy();
  afx_msg void OnPaste();
  afx_msg void OnUndo();
  afx_msg void OnRedo();
  afx_msg void OnZoomIn();
  afx_msg void OnZoomOut();
  virtual bool GetSavePath(char* path);
  virtual bool SaveDocument(const char* path);
  // view handlers
  virtual void IV_ImageChanged();
  virtual void IV_ColorChanged(RGBA color);
  virtual void PV_ColorChanged(int index, RGB color);
  virtual void CV_ColorChanged(int index, RGB color);
  virtual void AV_AlphaChanged(byte alpha);
  // edit range
  afx_msg void OnEditRange();
private:
  std::vector<int> GetWhichCharacters() const;
  void GetFontMinMax(const std::vector<int> character_list, int& min_x, int& min_y, int& max_x, int& max_y) const;
private:
  sFont m_Font;
  int   m_CurrentCharacter;
  RGBA  m_CurrentColor;
  bool         m_Created;
  CImageView   m_ImageView;
  CPaletteView m_PaletteView;
  CColorView   m_ColorView;
  CAlphaView   m_AlphaView;
  CScrollBar   m_ScrollBar;
  CFontPreviewPalette* m_FontPreviewPalette;
  DECLARE_MESSAGE_MAP()
};
#endif
