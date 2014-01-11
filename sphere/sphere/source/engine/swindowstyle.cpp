
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "swindowstyle.hpp"

////////////////////////////////////////////////////////////////////////////////
SWINDOWSTYLE::SWINDOWSTYLE()
{
    for (int i = 9 -1 ; i>=0 ;  --i)
        m_Images[i] = NULL;
}

////////////////////////////////////////////////////////////////////////////////
SWINDOWSTYLE::~SWINDOWSTYLE()
{
    Destroy();
}

////////////////////////////////////////////////////////////////////////////////
bool
SWINDOWSTYLE::Load(const char* filename, IFileSystem& fs)
{
    Destroy();

    // load the file
    if (m_WindowStyle.Load(filename, fs) == false)
        return false;

    Initialize();
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
SWINDOWSTYLE::CreateFromWindowStyle(const sWindowStyle& ws)
{
    Destroy();
    m_WindowStyle = ws;
    Initialize();
    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
SWINDOWSTYLE::Destroy()
{
    for (int i = 0; i < 9; i++)
    {
        if (m_Images[i])
        {
            DestroyImage(m_Images[i]);
            m_Images[i] = NULL;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
int
SWINDOWSTYLE::GetBackgroundMode() const
{
    return m_WindowStyle.GetBackgroundMode();
}

////////////////////////////////////////////////////////////////////////////////
bool
SWINDOWSTYLE::DrawBackground(int x, int y, int w, int h, int background_mode, const RGBA& mask, bool is_masked) const
{
    IMAGE image = m_Images[8];

    x -= m_WindowStyle.GetEdgeOffset(sWindowStyle::EDGE_LEFT);
    y -= m_WindowStyle.GetEdgeOffset(sWindowStyle::EDGE_TOP);
    w += m_WindowStyle.GetEdgeOffset(sWindowStyle::EDGE_LEFT) + m_WindowStyle.GetEdgeOffset(sWindowStyle::EDGE_RIGHT);
    h += m_WindowStyle.GetEdgeOffset(sWindowStyle::EDGE_TOP)  + m_WindowStyle.GetEdgeOffset(sWindowStyle::EDGE_BOTTOM);

    if ((background_mode == sWindowStyle::TILED) || (background_mode == sWindowStyle::TILED_GRADIENT))
    {
        int ox, oy, ow, oh;
        int width = GetImageWidth(image);
        int height = GetImageHeight(image);
        GetClippingRectangle(&ox, &oy, &ow, &oh);
        SetClippingRectangle(x, y, w, h);

        if ( !is_masked )
        {
            //for (int ix = 0; ix < w / width + 1; ix++)
			for (int ix = w / width ; ix>=0 ; --ix)
            {
                //for (int iy = 0; iy < h / height + 1; iy++)
				for (int iy = h / height ; iy>=0 ; --iy)
                {
                    BlitImage(image, x + ix * width, y + iy * height, CImage32::BLEND);
                }
            }
        }
        else
        {
            //for (int ix = 0; ix < w / width + 1; ix++)
			for (int ix = w / width; ix>=0; --ix)
            {
                //for (int iy = 0; iy < h / height + 1; iy++)
				for (int iy = h / height ; iy>=0 ; --iy)
                {
                    BlitImageMask(image, x + ix * width, y + iy * height, CImage32::BLEND, mask, CImage32::MULTIPLY);
                }
            }
        }

        SetClippingRectangle(ox, oy, ow, oh);
    }
    else if ((background_mode == sWindowStyle::STRETCHED) || (background_mode == sWindowStyle::STRETCHED_GRADIENT))
    {
        int tx[4] = { x, x + w, x + w, x };
        int ty[4] = { y, y, y + h, y + h };
        if ( !is_masked )
        {
            TransformBlitImage(image, tx, ty, CImage32::BLEND);
        }
        else
        {
            TransformBlitImageMask(image, tx, ty, CImage32::BLEND, mask, CImage32::MULTIPLY);
        }
    }

    if ((background_mode == sWindowStyle::GRADIENT) || (background_mode == sWindowStyle::STRETCHED_GRADIENT) || (background_mode == sWindowStyle::TILED_GRADIENT))
    {
        RGBA colors[4];
        colors[0] = m_WindowStyle.GetBackgroundColor(sWindowStyle::BACKGROUND_UPPER_LEFT);
        colors[1] = m_WindowStyle.GetBackgroundColor(sWindowStyle::BACKGROUND_UPPER_RIGHT);
        colors[2] = m_WindowStyle.GetBackgroundColor(sWindowStyle::BACKGROUND_LOWER_RIGHT);
        colors[3] = m_WindowStyle.GetBackgroundColor(sWindowStyle::BACKGROUND_LOWER_LEFT);

        if ( is_masked )
        {
            Blend4(colors[0], mask, mask.alpha);
            Blend4(colors[1], mask, mask.alpha);
            Blend4(colors[2], mask, mask.alpha);
            Blend4(colors[3], mask, mask.alpha);
        }

        DrawGradientRectangle(x, y, w, h, colors);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
SWINDOWSTYLE::DrawCorner(int index, int x, int y, const RGBA& mask, bool is_masked) const
{
    if (index < 0 || index >= 9)
        return false;
    if ( !is_masked )
    {
        BlitImage(m_Images[index], x, y, CImage32::BLEND);
    }
    else
    {
        BlitImageMask(m_Images[index], x, y, CImage32::BLEND, mask, CImage32::MULTIPLY);
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool
SWINDOWSTYLE::DrawUpperLeftCorner(int x, int y, const RGBA& mask, bool is_masked) const
{
    return DrawCorner(0, x, y, mask, is_masked);
}

bool
SWINDOWSTYLE::DrawUpperRightCorner(int x, int y, const RGBA& mask, bool is_masked) const
{
    return DrawCorner(2, x, y, mask, is_masked);
}

bool
SWINDOWSTYLE::DrawLowerRightCorner(int x, int y, const RGBA& mask, bool is_masked) const
{
    return DrawCorner(4, x, y, mask, is_masked);
}

bool
SWINDOWSTYLE::DrawLowerLeftCorner(int x, int y, const RGBA& mask, bool is_masked) const
{
    return DrawCorner(6, x, y, mask, is_masked);
}

////////////////////////////////////////////////////////////////////////////////
bool
SWINDOWSTYLE::DrawHorizontalEdge(int index, int x, int y, int w, int h, const RGBA& mask, bool is_masked) const
{
    if (index < 0 || index >= 9)
        return false;

    IMAGE image = m_Images[index];
    int width = GetImageWidth(image);
    int height = GetImageHeight(image);

    int ox, oy, ow, oh;
    GetClippingRectangle(&ox, &oy, &ow, &oh);

    SetClippingRectangle(x, y, width, h);
    if ( !is_masked )
    {
        //for (int i = 0; i < h / height + 1; i++)
		for (int i = h / height; i>=0; --i)
            BlitImage(image, x, y + i * height, CImage32::BLEND);
    }
    else
    {
        //for (int i = 0; i < h / height + 1; i++)
		for (int i = h / height; i>=0; --i)
            BlitImageMask(image, x, y + i * height, CImage32::BLEND, mask, CImage32::MULTIPLY);
    }

    SetClippingRectangle(ox, oy, ow, oh);
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool
SWINDOWSTYLE::DrawVerticalEdge(int index, int x, int y, int w, int h, const RGBA& mask, bool is_masked) const
{
    if (index < 0 || index >= 9)
        return false;

    IMAGE image = m_Images[index];
    int width = GetImageWidth(image);
    int height = GetImageHeight(image);

    int ox, oy, ow, oh;
    GetClippingRectangle(&ox, &oy, &ow, &oh);

    SetClippingRectangle(x, y, w, height);
    if ( !is_masked )
    {
        //for (int i = 0; i < w / width + 1; i++)
		for (int i = w / width; i>=0; --i)
            BlitImage(image, x + i * width, y, CImage32::BLEND);
    }
    else
    {
        //for (int i = 0; i < w / width + 1; i++)
		for (int i = w / width; i>=0; --i)
            BlitImageMask(image, x + i * width, y, CImage32::BLEND, mask, CImage32::MULTIPLY);
    }

    SetClippingRectangle(ox, oy, ow, oh);
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool
SWINDOWSTYLE::DrawTopEdge(int x, int y, int w, int h, const RGBA& mask, bool is_masked) const
{
    return DrawVerticalEdge(1, x, y, w, h, mask, is_masked);
}

///////////////////////////////////////////////////////////////////////////////
bool
SWINDOWSTYLE::DrawBottomEdge(int x, int y, int w, int h, const RGBA& mask, bool is_masked) const
{
    return DrawVerticalEdge(5, x, y, w, h, mask, is_masked);
}

////////////////////////////////////////////////////////////////////////////////
bool
SWINDOWSTYLE::DrawLeftEdge(int x, int y, int w, int h, const RGBA& mask, bool is_masked) const
{
    return DrawHorizontalEdge(7, x, y, w, h, mask, is_masked);
}

////////////////////////////////////////////////////////////////////////////////
bool
SWINDOWSTYLE::DrawRightEdge(int x, int y, int w, int h, const RGBA& mask, bool is_masked) const
{
    return DrawHorizontalEdge(3, x, y, w, h, mask, is_masked);
}

////////////////////////////////////////////////////////////////////////////////
bool
SWINDOWSTYLE::DrawWindow(int x, int y, int w, int h, const RGBA& mask, bool is_masked) const
{
    //---- Draw middle ----//
    DrawBackground(x, y, w, h, m_WindowStyle.GetBackgroundMode(), mask, is_masked);

    //---- Draw edges ----//
    DrawTopEdge(x, (y - GetImageHeight(m_Images[1])), w, h, mask, is_masked);
    DrawBottomEdge(x, (y + h), w, h, mask, is_masked);
    DrawLeftEdge((x - GetImageWidth(m_Images[7])), y, w, h, mask, is_masked);
    DrawRightEdge((x + w), y, w, h, mask, is_masked);

    //---- Draw corners ----//
    DrawUpperLeftCorner (x - GetImageWidth(m_Images[0]), y - GetImageHeight(m_Images[0]), mask, is_masked);
    DrawUpperRightCorner(x + w, y - GetImageHeight(m_Images[2]), mask, is_masked);
    DrawLowerRightCorner(x + w, y + h, mask, is_masked);
    DrawLowerLeftCorner (x - GetImageWidth(m_Images[6]), y + h, mask, is_masked);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
SWINDOWSTYLE::Initialize()
{
    // convert the file data into usable images
    for (int i = 0; i < 9; i++)
    {
        CImage32& image = m_WindowStyle.GetBitmap(i);
        m_Images[i] = CreateImage(image.GetWidth(), image.GetHeight(), image.GetPixels());
    }
}

////////////////////////////////////////////////////////////////////////////////

int
SWINDOWSTYLE::GetBorder(int index)
{
	switch(index){
		case sWindowStyle::EDGE_LEFT:
			return GetImageWidth(m_Images[sWindowStyle::LEFT]);
			break;
		case sWindowStyle::EDGE_TOP:
			return GetImageHeight(m_Images[sWindowStyle::TOP]);
			break;
		case sWindowStyle::EDGE_RIGHT:
			return GetImageWidth(m_Images[sWindowStyle::RIGHT]);
			break;
		case sWindowStyle::EDGE_BOTTOM:
			return GetImageHeight(m_Images[sWindowStyle::BOTTOM]);
			break;
		default:
			return -1;
			break;
	};

	//m_WindowStyle.GetEdgeOffset(sWindowStyle::EDGE_LEFT);
}

