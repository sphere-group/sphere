#pragma warning(disable : 4786)
#include "TilesetView.hpp"
#include "NumberDialog.hpp"
#include "TilePropertiesDialog.hpp"
#include "Editor.hpp"
#include "FileDialogs.hpp"
#include "resource.h"
#include "FontGradientDialog.hpp"
#include "../common/primitives.hpp"
#include "../common/minmax.hpp"
#include "Configuration.hpp"
#include "Keys.hpp"
#include "EditRange.hpp"

// static const int UPDATE_TILEVIEW_TIMER = 200;
static int s_iTilesetViewID = 1000;

BEGIN_MESSAGE_MAP(CTilesetView, CWnd)
  ON_WM_PAINT()
  ON_WM_SIZE()
  ON_WM_VSCROLL()
  ON_WM_LBUTTONDOWN()
  ON_WM_MOUSEMOVE()
  ON_WM_RBUTTONUP()
  ON_WM_LBUTTONUP()
  ON_COMMAND(ID_TILESETVIEW_INSERTTILE,    OnInsertTile)
  ON_COMMAND(ID_TILESETVIEW_APPENDTILE,    OnAppendTile)
  ON_COMMAND(ID_TILESETVIEW_DELETETILE,    OnDeleteTile)
  ON_COMMAND(ID_TILESETVIEW_PROPERTIES,    OnTileProperties)
  ON_COMMAND(ID_TILESETVIEW_INSERTTILES,   OnInsertTiles)
  ON_COMMAND(ID_TILESETVIEW_APPENDTILES,   OnAppendTiles)
  ON_COMMAND(ID_TILESETVIEW_DELETETILES,   OnDeleteTiles)
  ON_COMMAND(ID_TILESETVIEW_INSERTTILESET, OnInsertTileset)
  ON_COMMAND(ID_TILESETVIEW_APPENDTILESET, OnAppendTileset)
  ON_COMMAND(ID_TILESETVIEW_ZOOM_1X, OnZoom1x)
  ON_COMMAND(ID_TILESETVIEW_ZOOM_2X, OnZoom2x)
  ON_COMMAND(ID_TILESETVIEW_ZOOM_4X, OnZoom4x)
  ON_COMMAND(ID_TILESETVIEW_ZOOM_8X, OnZoom8x)
  ON_COMMAND(ID_FILE_ZOOM_IN, OnZoomIn)
  ON_COMMAND(ID_FILE_ZOOM_OUT, OnZoomOut)
  ON_COMMAND(ID_TILESETVIEW_MOVE_BACK,    OnMoveBack)
  ON_COMMAND(ID_TILESETVIEW_MOVE_FORWARD, OnMoveForward)
  ON_COMMAND(ID_TILESETVIEW_MOVE_OTHER,   OnMoveOther)
  ON_COMMAND(ID_TILESETVIEW_VIEWGRID, OnViewTileGrid)
  ON_COMMAND(ID_TILESETVIEW_VIEW_OBSTRUCTIONS, OnViewTileObstructions)
//  ON_COMMAND(ID_TILESETVIEW_ER_ROTATE_CW,             OnEditRangeRotateCW)
//  ON_COMMAND(ID_TILESETVIEW_ER_ROTATE_CCW,            OnEditRangeRotateCCW)
  ON_COMMAND(ID_TILESETVIEW_ER_SLIDE_UP,              OnEditRange)
  ON_COMMAND(ID_TILESETVIEW_ER_SLIDE_RIGHT,           OnEditRange)
  ON_COMMAND(ID_TILESETVIEW_ER_SLIDE_DOWN,            OnEditRange)
  ON_COMMAND(ID_TILESETVIEW_ER_SLIDE_LEFT,            OnEditRange)
  ON_COMMAND(ID_TILESETVIEW_ER_SLIDE_OTHER,           OnEditRange)
  ON_COMMAND(ID_TILESETVIEW_ER_FLIP_HORIZONTALLY,     OnEditRange)
  ON_COMMAND(ID_TILESETVIEW_ER_FLIP_VERTICALLY,       OnEditRange)
//  ON_COMMAND(ID_TILESETVIEW_ER_FILL_RGB,              OnEditRangeFillRGB)
//  ON_COMMAND(ID_TILESETVIEW_ER_FILL_ALPHA,            OnEditRangeFillAlpha)
//  ON_COMMAND(ID_TILESETVIEW_ER_FILL_BOTH,             OnEditRangeFillBoth)
  ON_COMMAND(ID_TILESETVIEW_ER_REPLACE_RGBA,            OnEditRange)
