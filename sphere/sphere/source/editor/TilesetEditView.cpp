// identifier too long
#pragma warning(disable : 4786)
#include "TilesetEditView.hpp"
#include "TilePropertiesDialog.hpp"
#include "NumberDialog.hpp"
#include "FileDialogs.hpp"
#include "resource.h"

////////////////////////////////////////////////////////////////////////////////

#define ID_TILESET_INSERTTILES      705
#define ID_TILESET_APPENDTILES      706
#define ID_TILESET_DELETETILE       707
#define ID_TILESET_REPLACEWITHIMAGE 708
#define ID_TILESET_INSERTIMAGE      709
#define ID_TILESET_APPENDIMAGE      710

////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CTilesetEditView, CHScrollWindow)
  ON_WM_SIZE()
  ON_COMMAND(ID_TILESET_INSERTTILES, OnTilesetInsertTiles)
  ON_COMMAND(ID_TILESET_APPENDTILES, OnTilesetAppendTiles)
  ON_COMMAND(ID_TILESET_DELETETILE, OnTilesetDeleteTile)
  ON_COMMAND(ID_TILESET_REPLACEWITHIMAGE, OnTilesetReplaceWithImage)
  ON_COMMAND(ID_TILESET_INSERTIMAGE,      OnTilesetInsertImage)
  ON_COMMAND(ID_TILESET_APPENDIMAGE,      OnTilesetAppendImage)
  ON_COMMAND(ID_IMAGEVIEW_PASTE, OnPaste)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////

CTilesetEditView::CTilesetEditView()
: m_Handler(NULL)
, m_Tileset(NULL)
, m_CurrentTile(0)
, m_Created(false)
, m_MultiTileWidth(0)
, m_MultiTileHeight(0)
, m_MultiTileData(NULL) // m_MultiTileData is not ours to free
{
}

////////////////////////////////////////////////////////////////////////////////

CTilesetEditView::~CTilesetEditView()
{
  if (m_Created) {
    DestroyWindow();
  }
}

////////////////////////////////////////////////////////////////////////////////

