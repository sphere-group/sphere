#pragma warning(disable : 4786)
#include <vector>
#include "SwatchPalette.hpp"
#include "Configuration.hpp"
#include "Keys.hpp"
#include "../common/configfile.hpp"
#include "../common/common_palettes.hpp"
#include "resource.h"
#include <stdio.h>
#include "Editor.hpp"

#define SWATCH_TILE_SIZE 10
BEGIN_MESSAGE_MAP(CSwatchPalette, CPaletteWindow)
  ON_WM_SIZE()
  ON_WM_PAINT()
  ON_WM_VSCROLL()
  ON_WM_LBUTTONDOWN()
  ON_WM_RBUTTONDOWN()

  ON_COMMAND(ID_SWATCHPALETTE_FILE_LOAD,     OnFileLoad)
  ON_COMMAND(ID_SWATCHPALETTE_FILE_SAVE,     OnFileSave)
  ON_COMMAND(ID_SWATCHPALETTE_DEFAULT_DOS,   OnDefaultDOS)
  ON_COMMAND(ID_SWATCHPALETTE_DEFAULT_VERGE, OnDefaultVERGE)
  ON_COMMAND(ID_SWATCHPALETTE_DEFAULT_PLASMA, OnDefaultPlasma)
  ON_COMMAND(ID_SWATCHPALETTE_DEFAULT_RGB332, OnDefaultRGB332)
  ON_COMMAND(ID_SWATCHPALETTE_DEFAULT_VISIBONE2, OnDefaultVisibone2)
  ON_COMMAND(ID_SWATCHPALETTE_DEFAULT_HSL256, OnDefaultHsl256)

  ON_COMMAND(ID_SWATCHPALETTE_INSERTBEFORE, OnInsertColorBefore)
  ON_COMMAND(ID_SWATCHPALETTE_INSERTAFTER,  OnInsertColorAfter)
  ON_COMMAND(ID_SWATCHPALETTE_REPLACE,      OnReplaceColor)
  ON_COMMAND(ID_SWATCHPALETTE_DELETE,       OnDeleteColor)

END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////
CSwatchPalette::CSwatchPalette(CDocumentWindow* owner, ISwatchPaletteHandler* handler)
: CPaletteWindow(owner, "Swatch",
  Configuration::Get(KEY_SWATCH_RECT),
  Configuration::Get(KEY_SWATCH_VISIBLE))
, m_Handler(handler)

, m_Color(CreateRGBA(0, 0, 0, 255))
, m_TopRow(0)
, m_SelectedColor(0)
, m_RightClickColor(0)
{
  ///*
  RECT rect;
  GetClientRect(&rect);
  OnSize(0, rect.right, rect.bottom);
  //*/
  UpdateScrollBar();
}

////////////////////////////////////////////////////////////////////////////////
void
CSwatchPalette::Destroy()
{
  if (IsWindow(m_hWnd)) {
    // store state
    RECT rect;
    GetWindowRect(&rect);
    Configuration::Set(KEY_SWATCH_RECT, rect);
    // FIXME: IsWindowVisible() always returns FALSE here
    // Configuration::Set(KEY_SWATCH_VISIBLE, IsWindowVisible() != FALSE);

    DestroyWindow();
  }
}

