#ifndef TILE_OBSTRUCTION_DIALOG_HPP
#define TILE_OBSTRUCTION_DIALOG_HPP
#include <afxwin.h>
#include "TileObstructionView.hpp"
#include "../common/Tileset.hpp"
class CTileObstructionDialog : public CDialog
{
public:
  CTileObstructionDialog(sTileset* tileset, sTile* tiles, int tile_index);
private:
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnSize(UINT type, int cx, int cy);
  afx_msg void OnPresets();
  afx_msg void OnPresetUnblocked();
  afx_msg void OnPresetBlocked();
  afx_msg void OnPresetUpperRight();
  afx_msg void OnPresetLowerRight();
  afx_msg void OnPresetLowerLeft();
  afx_msg void OnPresetUpperLeft();
  afx_msg void OnPresetTopHalf();
  afx_msg void OnPresetBottomHalf();
  afx_msg void OnPresetLeftHalf();
  afx_msg void OnPresetRightHalf();
  afx_msg void OnPresetCustom();
  afx_msg void OnFileSave();
private:
  bool StoreTile();
  afx_msg void OnNext();
  afx_msg void OnPrevious();
private:
  int m_current_tile;
  sTileset* m_tileset;
  sTile* m_tiles;
  std::vector<sObstructionMap> m_obstruction_maps;
  sTile m_edit_tile;  // the tile you edit
  CTileObstructionView m_obstruction_view;
  DECLARE_MESSAGE_MAP()
};
#endif
