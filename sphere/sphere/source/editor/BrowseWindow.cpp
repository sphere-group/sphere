// identifier too long
#pragma warning(disable : 4786)

#include "BrowseWindow.hpp"
#include "../common/minmax.hpp"
#include "Editor.hpp"
#include "resource.h"
#include "MainWindow.hpp"
#include "ResizeDialog.hpp"
#include "../common/Spriteset.hpp"
#include "../common/WindowStyle.hpp"
#include "../common/Map.hpp"
#include "../common/Font.hpp"
#include "../common/AnimationFactory.hpp"
#include "../common/system.hpp"

static const int BROWSE_TIMER = 9001;

////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CBrowseWindow, CDocumentWindow)
  ON_WM_SIZE()
  ON_WM_KEYDOWN()
  ON_WM_PAINT()
  ON_WM_MOUSEMOVE()
  ON_WM_TIMER()
  ON_WM_VSCROLL()
  ON_WM_LBUTTONDOWN()
  ON_COMMAND(ID_BROWSE_REFRESH, OnBrowseListRefresh)
	ON_COMMAND(ID_BROWSE_SETSIZE, OnBrowseSetBrowseSize)
  ON_COMMAND(ID_BROWSE_RESIZE, OnBrowseResizeThumbs)
  ON_UPDATE_COMMAND_UI(ID_BROWSE_RESIZE, OnUpdateBrowseResizeThumbs)
  ON_COMMAND(ID_FILE_ZOOM_IN, OnZoomIn)
  ON_COMMAND(ID_FILE_ZOOM_OUT, OnZoomOut)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////

CBrowseWindow::CBrowseWindow(const char* folder, const char* filter)
: CDocumentWindow(folder, IDR_BROWSE, CSize(400, 100))
, m_SelectedImage(0)
, m_BlitTile(NULL)
, m_TopRow(0)
, m_Created(false)
, m_Resize(true)
{
  m_Folder = folder;
  m_Filter = filter;
  m_ImageWidth = 100;
  m_ImageHeight = 100;
  m_BlitTile = new CDIBSection(
    (int) (m_ImageWidth * m_ZoomFactor.GetZoomFactor()),
    (int) (m_ImageHeight * m_ZoomFactor.GetZoomFactor()),
    32
  );
  m_FileList = GetFileList(filter);
  Create();
}

////////////////////////////////////////////////////////////////////////////////

CBrowseWindow::~CBrowseWindow()
{
  // destroy all the child windows
  Destroy();
  if (m_BlitTile) {
    delete m_BlitTile;
    m_BlitTile = NULL;
  }
  ClearBrowseList();
  m_FileList.clear();
}

////////////////////////////////////////////////////////////////////////////////

void
CBrowseWindow::Create()
{
  // create the window
  CDocumentWindow::Create(
    AfxRegisterWndClass(0, AfxGetApp()->LoadCursor(IDI_BROWSE)),
    WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_OVERLAPPEDWINDOW // window styles
  );
  m_Created = true;  // the window and children are ready!
  // move things to their rightful places
  RECT ClientRect;
  GetClientRect(&ClientRect);
  OnSize(0, ClientRect.right - ClientRect.left, ClientRect.bottom - ClientRect.top);
  m_Timer = SetTimer(BROWSE_TIMER, 25, NULL);
}

////////////////////////////////////////////////////////////////////////////////

void
CBrowseWindow::Destroy()
{
  //KillTimer(m_Timer);
  ClearBrowseList();
  m_FileList.clear();
  DestroyWindow();
}

////////////////////////////////////////////////////////////////////////////////

