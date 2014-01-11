#include "TilePropertiesDialog.hpp"
#include "TileObstructionDialog.hpp"
#include "../common/Tileset.hpp"
#include "resource.h"
BEGIN_MESSAGE_MAP(CTilePropertiesDialog, CDialog)
  ON_BN_CLICKED(IDC_ANIMATED, OnAnimatedClicked)
  
  ON_BN_CLICKED(IDC_NEXT,     OnNextTile)
  ON_BN_CLICKED(IDC_PREVIOUS, OnPrevTile)
  ON_BN_CLICKED(IDC_EDIT_OBSTRUCTIONS, OnEditObstructions)
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////
CTilePropertiesDialog::CTilePropertiesDialog(sTileset* tileset, int tile)
: CDialog(IDD_TILE_PROPERTIES)
, m_Tileset(tileset)
, m_Tiles(NULL)
, m_Tile(tile)
{
  // dialog will modify these items
  // they will be set into tileset when OK is pressed
  m_Tiles = new sTile[m_Tileset->GetNumTiles()];
  if (m_Tiles) {
    for (int i = 0; i < m_Tileset->GetNumTiles(); i++)
      m_Tiles[i] = m_Tileset->GetTile(i);
  }
}
////////////////////////////////////////////////////////////////////////////////
CTilePropertiesDialog::~CTilePropertiesDialog()
{
  if (m_Tiles) {
    delete[] m_Tiles;
  }
  m_Tiles = NULL;
}
////////////////////////////////////////////////////////////////////////////////
BOOL
CTilePropertiesDialog::OnInitDialog()
{
  UpdateDialog();
  return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
void
CTilePropertiesDialog::OnOK()
{
  if (StoreCurrentTile())
  {
    if (m_Tiles) {
      for (int i = 0; i < m_Tileset->GetNumTiles(); i++)
        m_Tileset->GetTile(i) = m_Tiles[i];
    }
    CDialog::OnOK();
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilePropertiesDialog::OnAnimatedClicked()
{
  if (IsDlgButtonChecked(IDC_ANIMATED) == BST_CHECKED)  // if it's checked
  {
    GetDlgItem(IDC_NEXT_TILE)->EnableWindow(TRUE);
    GetDlgItem(IDC_DELAY)->EnableWindow(TRUE);
    // if the next tile is zero or invalid, set the tile to default tile animation
    if (m_Tiles[m_Tile].GetNextTile() <= 0 || m_Tiles[m_Tile].GetNextTile() >= m_Tileset->GetNumTiles())
    {
      int tile = m_Tile + 1;
      if (tile >= m_Tileset->GetNumTiles())
        tile = 0;
      m_Tiles[m_Tile].SetNextTile(tile);
      m_Tiles[m_Tile].SetDelay(8);
      m_Tiles[m_Tile].SetAnimated(true);
      UpdateDialog();
    }
  }
  else
  {
    GetDlgItem(IDC_NEXT_TILE)->EnableWindow(FALSE);
    GetDlgItem(IDC_DELAY)->EnableWindow(FALSE);
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilePropertiesDialog::OnNextTile()
{
  if (StoreCurrentTile())
  {
    m_Tile++;
    if (m_Tile > m_Tileset->GetNumTiles() - 1)
      m_Tile = 0;
    UpdateDialog();
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilePropertiesDialog::OnPrevTile()
{
  if (StoreCurrentTile())
  {
    m_Tile--;
    if (m_Tile < 0)
      m_Tile = m_Tileset->GetNumTiles() - 1;
    UpdateDialog();
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilePropertiesDialog::OnEditObstructions()
{
  if (!m_Tiles)
    return;
  CTileObstructionDialog dialog(m_Tileset, m_Tiles, m_Tile);
  dialog.DoModal();
}
////////////////////////////////////////////////////////////////////////////////
void
CTilePropertiesDialog::UpdateDialog()
{
  CString title;
  title.Format("Tile Properties - %d/%d", m_Tile, m_Tileset->GetNumTiles());
  SetWindowText(title);
  if (!m_Tiles)
    return;
  GetDlgItem(IDC_NEXT)->EnableWindow(m_Tileset->GetNumTiles() > 1 ? TRUE : FALSE);
  GetDlgItem(IDC_PREVIOUS)->EnableWindow(m_Tileset->GetNumTiles() > 1 ? TRUE : FALSE);
  // put default values in
  sTile& tile = m_Tiles[m_Tile];
  // next tile
  SetDlgItemInt(IDC_NEXT_TILE, tile.GetNextTile());
  // delay
  SetDlgItemInt(IDC_DELAY, tile.GetDelay());
  // name
  SetDlgItemText(IDC_TILENAME, tile.GetName().c_str());
  // animation flag
  if (tile.IsAnimated())
  {
    CheckDlgButton(IDC_ANIMATED, BST_CHECKED);
    GetDlgItem(IDC_NEXT_TILE)->EnableWindow(TRUE);
    GetDlgItem(IDC_DELAY)->EnableWindow(TRUE);
  }
  else
  {
    CheckDlgButton(IDC_ANIMATED, BST_UNCHECKED);
    GetDlgItem(IDC_NEXT_TILE)->EnableWindow(FALSE);
    GetDlgItem(IDC_DELAY)->EnableWindow(FALSE);
  }
}
////////////////////////////////////////////////////////////////////////////////
bool
CTilePropertiesDialog::StoreCurrentTile()
{
  if (!m_Tiles)
    return false;
  bool animated = (IsDlgButtonChecked(IDC_ANIMATED) == BST_CHECKED);
  int nexttile = GetDlgItemInt(IDC_NEXT_TILE);
  int delay    = GetDlgItemInt(IDC_DELAY);
  CString name;
  GetDlgItemText(IDC_TILENAME, name);
  if (animated == true &&
      (nexttile < 0 ||
       nexttile >= m_Tileset->GetNumTiles()))
  {
    char error_message[80];
    sprintf(error_message, "Next tile must be between 0 and %d", m_Tileset->GetNumTiles() - 1);
    MessageBox(error_message);
    return false;
  }
  if (animated && delay == 0)
  {
    MessageBox("Delay must be greater than zero");
    return false;
  }
  // put default values in
  m_Tiles[m_Tile].SetAnimated(animated);
  m_Tiles[m_Tile].SetNextTile(nexttile);
  m_Tiles[m_Tile].SetDelay(delay);
  m_Tiles[m_Tile].SetName(std::string(name));
  return true;
}
////////////////////////////////////////////////////////////////////////////////
