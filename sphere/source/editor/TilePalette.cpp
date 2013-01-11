#include "TilePalette.hpp"
#include "Configuration.hpp"
#include "Keys.hpp"
#include "resource.h"
BEGIN_MESSAGE_MAP(CTilePalette, CPaletteWindow)
  ON_COMMAND(ID_FILE_ZOOM_IN, OnZoomIn)
  ON_COMMAND(ID_FILE_ZOOM_OUT, OnZoomOut)
  ON_WM_SIZE()
#if defined(TILESET_PALETTE_DOCKED)
  ON_WM_SIZING()
#endif
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////
CTilePalette::CTilePalette(CDocumentWindow* owner, ITilesetViewHandler* handler, sTileset* tileset)
: CPaletteWindow(owner, "Tiles",
  Configuration::Get(KEY_TILES_RECT),
  Configuration::Get(KEY_TILES_VISIBLE))
, m_Created(false)
{
// What is this?
//  CRect start_rect = Configuration::Get(KEY_TILES_RECT);
//  if (start_rect.left == -1 && start_rect.top == -1 && start_rect.right == -1 && start_rect.bottom == -1)
//    start_rect = CRect(0, 0, 80, 80);
  m_TilesetView.Create(handler, this, tileset);
  m_Created = true;
  
#if defined(TILESET_PALETTE_DOCKED)
  ModifyStyle(WS_CAPTION, 0);
#endif
  RECT rect;
  GetClientRect(&rect);
  OnSize(0, rect.right, rect.bottom);
}
////////////////////////////////////////////////////////////////////////////////
void
CTilePalette::Destroy()
{
  // save state
  RECT rect;
  GetWindowRect(&rect);
  Configuration::Set(KEY_TILES_RECT, rect);
  // FIXME: IsWindowVisible() always returns FALSE here
  //Configuration::Set(KEY_TILES_VISIBLE, IsWindowVisible() != FALSE);
  // destroy window
  DestroyWindow();
}
////////////////////////////////////////////////////////////////////////////////
void
CTilePalette::TileChanged(int tile)
{
  m_TilesetView.TileChanged(tile);
}
////////////////////////////////////////////////////////////////////////////////
void
CTilePalette::TilesetChanged()
{
  m_TilesetView.TilesetChanged();
}                            
////////////////////////////////////////////////////////////////////////////////
void
CTilePalette::SelectTile(int tile)
{
  m_TilesetView.SetSelectedTile(tile);
}
////////////////////////////////////////////////////////////////////////////////
int
CTilePalette::GetSelectedTile() const
{
  return m_TilesetView.GetSelectedTile();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilePalette::OnSize(UINT type, int cx, int cy)
{
  if (m_Created) {
    m_TilesetView.MoveWindow(0, 0, cx, cy);
  }
}
////////////////////////////////////////////////////////////////////////////////
#if defined(TILESET_PALETTE_DOCKED)
afx_msg void
CTilePalette::OnSizing(UINT side, LPRECT rect)
{
//  WMSZ_BOTTOM
//  WMSZ_BOTTOMLEFT
//  WMSZ_BOTTOMRIGHT
//  WMSZ_LEFT
//  WMSZ_RIGHT
//  WMSZ_TOP
//  WMSZ_TOPLEFT
//  WMSZ_TOPRIGHT
  if (side != WMSZ_LEFT) {
    RECT original;
    GetWindowRect(&original);
    rect->top = original.top;
    rect->bottom = original.bottom;
    rect->left = original.left;
    rect->right = original.right;
  }
  CMiniFrameWnd::OnSizing(side, rect);
}
#endif
////////////////////////////////////////////////////////////////////////////////
int
CTilePalette::GetNumTilesPerRow() const
{
  if (m_Created)
    return m_TilesetView.GetNumTilesPerRow();
  return 0;
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilePalette::OnZoomIn()
{
  m_TilesetView.SendMessage(WM_COMMAND, MAKEWPARAM(ID_FILE_ZOOM_IN, 0), 0);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilePalette::OnZoomOut()
{
  m_TilesetView.SendMessage(WM_COMMAND, MAKEWPARAM(ID_FILE_ZOOM_OUT, 0), 0);
}
////////////////////////////////////////////////////////////////////////////////
