#ifndef IMAGE_VIEW_HPP
#define IMAGE_VIEW_HPP
//#define SCROLLABLE_IMAGE_WINDOW 1
#ifdef SCROLLABLE_IMAGE_WINDOW
#include "ScrollWindow.hpp"
#endif
#include <afxwin.h>
#include "../common/Image32.hpp"
#include "SwatchPalette.hpp"
#include "ImageToolPalette.hpp"
#include "DIBSection.hpp"
#include "Clipboard.hpp"
class CDocumentWindow;  // #include "DocumentWindow.hpp"
class IImageViewHandler
{
public:
  virtual void IV_ImageChanged() = 0;
  virtual void IV_ColorChanged(RGBA color) = 0;
};
class CImageView :
#ifdef SCROLLABLE_IMAGE_WINDOW
  public CScrollWindow,
#else
  public CWnd,
#endif
  public ISwatchPaletteHandler,
  public IToolPaletteHandler
{
public:
  CImageView();
  ~CImageView();
  BOOL Create(CDocumentWindow* owner, IImageViewHandler* handler, CWnd* parent_window);
  bool        SetImage(int width, int height, const RGBA* pixels, bool reset_undo_states);
  int         GetWidth() const;
  int         GetHeight() const;
  RGBA*       GetPixels();
  const RGBA* GetPixels() const;
  void SetColor(int index, RGBA color);
  RGBA GetColor(int index = 0) const;
  void FillRGB();
  void FillAlpha();
  void BeforeImageChanged();
  void AfterImageChanged();
#ifdef SCROLLABLE_IMAGE_WINDOW
private:
  void UpdateScrollBars();
  int GetPageSizeX();
  int GetPageSizeY();
  int GetTotalTilesX();
  int GetTotalTilesY();
  virtual void OnHScrollChanged(int x);
  virtual void OnVScrollChanged(int y);
  int m_CurrentX;
  int m_CurrentY;
#endif
private:
  enum MergeMethod { Merge_Replace, Merge_Blend, Merge_IntoSelection };
  struct Image {
    int width;
    int height;
    RGBA* pixels;
  };
public:
  bool Copy();
  bool PasteChannels(bool red, bool green, bool blue, bool alpha, int merge_method = Merge_Replace);
  bool Paste();
  bool PasteRGB();
  bool PasteAlpha();
  bool CanUndo() const;
  void Undo();
  void Redo();
  bool CanRedo() const;
private:
  enum Tool 
  { 
    Tool_Pencil, 
    Tool_Line, 
    Tool_Rectangle, 
    Tool_Circle, 
    Tool_Ellipse, 
    Tool_Fill, 
    Tool_Selection, 
    Tool_FreeSelection 
  };
private:
  virtual void SP_ColorSelected(RGBA color);
  virtual void TP_ToolSelected(int tool, int tool_index);
  void UpdateCursor(UINT flags, CPoint point);
  POINT ConvertToPixel(POINT point);
  bool InImage(POINT p);
  enum SelectionType { ST_Rectangle, ST_Free };
  SelectionType m_SelectionType;
  bool InSelection(POINT p);
  RGBA* GetSelectionPixels();
  void UpdateSelectionPixels(const RGBA* pixels, int sx, int sy, int sw, int sh);
public:
  int GetSelectionLeftX();
  int GetSelectionTopY();
  int GetSelectionWidth();
  int GetSelectionHeight();
private:
  void FreeSelectionPixels(RGBA* pixels);
  void InvalidateSelection(int sx, int sy, int sw, int sh);
  void ClipPointToWithinImage(POINT* point);
  void Click(bool force_draw);
  void Fill();
  bool IsColorToReplace(RGBA pixel, RGBA colorToReplace);
  void FillMe(int x, int y, RGBA colorToReplace);
  void Line();
  void Rectangle();
  void Circle();
  void Ellipse();
  void UpdateSelection();
  void Selection();
  void GetColor(RGBA* color, int x, int y);
  void AddUndoState();
  void ResetUndoStates();
  void ResetRedoStates();
  afx_msg void OnPaint();
  void PaintLine(CImage32& pImage);
  void PaintRectangle(CImage32& pImage);
  void PaintCircle(CImage32& pImage);
  void PaintEllipse(CImage32& pImage);
  afx_msg void OnSize(UINT type, int cx, int cy);
  void OnMouseClick(int index, UINT flags, CPoint point);
  afx_msg void OnLButtonDown(UINT flags, CPoint point);
  afx_msg void OnLButtonUp(UINT flags, CPoint point);
  afx_msg void OnRButtonDown(UINT flags, CPoint point);
  afx_msg void OnRButtonUp(UINT flags, CPoint point);
  afx_msg void OnMouseMove(UINT flags, CPoint point);
  afx_msg void OnChar(UINT nchar, UINT nrepcnt, UINT flags);
public:
  afx_msg void OnKeyDown(UINT vk, UINT repeat, UINT flags);
  afx_msg void OnKeyUp(UINT vk, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT event);
private:
  afx_msg void OnColorPicker();
  afx_msg void OnUndo();
  afx_msg void OnRedo();
  afx_msg void OnCopy();
  afx_msg void OnPaste();
  afx_msg void OnPasteRGB();
  afx_msg void OnPasteAlpha();
  afx_msg void OnPasteIntoSelection();
  afx_msg void OnViewGrid();
  afx_msg void OnToggleViewAlphaMask();
  
  afx_msg void OnBlendModeBlend();
  afx_msg void OnBlendModeReplace();
  afx_msg void OnBlendModeRGBOnly();
  afx_msg void OnBlendModeAlphaOnly();
  afx_msg void OnBlendModeAdditive();
  afx_msg void OnBlendModeSubtractive();
  afx_msg void OnBlendModeMultiplicative();
  afx_msg void OnBlendModeAverage();
  afx_msg void OnBlendModeInvert();
  
  afx_msg void OnRotateCW();
  afx_msg void OnRotateCCW();
  afx_msg void OnSlideUp();
  afx_msg void OnSlideRight();
  afx_msg void OnSlideDown();
  afx_msg void OnSlideLeft();
  afx_msg void OnSlideOther();
  afx_msg void OnFlipHorizontally();
  afx_msg void OnFlipVertically();
  afx_msg void OnFillRGB();
  afx_msg void OnFillAlpha();
  afx_msg void OnFillBoth();
  afx_msg void OnReplaceRGBA();
  afx_msg void OnReplaceRGB();
  afx_msg void OnReplaceAlpha();
  afx_msg void OnFilterCustom();
  afx_msg void OnFilterGrayscale();
  afx_msg void OnFilterSaturate();
  afx_msg void OnFilterColorAdjuster();
  afx_msg void OnFilterBlur();
  afx_msg void OnFilterNoise();
  afx_msg void OnFilterAdjustBrightness();
  afx_msg void OnFilterAdjustGamma();
  afx_msg void OnFilterNegativeImage(bool red, bool green, bool blue, bool alpha);
  afx_msg void OnFilterNegativeImageRGB();
  afx_msg void OnFilterNegativeImageAlpha();
  afx_msg void OnFilterNegativeImageRGBA();
  afx_msg void OnFilterSolarize();
  afx_msg void OnSetColorAlpha();
  afx_msg void OnScaleAlpha();
	afx_msg LRESULT OnGetAccelerator(WPARAM wParam, LPARAM lParam);
public:
  afx_msg void OnToolChanged(UINT id, int tool_index);
  BOOL IsToolAvailable(UINT id);
private:
  IImageViewHandler* m_Handler;
  CImage32 m_Image;
  CDIBSection* m_BlitTile;  // for rendering tiles
  CSwatchPalette*    m_SwatchPalette;
  //CImageToolPalette* m_ToolPalette;
  // drawing
  RGBA  m_Colors[2];
  POINT m_StartPoint;
  POINT m_CurPoint;
  POINT m_LastPoint;
  bool  m_MouseDown[2];
  int m_SelectedTools[2];
  int m_CurrentTool;
  int m_SelectionX;
  int m_SelectionY;
  int m_SelectionWidth;
  int m_SelectionHeight;
  RGBA m_ColorMask1;
  RGBA m_ColorMask2;
  std::vector<POINT> m_SelectionPoints;
  // undo
  int    m_NumUndoImages;
  Image* m_UndoImages;
  int    m_NumRedoImages;
  Image* m_RedoImages;
  bool m_ShowGrid;
  bool m_ShowAlphaMask;
  int m_RedrawX;
  int m_RedrawY;
  int m_RedrawWidth;
  int m_RedrawHeight;
  bool key_up, key_down, key_left, key_right;
public:
  CClipboard* m_Clipboard;
private:
  DECLARE_MESSAGE_MAP()
};
#endif
