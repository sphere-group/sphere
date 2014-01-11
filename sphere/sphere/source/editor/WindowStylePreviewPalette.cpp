#include "WindowStylePreviewPalette.hpp"
#include "Configuration.hpp"
#include "Keys.hpp"
#include "resource.h"

BEGIN_MESSAGE_MAP(CWindowStylePreviewPalette, CPaletteWindow)
  ON_WM_PAINT()   
  ON_WM_RBUTTONUP()

  ON_COMMAND(ID_FILE_ZOOM_IN, OnZoomIn)
  ON_COMMAND(ID_FILE_ZOOM_OUT, OnZoomOut)

END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////
CWindowStylePreviewPalette::CWindowStylePreviewPalette(CDocumentWindow* owner, sWindowStyle* windowstyle)
: CPaletteWindow(owner, "WindowStyle Preview",
  Configuration::Get(KEY_WINDOWSTYLE_PREVIEW_RECT),
  Configuration::Get(KEY_WINDOWSTYLE_PREVIEW_VISIBLE))
,  m_WindowStyle(windowstyle)
, m_BlitImage(NULL)
{
  OnZoom(1);
}

////////////////////////////////////////////////////////////////////////////////
void
CWindowStylePreviewPalette::OnWindowStyleChanged()
{
	Invalidate();
}

