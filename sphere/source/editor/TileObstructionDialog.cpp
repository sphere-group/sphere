#include "TileObstructionDialog.hpp"
#include "resource.h"
#include <stdio.h>
#if 0
#include "FileDialogs.hpp"
#endif

BEGIN_MESSAGE_MAP(CTileObstructionDialog, CDialog)
  ON_WM_SIZE()
  ON_COMMAND(IDC_PRESETS, OnPresets)
  
  ON_COMMAND(ID_OBSTRUCTIONPRESETS_UNBLOCKED,  OnPresetUnblocked)
  ON_COMMAND(ID_OBSTRUCTIONPRESETS_BLOCKED,    OnPresetBlocked)
  ON_COMMAND(ID_OBSTRUCTIONPRESETS_UPPERRIGHT, OnPresetUpperRight)
  ON_COMMAND(ID_OBSTRUCTIONPRESETS_LOWERRIGHT, OnPresetLowerRight)
  ON_COMMAND(ID_OBSTRUCTIONPRESETS_LOWERLEFT,  OnPresetLowerLeft)
  ON_COMMAND(ID_OBSTRUCTIONPRESETS_UPPERLEFT,  OnPresetUpperLeft)

  ON_COMMAND(ID_OBSTRUCTIONPRESETS_LEFTHALF,    OnPresetLeftHalf)
  ON_COMMAND(ID_OBSTRUCTIONPRESETS_RIGHTHALF,   OnPresetRightHalf)
  ON_COMMAND(ID_OBSTRUCTIONPRESETS_TOPHALF,     OnPresetTopHalf)
  ON_COMMAND(ID_OBSTRUCTIONPRESETS_BOTTOMHALF,  OnPresetBottomHalf)

  ON_COMMAND(ID_OBSTRUCTIONPRESETS_CUSTOM1,  OnPresetCustom)
  ON_COMMAND(ID_OBSTRUCTIONPRESETS_CUSTOM2,  OnPresetCustom)
  ON_COMMAND(ID_OBSTRUCTIONPRESETS_CUSTOM3,  OnPresetCustom)
  ON_COMMAND(ID_OBSTRUCTIONPRESETS_CUSTOM4,  OnPresetCustom)
  ON_COMMAND(ID_OBSTRUCTIONPRESETS_CUSTOM5,  OnPresetCustom)
  ON_COMMAND(ID_OBSTRUCTIONPRESETS_CUSTOM6,  OnPresetCustom)
  ON_COMMAND(ID_OBSTRUCTIONPRESETS_CUSTOM7,  OnPresetCustom)
  ON_COMMAND(ID_OBSTRUCTIONPRESETS_CUSTOM8,  OnPresetCustom)

  ON_COMMAND(IDC_NEXT, OnNext)
  ON_COMMAND(IDC_PREVIOUS, OnPrevious)

#if 0
  ON_COMMAND(ID_OBSTRUCTIONPRESETS_SAVE,        OnFileSave)
#endif

END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////
CTileObstructionDialog::CTileObstructionDialog(sTileset* tileset, sTile* tiles, int tile_index)
: CDialog(IDD_TILE_OBSTRUCTION_DIALOG)
, m_tileset(tileset)
, m_tiles(tiles)
, m_current_tile(tile_index)
{
  m_edit_tile = m_tiles[m_current_tile];
  m_obstruction_maps.resize(tileset->GetNumTiles());

  for (unsigned int i = 0; i < m_obstruction_maps.size(); i++) {
    m_obstruction_maps[i] = tiles[i].GetObstructionMap();
  }
}

