#pragma warning(disable : 4786)
#include "LayerView.hpp"
#include "LayerPropertiesDialog.hpp"
#include "Editor.hpp"
#include "FileDialogs.hpp"
#include "NumberDialog.hpp"
#include "../common/Map.hpp"
#include "resource.h"
#include "../common/Filters.hpp"
const int LAYER_BUTTON_HEIGHT = 20;
BEGIN_MESSAGE_MAP(CLayerView, CVScrollWindow)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_PAINT()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_RBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_COMMAND(ID_LAYERVIEW_INSERTLAYER,          OnInsertLayer)
  ON_COMMAND(ID_LAYERVIEW_INSERTLAYERFROMIMAGE, OnInsertLayerFromImage)
  ON_COMMAND(ID_LAYERVIEW_DELETELAYER,          OnDeleteLayer)
  ON_COMMAND(ID_LAYERVIEW_DUPLICATELAYER,       OnDuplicateLayer)
  ON_COMMAND(ID_LAYERVIEW_PROPERTIES,           OnLayerProperties)
  ON_COMMAND(ID_LAYERVIEW_EXPORTASIMAGE,        OnExportLayer)
  ON_COMMAND(ID_LAYERVIEW_EXPORT_ALL_VISIBLE_LAYERS_AS_IMAGE, OnExportAllVisibleLayers)
  ON_COMMAND(ID_LAYERVIEW_FLATTEN_VISIBLE_LAYERS, OnFlattenVisibleLayers)
  ON_COMMAND(ID_LAYERVIEW_SLIDE_UP,             OnLayerSlideUp)
  ON_COMMAND(ID_LAYERVIEW_SLIDE_RIGHT,          OnLayerSlideRight)
  ON_COMMAND(ID_LAYERVIEW_SLIDE_DOWN,           OnLayerSlideDown)
  ON_COMMAND(ID_LAYERVIEW_SLIDE_LEFT,           OnLayerSlideLeft)
  ON_COMMAND(ID_LAYERVIEW_SLIDE_OTHER,          OnLayerSlideOther)
  ON_COMMAND(ID_LAYERVIEW_MOVE_UP, OnLayerMoveUp)
  ON_COMMAND(ID_LAYERVIEW_MOVE_DOWN, OnLayerMoveDown)
  ON_COMMAND(ID_LAYERVIEW_TOGGLE_LOCK_LAYERS_IN_PLACE, OnToggleLockLayersInPlace)
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////
CLayerView::CLayerView()
: m_Handler(NULL)
, m_Map(NULL)
, m_TopLayer(0)
, m_SelectedLayer(0)
, m_SelectedTile(0)
, m_IsDragging(false)
, m_LayersAreLockedInPlace(false)
{
}
////////////////////////////////////////////////////////////////////////////////
CLayerView::~CLayerView()
{
  DestroyWindow();
}
////////////////////////////////////////////////////////////////////////////////
BOOL
CLayerView::Create(ILayerViewHandler* handler, CWnd* parent, sMap* map)
{
  m_Handler = handler;
  m_Map = map;
  return CWnd::Create(
    AfxRegisterWndClass(0, LoadCursor(NULL, IDC_ARROW), NULL, LoadIcon(NULL, IDI_APPLICATION)),
    "LayerView",
    WS_CHILD | WS_VISIBLE | WS_VSCROLL,
    CRect(0, 0, 0, 0),
    parent,
    0
  );
}
////////////////////////////////////////////////////////////////////////////////
int
CLayerView::GetSelectedLayer() const
{
  return m_SelectedLayer;
}
////////////////////////////////////////////////////////////////////////////////
void
CLayerView::SetSelectedTile(int tile)
{
  m_SelectedTile = tile;
}
////////////////////////////////////////////////////////////////////////////////
afx_msg int
CLayerView::OnCreate(CREATESTRUCT* createstruct)
{
  UpdateScrollBar();
  return 0;
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CLayerView::OnSize(UINT type, int cx, int cy)
{
  UpdateScrollBar();
  Invalidate();
}
////////////////////////////////////////////////////////////////////////////////
static HPEN CreateShadedPen(RGB color1, RGB color2, int alpha)
{
  RGB color = {
    (color1.red   * alpha + color2.red   * (256 - alpha)) / 256,
    (color1.green * alpha + color2.green * (256 - alpha)) / 256,
    (color1.blue  * alpha + color2.blue  * (256 - alpha)) / 256,
  };
  return CreatePen(PS_SOLID, 1, RGB(color.red, color.green, color.blue));
}
////////////////////////////////////////////////////////////////////////////////
static void DrawLayerButton(HDC dc, RECT rect, const sLayer& layer, bool pushed)
{
  // get the menu color
  DWORD sys_color = GetSysColor(COLOR_MENU);
  RGB menu_color = {
    GetRValue(sys_color),
    GetGValue(sys_color),
    GetBValue(sys_color),
  };
  RGB white = { 255, 255, 255 };
  RGB black = { 0,   0,   0   };
  if (pushed) {
    std::swap(white, black);
  }
  HPEN very_light = CreateShadedPen(menu_color, white, 64);
  HPEN light      = CreateShadedPen(menu_color, white, 192);
  HPEN dark       = CreateShadedPen(menu_color, black, 192);
  HPEN very_dark  = CreateShadedPen(menu_color, black, 64);
  SaveDC(dc);
  SetROP2(dc, R2_COPYPEN);
  
  // draw the outer border
  SelectObject(dc, very_light);
  MoveToEx(dc, rect.left, rect.bottom - 1, NULL);
  LineTo(dc, rect.left, rect.top);
  LineTo(dc, rect.right - 1, rect.top);
  SelectObject(dc, very_dark);
  MoveToEx(dc, rect.left, rect.bottom - 1, NULL);
  LineTo(dc, rect.right - 1, rect.bottom - 1);
  LineTo(dc, rect.right - 1, rect.top);
 
  InflateRect(&rect, -1, -1);
  // draw the inner border
  SelectObject(dc, light);
  MoveToEx(dc, rect.left, rect.bottom - 1, NULL);
  LineTo(dc, rect.left, rect.top);
  LineTo(dc, rect.right - 1, rect.top);
  SelectObject(dc, dark);
  MoveToEx(dc, rect.left, rect.bottom - 1, NULL);
  LineTo(dc, rect.right - 1, rect.bottom - 1);
  LineTo(dc, rect.right - 1, rect.top);
 
  InflateRect(&rect, -1, -1);
  
  // draw the center of the button
  FillRect(dc, &rect, (HBRUSH)(COLOR_MENU + 1));
  int y = (rect.bottom - rect.top) / 2;
  int icon = (layer.IsVisible() ? IDI_LAYER_VISIBLE : IDI_LAYER_INVISIBLE);
  DrawIconEx(
    dc,
    rect.left + y - 8,
    rect.top + y - 8,
    LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(icon)),
    16,
    16,
    0,
    NULL,
    DI_NORMAL);
  rect.left += 18;
  SelectObject(dc, GetStockObject(DEFAULT_GUI_FONT));
  SetTextColor(dc, 0x000000);
  SetBkColor(dc, 0xFFFFFF);
  SetBkMode(dc, TRANSPARENT);
  DrawText(dc, layer.GetName(), strlen(layer.GetName()), &rect, DT_CENTER | DT_VCENTER);
  // clean up
  RestoreDC(dc, -1);
  DeleteObject(very_light);
  DeleteObject(light);
  DeleteObject(dark);
  DeleteObject(very_dark);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CLayerView::OnPaint()
{
  RECT client_rect;
  GetClientRect(&client_rect);
  CPaintDC dc_(this);
  HDC dc = dc_.m_hDC;
  
  for (int i = 0; i < client_rect.bottom / LAYER_BUTTON_HEIGHT + 1; i++) {
    RECT rect = {
      0,
      i * LAYER_BUTTON_HEIGHT,
      client_rect.right,
      (i + 1) * LAYER_BUTTON_HEIGHT
    };
    
    // visibility check
    if (!RectVisible(dc, &rect)) {
      continue;
    }
    // if the layer isn't part of the map, draw an empty rectangle
    if ((i + m_TopLayer) >= m_Map->GetNumLayers()) {
      FillRect(dc, &rect, (HBRUSH)(COLOR_APPWORKSPACE + 1));
      continue;
    }
    // draw layers upside down!
    int layer = m_Map->GetNumLayers() - i - m_TopLayer - 1;
    DrawLayerButton(dc, rect, m_Map->GetLayer(layer), (m_SelectedLayer == layer));
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CLayerView::OnLButtonDown(UINT flags, CPoint point)
{
  Click(point.x, point.y, true);
  m_IsDragging = true;
  SetCapture();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CLayerView::OnLButtonUp(UINT flags, CPoint point)
{
  m_IsDragging = false;
  ReleaseCapture();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CLayerView::OnRButtonUp(UINT flags, CPoint point)
{
  CRect client_rect;
  GetClientRect(&client_rect);
  HMENU _menu = LoadMenu(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_LAYERVIEW));
  HMENU menu = GetSubMenu(_menu, 0);
  // simulate a left click to set the selected layer
  Click(point.x, point.y, false);
  if (AreLayersLockedInPlace()) {
    CheckMenuItem(menu, ID_LAYERVIEW_TOGGLE_LOCK_LAYERS_IN_PLACE, MF_BYCOMMAND | MF_CHECKED);
  }
  // make sure the right things are greyed and stuff
  if (point.x >= 0 &&
      point.x < client_rect.right &&
      point.y >= 0 &&
      point.y < LAYER_BUTTON_HEIGHT * m_Map->GetNumLayers())
  {
    // enable delete command if it's available
    if (m_Map->GetNumLayers() > 1)
      EnableMenuItem(menu, ID_LAYERVIEW_DELETELAYER, MF_BYCOMMAND | MF_ENABLED);
    else
      EnableMenuItem(menu, ID_LAYERVIEW_DELETELAYER, MF_BYCOMMAND | MF_GRAYED);
    int num_visible = 0;
    for (int i = 0; i < m_Map->GetNumLayers(); i++) {
      if (m_Map->GetLayer(i).IsVisible()) {
        num_visible += 1;
      }
    }
    if (num_visible > 0) {
      EnableMenuItem(menu, ID_LAYERVIEW_EXPORT_ALL_VISIBLE_LAYERS_AS_IMAGE, MF_BYCOMMAND | MF_ENABLED);
    }
    else {
      EnableMenuItem(menu, ID_LAYERVIEW_EXPORT_ALL_VISIBLE_LAYERS_AS_IMAGE, MF_BYCOMMAND | MF_GRAYED);
    }
    if (num_visible > 1) {
      EnableMenuItem(menu, ID_LAYERVIEW_FLATTEN_VISIBLE_LAYERS, MF_BYCOMMAND | MF_ENABLED);
    }
    else {
      EnableMenuItem(menu, ID_LAYERVIEW_FLATTEN_VISIBLE_LAYERS, MF_BYCOMMAND | MF_GRAYED);
    }
    if (m_SelectedLayer == m_Map->GetNumLayers() - 1 || !(m_Map->GetNumLayers() > 1)) {
      EnableMenuItem(menu, ID_LAYERVIEW_MOVE_UP, MF_BYCOMMAND | MF_GRAYED);
    }
    if (m_SelectedLayer == 0 || !(m_Map->GetNumLayers() > 1)) {
      EnableMenuItem(menu, ID_LAYERVIEW_MOVE_DOWN, MF_BYCOMMAND | MF_GRAYED);
    }
    EnableMenuItem(menu, ID_LAYERVIEW_PROPERTIES, MF_BYCOMMAND | MF_ENABLED);
    ClientToScreen(&point);
    TrackPopupMenu(menu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, point.x, point.y, 0, m_hWnd, NULL);
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CLayerView::OnMouseMove(UINT flags, CPoint point)
{
  if (m_IsDragging && !AreLayersLockedInPlace()) {
    // see if we're dragging up or down
    point.y -= m_TopLayer * LAYER_BUTTON_HEIGHT;
    int layer = (m_Map->GetNumLayers() * LAYER_BUTTON_HEIGHT - point.y) / LAYER_BUTTON_HEIGHT;
    // drag down
    while (m_SelectedLayer > 0 && layer < m_SelectedLayer) {
      m_Map->SwapLayers(m_SelectedLayer, m_SelectedLayer - 1);
      m_SelectedLayer--;
      Invalidate();
      m_Handler->LV_MapChanged();
      m_Handler->LV_SelectedLayerChanged(m_SelectedLayer);
    }
    // drag up
    while (m_SelectedLayer < m_Map->GetNumLayers() - 1 && layer > m_SelectedLayer) {
      m_Map->SwapLayers(m_SelectedLayer, m_SelectedLayer + 1);
      m_SelectedLayer++;
      Invalidate();
      m_Handler->LV_MapChanged();
      m_Handler->LV_SelectedLayerChanged(m_SelectedLayer);
    }
  }
  int layer = point.y / LAYER_BUTTON_HEIGHT;
  if (layer >= 0 && layer < m_Map->GetNumLayers()) {
    CString str;
    str.Format("Layer '%d'", m_Map->GetNumLayers() - 1 - layer);
    GetStatusBar()->SetWindowText(str);
  }
}
////////////////////////////////////////////////////////////////////////////////
void
CLayerView::OnVScrollChanged(int y)
{
  m_TopLayer = y;
  Invalidate();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CLayerView::OnInsertLayer()
{
  sLayer& sl = m_Map->GetLayer(m_SelectedLayer);
  sLayer layer(sl.GetWidth(), sl.GetHeight());
  layer.SetName("Unnamed Layer");
  m_Map->InsertLayer(m_SelectedLayer + 1, layer); // m_SelectedLayer + 1 = above
  // update visibility buttons
  UpdateScrollBar();
  Invalidate();
  m_Handler->LV_MapChanged();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CLayerView::OnInsertLayerFromImage()
{
  // get image filename
  CImageFileDialog dialog(FDM_OPEN);
  if (dialog.DoModal() != IDOK) {
    return;
  }
  // load image
  CImage32 image;
  if (!image.Load(dialog.GetPathName())) {
    MessageBox("Could not load image", "Insert Layer From Image");
    return;
  }
  // import image into the map
  sMap map;
  if (!map.BuildFromImage(
        image,
        m_Map->GetTileset().GetTileWidth(),
        m_Map->GetTileset().GetTileHeight())
  ) {
    MessageBox("Could not import layer from image", "Insert Layer From Image");
    return;
  }
  int where = m_SelectedLayer + 1;  // above
  m_Map->InsertLayerFromMap(where, map, 0);  
  m_Map->GetLayer(where).SetName(dialog.GetFileName());
  UpdateScrollBar();
  Invalidate();
  m_Handler->LV_MapChanged();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CLayerView::OnDeleteLayer()
{
  int result = MessageBox("Delete associated tiles?", "Delete Layer", MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON3);
  if (result == IDCANCEL) {
    return;
  }
  m_Map->DeleteLayer(m_SelectedLayer, (result == IDYES));
  // make sure the selected layer is still valid
  if (m_SelectedLayer > 0) {
    m_SelectedLayer--;
    m_Handler->LV_SelectedLayerChanged(m_SelectedLayer);
  }
  // make sure the base is a valid layer...
  if (m_Map->GetStartLayer() >= m_Map->GetNumLayers()) {
    m_Map->SetStartLayer(m_Map->GetNumLayers() - 1);
  }
  // make sure the start layer has parallax disabled
  m_Map->GetLayer(m_Map->GetStartLayer()).EnableParallax(false);
  UpdateScrollBar();
  Invalidate();
  m_Handler->LV_MapChanged();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CLayerView::OnDuplicateLayer()
{
  int result = MessageBox("Make a duplicate layer?", "Duplicate Layer", MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON3);
  if (result != IDYES) {
    return;
  }
  m_Map->DuplicateLayer(m_SelectedLayer);
  UpdateScrollBar();
  Invalidate();
  m_Handler->LV_MapChanged();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void 
CLayerView::OnToggleLockLayersInPlace()
{
  ToggleLockLayersInPlace();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CLayerView::OnLayerProperties()
{
  // calculate if parallax should be allowed
  bool parallax_enabled = false;
  for (int i = 0; i < m_Map->GetNumLayers(); i++) {
    if (i != m_SelectedLayer && m_Map->GetLayer(i).HasParallax() == false) {
      parallax_enabled = true;
    }
  }
  parallax_enabled = parallax_enabled && (m_Map->GetStartLayer() != m_SelectedLayer);
  // show the dialog box
  CLayerPropertiesDialog Dialog(m_Map->GetLayer(m_SelectedLayer), parallax_enabled);
  if (Dialog.DoModal() == IDOK) {
    Invalidate();
    m_Handler->LV_MapChanged();
  }
}
////////////////////////////////////////////////////////////////////////////////
static bool LayerToImage(CImage32* image, sLayer& layer, sTileset& tileset)
{
  // if (image.GetWidth() != layer.GetWidth() || image.GetHeight() != layer.GetHeight())
  //  return false;
  RGBA* image_pixels = image->GetPixels();
  int tile_width  = tileset.GetTileWidth();
  int tile_height = tileset.GetTileHeight();
  int image_width  = layer.GetWidth()  * tile_width;
  // int image_height = layer.GetHeight() * tile_height;
  // render the image
  for (int iy = 0; iy < layer.GetHeight(); iy++) {
    for (int ix = 0; ix < layer.GetWidth(); ix++) {
      RGBA* tile_pixels = tileset.GetTile(layer.GetTile(ix, iy)).GetPixels();
      int dsx = ix * tile_width;
      int dsy = iy * tile_height;
      // render this tile into the image
      for (int i = 0; i < tile_height; i++) {
        memcpy(
          image_pixels + (dsy + i) * image_width + dsx,
          tile_pixels + i * tile_width,
          sizeof(RGBA) * tile_width
        );
      }
    } // end for x
  } // end for y
  return true;
}
///////////////////////////////////////////////////////////////////////////////
static bool ExportLayerAsImage(const char* filename, sLayer& layer, sTileset& tileset)
{
  // create image
  int tile_width  = tileset.GetTileWidth();
  int tile_height = tileset.GetTileHeight();
  int image_width  = layer.GetWidth()  * tile_width;
  int image_height = layer.GetHeight() * tile_height;
  CImage32 image(image_width, image_height);
  if (LayerToImage(&image, layer, tileset))
    return image.Save(filename);
  return false;
}
///////////////////////////////////////////////////////////////////////////////
afx_msg void
CLayerView::OnExportLayer()
{
  // get file name to export to
  CImageFileDialog dialog(FDM_SAVE, "Export Layer as Image");
  if (dialog.DoModal() == IDOK) {
    sLayer& layer = m_Map->GetLayer(m_SelectedLayer);
    sTileset& tileset = m_Map->GetTileset();
    // do the export
    if (!ExportLayerAsImage(dialog.GetPathName(), layer, tileset)) {
      MessageBox("Could not save image", "Export Layer as Image", MB_OK);
    } else {
      MessageBox("Exported layer!", "Export Layer as Image", MB_OK);
    }
  }
}
///////////////////////////////////////////////////////////////////////////////
static bool ExportAllVisibleLayersAsImage(const char* filename, sTileset& tileset, std::vector<sLayer> layers)
{
  if (!(layers.size() >= 1))
    return false;
  
  int tile_width  = tileset.GetTileWidth();
  int tile_height = tileset.GetTileHeight();
  int dest_image_width = 0;
  int dest_image_height = 0;
  unsigned int i;

  // find the size of the image we're going to create
  for (i = 0; i < layers.size(); i++) {
    if (layers[i].GetWidth() * tile_width > dest_image_width) {
      dest_image_width = layers[i].GetWidth() * tile_width;
    }
    if (layers[i].GetHeight() * tile_height > dest_image_height) {
      dest_image_height = layers[i].GetHeight() * tile_height;
    }
  }
  if (dest_image_width <= 0 || dest_image_height <= 0)
    return false;
  // create destination/output image
  CImage32 dest_image(dest_image_width, dest_image_height);
  if (layers.size() > 0) { // start from the bottom and work our way to the top ;)
    if (!LayerToImage(&dest_image, layers[0], tileset)) {
      return false;
    }
  }
  else // nothing to export
    return false;
  // we already have the first layer, now we do the rest
  for (i = 1; i < layers.size(); i++) {
    int image_width  = layers[i].GetWidth()  * tile_width;
    int image_height = layers[i].GetHeight() * tile_height;
    CImage32 src_image(image_width, image_height);
    
    if ( !LayerToImage(&src_image, layers[i], tileset) ) {
      return false;
    }
    else {
      // blend (dest_image, src_image)
      BlendImage(dest_image_width, dest_image_height, image_width, image_height, dest_image.GetPixels(), src_image.GetPixels());
    }
  }
 
  return dest_image.Save(filename);
}
///////////////////////////////////////////////////////////////////////////////
afx_msg void
CLayerView::OnExportAllVisibleLayers()
{
  // get file name to export to
  CImageFileDialog dialog(FDM_SAVE, "Export All Visible Layers as Image");
  if (dialog.DoModal() == IDOK) {
    sTileset& tileset = m_Map->GetTileset();
    std::vector<sLayer> layers;
    // get a list of the visible layers
    for (int i = 0; i < m_Map->GetNumLayers(); i++) {
      if (m_Map->GetLayer(i).IsVisible()) {
        layers.push_back(m_Map->GetLayer(i));
      }
    }
    if (layers.size() <= 0)
      MessageBox("No layers are visible!", "No layers are visible", MB_OK);
    else {
      // do the export
      if (!ExportAllVisibleLayersAsImage(dialog.GetPathName(), tileset, layers)) {
        MessageBox("Could not save image", "Export All Visible Layers as Image", MB_OK);
      } else {
        MessageBox("Exported all visible layers!", "Exported all Visible Layers as Image", MB_OK);
      }
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
/**
  This takes all the visible layers, and merges them into one,
  leaving the old layers in tact (but invisible)
*/
afx_msg void
CLayerView::OnFlattenVisibleLayers()
{
  unsigned int i;

  std::vector<sLayer> layers;
  // get a list of the visible layers
  for (i = 0; i < (unsigned int)m_Map->GetNumLayers(); i++) {
    if (m_Map->GetLayer(i).IsVisible()) {
      layers.push_back(m_Map->GetLayer(i));
    }
  }
  if (!(layers.size() >= 1)) {
    MessageBox("No layers are visible!", "No layers are visible", MB_OK);
    return;
  }
  else 
  if (layers.size() == 1) {
    return;
  }
  int result = MessageBox("Flatten all visible layers?", "Flatten Layers", MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON3);
  if (result != IDYES) {
    return;
  }
  int width = 0;
  int height = 0;
  // find the size of the image we're going to create
  for (i = 0; i < layers.size(); i++) {
    if (layers[i].GetWidth() > width) {
      width = layers[i].GetWidth();
    }
    if (layers[i].GetHeight() > height) {
      height = layers[i].GetHeight();
    }
  }
  if (width <= 0 || height <= 0)
    return;
  sLayer new_layer;
  new_layer.SetName("flattened_layer");
  new_layer.Resize(width, height);
  sTileset& tileset = m_Map->GetTileset();
  sTile new_tile(tileset.GetTileWidth(), tileset.GetTileHeight());
  std::vector<int> blended_tile_indexes;
  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x) {
      new_tile.Clear();
      blended_tile_indexes.clear();
      for (unsigned int i = 0; i < layers.size(); ++i) {
        if (y < layers[i].GetHeight() && x < layers[i].GetWidth()) {
          sTile& tile = tileset.GetTile(layers[i].GetTile(x, y));
          bool is_empty = true;
          for (int iy = 0; iy < tile.GetHeight(); ++iy) {
            for (int ix = 0; ix < tile.GetWidth(); ++ix) {
              if (tile.GetPixel(ix, iy).alpha != 0) {
                is_empty = false;
                break;
              }
            }
          }
          if (is_empty == false) {
            blended_tile_indexes.push_back(layers[i].GetTile(x, y));
            BlendImage(new_tile.GetWidth(), new_tile.GetHeight(), tile.GetWidth(), tile.GetHeight(), new_tile.GetPixels(), tile.GetPixels());
          }
        }
      }
      if (blended_tile_indexes.size() == 1) {
        new_layer.SetTile(x, y, blended_tile_indexes[0]);
      }
      else {
        int tile_index = -1;
        // make the tile fully opaque
        new_tile.SetAlpha(255, true);
        // see if the new_tile already exists within the tileset
        for (int i = 0; i < tileset.GetNumTiles(); ++i) {
          if (tileset.GetTile(i) == new_tile) {
            tile_index = i;
            break;
          }
        } 
        
        if (tile_index == -1) {
          tile_index = tileset.GetNumTiles();
          tileset.AppendTiles(1);
          tileset.GetTile(tile_index) = new_tile;
        }
        new_layer.SetTile(x, y, tile_index);                 
      }
    }
  }
  // turn the layers that were visible off
  for (i = 0; i < layers.size(); i++) {
    m_Map->GetLayer(i).SetVisible(false);
  }
  m_Map->AppendLayer(new_layer);
    
  Invalidate();
  m_Handler->LV_MapChanged();
}
////////////////////////////////////////////////////////////////////////////////
void
CLayerView::Click(int x, int y, bool left)
{
  CRect client_rect;
  GetClientRect(&client_rect);
  if (x >= 0 &&
      x < client_rect.right &&
      y >= 0 &&
      y < LAYER_BUTTON_HEIGHT * m_Map->GetNumLayers()
  ) {
    y -= m_TopLayer * LAYER_BUTTON_HEIGHT;
    int layer = (m_Map->GetNumLayers() * LAYER_BUTTON_HEIGHT - y) / LAYER_BUTTON_HEIGHT;
    if ( !(layer >= 0 && layer < m_Map->GetNumLayers()) )
      return;
    // if x is less than 20, we clicked on the visibility button
    if (x < 20 && left == true) {
      sLayer& l = m_Map->GetLayer(layer);
      l.SetVisible(!l.IsVisible());
      Invalidate();
      m_Handler->LV_MapChanged();
    } else {
      if (!AreLayersLockedInPlace()) {
        // layers are displayed upside down
        m_SelectedLayer = layer;
        Invalidate();
        // tell parent that the layer changed
        m_Handler->LV_SelectedLayerChanged(m_SelectedLayer);
      }
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
void
CLayerView::UpdateScrollBar()
{
  RECT client_rect;
  GetClientRect(&client_rect);
  int page_size = client_rect.bottom / LAYER_BUTTON_HEIGHT;
  // set the scroll range
  SetVScrollRange(m_Map->GetNumLayers(), page_size);
}
////////////////////////////////////////////////////////////////////////////////
bool
CLayerView::AreLayersLockedInPlace()
{
  return m_LayersAreLockedInPlace;
}
////////////////////////////////////////////////////////////////////////////////
void
CLayerView::ToggleLockLayersInPlace()
{
  m_LayersAreLockedInPlace = !m_LayersAreLockedInPlace;
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CLayerView::OnLayerMoveUp()
{
  if (m_SelectedLayer + 1 < m_Map->GetNumLayers()) {
    m_Map->SwapLayers(m_SelectedLayer, m_SelectedLayer + 1);
    m_SelectedLayer++;
    Invalidate();
    m_Handler->LV_MapChanged();
    m_Handler->LV_SelectedLayerChanged(m_SelectedLayer);
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CLayerView::OnLayerMoveDown()
{
  if (m_SelectedLayer - 1 >= 0) {
    m_Map->SwapLayers(m_SelectedLayer, m_SelectedLayer - 1);
    m_SelectedLayer--;
    Invalidate();
    m_Handler->LV_MapChanged();
    m_Handler->LV_SelectedLayerChanged(m_SelectedLayer);
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CLayerView::OnLayerSlideUp() {
  m_Map->Translate(0, -1, m_SelectedLayer);
  Invalidate();
  m_Handler->LV_MapChanged();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CLayerView::OnLayerSlideRight() {
  m_Map->Translate(1, 0, m_SelectedLayer);
  Invalidate();
  m_Handler->LV_MapChanged();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CLayerView::OnLayerSlideDown() {
  m_Map->Translate(0, 1, m_SelectedLayer);
  Invalidate();
  m_Handler->LV_MapChanged();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CLayerView::OnLayerSlideLeft() {
  m_Map->Translate(-1, 0, m_SelectedLayer);
  Invalidate();
  m_Handler->LV_MapChanged();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CLayerView::OnLayerSlideOther()
{
  char horizontal_title[1024] = {0};
  char vertical_title[1024] = {0};
  sprintf (horizontal_title, "Slide Horizontally [%d - %d]", -m_Map->GetLayer(m_SelectedLayer).GetWidth(), m_Map->GetLayer(m_SelectedLayer).GetWidth());
  sprintf (vertical_title,   "Slide Vertically [%d - %d]", -m_Map->GetLayer(m_SelectedLayer).GetHeight(), m_Map->GetLayer(m_SelectedLayer).GetHeight());
  CNumberDialog dx(horizontal_title, "Value", 0, -m_Map->GetLayer(m_SelectedLayer).GetWidth(), m_Map->GetLayer(m_SelectedLayer).GetWidth()); 
  if (dx.DoModal() == IDOK) {
    CNumberDialog dy(vertical_title, "Value", 0, -m_Map->GetLayer(m_SelectedLayer).GetHeight(), m_Map->GetLayer(m_SelectedLayer).GetHeight()); 
    if (dy.DoModal() == IDOK) {
      if (dx.GetValue() != 0 || dy.GetValue() != 0) {
        m_Map->Translate(dx.GetValue(), dy.GetValue(), m_SelectedLayer);
        Invalidate();
        m_Handler->LV_MapChanged();
      }
    }
  }
}
////////////////////////////////////////////////////////////////////////////////