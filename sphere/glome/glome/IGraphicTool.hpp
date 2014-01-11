#ifndef I_GRAPHIC_TOOL_HPP
#define I_GRAPHIC_TOOL_HPP

#include <wx.h>
#include "Image.hpp"

namespace sde {

  struct ToolEvent {
    Point loc; // Location in the buffer x/y, not on-screen.
    bool  IsButtonDown; // Refers to the left mouse button, unless
                        // someone can give me a good reason why
		        // right-clicks should be turned over to
		        // graphic tools, as well.
    bool  IsShiftDown;
    bool  IsCtrlDown;
    bool  IsMetaDown;
  };

  class IGraphicTool
  {
  public:
    virtual void      IGraphicTool(Image* img, GraphicToolSet& toolset)=0;
    virtual void      ~IGraphicTool()=0;
    virtual wxString  GetBitmapName()=0;
    virtual void      OnClick(ToolEvent event)=0;
  };

  class GraphicToolSet
  {
  public:
    void GraphicToolSet(wxToolbar* toolbar, Image* img);
    bool AddTool(wxString name, IGraphicTool& tool);
    void RemoveTool(wxString name);
    void OnToolEvent(ToolEvent event);
    RGBA GetColor();
    void SetColor(RGBA color);
  };

}

#endif // I_GRAPHIC_TOOL_HPP
