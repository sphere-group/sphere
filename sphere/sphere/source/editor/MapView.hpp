#ifndef MAP_VIEW_HPP
#define MAP_VIEW_HPP
#include "ScrollWindow.hpp"
#include <afxext.h>
#include "../common/Map.hpp"
#include "../common/Tileset.hpp"
#include "DIBSection.hpp"
#include "MapToolPalette.hpp"
#include "Clipboard.hpp"

#include "MapScripting.hpp"
class IMapViewHandler
{
public:
  virtual void MV_MapChanged() = 0;
  virtual void MV_SelectedTileChanged(int tile) = 0;
};

class CMapView : public CScrollWindow, public IToolPaletteHandler
{
public:
  CMapView();
  ~CMapView();
  BOOL Create(CDocumentWindow* owner, IMapViewHandler* handler, CWnd* parent, sMap* map);
  void SetZoomFactor(double factor);
  double  GetZoomFactor();
  void TilesetChanged();
  int GetSelectedTile();
  void SelectTile(int tile);
  void SelectLayer(int layer);
  void UpdateScrollBars();
  void SetTileSelection(int width, int height, unsigned int* tiles);

private:
  int m_MultiTileWidth;
  int m_MultiTileHeight;
  unsigned int* m_MultiTileData;

private:
  int GetPageSizeX();
  int GetPageSizeY();
  int GetTotalTilesX();
  int GetTotalTilesY();

  int RoundX(int x);
  int RoundY(int y);

  void Click(CPoint point);
  bool __SetTile__(int tx, int ty, int layer_index, int new_tile);
  bool SetTile(int tx, int ty);
  void SelectTileUnderPoint(CPoint point);
  void PasteEntityUnderPoint(CPoint point);
  void MapAreaCopy();
  void LayerAreaCopy();
  void PasteMapUnderPoint(CPoint point);
  void FillMe(int x, int y, int layer, int tileToReplace);
  void FillRectArea();
  void FillArea();
  void EntityCopy(CPoint point);
  void EntityPaste(CPoint point);
  std::string GenerateUniquePersonName(std::string current_name, std::string filename);

  bool IsWithinSelectFillArea(int x, int y);
  void DrawTile(CDC& dc, const RECT& rect, int tx, int ty);
  void DrawObstructions(CDC& dc);
  void DrawZones(CDC& dc);
  void DrawPreviewLine(CDC& dc, int x1, int y1, int x2, int y2);
  void DrawPreviewBox(CDC& dc, int x1, int y1, int x2, int y2);

  void UpdateObstructionTile(int tile);
  void UpdateObstructionTiles();

	//void InitAnimations();
  int FindSpritesetImageIconsIndex(int person_index, std::string filename);
private:
  afx_msg void OnDestroy();
  afx_msg void OnPaint();
  afx_msg void OnSize(UINT type, int cx, int cy);
  afx_msg void OnLButtonDown(UINT flags, CPoint point);
  afx_msg void OnMouseMove(UINT flags, CPoint point);
  afx_msg void OnLButtonUp(UINT flags, CPoint point);
  afx_msg void OnRButtonUp(UINT flags, CPoint point);
  //afx_msg void OnTimer(UINT event);

  virtual void OnHScrollChanged(int x);
  virtual void OnVScrollChanged(int y);
  virtual void TP_ToolSelected(int tool, int tool_index);

  void UpdateCursor(UINT flags, CPoint point);
public:
  void OnToolChanged(UINT id, int tool_index);
  BOOL IsToolAvailable(UINT id);

public:
  enum editMode {
    tool_1x1Tile,
    tool_3x3Tile,
    tool_5x5Tile,
    tool_SelectTile,
    tool_FillRectArea,
    tool_FillArea,
    tool_FillLayer,
    tool_CopyArea,
    tool_Paste,
    tool_MoveEntity,
    tool_ObsSegment,
    tool_ObsDeleteSegment,
	tool_ObsMoveSegmentPoint,
    tool_ZoneAdd,
    tool_ZoneEdit,
    tool_ZoneMove,
    tool_ZoneDelete,
    tool_Script,
  };

private:
  sMapScripting m_Scripter;
  bool m_script_running;
  void ExecuteScript(const char* string);

private:
  static DWORD WINAPI ThreadRoutine(LPVOID parameter);

public:
  bool SetTile(int x, int y, int layer, int tile) {
    if ( !(layer >= 0 && layer < m_Map->GetNumLayers()) ) {
      return false;
    }

    if ( !(x >= 0 && y >= 0 && x < m_Map->GetLayer(layer).GetWidth() && y < m_Map->GetLayer(layer).GetHeight()) ) {
      return false;
    }

    if ( !(tile >= 0 && tile < m_Map->GetTileset().GetNumTiles()) ) {
      return false;
    }

    __SetTile__(x, y, layer, tile);
    return true;
  }

  bool GetTile(int x, int y, int layer, int& tile) {
    if ( !(layer >= 0 && layer < m_Map->GetNumLayers()) ) {
      return false;
    }

    if ( !(x >= 0 && y >= 0 && x < m_Map->GetLayer(layer).GetWidth() && y < m_Map->GetLayer(layer).GetHeight()) ) {
      return false;
    }

    tile = m_Map->GetLayer(layer).GetTile(x, y);
    return true;
  }

