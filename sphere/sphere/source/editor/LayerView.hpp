#ifndef LAYER_VIEW_HPP
#define LAYER_VIEW_HPP
#include <vector>
#include "VScrollWindow.hpp"
#include <afxext.h>
class sMap;  // #include "../sphere/common/Map.hpp"
class ILayerViewHandler
{
public:
  virtual void LV_MapChanged() = 0;
  virtual void LV_SelectedLayerChanged(int layer) = 0;
};
class CLayerView : public CVScrollWindow
{
public:
  CLayerView();
  ~CLayerView();
  BOOL Create(ILayerViewHandler* handler, CWnd* parent, sMap* map);
  int GetSelectedLayer() const;
  void SetSelectedTile(int tile);
  bool AreLayersLockedInPlace();
  void ToggleLockLayersInPlace();
private:
  afx_msg int OnCreate(CREATESTRUCT* createstruct);
  afx_msg void OnSize(UINT type, int cx, int cy);
  afx_msg void OnPaint();
  afx_msg void OnLButtonDown(UINT flags, CPoint point);
  afx_msg void OnLButtonUp(UINT flags, CPoint point);
  afx_msg void OnRButtonUp(UINT flags, CPoint point);
  afx_msg void OnMouseMove(UINT flags, CPoint point);
  virtual void OnVScrollChanged(int y);
  afx_msg void OnInsertLayer();
  afx_msg void OnInsertLayerFromImage();
  afx_msg void OnDeleteLayer();
  afx_msg void OnDuplicateLayer();
  afx_msg void OnLayerProperties();
  afx_msg void OnExportLayer();
  afx_msg void OnExportAllVisibleLayers();
  afx_msg void OnToggleLockLayersInPlace();
  afx_msg void OnFlattenVisibleLayers();
  afx_msg void OnLayerSlideUp();
  afx_msg void OnLayerSlideRight();
  afx_msg void OnLayerSlideDown();
  afx_msg void OnLayerSlideLeft();
  afx_msg void OnLayerSlideOther();
  afx_msg void OnLayerMoveUp();
  afx_msg void OnLayerMoveDown();
  void Click(int x, int y, bool left);
  void UpdateScrollBar();
private:
  ILayerViewHandler* m_Handler;
  sMap* m_Map;
  int m_TopLayer; // for scrolling (btw, this is upside down)
  int m_SelectedLayer;
  int m_SelectedTile;
  bool m_IsDragging;
  bool m_LayersAreLockedInPlace;
  DECLARE_MESSAGE_MAP();
};
#endif
