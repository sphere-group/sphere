#ifndef TILESET_EDIT_VIEW_HPP
#define TILESET_EDIT_VIEW_HPP
#include "HScrollWindow.hpp"
#include "ImageView.hpp"
#include "PaletteView.hpp"
#include "ColorView.hpp"
#include "AlphaView.hpp"
#include "SwatchPalette.hpp"
#include "../common/Tileset.hpp"
class ITilesetEditViewHandler
{
public:
  virtual void TEV_SelectedTileChanged(int tile) = 0;
  virtual void TEV_TileModified(int tile) = 0;
  virtual void TEV_TilesetModified() = 0;
};
class CTilesetEditView
  : public CHScrollWindow
  , private IImageViewHandler
  , private IPaletteViewHandler
  , private IColorViewHandler
  , private IAlphaViewHandler
{
public:
  CTilesetEditView();
  ~CTilesetEditView();
  BOOL Create(CWnd* parent, CDocumentWindow* owner, ITilesetEditViewHandler* handler, sTileset* tileset);
  void TilesetChanged();
  void SelectTile(int tile);
  void SP_ColorSelected(RGBA color);
public:
  void OnToolChanged(UINT id, int tool_index);
  BOOL IsToolAvailable(UINT id);
  afx_msg void OnKeyDown(UINT vk, UINT repeat, UINT flags);
  afx_msg void OnKeyUp(UINT vk, UINT repeat, UINT flags);
  afx_msg void OnTimer(UINT event);
private:
  void UpdateImageView();
  void UpdateScrollBar();
  virtual void OnHScrollChanged(int x);
  afx_msg void OnSize(UINT type, int cx, int cy);
  afx_msg void OnTilesetInsertTiles();
  afx_msg void OnTilesetAppendTiles();
  afx_msg void OnUpdateTilesetDeleteTile(CCmdUI* cmdui);
  afx_msg void OnTilesetDeleteTile();
  afx_msg void OnTilesetReplaceWithImage();
  afx_msg void OnTilesetInsertImage();
  afx_msg void OnTilesetAppendImage();
  virtual void IV_ImageChanged();
  virtual void IV_ColorChanged(RGBA color);
  virtual void PV_ColorChanged(int index, RGB color);
  virtual void CV_ColorChanged(int index, RGB color);
  virtual void AV_AlphaChanged(byte alpha);
  afx_msg void OnPaste();
  afx_msg void OnUndo();
  afx_msg void OnRedo();
#if 1
public:
  void SetTileSelection(int width, int height, unsigned int* tiles);
private:
  int m_MultiTileWidth;
  int m_MultiTileHeight;
  unsigned int* m_MultiTileData;
#endif
private:
  ITilesetEditViewHandler* m_Handler;
  sTileset* m_Tileset;
  int   m_CurrentTile;
  sTile m_UndoTile;
  bool m_Created;
  // views
  CImageView   m_ImageView;
  CPaletteView m_PaletteView;
  CColorView   m_ColorView;
  CAlphaView   m_AlphaView;
  DECLARE_MESSAGE_MAP()
};
#endif