BOOL
CTilesetEditView::Create(CWnd* parent, CDocumentWindow* owner, ITilesetEditViewHandler* handler, sTileset* tileset)
{
  CWnd::Create(
    AfxRegisterWndClass(0, LoadCursor(NULL, IDC_ARROW), NULL, NULL),
    "",
    WS_CHILD | WS_VISIBLE | WS_HSCROLL,
    CRect(0, 0, 0, 0),
    parent,
    1000);
  m_Handler = handler;
  m_Tileset = tileset;
  // create the views
  m_ImageView.Create(owner, this, this);
  m_PaletteView.Create(this, this);
  m_ColorView.SetNumColors(2);
  m_ColorView.Create(this, this);
  m_AlphaView.Create(this, this);
  m_Created = true;
  // put everything in the right place
  RECT rect;
  GetClientRect(&rect);
  OnSize(0, rect.right, rect.bottom);
  UpdateImageView();
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////

void
CTilesetEditView::TilesetChanged()
{
  UpdateImageView();
  UpdateScrollBar();
  Invalidate(); 
}

////////////////////////////////////////////////////////////////////////////////

void
CTilesetEditView::SelectTile(int tile)
{
  m_CurrentTile = tile;
  UpdateImageView();
  UpdateScrollBar();
}

////////////////////////////////////////////////////////////////////////////////

void
CTilesetEditView::SetTileSelection(int width, int height, unsigned int* tiles)
{
  if (width > 0 && height > 0 && tiles != NULL) {
    m_MultiTileWidth = width;
    m_MultiTileHeight = height;
    m_MultiTileData = tiles;
  }
  else {
    m_MultiTileWidth  = 0;
    m_MultiTileHeight = 0;
    m_MultiTileData   = NULL;
  }
  UpdateImageView();
  UpdateScrollBar();
  Invalidate();
}

////////////////////////////////////////////////////////////////////////////////

void
CTilesetEditView::SP_ColorSelected(RGBA color)
{
  byte alpha = color.alpha;
  RGB  rgb   = { color.red, color.green, color.blue };
  m_ImageView.SetColor(0, color);
  m_ColorView.SetColor(0, rgb);
  m_AlphaView.SetAlpha(alpha);
}

////////////////////////////////////////////////////////////////////////////////

void
CTilesetEditView::UpdateImageView()
{
  if (m_MultiTileWidth > 0 && m_MultiTileHeight > 0 && m_MultiTileData != NULL) {
    CImage32 image;
    image.SetBlendMode(CImage32::REPLACE);
    if (image.Create(m_MultiTileWidth * m_Tileset->GetTileWidth(), m_MultiTileHeight * m_Tileset->GetTileHeight())) {
      for (int y = 0; y < m_MultiTileHeight; y++) {
        for (int x = 0; x < m_MultiTileWidth; x++) {
          unsigned int tile_index = m_MultiTileData[(y * m_MultiTileWidth) + x];
          sTile& tile = m_Tileset->GetTile(tile_index);
          tile.SetBlendMode(CImage32::REPLACE);
          image.BlitImage(tile, x * m_Tileset->GetTileWidth(), y * m_Tileset->GetTileHeight());
        }
      }
      m_ImageView.SetImage(image.GetWidth(), image.GetHeight(), image.GetPixels(), true);
      return;
    }
  }
  
  sTile& tile = m_Tileset->GetTile(m_CurrentTile);
  m_ImageView.SetImage(tile.GetWidth(), tile.GetHeight(), tile.GetPixels(), true);
}

////////////////////////////////////////////////////////////////////////////////

void
CTilesetEditView::UpdateScrollBar()
{
  SetHScrollPosition(m_CurrentTile);
  SetHScrollRange(m_Tileset->GetNumTiles(), 1);
}

////////////////////////////////////////////////////////////////////////////////

void
CTilesetEditView::OnHScrollChanged(int x)
{
  m_CurrentTile = x;
  UpdateImageView();
  UpdateScrollBar();
  m_Handler->TEV_SelectedTileChanged(x);
}

////////////////////////////////////////////////////////////////////////////////

static inline void twMoveControl(CWnd& control, int x, int y, int w, int h)
{
  control.MoveWindow(x, y, w, h, FALSE);
  control.Invalidate();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CTilesetEditView::OnSize(UINT type, int cx, int cy)
{
  const int PaletteWidth = 60;
  const int AlphaWidth = 32;
  if (m_Created) {
    
    twMoveControl(m_ImageView, 0, 0, cx - PaletteWidth - AlphaWidth, cy);
    twMoveControl(m_PaletteView, cx - PaletteWidth - AlphaWidth, 0, PaletteWidth, cy - PaletteWidth);
    twMoveControl(m_ColorView, cx - PaletteWidth - AlphaWidth, cy - PaletteWidth, PaletteWidth, PaletteWidth);
    twMoveControl(m_AlphaView, cx - AlphaWidth, 0, AlphaWidth, cy);
    UpdateScrollBar();
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CTilesetEditView::OnTilesetInsertTiles()
{
  CNumberDialog NumberDialog("Insert Tiles", "How many tiles do you want to insert?", 1, 1, 256);
  if (NumberDialog.DoModal() == IDOK)
  {
    m_Tileset->InsertTiles(m_CurrentTile, NumberDialog.GetValue());
    UpdateScrollBar();
    UpdateImageView();
    
    m_Handler->TEV_TilesetModified();
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CTilesetEditView::OnTilesetAppendTiles()
{
  CNumberDialog NumberDialog("Append Tiles", "How many tiles do you want to append?", 1, 1, 256);
  if (NumberDialog.DoModal() == IDOK)
  {
    m_Tileset->AppendTiles(NumberDialog.GetValue());
    UpdateScrollBar();
    UpdateImageView();
    m_Handler->TEV_TilesetModified();
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CTilesetEditView::OnUpdateTilesetDeleteTile(CCmdUI* cmdui)
{
  if (m_Tileset->GetNumTiles() > 1)
    cmdui->Enable();
  else
    cmdui->Enable(FALSE);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CTilesetEditView::OnTilesetDeleteTile()
{
  m_Tileset->DeleteTiles(m_CurrentTile, 1);
  if (m_CurrentTile >= m_Tileset->GetNumTiles())
    m_CurrentTile--;
  m_Handler->TEV_TilesetModified();
  UpdateScrollBar();
  UpdateImageView();
  m_Handler->TEV_SelectedTileChanged(m_CurrentTile);
  m_Handler->TEV_TilesetModified();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CTilesetEditView::OnTilesetReplaceWithImage()
{
  if (MessageBox("Are you sure?", NULL, MB_YESNO) == IDNO)
    return;
  CImageFileDialog FileDialog(FDM_OPEN);
  if (FileDialog.DoModal() == IDOK)
  {
    if (!m_Tileset->Import_Image(FileDialog.GetPathName()))
    {
      MessageBox("Error: Could not import image");
      return;
    }
    m_Handler->TEV_TilesetModified();
    UpdateScrollBar();
    UpdateImageView();
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CTilesetEditView::OnTilesetInsertImage()
{
  CImageFileDialog FileDialog(FDM_OPEN);
  if (FileDialog.DoModal() == IDOK)
  {
    if (!m_Tileset->InsertImage(m_CurrentTile, FileDialog.GetPathName()))
    {
      MessageBox("Error: Could not insert image");
      return;
    }
    m_Handler->TEV_TilesetModified();
    UpdateScrollBar();
    UpdateImageView();
    m_Handler->TEV_TilesetModified();
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CTilesetEditView::OnTilesetAppendImage()
{
  CImageFileDialog FileDialog(FDM_OPEN);
  if (FileDialog.DoModal() == IDOK)
  {
    if (!m_Tileset->AppendImage(FileDialog.GetPathName()))
    {
      MessageBox("Error: Could not append image");
      return;
    }
    m_Handler->TEV_TilesetModified();
    UpdateScrollBar();
    UpdateImageView();
    m_Handler->TEV_TilesetModified();
  }
}

////////////////////////////////////////////////////////////////////////////////

void
CTilesetEditView::IV_ImageChanged()
{
  if (m_MultiTileWidth > 0 && m_MultiTileHeight > 0 && m_MultiTileData != NULL)
  {
    const int tile_width  = m_Tileset->GetTileWidth();
    const int tile_height = m_Tileset->GetTileHeight();
    const int image_width = m_ImageView.GetWidth();
    const int image_height = m_ImageView.GetHeight();

    for (int ty = 0; ty < m_MultiTileHeight; ty++) {
      for (int tx = 0; tx < m_MultiTileWidth; tx++) {
        const unsigned int tile = m_MultiTileData[(ty * m_MultiTileWidth) + tx];
        for (int iy = 0; iy < tile_height; iy++) {
          for (int ix = 0; ix < tile_width; ix++)
          {
            int pixel_x = ((tx * tile_width)  + ix);
            int pixel_y = ((ty * tile_height) + iy);
            int pixel_index = (pixel_y * image_width) + pixel_x;

            if (pixel_x >= 0 && pixel_x < image_width
             && pixel_y >= 0 && pixel_y < image_height
             && pixel_index >= 0 && pixel_index < image_width * image_height)
            {
              RGBA pixel = m_ImageView.GetPixels()[pixel_index];
              m_Tileset->GetTile(tile).SetPixel(ix, iy, pixel);
            }
          }
        }

        m_Handler->TEV_TileModified(tile);
      }
    }

    return;
  }

  // store the old data
  memcpy(
    m_Tileset->GetTile(m_CurrentTile).GetPixels(),
    m_ImageView.GetPixels(),
    m_Tileset->GetTileWidth() * m_Tileset->GetTileHeight() * sizeof(RGBA));
  m_Handler->TEV_TileModified(m_CurrentTile);
}

////////////////////////////////////////////////////////////////////////////////

void
CTilesetEditView::IV_ColorChanged(RGBA color)
{
  RGBA rgba = m_ImageView.GetColor();
  RGB rgb = { rgba.red, rgba.green, rgba.blue };
  m_ColorView.SetColor(0, rgb);
  m_AlphaView.SetAlpha(rgba.alpha);
}

////////////////////////////////////////////////////////////////////////////////

void
CTilesetEditView::PV_ColorChanged(int index, RGB color)
{
  m_ColorView.SetColor(index, color);
  byte alpha = m_AlphaView.GetAlpha();
  RGBA c = { color.red, color.green, color.blue, alpha };
  m_ImageView.SetColor(index, c);
}

////////////////////////////////////////////////////////////////////////////////

void
CTilesetEditView::CV_ColorChanged(int index, RGB color)
{
  byte alpha = m_AlphaView.GetAlpha();
  RGBA rgba = { color.red, color.green, color.blue, alpha };
  m_ImageView.SetColor(index, rgba);
}

////////////////////////////////////////////////////////////////////////////////

void
CTilesetEditView::AV_AlphaChanged(byte alpha)
{
  RGBA rgba = m_ImageView.GetColor();
  rgba.alpha = alpha;
  m_ImageView.SetColor(0, rgba);
}

////////////////////////////////////////////////////////////////////////////////

void
CTilesetEditView::OnPaste()
{
  m_ImageView.SendMessage(WM_COMMAND, MAKEWPARAM(ID_IMAGEVIEW_PASTE, 0), 0);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CTilesetEditView::OnKeyDown(UINT vk, UINT repeat, UINT flags)
{
  m_ImageView.OnKeyDown(vk, repeat, flags);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CTilesetEditView::OnKeyUp(UINT vk, UINT repeat, UINT flags)
{
  m_ImageView.OnKeyUp(vk, repeat, flags);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CTilesetEditView::OnTimer(UINT event)
{
  m_ImageView.OnTimer(event);
}

////////////////////////////////////////////////////////////////////////////////

void
CTilesetEditView::OnToolChanged(UINT id, int tool_index)
{
  m_ImageView.OnToolChanged(id, tool_index);
}

////////////////////////////////////////////////////////////////////////////////

BOOL
CTilesetEditView::IsToolAvailable(UINT id)
{
  return m_ImageView.IsToolAvailable(id);
}

////////////////////////////////////////////////////////////////////////////////
