#ifndef MAP_TOOL_PALETTE_HPP
#define MAP_TOOL_PALETTE_HPP
#include "ToolPalette.hpp"
class CMapToolPalette : public CToolPalette
{
public:
  CMapToolPalette(CDocumentWindow* owner, IToolPaletteHandler* handler);
  virtual void Destroy();
};
#endif
