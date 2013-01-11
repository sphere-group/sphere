#ifndef TILESET_VIEW_HPP
#define TILESET_VIEW_HPP
#include <afxwin.h>
#include <afxext.h>
#include "DIBSection.hpp"
#include "../common/Tileset.hpp"
#include "Zoomer.hpp"
class ITilesetViewHandler
{
public:
  virtual void TV_SelectedTileChanged(int tile) = 0;
  virtual void TV_TilesetChanged() = 0;
  virtual void TV_InsertedTiles(int at, int numtiles) = 0;
  virtual void TV_DeletedTiles(int at, int numtiles) = 0;
  virtual void TV_SwapTiles(std::vector<int> list_a, std::vector<int> list_b) = 0;
  virtual void TV_TilesetSelectionChanged(int width, int height, unsigned int* tiles) = 0;
};
class CTilesetView : public CWnd
{
public:
  CTilesetView();
  ~CTilesetView();
  BOOL Create(ITilesetViewHandler* handler, CWnd* parent, sTileset* tileset);
  void TileChanged(int tile);
  void TilesetChanged();
  void SetSelectedTile(int tile);
  int  GetSelectedTile() const;
  int GetNumTilesPerRow() const;
private:
  int GetPageSize();
  int GetNumRows();
  void UpdateScrollBar();
  afx_msg void OnPaint();
  afx_msg void OnSize(UINT type, int cx, int cy);
  afx_msg void OnVScroll(UINT code, UINT pos, CScrollBar* scroll_bar);
  afx_msg void OnLButtonDown(UINT flags, CPoint point);
  afx_msg void OnMouseMove(UINT flags, CPoint point);
  afx_msg void OnRButtonUp(UINT flags, CPoint point);
  afx_msg void OnLButtonUp(UINT flags, CPoint point);
#if 1 || defined(TILESET_PALETTE_DOCKED)
  afx_msg void OnSizing(UINT nside, LPRECT lpRect);
#endif
  afx_msg void OnInsertTile();
  afx_msg void OnAppendTile();
  afx_msg void OnDeleteTile();
  afx_msg void OnTileProperties();
  afx_msg void OnInsertTiles();
  afx_msg void OnAppendTiles();
  afx_msg void OnDeleteTiles();
  afx_msg void OnInsertTileset();
  afx_msg void OnAppendTileset();
  void SelectTileAtPoint(CPoint point);
  
  void OnZoom(double zoom_factor);
  afx_msg void OnZoom1x();
  afx_msg void OnZoom2x();
  afx_msg void OnZoom4x();
  afx_msg void OnZoom8x();
  afx_msg void OnZoomIn();
  afx_msg void OnZoomOut();
  afx_msg void OnMoveBack();
  afx_msg void OnMoveForward();
  afx_msg void OnMoveOther();
  afx_msg void OnViewTileGrid();
  afx_msg void OnViewTileObstructions();
  afx_msg void OnEditRange();
private:
  ITilesetViewHandler* m_Handler;
  sTileset* m_Tileset;
  int m_TopRow;
  int m_SelectedTile;
  Zoomer m_ZoomFactor;
  CDIBSection* m_BlitTile;
  bool m_MenuShown;
  void OnSwap(int new_index);
  bool m_ShowTileGrid;
  bool m_ShowTileObstructions;
  std::vector<sTile> m_TileObstructions;
  void UpdateObstructionTiles();
  void UpdateObstructionTile(int tile);
private:
  unsigned int* m_TileSelection;
  bool m_MouseDown;
  bool m_UsingMultiTileSelection;
  CPoint m_StartPoint;
  CPoint m_CurPoint;
public:
  int GetTileSelectionLeftX();
  int GetTileSelectionTopY();
  int GetTileSelectionRightX();
  int GetTileSelectionLowerY();
  int GetTileSelectionWidth();
  int GetTileSelectionHeight();
  unsigned int* GetTileSelection();
  DECLARE_MESSAGE_MAP()
};
#endif
