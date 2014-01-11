#include "IGraphicTool.hpp"
#include "Image.hpp"

namespace sde {

  class PixelTool : public IGraphicTool
  {
  public:
    void PixelTool(Image* img, GraphicToolSet& toolset)
    {
      m_image = img;
      m_safe_image = new Image(*img);
      m_toolset = toolset;
    }

    void ~PixelTool() { }

    wxString GetBitmapName()
    {
      return "pixeltool.png";
    }

    void OnClick(ToolEvent event) 
    {
      if (!m_WasDown) 
	{
	  if (!event.IsButtonDown) return;
	  else 
	    {
	      m_WasDown = true;
	      delete m_SafeImage;
	      m_SafeImage = new Image(*m_image);
	      m_image.SetPixel(event.loc, m_toolset.GetColor());
	    }
	}
      else
	{
	  if (!event.IsButtonDown) 
	    {
	      m_WasDown = false;
	      if (event.IsCtrlDown) 
		{
		  m_image = m_SafeImage;
		  return;
		}
	    }
	  else
	    {
	      m_image.SetPixel(event.loc, m_toolset.GetColor());
	    }
	}
    }
  protected:
    Image* m_image;
    Image* m_SafeImage;
    GraphicToolSet m_toolset;
  };

  class DropperTool : public IGraphicTool 
  {
  public:
    void DropperTool(Image* img, GraphicToolSet& toolset) 
    {
      m_image = img;
      m_toolset = toolset;
    }

    void ~DropperTool() { }

    wxString GetBitmapName() 
    {
      return "droppertool.png";
    }

    void OnClick(ToolEvent event) {
      if (!event.IsButtonDown)
      {
	if (!event.IsCtrlDown) return;
	m_toolset.SetColor(m_image.GetPixel(event.loc));
      }
    }
  };


}