////////////////////////////////////////////////////////////////////////////////
void
CSwatchPalette::SetColor(RGBA color)
{
  m_Color = color;
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSwatchPalette::OnSize(UINT type, int cx, int cy)
{
  if (cx > 0)
  {
    // if the current top row is greater than the total number of rows minus the page size
    if (m_TopRow > GetNumRows() - GetPageSize())
    {
      // move the top row up
      m_TopRow = GetNumRows() - GetPageSize();
      if (m_TopRow < 0)
        m_TopRow = 0;
      UpdateScrollBar();
      Invalidate();
    }
  }

  // reflect the changes
  UpdateScrollBar();
  Invalidate();

  CWnd::OnSize(type, cx, cy);
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSwatchPalette::OnPaint()
{
  CSwatchServer* swatch = CSwatchServer::Instance();

  CPaintDC dc(this);
  CBrush brush;
  RECT rect;

  GetClientRect(&rect);
  if (!swatch) {
    // draw black rectangle
    dc.FillRect(&rect, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
    return;
  }
  
  for (int iy = 0; iy < rect.bottom / SWATCH_TILE_SIZE + 1; iy++)
    for (int ix = 0; ix < rect.right / SWATCH_TILE_SIZE + 1; ix++)
    {
      int num_colors_x = rect.right / SWATCH_TILE_SIZE;
      int ic = (iy + m_TopRow) * num_colors_x + ix;

      RECT Color_Location = {
        ix * SWATCH_TILE_SIZE,
        iy * SWATCH_TILE_SIZE,
        (ix + 1) * SWATCH_TILE_SIZE,
        (iy + 1) * SWATCH_TILE_SIZE
      };

      if (!dc.RectVisible(&Color_Location))
        continue;
      if (ix < num_colors_x && ic < swatch->GetNumColors())
      {
        RGBA rgba = swatch->GetColor(ic);
        brush.CreateSolidBrush(RGB(rgba.red, rgba.green, rgba.blue));
        dc.FillRect(&Color_Location, &brush);
        brush.DeleteObject();

        if (ic == m_SelectedColor)
        {
          brush.CreateSolidBrush(RGB(0, 0, 0));
          dc.FrameRect(&Color_Location, &brush);
          brush.DeleteObject();

          Color_Location.left++;
          Color_Location.top++;
          Color_Location.right--;
          Color_Location.bottom--;
          brush.CreateSolidBrush(RGB(255, 255, 255));
          dc.FrameRect(&Color_Location, &brush);
          brush.DeleteObject();

          Color_Location.left++;
          Color_Location.top++;
          Color_Location.right--;
          Color_Location.bottom--;
          brush.CreateSolidBrush(RGB(0, 0, 0));
          dc.FrameRect(&Color_Location, &brush);
          brush.DeleteObject();
        }
      }
      else
      {
        // draw black rectangle
        dc.FillRect(&Color_Location, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
      }
    }
  
  brush.DeleteObject();
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSwatchPalette::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  int old_y = m_TopRow;

  switch (nSBCode)
  {
    case SB_LINEDOWN:   m_TopRow++;                break;
    case SB_LINEUP:     m_TopRow--;                break;
    case SB_PAGEDOWN:   m_TopRow += GetPageSize(); break;
    case SB_PAGEUP:     m_TopRow -= GetPageSize(); break;
    case SB_THUMBTRACK: m_TopRow = (int)nPos;      break;
  }

  // validate the values
  if (m_TopRow > GetNumRows() - GetPageSize())
    m_TopRow = GetNumRows() - GetPageSize();
  if (m_TopRow < 0)
    m_TopRow = 0;

  UpdateScrollBar();
  //Invalidate();
  CDC* dc = GetDC();
  CRgn updateRgn;
  dc->ScrollDC(0, (old_y - m_TopRow) * SWATCH_TILE_SIZE, NULL, NULL, &updateRgn, NULL);
  InvalidateRgn(&updateRgn);
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSwatchPalette::OnLButtonDown(UINT nFlags, CPoint point)
{
  CSwatchServer* swatch = CSwatchServer::Instance();
  if (!swatch) return;

  RECT rect;
  GetClientRect(&rect);
  
  int num_colors_x = rect.right / SWATCH_TILE_SIZE;
  int ix = point.x / SWATCH_TILE_SIZE;
  int iy = point.y / SWATCH_TILE_SIZE;
  int ic = (iy + m_TopRow) * num_colors_x + ix;

  if (ix >= num_colors_x) {
    return;
  }

  if (ic < swatch->GetNumColors())
  {
    RGBA rgba = swatch->GetColor(ic);
    m_Color = rgba;
    m_Handler->SP_ColorSelected(rgba);

    CRgn rgn;
    // "clean" the old color
    if (m_SelectedColor >= m_TopRow * num_colors_x &&
        m_SelectedColor <  (m_TopRow + (rect.bottom / SWATCH_TILE_SIZE)) * num_colors_x)
    {
      int offset = m_SelectedColor - (m_TopRow * num_colors_x);
      int y = (offset / num_colors_x) * SWATCH_TILE_SIZE;
      int x = (offset % num_colors_x) * SWATCH_TILE_SIZE;
      rgn.CreateRectRgn(x, y, x + SWATCH_TILE_SIZE, y + SWATCH_TILE_SIZE);
      InvalidateRgn(&rgn, true);
      rgn.DeleteObject();
    }
    m_SelectedColor = ic;

    // refresh the new position
    int offset = ic - (m_TopRow * num_colors_x);
    iy = (offset / num_colors_x) * SWATCH_TILE_SIZE;
    ix = (offset % num_colors_x) * SWATCH_TILE_SIZE;
    rgn.CreateRectRgn(ix, iy, ix + SWATCH_TILE_SIZE, iy + SWATCH_TILE_SIZE);
    InvalidateRgn(&rgn, true);
    rgn.DeleteObject();
  }
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSwatchPalette::OnRButtonDown(UINT nFlags, CPoint point)
{
  RECT rect;
  GetClientRect(&rect);
  
  int num_colors_x = rect.right / SWATCH_TILE_SIZE;
  int ix = point.x / SWATCH_TILE_SIZE;
  int iy = point.y / SWATCH_TILE_SIZE;
  int ic = (iy + m_TopRow) * num_colors_x + ix;

  if (ix >= num_colors_x) {
    return;
  }

  m_RightClickColor = ic;
  // popup menu
  HMENU menu = LoadMenu(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_SWATCHPALETTE));
  HMENU submenu = GetSubMenu(menu, 0);
  
  ClientToScreen(&point);
  TrackPopupMenu(submenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, point.x, point.y, 0, m_hWnd, NULL);
  DestroyMenu(menu);
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSwatchPalette::OnFileLoad()
{
  CSwatchServer* swatch = CSwatchServer::Instance();
  if (!swatch) return;

  CFileDialog Dialog(
    TRUE, "sswatch", NULL,
    OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
    "Sphere Swatch Files (*.sswatch)|*.sswatch|"
    "All Files (*.*)|*.*||"
  );

  SetCurrentDirectory(GetMainWindow()->GetDefaultFolder());
  Dialog.m_ofn.lpstrInitialDir = GetMainWindow()->GetDefaultFolder();

  if (Dialog.DoModal() != IDOK) {
    return;
  }

  // open file
  CConfigFile file;
  if (!file.Load(Dialog.GetPathName()))
    return;

  // check version  
  int ver = file.ReadInt("sphere_swatch", "version", 0);
  if (ver != 1) {
    return;
  }

  // read colors
  int NumColors = file.ReadInt("", "numcolors", 0);

  swatch->Clear();
  for (int i = 0; i < NumColors; i++) {
    char color_str[80];
    sprintf(color_str, "color%d", i);
    
    RGBA rgba;
    rgba.red   = file.ReadInt(color_str, "red",   0);
    rgba.green = file.ReadInt(color_str, "green", 0);
    rgba.blue  = file.ReadInt(color_str, "blue",  0);
    rgba.alpha = file.ReadInt(color_str, "alpha", 255);
    swatch->SetColor(i, rgba);
  }

  UpdateScrollBar();
  Invalidate();
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSwatchPalette::OnFileSave()
{
  CSwatchServer* swatch = CSwatchServer::Instance();

  CFileDialog Dialog(
    FALSE, "sswatch", NULL,
    OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
    "Sphere Swatch Files (*.sswatch)|*.sswatch|" \
    "C source file (*.cswatch)|*.cswatch|" \
    "All Files (*.*)|*.*||");

  SetCurrentDirectory(GetMainWindow()->GetDefaultFolder());
  Dialog.m_ofn.lpstrInitialDir = GetMainWindow()->GetDefaultFolder();

  if (Dialog.DoModal() != IDOK) {
    return;
  }

  if (Dialog.GetFileExt() == "cswatch") {
    FILE* file = fopen(Dialog.GetPathName(), "wb+");
    if (file == NULL)
      return;

    fprintf(file, "RGB %s_palette[%d] =\n{\n", Dialog.GetFileTitle(), swatch->GetNumColors());
    for (int i = 0; i < swatch->GetNumColors(); i++)
    {
      RGBA rgba = swatch->GetColor(i);
      fprintf(file, "  {%d, %d, %d}%s\n", rgba.red, rgba.green, rgba.blue, i < swatch->GetNumColors() - 1 ? "," : "");
    }

    fprintf(file, "};\n"); 
    fclose(file);
  }
  else {
    // create swatch file
    CConfigFile file;
    file.WriteInt("sphere_swatch", "version", 1);
    file.WriteInt("", "numcolors", swatch->GetNumColors());

    for (int i = 0; i < swatch->GetNumColors(); i++)
    {
      char color_str[80];
      sprintf(color_str, "color%d", i);

      RGBA rgba = swatch->GetColor(i);
      file.WriteInt(color_str, "red",   rgba.red);
      file.WriteInt(color_str, "green", rgba.green);
      file.WriteInt(color_str, "blue",  rgba.blue);
      file.WriteInt(color_str, "alpha", rgba.alpha);
    }

    file.Save(Dialog.GetPathName());
  }
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSwatchPalette::OnDefaultDOS()
{
  CSwatchServer* swatch = CSwatchServer::Instance();
  if (!swatch) return;

  swatch->Clear();
  for (int i = 0; i < 256; i++)
  {
    RGBA color = {
      dos_palette[i].red,
      dos_palette[i].green,
      dos_palette[i].blue,
      255
    };
    swatch->SetColor(i, color);
  }

  UpdateScrollBar();
  Invalidate();
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSwatchPalette::OnDefaultVERGE()
{
  CSwatchServer* swatch = CSwatchServer::Instance();
  if (!swatch) return;

  swatch->Clear();
  for (int i = 0; i < 256; i++)
  {
    RGBA color = {
      verge_palette[i].red,
      verge_palette[i].green,
      verge_palette[i].blue,
      255
    };
    swatch->SetColor(i, color);
  }

  UpdateScrollBar();
  Invalidate();
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSwatchPalette::OnDefaultPlasma()
{
  CSwatchServer* swatch = CSwatchServer::Instance();
  if (!swatch) return;

  swatch->Clear();
  for (int i = 0; i < 256; i++)
  {
    RGBA color = {
      plasma_palette[i].red,
      plasma_palette[i].green,
      plasma_palette[i].blue,
      255
    };
    swatch->SetColor(i, color);
  }

  UpdateScrollBar();
  Invalidate();
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSwatchPalette::OnDefaultRGB332()
{
  CSwatchServer* swatch = CSwatchServer::Instance();
  if (!swatch) return;

  swatch->Clear();
  for (int i = 0; i < 256; i++)
  {
    RGBA color = {
      rgb332_palette[i].red,
      rgb332_palette[i].green,
      rgb332_palette[i].blue,
      255
    };
    swatch->SetColor(i, color);
  }

  UpdateScrollBar();
  Invalidate();
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSwatchPalette::OnDefaultVisibone2()
{
  CSwatchServer* swatch = CSwatchServer::Instance();
  if (!swatch) return;

  swatch->Clear();
  for (int i = 0; i < 256; i++)
  {
    RGBA color = {
      visibone2_palette[i].red,
      visibone2_palette[i].green,
      visibone2_palette[i].blue,
      255
    };
    swatch->SetColor(i, color);
  }

  UpdateScrollBar();
  Invalidate();
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSwatchPalette::OnDefaultHsl256()
{
  CSwatchServer* swatch = CSwatchServer::Instance();
  if (!swatch) return;

  swatch->Clear();
  for (int i = 0; i < 256; i++)
  {
    RGBA color = {
      hsl256_palette[i].red,
      hsl256_palette[i].green,
      hsl256_palette[i].blue,
      255
    };
    swatch->SetColor(i, color);
  }

  UpdateScrollBar();
  Invalidate();
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSwatchPalette::OnInsertColorBefore()
{
  CSwatchServer* swatch = CSwatchServer::Instance();
  if (!swatch) return;

  swatch->InsertColor(m_RightClickColor, m_Color);
  UpdateScrollBar();
  Invalidate();
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSwatchPalette::OnInsertColorAfter()
{
  CSwatchServer* swatch = CSwatchServer::Instance();
  if (!swatch) return;

  swatch->InsertColor(m_RightClickColor + 1, m_Color);
  UpdateScrollBar();
  Invalidate();
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSwatchPalette::OnReplaceColor()
{
  CSwatchServer* swatch = CSwatchServer::Instance();
  if (!swatch) return;

  if (m_RightClickColor >= 0 && m_RightClickColor < swatch->GetNumColors()) {
    swatch->SetColor(m_RightClickColor, m_Color);

    UpdateScrollBar();
    Invalidate();
  }
 }

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSwatchPalette::OnDeleteColor()
{
  CSwatchServer* swatch = CSwatchServer::Instance();
  if (!swatch) return;
  swatch->DeleteColor(m_RightClickColor);

  if (m_SelectedColor > swatch->GetNumColors() - 1) {
    m_SelectedColor = swatch->GetNumColors() - 1;
    if (m_SelectedColor == -1) {
      m_SelectedColor = 0;
    }
  }

  UpdateScrollBar();
  Invalidate();
}

////////////////////////////////////////////////////////////////////////////////
void
CSwatchPalette::UpdateScrollBar()
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

////////////////////////////////////////////////////////////////////////////////
int
CSwatchPalette::GetPageSize()
{
  RECT ClientRect;
  GetClientRect(&ClientRect);
  return ClientRect.bottom / SWATCH_TILE_SIZE;
}

////////////////////////////////////////////////////////////////////////////////
int
CSwatchPalette::GetNumRows()
{
  CSwatchServer* swatch = CSwatchServer::Instance();
  if (!swatch) return -1;

  RECT client_rect;
  GetClientRect(&client_rect);
  int num_colors_x = client_rect.right / SWATCH_TILE_SIZE;

  if (num_colors_x == 0)
    return -1;
  else
    return (swatch->GetNumColors() + num_colors_x - 1) / num_colors_x;
}

////////////////////////////////////////////////////////////////////////////////