  bool GetTileWidth(int& width) {
    width = m_Map->GetTileset().GetTileWidth();
    return true;
  }

  bool GetTileHeight(int& height) {
    height = m_Map->GetTileset().GetTileHeight();
    return true;
  }

  bool GetNumTiles(int& num_tiles) {
    num_tiles = m_Map->GetTileset().GetNumTiles();
    return true;
  }

  bool GetLayerWidth(const int layer, int& width) {
    if ( !(layer >= 0 && layer < m_Map->GetNumLayers()) ) {
      return false;
    }

    width = m_Map->GetLayer(layer).GetWidth();
    return true;
  }

  bool GetLayerHeight(const int layer, int& height) {
    if ( !(layer >= 0 && layer < m_Map->GetNumLayers()) ) {
      return false;
    }

    height = m_Map->GetLayer(layer).GetHeight();
    return true;
  }

  bool GetNumLayers(int& num_layers) {
    num_layers = m_Map->GetNumLayers();
    return true;
  }

  bool GetLayerScaleFactorX(const int layer, double& zoom_factor) {
    if ( !(layer >= 0 && layer < m_Map->GetNumLayers()) ) {
      return false;
    }

    zoom_factor = m_ZoomFactor;
    return true;
  }

  bool GetLayerScaleFactorY(const int layer, double& zoom_factor) {
    if ( !(layer >= 0 && layer < m_Map->GetNumLayers()) ) {
      return false;
    }

    zoom_factor = m_ZoomFactor;
    return true;
  }

  bool MapToScreenX(const int layer, const int mx, int& sx) {
    if ( !(layer >= 0 && layer < m_Map->GetNumLayers()) ) {
      return false;
    }

    sx = (int) (mx - ((m_CurrentX * m_Map->GetTileset().GetTileWidth()) * m_ZoomFactor));
    return true;
  }

  bool MapToScreenY(const int layer, const int my, int& sy) {
    if ( !(layer >= 0 && layer < m_Map->GetNumLayers()) ) {
      return false;
    }

    sy = (int) (my - ((m_CurrentY * m_Map->GetTileset().GetTileHeight()) * m_ZoomFactor));
    return true;
  }

  bool ScreenToMapX(const int layer, const int sx, int& mx) {
    if ( !(layer >= 0 && layer < m_Map->GetNumLayers()) ) {
      return false;
    }

    mx = (int) (sx + ((m_CurrentX * m_Map->GetTileset().GetTileWidth()) * m_ZoomFactor));
    return true;
  }

  bool ScreenToMapY(const int layer, const int sy, int& my) {
    if ( !(layer >= 0 && layer < m_Map->GetNumLayers()) ) {
      return false;
    }

    my = (int) (sy + ((m_CurrentY * m_Map->GetTileset().GetTileHeight()) * m_ZoomFactor));
    return true;
  }

  bool IsMouseButtonPressed(int button) {
    bool pressed = false;
    if (button == 0)
      pressed = m_Clicked;
    return pressed;
  }

  int GetScreenWidth() {
    RECT ClientRect;
    GetClientRect(&ClientRect);
    return ClientRect.right;
  }

  int GetScreenHeight() {
    RECT ClientRect;
    GetClientRect(&ClientRect);
    return ClientRect.bottom;
  }

private:
  IMapViewHandler* m_Handler;

  //CMapToolPalette* m_ToolPalette;
  sMap* m_Map;
  CDIBSection* m_BlitTile;  // for rendering tiles
  CClipboard* m_Clipboard;

  double m_ZoomFactor;
  RGB m_ObstructionColor;
  RGB m_HighlightColor;
  RGB m_ZoneColor;
  int m_CurrentTool;
  int m_PreviousTool;

  int m_CurrentX;
  int m_CurrentY;
  int m_CurrentCursorTileX;
  int m_CurrentCursorTileY;
  int m_StartCursorTileX;
  int m_StartCursorTileY;

  int m_StartX;
  int m_StartY;

  int m_MoveIndex; // index of the object we are moving currently
  int m_PreviewX;
  int m_PreviewY;
  int m_PreviewOldX;
  int m_PreviewOldY;
  int m_PreviewLineOn;
  int m_PreviewBoxOn;

  int m_RedrawWindow;
  int m_RedrawPreviewLine;

  int m_SelectedTile;
  int m_SelectedLayer;

  bool m_Clicked;
  int m_ViewGridType;
  bool m_ShowTileObstructions;
  std::vector<sTile> m_TileObstructions; // when View->Tile Obstructions is on, we render these
	//bool m_ShowAnimations;

  //UINT m_Timer;
	//int  m_FrameTick;

  void GetRedrawRect(int& offset_x, int& offset_y, int& width, int& height);
  enum SpritesetDrawTypes {
    SDT_ICON,
    SDT_MINI_IMAGE,
    SDT_IMAGE,
  };
  unsigned char m_SpritesetDrawType;

  class SpritesetImageIcon {
    public:
      SpritesetImageIcon() {
        created = false;
      }
      std::string filename;
      CImage32 image;
      CImage32 icon;
      int x1;
      int y1;
      int x2;
      int y2;
      bool created;
  };

  std::vector<SpritesetImageIcon> m_SpritesetImageIcons;
  DECLARE_MESSAGE_MAP()
};

#endif
