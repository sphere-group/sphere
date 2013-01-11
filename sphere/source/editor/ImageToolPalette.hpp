#ifndef IMAGE_TOOL_PALETTE_HPP
#define IMAGE_TOOL_PALETTE_HPP
#include "ToolPalette.hpp"
class CImageToolPalette : public CToolPalette
{
public:
  CImageToolPalette(CDocumentWindow* owner, IToolPaletteHandler* handler);
  virtual void Destroy();
};
#endif
