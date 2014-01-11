#include "TileObstructionView.hpp"
#include "../common/primitives.hpp"
#include "ImageRender.hpp"
BEGIN_MESSAGE_MAP(CTileObstructionView, CWnd)
  ON_WM_PAINT()
  ON_WM_SIZE()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////
CTileObstructionView::CTileObstructionView()
: m_tile(NULL)
, m_pixels(NULL)
, m_mouse_down(false)
{
}
////////////////////////////////////////////////////////////////////////////////
CTileObstructionView::~CTileObstructionView()
{
  if (m_pixels) {
    delete[] m_pixels;
  }
  m_pixels = NULL;
  m_tile = NULL;
}
////////////////////////////////////////////////////////////////////////////////
BOOL
CTileObstructionView::Create(CWnd* parent, sTile* tile)
{
  if (m_pixels) {
    delete[] m_pixels;
    m_pixels = NULL;
  }
  m_tile = tile;
  if (tile != NULL) {
    if (tile->GetWidth() > 0 && tile->GetHeight() > 0) {
      m_pixels = new RGBA[tile->GetWidth() * tile->GetHeight()];
    }
  }
  m_BlitTile.Create(16, 16, 32);
  
  return CWnd::Create(
    AfxRegisterWndClass(0, LoadCursor(NULL, IDC_ARROW), NULL, NULL),
    "tile obstruction view",
    WS_CHILD | WS_VISIBLE,
    CRect(0, 0, 0, 0),
    parent,
    0
  );
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTileObstructionView::OnPaint()
{
  CPaintDC dc_(this);
  HDC dc = dc_.m_hDC;
  // get window size
  RECT rect;
  GetClientRect(&rect);
  if (!m_pixels || !m_tile) {
    FillRect(dc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
    return;
  }
  // draw tile and obstruction lines onto backbuffer tile
  RenderTile();
  // calculate size of pixel squares
  double hsize = (double)rect.right  / (double)m_tile->GetWidth();
  double vsize = (double)rect.bottom / (double)m_tile->GetHeight();
  StretchedBlit(dc_, &m_BlitTile,
                hsize, vsize,
                m_tile->GetWidth(), m_tile->GetHeight(),
                m_pixels, true, &rect,
                0, 0, m_tile->GetWidth(), m_tile->GetHeight(),
                0, 0);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTileObstructionView::OnSize(UINT type, int cx, int cy)
{
  Invalidate();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTileObstructionView::OnLButtonDown(UINT flags, CPoint point)
{
  m_mouse_down = true;
  m_start_point = WindowToTile(point);
  SetCapture();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTileObstructionView::OnLButtonUp(UINT flags, CPoint point)
{
  CPoint end_point = WindowToTile(point);
  if (m_tile != NULL) {
    m_tile->GetObstructionMap().AddSegment(
      m_start_point.x,
      m_start_point.y,
      end_point.x,
      end_point.y
    );
  }
  m_mouse_down = false;
  ReleaseCapture();
  Invalidate();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTileObstructionView::OnMouseMove(UINT flags, CPoint point)
{
  m_current_point = WindowToTile(point);
  Invalidate();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTileObstructionView::OnRButtonDown(UINT flags, CPoint point)
{
  point = WindowToTile(point);
  if (m_tile != NULL) {
    m_tile->GetObstructionMap().RemoveSegmentByPoint(point.x, point.y);
  }
  Invalidate();
}
////////////////////////////////////////////////////////////////////////////////
void
CTileObstructionView::RenderTile()
{
  if (!m_pixels || !m_tile)
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
  // draw the tile's pixels
  memcpy(
    m_pixels,
    m_tile->GetPixels(),
    m_tile->GetWidth() * m_tile->GetHeight() * sizeof(RGBA)
  );
  // draw the obstruction segments
  Local::Color c;
  RECT clipper = { 0, 0, m_tile->GetWidth() - 1, m_tile->GetHeight() - 1 };
  sObstructionMap& obs_map = m_tile->GetObstructionMap();
  for (int i = 0; i < obs_map.GetNumSegments(); i++) {
    
    const sObstructionMap::Segment& s = obs_map.GetSegment(i);
    //(x1 = 0, s2 = 2, y1 = 0, y2 = 1)
    primitives::Line(
      m_pixels,
      m_tile->GetWidth(),
      s.x1,
      s.y1,
      s.x2,
      s.y2,
      c,
      clipper,
      Local::CopyRGBA
    );
  }
  /*
  for (int y = 0; y < m_tile->GetHeight(); y++) {
    for (int x = 0; x < m_tile->GetWidth(); x++) {
      if (obs_map.TestRectangle(x, y, x, y)) {
        primitives::Point(m_pixels, m_tile->GetWidth(), x, y, CreateRGBA(0, 255, 0, 255), clipper, Local::CopyRGBA);
      }
    }
  }
  */
  // draw current line
  if (m_mouse_down) {
    primitives::Line(
      m_pixels,
      m_tile->GetWidth(),
      m_start_point.x,
      m_start_point.y,
      m_current_point.x,
      m_current_point.y,
      c,
      clipper,
      Local::CopyRGBA
    );
  }
}
////////////////////////////////////////////////////////////////////////////////
CPoint
CTileObstructionView::WindowToTile(CPoint p)
{
  RECT rect;
  GetClientRect(&rect);
  p.x = p.x * m_tile->GetWidth()  / rect.right;
  p.y = p.y * m_tile->GetHeight() / rect.bottom;
  return p;
}
////////////////////////////////////////////////////////////////////////////////
