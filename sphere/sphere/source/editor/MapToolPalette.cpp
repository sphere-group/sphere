#include "MapToolPalette.hpp"
#include "Configuration.hpp"
#include "Keys.hpp"
#include "resource.h"

////////////////////////////////////////////////////////////////////////////////
CMapToolPalette::CMapToolPalette(CDocumentWindow* owner, IToolPaletteHandler* handler)
: CToolPalette(owner, handler, "Map Tools",
  Configuration::Get(KEY_MAP_TOOLS_RECT),
  Configuration::Get(KEY_MAP_TOOLS_VISIBLE))
{
  AddTool(IDI_MAPTOOL_ZOOM_1X,     "Map Zoom 1x");
  AddTool(IDI_MAPTOOL_ZOOM_2X,     "Map Zoom 2x");
  AddTool(IDI_MAPTOOL_ZOOM_4X,     "Map Zoom 4x");
  AddTool(IDI_MAPTOOL_ZOOM_8X,     "Map Zoom 8x");
  AddTool(IDI_MAPTOOL_GRID_TILE,   "Show Tile Grid");
  AddTool(IDI_MAPTOOL_1X1,         "Pencil Tool (1x1)");
  AddTool(IDI_MAPTOOL_3X3,         "Pencil Tool (3x3)");
  AddTool(IDI_MAPTOOL_5X5,         "Pencil Tool (5x5)");
  AddTool(IDI_MAPTOOL_SELECTTILE,  "Select Tile");
  AddTool(IDI_MAPTOOL_FILLRECTAREA,"Fill Rectangular Area");
  AddTool(IDI_MAPTOOL_FILLAREA,    "Fill Area");
  AddTool(IDI_MAPTOOL_FILL_LAYER,  "Fill Layer");
  AddTool(IDI_MAPTOOL_COPYAREA,    "Copy Area");
  AddTool(IDI_MAPTOOL_PASTE,       "Paste Area");
  AddTool(IDI_MAPTOOL_MOVEENTITY,  "Move Entity");
  AddTool(IDI_MAPTOOL_OBS_SEGMENT, "Obstruction Segment");
  AddTool(IDI_MAPTOOL_OBS_DELETE,  "Delete Obstruction Segment");
  AddTool(IDI_MAPTOOL_OBS_MOVE_PT, "Move Obstruction Segment Point");
  AddTool(IDI_MAPTOOL_ZONEADD,     "Add a Zone");
  AddTool(IDI_MAPTOOL_ZONEEDIT,    "Edit a Zone");
  AddTool(IDI_MAPTOOL_ZONEMOVE,    "Move a Zone");
  AddTool(IDI_MAPTOOL_ZONEDELETE,  "Delete a Zone");
}

////////////////////////////////////////////////////////////////////////////////
void
CMapToolPalette::Destroy()
{
  // store state
  RECT rect;
  GetWindowRect(&rect);
  Configuration::Set(KEY_MAP_TOOLS_RECT, rect);

  // FIXME: IsWindowVisible() always returns FALSE here
  // Configuration::Set(KEY_MAP_TOOLS_VISIBLE, IsWindowVisible() != FALSE);

  CToolPalette::Destroy();
}

////////////////////////////////////////////////////////////////////////////////