////////////////////////////////////////////////////////////////////////////////
void
CWindowStylePreviewPalette::Destroy()
{
  delete m_BlitImage;

  // save state
  RECT rect;
  GetWindowRect(&rect);
  Configuration::Set(KEY_WINDOWSTYLE_PREVIEW_RECT, rect);
  // FIXME: IsWindowVisible() always returns FALSE here
  // Configuration::Set(KEY_WINDOWSTYLE_PREVIEW_VISIBLE, IsWindowVisible() != FALSE);

  // destroy window
  DestroyWindow();
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStylePreviewPalette::DrawBitmap(CPaintDC& dc, int bitmap, int x, int y, int mode)
{
  RECT ClientRect;
  GetClientRect(&ClientRect);

  if (!m_BlitImage || m_BlitImage->GetPixels() == NULL) {
    dc.FillRect(&ClientRect, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
    return;
  }

  int blit_width  = m_BlitImage->GetWidth();
  int blit_height = m_BlitImage->GetHeight();

  // draw the frame
  // fill the DIB section
  BGRA* pixels = (BGRA*)m_BlitImage->GetPixels();
  const CImage32& m_Image = m_WindowStyle->GetBitmap(bitmap);
  
  int iy;

  // make a checkerboard
//  for (int iy = 0; iy < m_Image.GetHeight(); iy++
//    for (int ix = 0; ix < m_Image.GetWidth(); ix++)
  for (iy = 0; iy < blit_height; iy++)
    for (int ix = 0; ix < blit_width; ix++)  
    {
      pixels[iy * blit_width + ix] = 
        ((ix / 8 + iy / 8) % 2 ?
        CreateBGRA(255, 255, 255, 255) :
        CreateBGRA(255, 192, 192, 255));
    }

  // draw the frame into it
	const RGBA* source = m_Image.GetPixels();
  for (iy = 0; iy < blit_height; iy++) {
    for (int ix = 0; ix < blit_width; ix++)
    {
      int ty = (int) (iy / m_ZoomFactor.GetZoomFactor());
      int tx = (int) (ix / m_ZoomFactor.GetZoomFactor());

      // this here would crash if the tileset has been resized
      // and the spriteset animation palette hasn't been informed of the resize
      if (tx >= 0 && tx < m_Image.GetWidth()
       && ty >= 0 && ty < m_Image.GetHeight()) {
    
         int t = (ty * m_Image.GetWidth()) + tx;    
         int d = (iy * blit_width) + ix;
         int alpha = source[t].alpha;

         pixels[d].red   = (source[t].red   * alpha + pixels[d].red   * (255 - alpha)) / 256;
         pixels[d].green = (source[t].green * alpha + pixels[d].green * (255 - alpha)) / 256;
         pixels[d].blue  = (source[t].blue  * alpha + pixels[d].blue  * (255 - alpha)) / 256;
       }
     }
   }
      
   // blit the frame
   CDC* tile = CDC::FromHandle(m_BlitImage->GetDC());
   dc.BitBlt((int) (ClientRect.left + x * m_ZoomFactor.GetZoomFactor()),
			       (int) (ClientRect.top  + y * m_ZoomFactor.GetZoomFactor()),
             (int) (m_Image.GetWidth()  * m_ZoomFactor.GetZoomFactor()),
	  	 	 	   (int) (m_Image.GetHeight() * m_ZoomFactor.GetZoomFactor()),
             tile, 0, 0, SRCCOPY);

}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStylePreviewPalette::DrawBackground(CPaintDC& dc, int bitmap, int x, int y)
{
  DrawBitmap(dc, bitmap, x, y, 0);
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStylePreviewPalette::DrawCorner(CPaintDC& dc, int bitmap, int x, int y)
{
  DrawBitmap(dc, bitmap, x, y, 0);
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStylePreviewPalette::DrawEdge(CPaintDC& dc, int bitmap, int x, int y)
{
  DrawBitmap(dc, bitmap, x, y, 0);
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStylePreviewPalette::OnPaint()
{
  CPaintDC dc(this);

	RECT ClientRect;
	GetClientRect(&ClientRect);

  if (!m_BlitImage || m_BlitImage->GetPixels() == NULL) {
    // draw black rectangle
    dc.FillRect(&ClientRect, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
    return;
  }

  int blit_width  = m_BlitImage->GetWidth();
  int blit_height = m_BlitImage->GetHeight();

  int left = m_WindowStyle->GetBitmap(sWindowStyle::UPPER_LEFT).GetWidth();
  int center = m_WindowStyle->GetBitmap(sWindowStyle::TOP).GetWidth(); // / m_ZoomFactor;
  int right = m_WindowStyle->GetBitmap(sWindowStyle::UPPER_RIGHT).GetWidth();

  int top = m_WindowStyle->GetBitmap(sWindowStyle::TOP).GetHeight();
  int middle = m_WindowStyle->GetBitmap(sWindowStyle::LEFT).GetHeight(); // / m_ZoomFactor;
  int bottom = m_WindowStyle->GetBitmap(sWindowStyle::BOTTOM).GetHeight(); // / m_ZoomFactor;

  // draw black rectangle around windowstyle
  if (1) {
    RECT rect = ClientRect;
    rect.left += (left + center + right);
    dc.FillRect(&rect, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
    rect.left -= (left + center + right);
    rect.top += (top + middle + bottom);
    dc.FillRect(&rect, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
    rect.top -= (top + middle + bottom);
  }

  DrawCorner(dc, sWindowStyle::UPPER_LEFT, 0, 0);
  DrawEdge(dc, sWindowStyle::TOP, left, 0);
  DrawCorner(dc, sWindowStyle::UPPER_RIGHT, left + center, 0);

  DrawCorner(dc, sWindowStyle::LEFT, 0, top);
  DrawBackground(dc, sWindowStyle::BACKGROUND, left, top);
  DrawCorner(dc, sWindowStyle::RIGHT, left + center, top);

  DrawCorner(dc, sWindowStyle::LOWER_LEFT, 0, top + middle);
  DrawEdge(dc, sWindowStyle::BOTTOM, left, top + middle);
  DrawCorner(dc, sWindowStyle::LOWER_RIGHT, left + center, top + middle);

  /*
  DrawEdge(dc, sWindowStyle::LEFT, 0, blit_height);
  DrawBackground(dc, sWindowStyle::BACKGROUND, blit_width, blit_height);
  DrawEdge(dc, sWindowStyle::RIGHT, blit_width + blit_width, blit_height);
  DrawEdge(dc, sWindowStyle::LOWER_LEFT, 0, blit_height + blit_height);
  DrawBackground(dc, sWindowStyle::BOTTOM, blit_width, blit_height + blit_height);
  DrawEdge(dc, sWindowStyle::LOWER_RIGHT, blit_width + blit_width, blit_height + blit_height);
  */
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStylePreviewPalette::OnRButtonUp(UINT flags, CPoint point)
{

}
///////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStylePreviewPalette::OnZoom(double zoom) {
  m_ZoomFactor.SetZoomFactor(zoom);

  if (m_BlitImage != NULL)
    delete m_BlitImage;

	int width  = 0;
	int height = 0;

	for (int i = 0; i < 9; i++) {
    if (m_WindowStyle->GetBitmap(i).GetWidth() > width)
      width = m_WindowStyle->GetBitmap(i).GetWidth();
    if (m_WindowStyle->GetBitmap(i).GetHeight() > height)
      height = m_WindowStyle->GetBitmap(i).GetHeight();
	}

  if (width > 0 && height > 0) {
    width = (int) (width * m_ZoomFactor.GetZoomFactor());
    height = (int) (height * m_ZoomFactor.GetZoomFactor());
    m_BlitImage = new CDIBSection(width, height, 32);
  }

  Invalidate();
}

//////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStylePreviewPalette::OnZoomIn()
{
  m_ZoomFactor.ZoomIn();
  OnZoom(m_ZoomFactor.GetZoomFactor());
}

//////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStylePreviewPalette::OnZoomOut()
{
  m_ZoomFactor.ZoomOut();
  OnZoom(m_ZoomFactor.GetZoomFactor());
}

//////////////////////////////////////////////////////////////////////////////