////////////////////////////////////////////////////////////////////////////////
BOOL
CTileObstructionDialog::OnInitDialog()
{
  m_obstruction_view.Create(this, &m_edit_tile);
  
  RECT rect;
  GetClientRect(&rect);
  OnSize(0, rect.right, rect.bottom);

  GetDlgItem(IDC_NEXT)->EnableWindow(m_tileset->GetNumTiles() > 1 ? TRUE : FALSE);
  GetDlgItem(IDC_PREVIOUS)->EnableWindow(m_tileset->GetNumTiles() > 1 ? TRUE : FALSE);

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
void
CTileObstructionDialog::OnOK()
{
  for (unsigned int i = 0; i < m_obstruction_maps.size(); i++) {
    m_tiles[i].GetObstructionMap() = m_obstruction_maps[i];
  }

  if (StoreTile())
    CDialog::OnOK();
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTileObstructionDialog::OnSize(UINT type, int cx, int cy)
{
  const int button_width  = 60;
  const int button_height = 30;

  // move the controls around
  if (m_obstruction_view.m_hWnd) {
    m_obstruction_view.MoveWindow(0, 0, cx - button_width, cy);
  }

  if (GetDlgItem(IDOK)) {
    GetDlgItem(IDOK)->MoveWindow(cx - button_width, 0, button_width, button_height);
  }

  if (GetDlgItem(IDCANCEL)) {
    GetDlgItem(IDCANCEL)->MoveWindow(cx - button_width, button_height, button_width, button_height);
  }

  if (GetDlgItem(IDC_PRESETS)) {
    GetDlgItem(IDC_PRESETS)->MoveWindow(cx - button_width, button_height * 3, button_width, button_height);
  }

  if (GetDlgItem(IDC_NEXT)) {
    GetDlgItem(IDC_NEXT)->MoveWindow(cx - button_width, button_height * 5, button_width, button_height);
  }

  if (GetDlgItem(IDC_PREVIOUS)) {
    GetDlgItem(IDC_PREVIOUS)->MoveWindow(cx - button_width, button_height * 6, button_width, button_height);
  }
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTileObstructionDialog::OnPresets()
{
  // get handle to button
  CWnd* button = GetDlgItem(IDC_PRESETS);
  
  // get coordinates of button
  RECT rect;
  button->GetWindowRect(&rect);

  HMENU obstruction_presets = LoadMenu(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_OBSTRUCTION_PRESETS));
  HMENU menu = GetSubMenu(obstruction_presets, 0);
  TrackPopupMenu(
    menu,
    TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON,
    rect.left,
    rect.bottom,
    0,
    m_hWnd,
    NULL
  );
}

////////////////////////////////////////////////////////////////////////////////
bool
CTileObstructionDialog::StoreTile()
{
  if (m_current_tile >= 0 && m_current_tile < m_tileset->GetNumTiles()
   && m_current_tile < (int) m_obstruction_maps.size())
  {
    m_tiles[m_current_tile] = m_edit_tile;
    m_obstruction_maps[m_current_tile] = m_edit_tile.GetObstructionMap();
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTileObstructionDialog::OnNext() {
  if (StoreTile()) {
    m_current_tile += 1;
    if (m_tileset->GetNumTiles() <= m_current_tile)
      m_current_tile = 0;
    m_edit_tile = m_tiles[m_current_tile];
    m_edit_tile.GetObstructionMap() = m_obstruction_maps[m_current_tile];
    Invalidate();
  }
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTileObstructionDialog::OnPrevious() {
  if (StoreTile()) {
    m_current_tile -= 1;
    if (m_current_tile < 0)
      m_current_tile = m_tileset->GetNumTiles() - 1;
    m_edit_tile = m_tiles[m_current_tile];
    m_edit_tile.GetObstructionMap() = m_obstruction_maps[m_current_tile];
    Invalidate();
  }
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTileObstructionDialog::OnFileSave()
{
#if 0
  CFileDialog Dialog(
    FALSE, "tobs", NULL,
    OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
    "C sourc file (*.ctobs)|*.ctobs|" \
    "All Files (*.*)|*.*||");

  if (Dialog.DoModal() != IDOK) {
    return;
  }

  FILE* file = fopen(Dialog.GetFileName(), "wb+");
  if (file) {
    fprintf(file, "afx_msg void\n");
    fprintf(file, "CTileObstructionDialog::OnPreset%s()\n", Dialog.GetFileTitle());
    fprintf(file, "{\n");
    fprintf(file, "  int w = m_tile->GetWidth()  - 1;\n");
    fprintf(file, "  int h = m_tile->GetHeight() - 1;\n\n");
    fprintf(file, "  sObstructionMap s;\n");

    for (int i = 0; i < m_edit_tile.GetObstructionMap().GetNumSegments(); i++) {
      const sObstructionMap::Segment& seg = m_edit_tile.GetObstructionMap().GetSegment(i);
      const int w = m_tile->GetWidth() - 1;
      const int h = m_tile->GetHeight() - 1;
      char strings[4][50];

      sprintf(strings[0], "%d*w/%d", seg.x1, w);
      sprintf(strings[1], "%d*h/%d", seg.y1, h);
      sprintf(strings[2], "%d*w/%d", seg.x2, w);
      sprintf(strings[3], "%d*h/%d", seg.y2, h);

      fprintf(file, "  s.AddSegment(%s, %s, %s, %s);\n", strings[0], strings[1], strings[2], strings[3]);
    }

    fprintf(file, "  m_edit_tile.GetObstructionMap() = s;\n");
    fprintf(file, "  m_obstruction_view.Invalidate();\n");
    fprintf(file, "}\n");

    fclose(file);
  }
#endif
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTileObstructionDialog::OnPresetUnblocked()
{
  sObstructionMap s;
  m_edit_tile.GetObstructionMap() = s;

  m_obstruction_view.Invalidate();
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTileObstructionDialog::OnPresetBlocked()
{
  int w = m_edit_tile.GetWidth()  - 1;
  int h = m_edit_tile.GetHeight() - 1;

  sObstructionMap s;
  s.AddSegment(0, 0, w, 0);
  s.AddSegment(w, 0, w, h);
  s.AddSegment(0, h, w, h);
  s.AddSegment(0, 0, 0, h);
  m_edit_tile.GetObstructionMap() = s;

  m_obstruction_view.Invalidate();
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTileObstructionDialog::OnPresetUpperRight()
{
  int w = m_edit_tile.GetWidth()  - 1;
  int h = m_edit_tile.GetHeight() - 1;

  sObstructionMap s;
  s.AddSegment(0, 0, w, 0);
  s.AddSegment(w, 0, w, h);
  s.AddSegment(0, 0, w, h);
  m_edit_tile.GetObstructionMap() = s;

  m_obstruction_view.Invalidate();
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTileObstructionDialog::OnPresetLowerRight()
{
  int w = m_edit_tile.GetWidth()  - 1;
  int h = m_edit_tile.GetHeight() - 1;

  sObstructionMap s;
  s.AddSegment(w, 0, w, h);
  s.AddSegment(0, h, w, h);
  s.AddSegment(0, h, w, 0);
  m_edit_tile.GetObstructionMap() = s;

  m_obstruction_view.Invalidate();
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTileObstructionDialog::OnPresetLowerLeft()
{
  int w = m_edit_tile.GetWidth()  - 1;
  int h = m_edit_tile.GetHeight() - 1;

  sObstructionMap s;
  s.AddSegment(0, 0, 0, h);
  s.AddSegment(0, h, w, h);
  s.AddSegment(0, 0, w, h);
  m_edit_tile.GetObstructionMap() = s;

  m_obstruction_view.Invalidate();
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTileObstructionDialog::OnPresetUpperLeft()
{
  int w = m_edit_tile.GetWidth()  - 1;
  int h = m_edit_tile.GetHeight() - 1;

  sObstructionMap s;
  s.AddSegment(0, 0, w, 0);
  s.AddSegment(0, 0, 0, h);
  s.AddSegment(0, h, w, 0);
  m_edit_tile.GetObstructionMap() = s;

  m_obstruction_view.Invalidate();
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTileObstructionDialog::OnPresetTopHalf()
{
  int w = m_edit_tile.GetWidth()  - 1;
  int h = (m_edit_tile.GetHeight() / 2);

  sObstructionMap s;
  s.AddSegment(0, 0, w, 0);
  s.AddSegment(0, 0, 0, h);
  s.AddSegment(0, h, w, h);
  s.AddSegment(w, 0, w, h);
  m_edit_tile.GetObstructionMap() = s;

  m_obstruction_view.Invalidate();
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTileObstructionDialog::OnPresetBottomHalf()
{
  int w = m_edit_tile.GetWidth()  - 1;
  int hh = (m_edit_tile.GetHeight() / 2);
  int fh = m_edit_tile.GetHeight() - 1;

  sObstructionMap s;
  s.AddSegment(0, hh, w, hh);
  s.AddSegment(0, hh, 0, fh);
  s.AddSegment(0, fh, w, fh);
  s.AddSegment(w, hh, w, fh);
  m_edit_tile.GetObstructionMap() = s;

  m_obstruction_view.Invalidate();
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTileObstructionDialog::OnPresetLeftHalf()
{
  int w = (m_edit_tile.GetWidth() / 2) - 1;
  int h = m_edit_tile.GetHeight() - 1;

  sObstructionMap s;
  s.AddSegment(0, 0, w, 0);
  s.AddSegment(0, 0, 0, h);
  s.AddSegment(0, h, w, h);
  s.AddSegment(w, 0, w, h);
  m_edit_tile.GetObstructionMap() = s;

  m_obstruction_view.Invalidate();
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTileObstructionDialog::OnPresetRightHalf()
{
  int hw = (m_edit_tile.GetWidth() / 2);
  int fw = m_edit_tile.GetWidth() - 1;
  int h = m_edit_tile.GetHeight() - 1;

  sObstructionMap s;
  s.AddSegment(hw, 0, fw, 0);
  s.AddSegment(hw, 0, hw, h);
  s.AddSegment(hw, h, fw, h);
  s.AddSegment(fw, 0, fw, h);
  m_edit_tile.GetObstructionMap() = s;

  m_obstruction_view.Invalidate();
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTileObstructionDialog::OnPresetCustom()
{
  int w = m_edit_tile.GetWidth()  - 1;
  int h = m_edit_tile.GetHeight() - 1;

  sObstructionMap s;
  const unsigned int id = GetCurrentMessage()->wParam;
  switch (id) {
    case (ID_OBSTRUCTIONPRESETS_CUSTOM1):
  s.AddSegment(0*w/15, 0*h/15, 3*w/15, 0*h/15);
  s.AddSegment(3*w/15, 0*h/15, 3*w/15, 15*h/15);
  s.AddSegment(3*w/15, 15*h/15, 0*w/15, 15*h/15);
  s.AddSegment(0*w/15, 15*h/15, 0*w/15, 0*h/15);
  s.AddSegment(15*w/15, 0*h/15, 12*w/15, 0*h/15);
  s.AddSegment(12*w/15, 0*h/15, 12*w/15, 15*h/15);
  s.AddSegment(12*w/15, 15*h/15, 15*w/15, 15*h/15);
  s.AddSegment(15*w/15, 15*h/15, 15*w/15, 0*h/15);
    break;

    case (ID_OBSTRUCTIONPRESETS_CUSTOM2):
  s.AddSegment(0*w/15, 0*h/15, 1*w/15, 0*h/15);
  s.AddSegment(1*w/15, 0*h/15, 1*w/15, 15*h/15);
  s.AddSegment(1*w/15, 15*h/15, 0*w/15, 15*h/15);
  s.AddSegment(0*w/15, 15*h/15, 0*w/15, 0*h/15);
  s.AddSegment(14*w/15, 0*h/15, 15*w/15, 0*h/15);
  s.AddSegment(15*w/15, 0*h/15, 15*w/15, 15*h/15);
  s.AddSegment(15*w/15, 15*h/15, 14*w/15, 15*h/15);
  s.AddSegment(14*w/15, 15*h/15, 14*w/15, 0*h/15);
    break;

    case (ID_OBSTRUCTIONPRESETS_CUSTOM3):
  s.AddSegment(15*w/15, 0*h/15, 0*w/15, 0*h/15);
  s.AddSegment(0*w/15, 0*h/15, 0*w/15, 3*h/15);
  s.AddSegment(0*w/15, 3*h/15, 16*w/15, 3*h/15);
  s.AddSegment(15*w/15, 3*h/15, 15*w/15, 0*h/15);
  s.AddSegment(0*w/15, 15*h/15, 0*w/15, 12*h/15);
  s.AddSegment(0*w/15, 12*h/15, 15*w/15, 13*h/15);
  s.AddSegment(15*w/15, 12*h/15, 15*w/15, 16*h/15);
  s.AddSegment(15*w/15, 15*h/15, 0*w/15, 15*h/15);
    break;

    case (ID_OBSTRUCTIONPRESETS_CUSTOM4):
  s.AddSegment(0*w/15, 0*h/15, 16*w/15, 0*h/15);
  s.AddSegment(15*w/15, 0*h/15, 15*w/15, 1*h/15);
  s.AddSegment(15*w/15, 1*h/15, 0*w/15, 1*h/15);
  s.AddSegment(0*w/15, 1*h/15, 0*w/15, 0*h/15);
  s.AddSegment(0*w/15, 15*h/15, 0*w/15, 14*h/15);
  s.AddSegment(0*w/15, 14*h/15, 15*w/15, 15*h/15);
  s.AddSegment(15*w/15, 14*h/15, 15*w/15, 15*h/15);
  s.AddSegment(15*w/15, 15*h/15, 0*w/15, 15*h/15);
    break;

    case (ID_OBSTRUCTIONPRESETS_CUSTOM5):
  s.AddSegment(0*w/15, 7*h/15, 7*w/15, 0*h/15);
  s.AddSegment(7*w/15, 0*h/15, 0*w/15, 0*h/15);
  s.AddSegment(0*w/15, 0*h/15, 0*w/15, 7*h/15);
    break;

    case (ID_OBSTRUCTIONPRESETS_CUSTOM6):
  s.AddSegment(8*w/15, 0*h/15, 15*w/15, 7*h/15);
  s.AddSegment(15*w/15, 7*h/15, 15*w/15, 0*h/15);
  s.AddSegment(15*w/15, 0*h/15, 9*w/15, 0*h/15);
    break;

    case (ID_OBSTRUCTIONPRESETS_CUSTOM7):
  s.AddSegment(0*w/15, 8*h/15, 7*w/15, 15*h/15);
  s.AddSegment(7*w/15, 15*h/15, 0*w/15, 15*h/15);
  s.AddSegment(0*w/15, 15*h/15, 0*w/15, 9*h/15);
    break;

    case (ID_OBSTRUCTIONPRESETS_CUSTOM8):
  s.AddSegment(15*w/15, 8*h/15, 8*w/15, 15*h/15);
  s.AddSegment(8*w/15, 15*h/15, 15*w/15, 15*h/15);
  s.AddSegment(15*w/15, 15*h/15, 15*w/15, 8*h/15);
    break;

  }
  m_edit_tile.GetObstructionMap() = s;
  m_obstruction_view.Invalidate();

}
////////////////////////////////////////////////////////////////////////////////
