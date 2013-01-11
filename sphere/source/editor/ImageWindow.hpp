#ifndef IMAGE_WINDOW_HPP
#define IMAGE_WINDOW_HPP
#include <afxcmn.h>
#include "SaveableDocumentWindow.hpp"
#include "../common/Image32.hpp"
#include "ImageView.hpp"
#include "PaletteView.hpp"
#include "ColorView.hpp"
#include "AlphaView.hpp"
class CImageWindow
  : public CSaveableDocumentWindow
  , private IImageViewHandler
  , private IPaletteViewHandler
  , private IColorViewHandler
  , private IAlphaViewHandler
{
#ifdef USE_SIZECBAR
	DECLARE_DYNAMIC(CImageWindow)
#endif
public:
  CImageWindow(const char* image = NULL, bool create_from_clipboard = false);
  ~CImageWindow();
private:
  void OnToolChanged(UINT id, int tool_index);
  BOOL IsToolAvailable(UINT id);
private:
  void UpdateImageView();
  afx_msg void OnSize(UINT type, int cx, int cy);
  afx_msg void OnKeyDown(UINT vk, UINT repeat, UINT flags);
  afx_msg void OnKeyUp(UINT vk, UINT repeat, UINT flags);
  afx_msg void OnTimer(UINT event);
  afx_msg void OnImageResize();
  afx_msg void OnImageRescale();
  afx_msg void OnImageResample();
  afx_msg void OnImageCrop();
  afx_msg void OnImageRotate();
  afx_msg void OnCountColorsUsed();
  afx_msg void OnImageAdjustBorders();
  afx_msg void OnImageViewOriginalSize();
  afx_msg void OnUpdateImageViewOriginalSizeCommand(CCmdUI* cmdui);
  virtual bool GetSavePath(char* path);
  virtual bool SaveDocument(const char* path);
  // view handlers
  virtual void IV_ImageChanged();
  virtual void IV_ColorChanged(RGBA color);
  virtual void PV_ColorChanged(int index, RGB color);
  virtual void CV_ColorChanged(int index, RGB color);
  virtual void AV_AlphaChanged(byte alpha);
  void OnCopy();
  void OnPaste();
  void OnUndo();
  void OnRedo();
private:
  bool       m_Created;
  CImage32   m_Image;
  CImageView m_ImageView;
  CPaletteView m_PaletteView;
  CColorView   m_ColorView;
  CAlphaView   m_AlphaView;
  UINT m_Timer;
  DECLARE_MESSAGE_MAP()
};
#endif