//  ON_COMMAND(ID_TILESETVIEW_ER_REPLACE_RGB,           OnEditRangeReplaceRGB)
//  ON_COMMAND(ID_TILESETVIEW_ER_REPLACE_ALPHA,         OnEditRangeReplaceAlpha)
//  ON_COMMAND(ID_TILESETVIEW_ER_FLT_BLUR,              OnEditRangeFilterBlur)
//  ON_COMMAND(ID_TILESETVIEW_ER_FLT_NOISE,              OnEditRangeFilterNoise)
//  ON_COMMAND(ID_TILESETVIEW_ER_FLT_ADJUST_BRIGHTNESS,  OnEditRangeFilterAdjustBrightness)
//  ON_COMMAND(ID_TILESETVIEW_ER_FLT_ADJUST_GAMMA,       OnEditRangeFilterAdjustGamma)
//  ON_COMMAND(ID_TILESETVIEW_ER_FLT_NEGATIVE_IMAGE_RGB, OnEditRangeFilterNegativeImageRGB)
//  ON_COMMAND(ID_TILESETVIEW_ER_FLT_NEGATIVE_IMAGE_ALPHA, OnEditRangeFilterNegativeImageAlpha)
//  ON_COMMAND(ID_TILESETVIEW_ER_FLT_NEGATIVE_IMAGE_RGBA,  OnEditRangeFilterNegativeImageRGBA)
//  ON_COMMAND(ID_TILESETVIEW_ER_FLT_SOLARIZE,             OnEditRangeFilterSolarize)
//  ON_COMMAND(ID_TILESETVIEW_ER_SETCOLORALPHA,         OnEditRangeSetColorAlpha)
//  ON_COMMAND(ID_TILESETVIEW_ER_SCALEALPHA,            OnEditRangeScaleAlpha)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////
CTilesetView::CTilesetView()
: m_Handler(NULL)
, m_Tileset(NULL)
, m_TopRow(0)
, m_SelectedTile(0)
, m_BlitTile(NULL)
, m_ShowTileObstructions(false)
, m_ShowTileGrid(false)
, m_MenuShown(false)
, m_MouseDown(false)
, m_UsingMultiTileSelection(false)
, m_TileSelection(NULL)
{
  m_StartPoint.x = m_StartPoint.y = 0;
  m_CurPoint.x = m_CurPoint.y = 0;
  int zoom_factor = Configuration::Get(KEY_TILES_ZOOM_FACTOR);
  if (zoom_factor <= 0 || zoom_factor > 8)
    zoom_factor = 1;
  m_ZoomFactor.SetZoomFactor(zoom_factor);
}
////////////////////////////////////////////////////////////////////////////////
CTilesetView::~CTilesetView()
{
  delete m_BlitTile;
  delete m_TileSelection;
  m_TileSelection = NULL;
  m_BlitTile = NULL;
  DestroyWindow();
}
////////////////////////////////////////////////////////////////////////////////
BOOL
CTilesetView::Create(ITilesetViewHandler* handler, CWnd* parent, sTileset* tileset)
{
  m_Handler = handler;
  m_Tileset = tileset;
    
  m_BlitTile = new CDIBSection(
    (int) (m_Tileset->GetTileWidth()  * m_ZoomFactor.GetZoomFactor()),
    (int) (m_Tileset->GetTileHeight() * m_ZoomFactor.GetZoomFactor()),
    32
  );
  BOOL retval = CWnd::Create(
    AfxRegisterWndClass(0, LoadCursor(NULL, IDC_ARROW), NULL, NULL),
    "",
    WS_CHILD | WS_VISIBLE | WS_VSCROLL,
    CRect(0, 0, 0, 0),
    parent,
    s_iTilesetViewID++);
  UpdateScrollBar();
  UpdateObstructionTiles();
  return retval;
}
////////////////////////////////////////////////////////////////////////////////
void
CTilesetView::UpdateObstructionTile(int tile) {
  if (!m_ShowTileObstructions)
    return;
  struct Local {
    struct Color {
      RGBA operator()(int, int) {
        return CreateRGBA(255, 0, 255, 255);
      }
    };
    static inline void CopyRGBA(RGBA& dest, RGBA src) {
      dest = src;
    }
  };
  sTile& src_tile = m_Tileset->GetTile(tile);
  sTile& dest_tile = m_TileObstructions[tile] = src_tile;
  RGBA* dest_pixels = dest_tile.GetPixels();
  // draw the obstruction segments
  Local::Color c;
  RECT clipper = { 0, 0, (dest_tile.GetWidth()  - 1), (dest_tile.GetHeight()  - 1)  };
  const sObstructionMap& obs_map = src_tile.GetObstructionMap();
  for (int i = 0; i < obs_map.GetNumSegments(); ++i) {
    const sObstructionMap::Segment& s = obs_map.GetSegment(i);
    primitives::Line(
      (RGBA*) dest_pixels,
      dest_tile.GetWidth(),
      s.x1, s.y1, s.x2, s.y2,
      c,
      clipper,
      Local::CopyRGBA
    );
  }
}
////////////////////////////////////////////////////////////////////////////////
void
CTilesetView::UpdateObstructionTiles() {
  if (m_ShowTileObstructions) {
    m_TileObstructions.resize(m_Tileset->GetNumTiles());
    for (int i = 0; i < int(m_TileObstructions.size()); ++i)
      UpdateObstructionTile(i);
  }
}
////////////////////////////////////////////////////////////////////////////////
void
CTilesetView::TileChanged(int tile)
{
  RECT client_rect;
  GetClientRect(&client_rect);
  int blit_width  = (int) (m_Tileset->GetTileWidth()  * m_ZoomFactor.GetZoomFactor());
  int blit_height = (int) (m_Tileset->GetTileHeight() * m_ZoomFactor.GetZoomFactor());
  int num_tiles_x = client_rect.right / blit_width;
  if (num_tiles_x == 0)
    return;
  int col = tile % num_tiles_x;
  int row = tile / num_tiles_x;
  
  int x = col * blit_width;
  int y = (row - m_TopRow) * blit_height;
  UpdateObstructionTile(tile);
  RECT rect;
  SetRect(&rect, x, y, x + blit_width, y + blit_height);
  InvalidateRect(&rect);
}
////////////////////////////////////////////////////////////////////////////////
void
CTilesetView::TilesetChanged()
{
  // do a verification on the selected tile
  if (m_SelectedTile < 0)
    m_SelectedTile = 0;
  if (m_SelectedTile > m_Tileset->GetNumTiles() - 1)
    m_SelectedTile = m_Tileset->GetNumTiles() - 1;

  if (m_UsingMultiTileSelection) {
    m_UsingMultiTileSelection = false;
    m_Handler->TV_TilesetSelectionChanged(GetTileSelectionWidth(), GetTileSelectionHeight(), GetTileSelection());
  }
  // resize blit tile if we must
  delete m_BlitTile;
  m_BlitTile = new CDIBSection(
    (int) (m_Tileset->GetTileWidth()  * m_ZoomFactor.GetZoomFactor()),
    (int) (m_Tileset->GetTileHeight() * m_ZoomFactor.GetZoomFactor()),
    32
  );
  UpdateObstructionTiles();
  Invalidate();
  UpdateScrollBar();
}
////////////////////////////////////////////////////////////////////////////////
void
CTilesetView::SetSelectedTile(int tile)
{
  if (m_UsingMultiTileSelection) {
    m_UsingMultiTileSelection = false;
    m_Handler->TV_TilesetSelectionChanged(GetTileSelectionWidth(), GetTileSelectionHeight(), GetTileSelection());
  }
  m_SelectedTile = tile;
  Invalidate();
  if (!m_BlitTile || m_BlitTile->GetPixels() == NULL)
    return;
  //scroll into view
  RECT client_rect;
  GetClientRect(&client_rect);
  int blit_width  = m_BlitTile->GetWidth();  
  int num_tiles_x = client_rect.right / blit_width;
  if (num_tiles_x <= 0)
    return;
  int row = tile / num_tiles_x;
  int maxRow = GetNumRows() - GetPageSize();
  m_TopRow = row > maxRow ? maxRow : row;
  UpdateScrollBar();
}
////////////////////////////////////////////////////////////////////////////////
int
CTilesetView::GetSelectedTile() const
{
  return m_SelectedTile;
}
////////////////////////////////////////////////////////////////////////////////
void
CTilesetView::UpdateScrollBar()
{
  int num_rows  = GetNumRows();
  int page_size = GetPageSize();
  // validate the values
  if (m_TopRow > num_rows - page_size)
    m_TopRow = num_rows - page_size;
  if (m_TopRow < 0)
    m_TopRow = 0;
  SCROLLINFO si;
  si.cbSize = sizeof(si);
  si.fMask  = SIF_ALL;
  si.nMin   = 0;
  if (page_size - num_rows)
  {
    si.nMax   = num_rows - 1;
    si.nPage  = page_size;
    si.nPos   = m_TopRow;
  }
  else
  {
    si.nMax   = 0xFFFF;
    si.nPage  = 0xFFFE;
    si.nPos   = 0;
  }
  SetScrollInfo(SB_VERT, &si);
}
////////////////////////////////////////////////////////////////////////////////
int
CTilesetView::GetPageSize()
{
  if (!m_BlitTile || m_BlitTile->GetPixels() == NULL)
    return -1;
  RECT ClientRect;
  GetClientRect(&ClientRect);
  return ClientRect.bottom / m_BlitTile->GetHeight();
}
////////////////////////////////////////////////////////////////////////////////
int
CTilesetView::GetNumRows()
{
  if (!m_BlitTile || m_BlitTile->GetPixels() == NULL)
    return -1;
  RECT client_rect;
  GetClientRect(&client_rect);
  int num_tiles_x = client_rect.right / m_BlitTile->GetWidth();
  if (num_tiles_x == 0)
    return -1;
  else
    return (m_Tileset->GetNumTiles() + num_tiles_x - 1) / num_tiles_x;
}
////////////////////////////////////////////////////////////////////////////////
int
CTilesetView::GetNumTilesPerRow() const
{
  if (!m_BlitTile || m_BlitTile->GetPixels() == NULL)
    return 0;
  RECT client_rect;
  GetClientRect(&client_rect);
  return client_rect.right / m_BlitTile->GetWidth();
}
////////////////////////////////////////////////////////////////////////////////
int
CTilesetView::GetTileSelectionLeftX()
{
  if (!m_UsingMultiTileSelection) return 0;
  if (!m_BlitTile || m_BlitTile->GetPixels() == NULL)
    return 0;
  CPoint start = m_StartPoint;
  CPoint end = m_CurPoint;
  RECT client_rect;
  GetClientRect(&client_rect);
  const int blit_width  = m_BlitTile->GetWidth();
  const int blit_height = m_BlitTile->GetHeight();
  start.x = start.x / blit_width;
  start.y = start.y / blit_height;
  end.x = end.x / blit_width;
  end.y = end.y / blit_height;
  int tileselection_left_x  = std::min(start.x, end.x);
  int tileselection_top_y   = std::min(start.y, end.y);
  int tileselection_right_x = std::max(start.x, end.x);
  int tileselection_lower_y = std::max(start.y, end.y);
  if (tileselection_left_x < 0)
    tileselection_left_x = 0;
  if (tileselection_top_y < 0)
    tileselection_top_y = 0;
  while (tileselection_lower_y > 0 && (tileselection_lower_y * (client_rect.right / blit_width)) + tileselection_right_x >= m_Tileset->GetNumTiles()) {
    tileselection_lower_y -= 1;
  }
  while (tileselection_right_x > 0 &&  (tileselection_lower_y * (client_rect.right / blit_width)) + tileselection_right_x >= m_Tileset->GetNumTiles()) {
    tileselection_right_x -= 1;
  }
  return tileselection_left_x;
}
////////////////////////////////////////////////////////////////////////////////
int
CTilesetView::GetTileSelectionRightX()
{
  if (!m_UsingMultiTileSelection) return 0;
  if (!m_BlitTile || m_BlitTile->GetPixels() == NULL)
    return 0;
 
  CPoint start = m_StartPoint;
  CPoint end = m_CurPoint;
  RECT client_rect;
  GetClientRect(&client_rect);
  const int blit_width  = m_BlitTile->GetWidth();
  const int blit_height = m_BlitTile->GetHeight();
  start.x = start.x / blit_width;
  start.y = start.y / blit_height;
  end.x = end.x / blit_width;
  end.y = end.y / blit_height;
  int tileselection_left_x  = std::min(start.x, end.x);
  int tileselection_top_y   = std::min(start.y, end.y);
  int tileselection_right_x = std::max(start.x, end.x);
  int tileselection_lower_y = std::max(start.y, end.y);
  if (tileselection_left_x < 0)
    tileselection_left_x = 0;
  if (tileselection_top_y < 0)
    tileselection_top_y = 0;

  while (tileselection_lower_y > 0 && (tileselection_lower_y * (client_rect.right / blit_width)) + tileselection_right_x >= m_Tileset->GetNumTiles())
    tileselection_lower_y -= 1;
  while (tileselection_right_x > 0 &&  (tileselection_lower_y * (client_rect.right / blit_width)) + tileselection_right_x >= m_Tileset->GetNumTiles())
    tileselection_right_x -= 1;

  return tileselection_right_x;
}
////////////////////////////////////////////////////////////////////////////////
int
CTilesetView::GetTileSelectionTopY() {
  if (!m_UsingMultiTileSelection) return 0;
  if (!m_BlitTile || m_BlitTile->GetPixels() == NULL)
    return 0;
  CPoint start = m_StartPoint;
  CPoint end = m_CurPoint;
  RECT client_rect;
  GetClientRect(&client_rect);
  const int blit_width  = m_BlitTile->GetWidth();
  const int blit_height = m_BlitTile->GetHeight();
  start.x = start.x / blit_width;
  start.y = start.y / blit_height;
  end.x = end.x / blit_width;
  end.y = end.y / blit_height;
  int tileselection_left_x  = std::min(start.x, end.x);
  int tileselection_top_y   = std::min(start.y, end.y);
  int tileselection_right_x = std::max(start.x, end.x);
  int tileselection_lower_y = std::max(start.y, end.y);
  if (tileselection_left_x < 0)
    tileselection_left_x = 0;
  if (tileselection_top_y < 0)
    tileselection_top_y = 0;

  while (tileselection_lower_y > 0 && (tileselection_lower_y * (client_rect.right / blit_width)) + tileselection_right_x >= m_Tileset->GetNumTiles())
    tileselection_lower_y -= 1;
  while (tileselection_right_x > 0 &&  (tileselection_lower_y * (client_rect.right / blit_width)) + tileselection_right_x >= m_Tileset->GetNumTiles())
    tileselection_right_x -= 1;

  return tileselection_top_y;
}
////////////////////////////////////////////////////////////////////////////////
int
CTilesetView::GetTileSelectionLowerY()
{
  if (!m_UsingMultiTileSelection) return 0;
  if (!m_BlitTile || m_BlitTile->GetPixels() == NULL)
    return 0;
  CPoint start = m_StartPoint;
  CPoint end = m_CurPoint;
  RECT client_rect;
  GetClientRect(&client_rect);
  const int blit_width  = m_BlitTile->GetWidth();
  const int blit_height = m_BlitTile->GetHeight();
  start.x = start.x / blit_width;
  start.y = start.y / blit_height;
  end.x = end.x / blit_width;
  end.y = end.y / blit_height;
  int tileselection_left_x  = std::min(start.x, end.x);
  int tileselection_top_y   = std::min(start.y, end.y);
  int tileselection_right_x = std::max(start.x, end.x);
  int tileselection_lower_y = std::max(start.y, end.y);
  if (tileselection_left_x < 0)
    tileselection_left_x = 0;
  if (tileselection_top_y < 0)
    tileselection_top_y = 0;

  while (tileselection_lower_y > 0 && (tileselection_lower_y * (client_rect.right / blit_width)) + tileselection_right_x >= m_Tileset->GetNumTiles())
    tileselection_lower_y -= 1;
  while (tileselection_right_x > 0 &&  (tileselection_lower_y * (client_rect.right / blit_width)) + tileselection_right_x >= m_Tileset->GetNumTiles())
    tileselection_right_x -= 1;
  
  return tileselection_lower_y;
}
////////////////////////////////////////////////////////////////////////////////
int
CTilesetView::GetTileSelectionWidth() {
  if (!m_UsingMultiTileSelection)
    return 0;
  return GetTileSelectionRightX() - GetTileSelectionLeftX() + 1;
}
////////////////////////////////////////////////////////////////////////////////
int
CTilesetView::GetTileSelectionHeight() {
  if (!m_UsingMultiTileSelection)
    return 0;
  return GetTileSelectionLowerY() - GetTileSelectionTopY() + 1;
}
////////////////////////////////////////////////////////////////////////////////
unsigned int*
CTilesetView::GetTileSelection()
{
  if (m_TileSelection)
    delete m_TileSelection;
  m_TileSelection = NULL;
  if (!m_UsingMultiTileSelection)
    return NULL;
  if (!m_BlitTile || m_BlitTile->GetPixels() == NULL)
    return NULL;
  RECT client_rect;
  GetClientRect(&client_rect);
  int blit_width  = m_BlitTile->GetWidth();
  int tileselection_left_x  = GetTileSelectionLeftX();
  int tileselection_top_y   = GetTileSelectionTopY();
  int width  = GetTileSelectionWidth();
  int height = GetTileSelectionHeight();
  if (GetTileSelectionWidth() * GetTileSelectionHeight() <= 1)
    return NULL;
  m_TileSelection = new unsigned int[width * height];
  if (m_TileSelection) {
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        int tile = ((tileselection_top_y + y)  * (client_rect.right / blit_width)) + (tileselection_left_x + x);
        m_TileSelection[y * width + x] = tile;
      }
    }
  }
  return m_TileSelection;
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilesetView::OnPaint()
{
  CPaintDC dc(this);
  
  RECT client_rect;
  GetClientRect(&client_rect);
  if (!m_BlitTile || m_BlitTile->GetPixels() == NULL) {
    dc.FillRect(&client_rect, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
    return;
  }
  int blit_width  = m_BlitTile->GetWidth();
  int blit_height = m_BlitTile->GetHeight();
  bool multi_tiles_selected = false;
  int tileselection_left_x  = GetTileSelectionLeftX();
  int tileselection_right_x = GetTileSelectionRightX();
  int tileselection_top_y   = GetTileSelectionTopY();
  int tileselection_lower_y = GetTileSelectionLowerY();
  if (m_UsingMultiTileSelection && (GetTileSelectionWidth() * GetTileSelectionHeight() > 1))
    multi_tiles_selected = true;
  
  bool show_grid = m_ShowTileGrid;
  for (int iy = 0; iy < client_rect.bottom / blit_height + 1; iy++)
    for (int ix = 0; ix < client_rect.right / blit_width + 1; ix++)
    {
      RECT Rect = {
        ix       * blit_width,
        iy       * blit_height,
        (ix + 1) * blit_width,
        (iy + 1) * blit_height,
      };
      if (dc.RectVisible(&Rect) == FALSE)
        continue;
      
      int num_tiles_x = client_rect.right / blit_width;
      int it = (iy + m_TopRow) * (client_rect.right / blit_width) + ix;
      if (ix < num_tiles_x && it < m_Tileset->GetNumTiles())
      {
        int py;

        // draw the tile
        // fill the DIB section
        BGRA* pixels = (BGRA*)m_BlitTile->GetPixels();
        
        // make a checkerboard
        for (py = 0; py < blit_height; py++)
          for (int px = 0; px < blit_width; px++)
          {
            pixels[py * blit_width + px] = 
              ((px / 8 + py / 8) % 2 ?
                CreateBGRA(255, 255, 255, 255) :
                CreateBGRA(255, 192, 192, 255)
              );
          }        
        // draw the tile into it
        const RGBA* tilepixels = m_Tileset->GetTile(it).GetPixels();
        if (m_ShowTileObstructions && it >= 0 && it < int(m_TileObstructions.size()))
          tilepixels = m_TileObstructions[it].GetPixels();
        
        for (py = 0; py < blit_height; py++)
          for (int px = 0; px < blit_width; px++)
          {
            int ty = (int) (py / m_ZoomFactor.GetZoomFactor());
            int tx = (int) (px / m_ZoomFactor.GetZoomFactor());
            int t = ty * m_Tileset->GetTileWidth() + tx;
            
            int d = py * blit_width + px;
            int alpha = tilepixels[t].alpha;
            pixels[d].red   = (tilepixels[t].red   * alpha + pixels[d].red   * (255 - alpha)) / 256;
            pixels[d].green = (tilepixels[t].green * alpha + pixels[d].green * (255 - alpha)) / 256;
            pixels[d].blue  = (tilepixels[t].blue  * alpha + pixels[d].blue  * (255 - alpha)) / 256;
          }
        
        // blit the tile
        CDC* tile = CDC::FromHandle(m_BlitTile->GetDC());
        dc.BitBlt(Rect.left, Rect.top, Rect.right - Rect.left, Rect.bottom - Rect.top, tile, 0, 0, SRCCOPY);
        // if the tile is selected, draw a pink rectangle around it
        bool is_selected = false;
        if (multi_tiles_selected) {
          if (ix >= tileselection_left_x
              && ix <= tileselection_right_x
              && (iy + m_TopRow) >= tileselection_top_y
              && (iy + m_TopRow) <= tileselection_lower_y) {
            is_selected = true;
          }
        } else if (it == m_SelectedTile) {
          is_selected = true;
        }
        if (is_selected) {
          HBRUSH newbrush = (HBRUSH)GetStockObject(NULL_BRUSH);
          CBrush* oldbrush = dc.SelectObject(CBrush::FromHandle(newbrush));
          HPEN newpen = (HPEN)CreatePen(PS_SOLID, 1, RGB(0xFF, 0x00, 0xFF));
          CPen* oldpen = dc.SelectObject(CPen::FromHandle(newpen));
          dc.Rectangle(&Rect);
          dc.SelectObject(oldbrush);
          DeleteObject(newbrush);
          dc.SelectObject(oldpen);
          DeleteObject(newpen);
        } else if (show_grid) {
          HBRUSH newbrush = (HBRUSH)GetStockObject(NULL_BRUSH);
          CBrush* oldbrush = dc.SelectObject(CBrush::FromHandle(newbrush));
          HPEN newpen = (HPEN)CreatePen(PS_SOLID, 1, RGB(0x00, 0xFF, 0xFF));
          CPen* oldpen = dc.SelectObject(CPen::FromHandle(newpen));
          dc.Rectangle(&Rect);
          dc.SelectObject(oldbrush);
          DeleteObject(newbrush);
          dc.SelectObject(oldpen);
          DeleteObject(newpen);
        }
      }
      else
      {
        // draw black rectangle
        dc.FillRect(&Rect, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
      }      
    }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilesetView::OnSize(UINT type, int cx, int cy)
{
  if (cx > 0)
  {
    // if the current top row is greater than the total number of rows minus the page size
    if (m_TopRow > GetNumRows() - GetPageSize())
    {
      // move the top row up
      m_TopRow = GetNumRows() - GetPageSize();
      if (m_TopRow < 0)
        m_TopRow = 0;
      Invalidate();
    }
  }
  static int last_cx = -1;
  if (last_cx != cx) {
    m_UsingMultiTileSelection = false;
    m_Handler->TV_TilesetSelectionChanged(GetTileSelectionWidth(), GetTileSelectionHeight(), GetTileSelection());
  }
  // reflect the changes
  UpdateScrollBar();
  Invalidate();
  CWnd::OnSize(type, cx, cy);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilesetView::OnVScroll(UINT code, UINT pos, CScrollBar* scroll_bar)
{
  switch (code)
  {
    case SB_LINEDOWN:   m_TopRow++;                break;
    case SB_LINEUP:     m_TopRow--;                break;
    case SB_PAGEDOWN:   m_TopRow += GetPageSize(); break;
    case SB_PAGEUP:     m_TopRow -= GetPageSize(); break;
    case SB_THUMBTRACK: m_TopRow = (int)pos;       break;
  }
  UpdateScrollBar();
  Invalidate();
}
////////////////////////////////////////////////////////////////////////////////
void
CTilesetView::SelectTileAtPoint(CPoint point)
{
  if (!m_BlitTile || m_BlitTile->GetPixels() == NULL)
    return;
  if (m_MenuShown)
    return;
  RECT client_rect;
  GetClientRect(&client_rect);
  int num_tiles_x = client_rect.right / m_BlitTile->GetWidth();
  int col = point.x / m_BlitTile->GetWidth();
  int row = point.y / m_BlitTile->GetHeight();
  // don't let user select tile off the right edge (and go to the next row)
  if (col >= num_tiles_x)
    return;
  
  int tile = (m_TopRow + row) * num_tiles_x + col;
  if (tile >= 0 && tile < m_Tileset->GetNumTiles()) {
    m_SelectedTile = tile;
  }
  Invalidate();

  // the selected tile changed, so tell the parent window
  m_Handler->TV_SelectedTileChanged(m_SelectedTile);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilesetView::OnLButtonDown(UINT flags, CPoint point)
{
  if (!m_BlitTile || m_BlitTile->GetPixels() == NULL)
    return;
  if (m_MenuShown)
    return;
  if (m_UsingMultiTileSelection || !(GetTileSelectionWidth() * GetTileSelectionHeight() > 1))
    m_UsingMultiTileSelection = false;
  
  SelectTileAtPoint(point);
  if (GetCapture() == NULL) {
    m_StartPoint = point; m_StartPoint.y += (m_TopRow * m_BlitTile->GetHeight());
    m_CurPoint   = point; m_CurPoint.y   += (m_TopRow * m_BlitTile->GetHeight());
    m_MouseDown = true;
    m_UsingMultiTileSelection = true;
    SetCapture();
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilesetView::OnLButtonUp(UINT flags, CPoint point)
{
  if (!m_MouseDown)
    return;
  m_CurPoint = point; m_CurPoint.y += (m_TopRow * m_BlitTile->GetHeight());
  m_MouseDown = false;
  ReleaseCapture();
  if (m_UsingMultiTileSelection && !(GetTileSelectionWidth() * GetTileSelectionHeight() > 1))
    m_UsingMultiTileSelection = false;
  
  GetTileSelection();
  m_Handler->TV_TilesetSelectionChanged(GetTileSelectionWidth(), GetTileSelectionHeight(), GetTileSelection());
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilesetView::OnMouseMove(UINT flags, CPoint point)
{
  if (!m_BlitTile || m_BlitTile->GetPixels() == NULL)
    return;
  if (m_UsingMultiTileSelection) {
    if (m_MouseDown) {
      m_CurPoint = point; m_CurPoint.y += (m_TopRow * m_BlitTile->GetHeight());
      Invalidate();
    }
    if (GetTileSelectionWidth() > 1 || GetTileSelectionHeight() > 1) {
      int tileselection_left_x  = GetTileSelectionLeftX();
      int tileselection_right_x = GetTileSelectionRightX();
      int tileselection_top_y   = GetTileSelectionTopY();
      int tileselection_lower_y = GetTileSelectionLowerY();
      
      CString tileinfo;
      tileinfo.Format("Tiles (%d,%d)->(%d,%d)", tileselection_left_x, tileselection_top_y,
                                      tileselection_right_x, tileselection_lower_y);
      GetStatusBar()->SetWindowText(tileinfo);
      return;
    }
  }
  {
    RECT client_rect;
    GetClientRect(&client_rect);
    int num_tiles_x = client_rect.right / m_BlitTile->GetWidth();
    int x = (int) (point.x / (m_Tileset->GetTileWidth()  * m_ZoomFactor.GetZoomFactor()));
    int y = (int) (point.y / (m_Tileset->GetTileHeight() * m_ZoomFactor.GetZoomFactor()));
    int tile = (m_TopRow + y) * num_tiles_x + x;
    if (tile >= 0 && tile < m_Tileset->GetNumTiles())
    {
      CString tilenum;
      tilenum.Format("Tile (%i/%i)", tile, m_Tileset->GetNumTiles());
      GetStatusBar()->SetWindowText(tilenum);
    }
    else
      GetStatusBar()->SetWindowText("");
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilesetView::OnRButtonUp(UINT flags, CPoint point)
{
  if (m_MenuShown)
    return;
  if (m_UsingMultiTileSelection && !(GetTileSelectionWidth() * GetTileSelectionHeight() > 1))
    m_UsingMultiTileSelection = false;
  
  // select the tile
  SelectTileAtPoint(point);
  // show pop-up menu
  ClientToScreen(&point);
  
  HMENU menu_ = LoadMenu(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_TILESETVIEW));
  HMENU menu = GetSubMenu(menu_, 0);
  if (m_ZoomFactor.GetZoomFactor() == 1) {
    CheckMenuItem(menu, ID_TILESETVIEW_ZOOM_1X, MF_BYCOMMAND | MF_CHECKED);
  } else if (m_ZoomFactor.GetZoomFactor() == 2) {
    CheckMenuItem(menu, ID_TILESETVIEW_ZOOM_2X, MF_BYCOMMAND | MF_CHECKED);
  } else if (m_ZoomFactor.GetZoomFactor() == 4) {
    CheckMenuItem(menu, ID_TILESETVIEW_ZOOM_4X, MF_BYCOMMAND | MF_CHECKED);
  } else if (m_ZoomFactor.GetZoomFactor() == 8) {
    CheckMenuItem(menu, ID_TILESETVIEW_ZOOM_8X, MF_BYCOMMAND | MF_CHECKED);
  }
  if ( !(m_Tileset->GetNumTiles() > 1) || m_SelectedTile == 0) {
    EnableMenuItem(menu, ID_TILESETVIEW_MOVE_BACK, MF_BYCOMMAND | MF_GRAYED);
  }
  if ( !(m_Tileset->GetNumTiles() > 1) || m_SelectedTile == m_Tileset->GetNumTiles() - 1) {
    EnableMenuItem(menu, ID_TILESETVIEW_MOVE_FORWARD, MF_BYCOMMAND | MF_GRAYED);
  }
  if ( !(m_Tileset->GetNumTiles() > 1) ) {
    EnableMenuItem(menu, ID_TILESETVIEW_MOVE_OTHER, MF_BYCOMMAND | MF_GRAYED);
  }
  if (m_ShowTileObstructions) {
    CheckMenuItem(menu, ID_TILESETVIEW_VIEW_OBSTRUCTIONS, MF_BYCOMMAND | MF_CHECKED);
  }
  // these menu items don't really make sense when you have a region of tiles selected
  if (GetTileSelectionWidth() * GetTileSelectionHeight() > 1)
  {
    EnableMenuItem(menu, ID_TILESETVIEW_INSERTTILE, MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_APPENDTILE, MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_DELETETILE, MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_PROPERTIES, MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_INSERTTILES,   MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_APPENDTILES,   MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_DELETETILES,   MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_INSERTTILESET, MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_APPENDTILESET, MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_MOVE_BACK,    MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_MOVE_FORWARD, MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_ER_ROTATE_CW,  MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_ER_ROTATE_CCW, MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_ER_SLIDE_UP,    MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_ER_SLIDE_RIGHT, MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_ER_SLIDE_DOWN,  MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_ER_SLIDE_LEFT,  MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_ER_SLIDE_OTHER, MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_ER_FLIP_HORIZONTALLY, MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_ER_FLIP_VERTICALLY,   MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_ER_FILL_RGB,   MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_ER_FILL_ALPHA, MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_ER_FILL_BOTH,  MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_ER_REPLACE_RGBA,  MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_ER_REPLACE_RGB,   MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_ER_REPLACE_ALPHA, MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_ER_FLT_BLUR,      MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_ER_FLT_NOISE,     MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_ER_FLT_ADJUST_BRIGHTNESS, MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_ER_FLT_ADJUST_GAMMA,      MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_ER_FLT_NEGATIVE_IMAGE_RGB,   MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_ER_FLT_NEGATIVE_IMAGE_ALPHA, MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_ER_FLT_NEGATIVE_IMAGE_RGBA,  MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_ER_FLT_SOLARIZE,  MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_ER_SETCOLORALPHA, MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_TILESETVIEW_ER_SCALEALPHA,    MF_BYCOMMAND | MF_GRAYED);
  }
  m_MenuShown = true;
  TrackPopupMenu(menu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, point.x, point.y, 0, m_hWnd, NULL);
  m_MenuShown = false;
  DestroyMenu(menu_);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilesetView::OnInsertTile()
{
  // adjust map tile indices around
  m_Handler->TV_InsertedTiles(m_SelectedTile, 1);
  m_Tileset->InsertTiles(m_SelectedTile, 1);
  m_Handler->TV_TilesetChanged();
  UpdateScrollBar();
  UpdateObstructionTiles();
  Invalidate();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilesetView::OnAppendTile()
{
  m_Tileset->AppendTiles(1);
  m_Handler->TV_TilesetChanged();
  UpdateScrollBar();
  UpdateObstructionTiles();
  Invalidate();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilesetView::OnDeleteTile()
{
  // don't allow the tileset to be less than 1 tile
  if (m_Tileset->GetNumTiles() > 1)
  {
    int result = MessageBox("Delete selected tile?", "Delete Tile", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);
    if (result == IDNO)
      return;
    
    // adjust map tile indices around
    m_Handler->TV_DeletedTiles(m_SelectedTile, 1);
 
    m_Tileset->DeleteTiles(m_SelectedTile, 1);
    // make sure selected tile is still valid
    if (m_SelectedTile >= m_Tileset->GetNumTiles()) {
      m_SelectedTile = m_Tileset->GetNumTiles() - 1;
      m_Handler->TV_SelectedTileChanged(m_SelectedTile);
    }
    m_Handler->TV_TilesetChanged();
    UpdateObstructionTiles();
    UpdateScrollBar();
    Invalidate();
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilesetView::OnTileProperties()
{
  CTilePropertiesDialog dialog(m_Tileset, m_SelectedTile);
  if (dialog.DoModal() == IDOK)
  {
    m_Handler->TV_TilesetChanged();
    UpdateObstructionTiles();
    Invalidate();
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilesetView::OnInsertTiles()
{
  char string[1024];
  sprintf (string, "Number of Tiles (1 - %d)", 255);
  CNumberDialog dialog("Insert Tiles", string, 1, 1, 255);
  if (dialog.DoModal() == IDOK)
  {
    // adjust map tile indices around
    m_Handler->TV_InsertedTiles(m_SelectedTile, dialog.GetValue());
    m_Tileset->InsertTiles(m_SelectedTile, dialog.GetValue());
    m_Handler->TV_TilesetChanged();
    UpdateScrollBar();
    UpdateObstructionTiles();
    Invalidate();
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilesetView::OnAppendTiles()
{
  char string[1024];
  sprintf (string, "Number of Tiles (1 - %d)", 255);
  CNumberDialog dialog("Append Tiles", string, 1, 1, 255);
  if (dialog.DoModal() == IDOK)
  {
    m_Tileset->AppendTiles(dialog.GetValue());
    m_Handler->TV_TilesetChanged();
    UpdateScrollBar();
    UpdateObstructionTiles();
    Invalidate();
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilesetView::OnDeleteTiles()
{
  char string[1024];
  sprintf (string, "Number of Tiles (1 - %d)", m_Tileset->GetNumTiles() - m_SelectedTile - (m_SelectedTile > 0 ? 0 : 1));
  CNumberDialog dialog("Delete Tiles", string, 1, 1, m_Tileset->GetNumTiles() - m_SelectedTile - (m_SelectedTile > 0 ? 0 : 1));
  if (dialog.DoModal() == IDOK)
  {
    // adjust map tile indices around
    m_Handler->TV_DeletedTiles(m_SelectedTile, dialog.GetValue());
    m_Tileset->DeleteTiles(m_SelectedTile, dialog.GetValue());
    // make sure selected tile is still valid
    if (m_SelectedTile >= m_Tileset->GetNumTiles()) {
      m_SelectedTile = m_Tileset->GetNumTiles() - 1;
      m_Handler->TV_SelectedTileChanged(m_SelectedTile);
    }
    m_Handler->TV_SelectedTileChanged(m_SelectedTile);
    m_Handler->TV_TilesetChanged();
    UpdateScrollBar();
    UpdateObstructionTiles();
    Invalidate();
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilesetView::OnInsertTileset()
{
  CTilesetFileDialog dialog(FDM_OPEN);
  if (dialog.DoModal() == IDOK) {
    // load the tileset
    sTileset tileset;
    if (!tileset.Load(dialog.GetPathName())) {
      MessageBox("Could not load tileset", "Insert Tileset");
      return;
    }
    // make sure it's compatible with the new one
    if (m_Tileset->GetTileWidth() != tileset.GetTileWidth()
        || m_Tileset->GetTileHeight() != tileset.GetTileHeight()) {
      MessageBox("Tilesets do not have the same tile size", "Insert Tileset");
      return;
    }
    // stick it in
    m_Tileset->InsertTiles(m_SelectedTile, tileset.GetNumTiles());
    for (int i = 0; i < tileset.GetNumTiles(); i++) {
      // next tile is relative to m_SelectedTile
      if (tileset.GetTile(i).IsAnimated()) {
       // tileset.GetTile(i).SetNextTile(tileset.GetTile(i).GetNextTile() + m_SelectedTile);
      }
      
      m_Tileset->GetTile(m_SelectedTile + i) = tileset.GetTile(i);
    }
    // adjust map tile indices around
    m_Handler->TV_InsertedTiles(m_SelectedTile, tileset.GetNumTiles());
    // notify window
    m_Handler->TV_TilesetChanged();
    UpdateObstructionTiles();
    UpdateScrollBar();
    Invalidate();
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilesetView::OnAppendTileset()
{
  CTilesetFileDialog dialog(FDM_OPEN);
  if (dialog.DoModal() == IDOK) {
    // load the tileset
    sTileset tileset;
    if (!tileset.Load(dialog.GetPathName())) {
      MessageBox("Could not load tileset", "Insert Tileset");
      return;
    }
    // make sure it's compatible with the new one
    if (m_Tileset->GetTileWidth() != tileset.GetTileWidth()
        || m_Tileset->GetTileHeight() != tileset.GetTileHeight()) {
      MessageBox("Tilesets do not have the same tile size", "Insert Tileset");
      return;
    }
    // stick it in
    int old_size = m_Tileset->GetNumTiles();
    m_Tileset->AppendTiles(tileset.GetNumTiles());
    for (int i = 0; i < tileset.GetNumTiles(); i++) {
      // next tile is relative to the end of the tileset
      if (tileset.GetTile(i).IsAnimated())
        tileset.GetTile(i).SetNextTile(tileset.GetTile(i).GetNextTile() + old_size);
      m_Tileset->GetTile(old_size + i) = tileset.GetTile(i);
    }
    // notify window
    m_Handler->TV_TilesetChanged();
    UpdateScrollBar();
    UpdateObstructionTiles();
    Invalidate();
  }
}
////////////////////////////////////////////////////////////////////////////////
void
CTilesetView::OnSwap(int new_index)
{
  int current = m_SelectedTile;
  
  // convenience
  int one = new_index;
  int two = current;
  std::vector<int> list_a; list_a.push_back(one); list_a.push_back(two);
  std::vector<int> list_b; list_b.push_back(two); list_b.push_back(one);
  // swap the tile indexes
  m_Handler->TV_SwapTiles(list_a, list_b);
  // swap the tiles
  std::swap(
    m_Tileset->GetTile(one),
    m_Tileset->GetTile(two)
  );
  m_SelectedTile = new_index;
  m_Handler->TV_SelectedTileChanged(m_SelectedTile);
  m_Handler->TV_TilesetChanged();
  UpdateObstructionTiles();
  Invalidate();
}
///////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilesetView::OnMoveBack()
{
  if (m_SelectedTile > 0)
    OnSwap(m_SelectedTile - 1);
}
///////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilesetView::OnMoveForward()
{
  if (m_SelectedTile < m_Tileset->GetNumTiles())
    OnSwap(m_SelectedTile + 1);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilesetView::OnMoveOther()
{
  int min_value = -m_SelectedTile;
  int max_value = (m_Tileset->GetNumTiles() - 1) - m_SelectedTile;
  char title[1024] = {0};
  sprintf (title, "Move Tiles [%d - %d]", min_value, max_value);
  CNumberDialog dx(title, "Value", 0, min_value, max_value); 
  if (dx.DoModal() == IDOK) {
    int value = dx.GetValue();
    if (value != 0) {
      const int current   = m_SelectedTile;
      const int new_index = m_SelectedTile + value;
      std::vector<int> list_a;
      std::vector<int> list_b;
      if (value < 0)
      {
        int i;

        // swap the tiles
        for (i = 0; i < abs(value); i++)
        {
          int one = current - i;
          int two = current - i - 1;
          {
            std::swap(
              m_Tileset->GetTile(one),
              m_Tileset->GetTile(two)
            );
          }
        }
        // swap the indices...
        for (i = 0; i <= abs(value); i++)
        {
          int one = new_index + i;
          int two = new_index + i + 1;
          if (one == current) {
            list_a.push_back(one);
            list_b.push_back(new_index);           
          }
          else
          {
            list_a.push_back(one);
            list_b.push_back(two);
          }
        }
      }
      else
      if (value > 0)
      {
        int i;

        // swap the tiles
        for (i = 0; i < abs(value); i++)
        {
          int one = current + i;
          int two = current + i + 1;
          {
            std::swap(
              m_Tileset->GetTile(one),
              m_Tileset->GetTile(two)
            );
          }
        }
        // swap the indices...
        for (i = 0; i <= abs(value); i++)
        {
          int one = new_index - i;
          int two = new_index - i - 1;
          if (one == current) {
            list_a.push_back(one);
            list_b.push_back(new_index);           
          }
          else
          {
            list_a.push_back(one);
            list_b.push_back(two);
          }
        }
      }
      // swap the tile indexes
      m_Handler->TV_SwapTiles(list_a, list_b);
      m_SelectedTile = new_index;
      m_Handler->TV_SelectedTileChanged(m_SelectedTile);
      m_Handler->TV_TilesetChanged();
      UpdateObstructionTiles();
      Invalidate();
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilesetView::OnViewTileGrid()
{
  m_ShowTileGrid = !m_ShowTileGrid;
  Invalidate();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilesetView::OnViewTileObstructions()
{
  m_ShowTileObstructions = !m_ShowTileObstructions;
  UpdateObstructionTiles();
  Invalidate();
}
////////////////////////////////////////////////////////////////////////////////
void
CTilesetView::OnZoom(double zoom_factor) {
  if (m_UsingMultiTileSelection) {
    m_UsingMultiTileSelection = false;
    GetTileSelection();
    m_Handler->TV_TilesetSelectionChanged(GetTileSelectionWidth(), GetTileSelectionHeight(), GetTileSelection());
  }
  m_ZoomFactor.SetZoomFactor(zoom_factor);
  delete m_BlitTile;
  m_BlitTile = new CDIBSection(
    (int) (m_Tileset->GetTileWidth()  * m_ZoomFactor.GetZoomFactor()),
    (int) (m_Tileset->GetTileHeight() * m_ZoomFactor.GetZoomFactor()),
    32
  );
  UpdateScrollBar();
  Invalidate();
  int zoom_factor_i = (int) m_ZoomFactor.GetZoomFactor();
  Configuration::Set(KEY_TILES_ZOOM_FACTOR, zoom_factor_i);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilesetView::OnZoom1x()
{
  OnZoom(1);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilesetView::OnZoom2x()
{
  OnZoom(2);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilesetView::OnZoom4x()
{
  OnZoom(4);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilesetView::OnZoom8x()
{
  OnZoom(8);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilesetView::OnZoomIn()
{
  //MessageBox("TilesetView::ZoomIn");
/*
  switch ((int)m_ZoomFactor) {
    case 1: OnZoom(2); break;
    case 2: OnZoom(4); break;
    case 4: OnZoom(8); break;
  }
*/
  m_ZoomFactor.ZoomIn();
  OnZoom(m_ZoomFactor.GetZoomFactor());
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilesetView::OnZoomOut()
{
  //MessageBox("TilesetView::ZoomOut");
/*
  switch ((int)m_ZoomFactor) {
    case 2: OnZoom(1); break;
    case 4: OnZoom(2); break;
    case 8: OnZoom(4); break;
  }
*/
  m_ZoomFactor.ZoomOut();
  OnZoom(m_ZoomFactor.GetZoomFactor());
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilesetView::OnEditRange()
{
  const unsigned int id = GetCurrentMessage()->wParam;
  if ( EditRange::OnEditRange("tileset", id, false, (void*) m_Tileset, m_SelectedTile) ) {
    m_Handler->TV_TilesetChanged();
    UpdateObstructionTiles();
    Invalidate();
  }
}
////////////////////////////////////////////////////////////////////////////////
