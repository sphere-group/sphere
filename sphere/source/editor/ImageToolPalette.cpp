#include "ImageToolPalette.hpp"
#include "Configuration.hpp"
#include "Keys.hpp"
#include "resource.h"
////////////////////////////////////////////////////////////////////////////////
CImageToolPalette::CImageToolPalette(CDocumentWindow* owner, IToolPaletteHandler* handler)
: CToolPalette(owner, handler, "Image Tools",
  Configuration::Get(KEY_IMAGE_TOOLS_RECT),
  Configuration::Get(KEY_IMAGE_TOOLS_VISIBLE))
{
  AddTool(IDI_IMAGETOOL_PENCIL,        "Pencil");
  AddTool(IDI_IMAGETOOL_LINE,          "Line");
  AddTool(IDI_IMAGETOOL_RECTANGLE,     "Rectangle");
  AddTool(IDI_IMAGETOOL_CIRCLE,        "Circle");
  AddTool(IDI_IMAGETOOL_ELLIPSE,       "Ellipse");
  AddTool(IDI_IMAGETOOL_FILL_SHAPE,    "Fill Shape");
  AddTool(IDI_IMAGETOOL_ANTIALIAS,     "Antialias Shape");
  AddTool(IDI_IMAGETOOL_FILL,          "Fill Area");
  AddTool(IDI_IMAGETOOL_SELECTION,     "Rectangular Selection");
  AddTool(IDI_IMAGETOOL_FREESELECTION, "Free Selection");
}
////////////////////////////////////////////////////////////////////////////////
void
CImageToolPalette::Destroy()
{
  // store state
  RECT rect;
  GetWindowRect(&rect);
  Configuration::Set(KEY_IMAGE_TOOLS_RECT, rect);
  // FIXME: IsWindowVisible() always returns FALSE here
  // Configuration::Set(KEY_IMAGE_TOOLS_VISIBLE, IsWindowVisible() != FALSE);
  CToolPalette::Destroy();
}
////////////////////////////////////////////////////////////////////////////////