void
CBrowseWindow::InvalidateTile(int tile)
{
  RECT client_rect;
  GetClientRect(&client_rect);
  int num_tiles_x = client_rect.right / m_BlitTile->GetWidth();
  if (num_tiles_x != 0) {
    int col = tile % num_tiles_x;
    int row = tile / num_tiles_x;
  
    int x = col * m_BlitTile->GetWidth();
    int y = (row - m_TopRow) * m_BlitTile->GetHeight();
    RECT rect;
    SetRect(&rect, x, y, x + m_BlitTile->GetWidth(), y + m_BlitTile->GetHeight());
    InvalidateRect(&rect);
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CBrowseWindow::OnTimer(UINT event) {
  //if (m_MenuShown)
  //  return;
  if (m_FileList.size() > 0) {
    char directory[MAX_PATH] = {0};
    if ( GetCurrentDirectory(MAX_PATH, directory) != 0) {
      if (SetCurrentDirectory(m_Folder.c_str()) != 0) {
        if (LoadFile(m_FileList[0].c_str())) {
          UpdateScrollBar();
          int tile = m_BrowseList.size() - 1;
          InvalidateTile(tile);
        }
        m_FileList.erase(m_FileList.begin());
        SetCurrentDirectory(directory);
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

void
CBrowseWindow::OpenFile(unsigned int index) {
  if (index >= 0 && index < m_BrowseList.size())
  {
    std::string path = m_Folder.c_str();
    path += "\\";
    path += m_BrowseList[index]->filename;
    ((CMainWindow*) GetMainWindow())->OpenGameFile(path.c_str());
  }
}

////////////////////////////////////////////////////////////////////////////////

bool
CBrowseWindow::LoadFile(const char* filename)
{
  bool valid = false;
  const int width = m_ImageWidth;
  const int height = m_ImageHeight;
  CImage32 image;
  sSpriteset spriteset;
  sWindowStyle windowstyle;
  sMap map;
  sFont font;
  IAnimation* animation;
  sTileset tileset;
  if (image.Load(filename)) {
    valid = true;
  }
  else if (spriteset.Load(filename)) {
    image = spriteset.GetImage(0);
    valid = true;
  }
  else if (windowstyle.Load(filename)) {
    CImage32 tl = windowstyle.GetBitmap(sWindowStyle::UPPER_LEFT);
    CImage32 tm = windowstyle.GetBitmap(sWindowStyle::TOP);
    CImage32 tr = windowstyle.GetBitmap(sWindowStyle::UPPER_RIGHT);
    CImage32 ml = windowstyle.GetBitmap(sWindowStyle::LEFT);
    CImage32 mm = windowstyle.GetBitmap(sWindowStyle::BACKGROUND);
    CImage32 mr = windowstyle.GetBitmap(sWindowStyle::RIGHT);
    CImage32 bl = windowstyle.GetBitmap(sWindowStyle::LOWER_LEFT);
    CImage32 bm = windowstyle.GetBitmap(sWindowStyle::BOTTOM);
    CImage32 br = windowstyle.GetBitmap(sWindowStyle::LOWER_RIGHT);
    tl.Rescale(width/3, height/3);
    tm.Rescale(width/3, height/3);
    tr.Rescale(width/3, height/3);
    ml.Rescale(width/3, height/3);
    mm.Rescale(width/3, height/3);
    mr.Rescale(width/3, height/3);
    bl.Rescale(width/3, height/3);
    bm.Rescale(width/3, height/3);
    br.Rescale(width/3, height/3);
    image.Create(width, height);
    image.BlitImage(tl, width/3*0, height/3*0);
    image.BlitImage(tm, width/3*1, height/3*0);
    image.BlitImage(tr, width/3*2, height/3*0);
    image.BlitImage(ml, width/3*0, height/3*1);
    image.BlitImage(mm, width/3*1, height/3*1);
    image.BlitImage(mr, width/3*2, height/3*1);
    image.BlitImage(bl, width/3*0, height/3*2);
    image.BlitImage(bm, width/3*1, height/3*2);
    image.BlitImage(br, width/3*2, height/3*2);
    valid = true;
  }
  else if (map.Load(filename) && map.GetNumLayers() > 0 && map.GetTileset().GetNumTiles() > 0) {
    int map_width = 0;
    int map_height = 0;
  
    for (int i = 0; i < map.GetNumLayers(); ++i) {
      map_width  = std::max(map_width, map.GetLayer(i).GetWidth());
      map_height = std::max(map_height, map.GetLayer(i).GetHeight());
    }
    map_width  = std::min(width, map_width);
    map_height = std::min(height, map_height);
    int xstep = width / map_width;
    int ystep = height / map_height;
    map.GetTileset().SetTileSize(xstep, ystep, true);
    image.Create(map_width, map_height);
    for (int layer = 0; layer < map.GetNumLayers(); ++layer) {
      int layer_width  = map.GetLayer(layer).GetWidth();
      int layer_height = map.GetLayer(layer).GetHeight();
      for (int y = 0; y < layer_height; y += ystep) {
        for (int x = 0; x < layer_width; x += xstep) {
          image.BlitImage(map.GetTileset().GetTile(map.GetLayer(layer).GetTile(x, y)), x, y);
        }
      }
    }
    valid = true;
  }
  else if (font.Load(filename)) {
    image.Create(width, height);
    int x = 0;
    int y = 0;
      
    const char* string = "test";
    for (int i = 0; i < 4; i++) {
      image.BlitImage(font.GetCharacter(string[i]), x, y);
      x += font.GetCharacter(i).GetWidth();
    }
    valid = true;
  }
  else if ((animation = LoadAnimation(filename)) != NULL) {
    image.Create(animation->GetWidth(), animation->GetHeight());
    animation->ReadNextFrame(image.GetPixels());
    valid = true;
  }
  else if (tileset.Load(filename)) {
    image.Create(width, height);
    for (int y = 0; y < 10; y++) {
      for (int x = 0; x < 10; x++) {
        image.BlitImage(tileset.GetTile((y * 3 + x) % tileset.GetNumTiles()), x * tileset.GetTileWidth(), y * tileset.GetTileHeight());
      }
    }
    valid = true;
  }
  if (valid) {
    if (m_Resize) {
     image.Rescale(width, height);
      if (image.GetWidth() != width && image.GetHeight() != height) {
        valid = false;
      }
    }
  }
  if (valid) {
    CBrowseInfo* b = new CBrowseInfo();
    if (b) {
      b->filename = filename;
      b->image = image;
      m_BrowseList.push_back(b);
    }
  }
  return valid;
}

////////////////////////////////////////////////////////////////////////////////

void
CBrowseWindow::ClearBrowseList()
{
  for (unsigned int i = 0; i < m_BrowseList.size(); i++) {
    delete m_BrowseList[i];
    m_BrowseList[i] = NULL;
  }
  m_BrowseList.clear();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CBrowseWindow::OnSize(UINT uType, int cx, int cy)
{
  CDocumentWindow::OnSize(uType, cx, cy);
  UpdateScrollBar();
  Invalidate();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CBrowseWindow::OnKeyDown(UINT vk, UINT repeat, UINT flags)
{
  int image = m_SelectedImage;
  if (vk == VK_RIGHT) {
    image = std::min(image + 1, (int)m_BrowseList.size() - 1);
  } else if (vk == VK_LEFT) {
    image = std::max(image - 1, 0);
  }
  RECT client_rect;
  GetClientRect(&client_rect);
	int num_per_row = 0;
  int num_tiles_x = client_rect.right / m_BlitTile->GetWidth();
  if (num_tiles_x >= 0)
    num_per_row = num_tiles_x;
	if (vk == VK_DOWN) {
		if ((unsigned int) (image + num_per_row) < m_BrowseList.size())
		image += num_per_row;
	} else if (vk == VK_UP) {
		if (image - num_per_row >= 0)
  		image -= num_per_row;
	}
  if (image != m_SelectedImage) {
    m_SelectedImage = image;
    Invalidate();
  }
  if (vk == VK_RETURN) {
    if (m_SelectedImage >= 0 && (unsigned int) m_SelectedImage < m_BrowseList.size()) {
      OpenFile(m_SelectedImage);
    }
  }
  UpdateScrollBar();
}

///////////////////////////////////////////////////////////////////////////////

afx_msg void
CBrowseWindow::OnPaint()
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
      if (ix < num_tiles_x && (it >= 0 && (unsigned int)it < m_BrowseList.size()))
      {
        // draw the tile
        // fill the DIB section
        BGRA* pixels = (BGRA*)m_BlitTile->GetPixels();
        
        // make a checkerboard
        for (int iy = 0; iy < blit_height; iy++)
          for (int ix = 0; ix < blit_width; ix++)
          {
            pixels[iy * blit_width + ix] = 
              ((ix / 8 + iy / 8) % 2 ?
                CreateBGRA(255, 255, 255, 255) :
                CreateBGRA(255, 192, 192, 255)
              );
          }
        // draw the tile into it
        int tile_width  = m_BrowseList[it]->GetWidth()  < blit_width  ? m_BrowseList[it]->GetWidth()  : blit_width;
        int tile_height = m_BrowseList[it]->GetHeight() < blit_height ? m_BrowseList[it]->GetHeight() : blit_height;
        const RGBA* tilepixels = m_BrowseList[it]->GetPixels();
        int tiy;

        for (tiy = 0; tiy < tile_height; tiy++)
          for (int tix = 0; tix < tile_width; tix++)
          {
            int ty = (int) (tiy / m_ZoomFactor.GetZoomFactor());
            int tx = (int) (tix / m_ZoomFactor.GetZoomFactor());
            int t = ty * m_BrowseList[it]->GetWidth() + tx;
            
            int d = tiy * blit_width + tix;
            int alpha = tilepixels[t].alpha;
            pixels[d].red   = (tilepixels[t].red   * alpha + pixels[d].red   * (255 - alpha)) / 256;
            pixels[d].green = (tilepixels[t].green * alpha + pixels[d].green * (255 - alpha)) / 256;
            pixels[d].blue  = (tilepixels[t].blue  * alpha + pixels[d].blue  * (255 - alpha)) / 256;
          }

        for (tiy  = m_BrowseList[it]->GetHeight(); tiy < blit_height; tiy++)
          for (int tix = m_BrowseList[it]->GetWidth(); tix < blit_width; tix++)
          {
            int d = tiy * blit_width + tix;
            pixels[d].red = 0;
            pixels[d].green = 0;
            pixels[d].blue = 0;
          }
        
        // blit the tile
        CDC* tile = CDC::FromHandle(m_BlitTile->GetDC());
        dc.BitBlt(Rect.left, Rect.top, Rect.right - Rect.left, Rect.bottom - Rect.top, tile, 0, 0, SRCCOPY);
        // if the tile is selected, draw a pink rectangle around it
        if (it == m_SelectedImage)
        {
          HBRUSH newbrush = (HBRUSH)GetStockObject(NULL_BRUSH);
          CBrush* oldbrush = dc.SelectObject(CBrush::FromHandle(newbrush));
          HPEN newpen = (HPEN)CreatePen(PS_SOLID, 1, RGB(0xFF, 0x00, 0xFF));
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
CBrowseWindow::OnLButtonDown(UINT flags, CPoint point)
{
  //if (m_MenuShown)
  //  return;
  RECT client_rect;
  GetClientRect(&client_rect);
  int num_tiles_x = client_rect.right / m_BlitTile->GetWidth();
  int col = point.x / m_BlitTile->GetWidth();
  int row = point.y / m_BlitTile->GetHeight();
  // don't let user select tile off the right edge (and go to the next row)
  if (col >= num_tiles_x) {
    return;
  }
  int tile = (m_TopRow + row) * num_tiles_x + col;
  if (tile >= 0 && (unsigned int) tile < m_BrowseList.size())
    m_SelectedImage = tile;
  UpdateScrollBar();
  Invalidate();
}

////////////////////////////////////////////////////////////////////////////////

/*
afx_msg void
CBrowseWindow::OnLButtonDblClk(flags, CPoint point)
{
}
*/

///////////////////////////////////////////////////////////////////////////////

afx_msg void
CBrowseWindow::OnMouseMove(UINT flags, CPoint point)
{
  RECT client_rect;
  GetClientRect(&client_rect);
  int num_tiles_x = client_rect.right / m_BlitTile->GetWidth();
  int x = (int) (point.x / (m_BlitTile->GetWidth()  * m_ZoomFactor.GetZoomFactor()));
  int y = (int) (point.y / (m_BlitTile->GetHeight() * m_ZoomFactor.GetZoomFactor()));
  int tile = (m_TopRow + y) * num_tiles_x + x;
  if (tile >= 0 && (unsigned int) tile < m_BrowseList.size())
  {
    CString browse_info;
    browse_info.Format("%s", m_BrowseList[tile]->filename.c_str());
    GetStatusBar()->SetWindowText(browse_info);
  }
  else
    GetStatusBar()->SetWindowText("");
}

///////////////////////////////////////////////////////////////////////////////

afx_msg void
CBrowseWindow::OnVScroll(UINT code, UINT pos, CScrollBar* scroll_bar)
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

///////////////////////////////////////////////////////////////////////////////

void
CBrowseWindow::UpdateScrollBar()
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

///////////////////////////////////////////////////////////////////////////////

int
CBrowseWindow::GetPageSize()
{
  RECT ClientRect;
  GetClientRect(&ClientRect);
  return ClientRect.bottom / m_BlitTile->GetHeight();
}

////////////////////////////////////////////////////////////////////////////////

int
CBrowseWindow::GetNumRows()
{
  RECT client_rect;
  GetClientRect(&client_rect);
  int num_tiles_x = client_rect.right / m_BlitTile->GetWidth();
  if (num_tiles_x == 0)
    return -1;
  else
    return (m_BrowseList.size() + num_tiles_x - 1) / num_tiles_x;
}

///////////////////////////////////////////////////////////////////////////////

afx_msg void
CBrowseWindow::OnZoomIn()
{
  m_ZoomFactor.ZoomIn();
  OnZoom(m_ZoomFactor.GetZoomFactor());
  Invalidate();
}

///////////////////////////////////////////////////////////////////////////////

afx_msg void
CBrowseWindow::OnZoomOut()
{
  m_ZoomFactor.ZoomOut();
  OnZoom(m_ZoomFactor.GetZoomFactor());
  Invalidate(); 
}

///////////////////////////////////////////////////////////////////////////////

void
CBrowseWindow::OnZoom(double zoom_factor)
{
  m_ZoomFactor.SetZoomFactor(zoom_factor);
  m_BlitTile = new CDIBSection(
    (int) (m_ImageWidth * m_ZoomFactor.GetZoomFactor()),
    (int) (m_ImageHeight * m_ZoomFactor.GetZoomFactor()),
    32
  );
}

///////////////////////////////////////////////////////////////////////////////

afx_msg void
CBrowseWindow::OnBrowseListRefresh() {
  m_FileList.clear();
  ClearBrowseList();
  char directory[MAX_PATH] = {0};
  if ( GetCurrentDirectory(MAX_PATH, directory) != 0) {
  
    if ( SetCurrentDirectory(m_Folder.c_str()) != 0 ) {
      m_FileList = GetFileList(m_Filter.c_str());
      Invalidate();
    }
    SetCurrentDirectory(directory);
  }
}

///////////////////////////////////////////////////////////////////////////////

afx_msg void
CBrowseWindow::OnBrowseSetBrowseSize() {
	CResizeDialog dialog("Resize Browse Image Size", m_ImageWidth, m_ImageHeight);
	dialog.SetRange(1, 4096, 1, 4096);
	if (dialog.DoModal() == IDOK) {
		if (dialog.GetWidth()  != m_ImageWidth
   	 && dialog.GetHeight() != m_ImageHeight
  	 && dialog.GetWidth()  >= 1
		 && dialog.GetWidth()   < 4096
		 && dialog.GetHeight() >= 1
		 && dialog.GetHeight()  < 4096)
    {
      m_ImageWidth = dialog.GetWidth();
      m_ImageHeight = dialog.GetHeight();
      OnZoom(m_ZoomFactor.GetZoomFactor()); 
		  OnBrowseListRefresh();
		}
  }
}

///////////////////////////////////////////////////////////////////////////////

afx_msg void
CBrowseWindow::OnBrowseResizeThumbs()
{
  m_Resize = !m_Resize;
  OnBrowseListRefresh();
}

///////////////////////////////////////////////////////////////////////////////

afx_msg void
CBrowseWindow::OnUpdateBrowseResizeThumbs(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_Resize ? TRUE : FALSE);
}

///////////////////////////////////////////////////////////////////////////////
