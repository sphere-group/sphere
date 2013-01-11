#include "TilePreviewPalette.hpp"
#include "Configuration.hpp"
#include "Keys.hpp"
#include "resource.h"

BEGIN_MESSAGE_MAP(CTilePreviewPalette, CPaletteWindow)
  ON_WM_PAINT()   
  ON_WM_RBUTTONUP()

END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////
CTilePreviewPalette::CTilePreviewPalette(CDocumentWindow* owner, CImage32 image)
: CPaletteWindow(owner, "Tile Preview",
  Configuration::Get(KEY_TILE_PREVIEW_RECT),
  Configuration::Get(KEY_TILE_PREVIEW_VISIBLE))
,  m_Image(image)
, m_BlitImage(NULL)
{
  OnZoom(1);
}

////////////////////////////////////////////////////////////////////////////////
void
CTilePreviewPalette::OnImageChanged(CImage32 image)
{
  bool changed_size =
    (image.GetWidth()  != m_Image.GetWidth()
  || image.GetHeight() != m_Image.GetHeight());
	
  m_Image = image;

  if (changed_size) {
    OnZoom(m_ZoomFactor.GetZoomFactor());
  }
  else {
    Invalidate();
  }
}

////////////////////////////////////////////////////////////////////////////////
void
CTilePreviewPalette::Destroy()
{
  if (m_BlitImage) {
    delete m_BlitImage;
    m_BlitImage = NULL;
  }

  // save state
  RECT rect;
  GetWindowRect(&rect);
  Configuration::Set(KEY_TILE_PREVIEW_RECT, rect);
  // FIXME: IsWindowVisible() always returns FALSE here
  //Configuration::Set(KEY_TILE_PREVIEW_VISIBLE, IsWindowVisible() != FALSE);

  // destroy window
  DestroyWindow();
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilePreviewPalette::OnPaint()
{
  CPaintDC dc(this);

  RECT ClientRect;
  GetClientRect(&ClientRect);

  if (!m_BlitImage || m_BlitImage->GetPixels() == NULL
    || m_Image.GetWidth() == 0 || m_Image.GetHeight() == 0 || m_Image.GetPixels() == NULL) {
    // draw black rectangle
    dc.FillRect(&ClientRect, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
    return;
  }

  int blit_width  = m_BlitImage->GetWidth();
  int blit_height = m_BlitImage->GetHeight();

  // draw black rectangle around tile
  RECT rect = ClientRect;
  rect.left += (blit_width * 3);
  dc.FillRect(&rect, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
  rect.left -= (blit_width * 3);
  rect.top += (blit_height * 3);
  dc.FillRect(&rect, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
  rect.top -= (blit_height * 3);

  for (int ty = 0; ty < 3; ty++)
  {
   	for (int tx = 0; tx < 3; tx++)
    {
      // draw the frame
      // fill the DIB section
      BGRA* pixels = (BGRA*)m_BlitImage->GetPixels();
      
      int iy;

      // make a checkerboard
      for (iy = 0; iy < blit_height; iy++) {
        for (int ix = 0; ix < blit_width; ix++) {
          pixels[iy * blit_width + ix] = 
            ((ix / 8 + iy / 8) % 2 ?
              CreateBGRA(255, 255, 255, 255) :
              CreateBGRA(255, 192, 192, 255));
        }
      }

      // draw the frame into it
      int frame_width  = m_Image.GetWidth()  < blit_width  ? m_Image.GetWidth()  : blit_width;
      int frame_height = m_Image.GetHeight() < blit_height ? m_Image.GetHeight() : blit_height;

      const RGBA* source = m_Image.GetPixels();
      for (iy = 0; iy < frame_height; iy++) {
        for (int ix = 0; ix < frame_width; ix++)
        {
          int ty = (int) (iy / m_ZoomFactor.GetZoomFactor());
          int tx = (int) (ix / m_ZoomFactor.GetZoomFactor());
   
          int t = (ty * m_Image.GetWidth()) + tx;    
          int d = (iy * blit_width) + ix;
          int alpha = source[t].alpha;

          pixels[d].red   = (source[t].red   * alpha + pixels[d].red   * (255 - alpha)) / 256;
          pixels[d].green = (source[t].green * alpha + pixels[d].green * (255 - alpha)) / 256;
          pixels[d].blue  = (source[t].blue  * alpha + pixels[d].blue  * (255 - alpha)) / 256;
        }
      }
      
      // blit the frame
      CDC* tile = CDC::FromHandle(m_BlitImage->GetDC());
      dc.BitBlt((int) (ClientRect.left + (tx * m_Image.GetWidth())  * m_ZoomFactor.GetZoomFactor()),
                (int) (ClientRect.top  + (ty * m_Image.GetHeight()) * m_ZoomFactor.GetZoomFactor()),
                blit_width,
                blit_height,
                tile, 0, 0, SRCCOPY);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilePreviewPalette::OnRButtonUp(UINT flags, CPoint point)
{
  /* //no menu needed for now
  // show pop-up menu
  ClientToScreen(&point);

  HMENU menu = ::LoadMenu(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_TILE_PREVIEW_PALETTE));
  HMENU submenu = GetSubMenu(menu, 0);

  TrackPopupMenu(submenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, point.x, point.y, 0, m_hWnd, NULL);
  DestroyMenu(menu);
  */
}

///////////////////////////////////////////////////////////////////////////////
afx_msg void
CTilePreviewPalette::OnZoom(double zoom) {
  m_ZoomFactor.SetZoomFactor(zoom);

  if (m_BlitImage != NULL)
  {
    delete m_BlitImage;
    m_BlitImage = NULL;
  } 

  int width  = (int) (m_Image.GetWidth() * m_ZoomFactor.GetZoomFactor());
  int height = (int) (m_Image.GetHeight() * m_ZoomFactor.GetZoomFactor()); 

  m_BlitImage = new CDIBSection(width, height, 32);
  RECT rect;
  rect.left = 0;
  rect.top  = 0;
  rect.right = width;
  rect.bottom = height;
  /*AdjustWindowRect(&rect, GetStyle(), FALSE);
  SetWindowPos(NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);*/

  Invalidate();
}

//////////////////////////////////////////////////////////////////////////////
