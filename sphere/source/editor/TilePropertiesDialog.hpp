#ifndef TILE_PROPERTIES_DIALOG_HPP
#define TILE_PROPERTIES_DIALOG_HPP
#include <afxwin.h>
#include "../common/Tileset.hpp"
class CTilePropertiesDialog : public CDialog
{
public:
  CTilePropertiesDialog(sTileset* tileset, int tile);
  ~CTilePropertiesDialog();
private:
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnAnimatedClicked();
  afx_msg void OnNextTile();
  afx_msg void OnPrevTile();
  afx_msg void OnEditObstructions();
private:
  void UpdateDialog();
  bool StoreCurrentTile();
private:
  sTileset* m_Tileset;
  sTile*    m_Tiles;  // modified by dialog
  int       m_Tile;
  DECLARE_MESSAGE_MAP()
};
#endif
