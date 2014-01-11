#include "SpritesetAnimationPalette.hpp"
#include "Configuration.hpp"
#include "Keys.hpp"
#include "resource.h"

static const int ANIMATION_TIMER = 9001;

BEGIN_MESSAGE_MAP(CSpritesetAnimationPalette, CPaletteWindow)
  ON_WM_PAINT()   
  ON_WM_RBUTTONUP()
  ON_WM_TIMER()
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////
CSpritesetAnimationPalette::CSpritesetAnimationPalette(CDocumentWindow* owner, sSpriteset* spriteset)
: CPaletteWindow(owner, "Spriteset Animation",
  Configuration::Get(KEY_SPRITESET_ANIMATION_RECT),
  Configuration::Get(KEY_SPRITESET_ANIMATION_VISIBLE))
, m_Spriteset(spriteset)
, m_SelectedDirection(0) //m_SelectedDirection(-1)
, m_BlitImage(NULL)
{
  OnZoom(1);
	//init the animation timer
  m_Timer = SetTimer(ANIMATION_TIMER, Configuration::Get(KEY_SPRITESET_ANIMATION_DELAY), NULL);
	ResetAnimation();
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetAnimationPalette::SetCurrentDirection(int direction)
{
	//only restart animation if it's really a new direction
	if (m_SelectedDirection == direction)
    return;
	m_SelectedDirection = direction;
	ResetAnimation();
	Invalidate();
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritesetAnimationPalette::Destroy()
{
  delete m_BlitImage;
  m_BlitImage = NULL;
  // save state
  RECT rect;
  GetWindowRect(&rect);
  Configuration::Set(KEY_SPRITESET_ANIMATION_RECT, rect);
  // FIXME: IsWindowVisible() always returns FALSE here
  // Configuration::Set(KEY_SPRITESET_ANIMATION_VISIBLE, IsWindowVisible() != FALSE);
#ifndef _DEBUG
  // TODO: Doesn't work in Debug mode, fix this.
  KillTimer(ANIMATION_TIMER);
#endif
  // destroy window
  DestroyWindow();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetAnimationPalette::OnPaint()
{
  CPaintDC dc(this);
  RECT ClientRect;
  GetClientRect(&ClientRect);
  
  if (!m_BlitImage || m_BlitImage->GetPixels() == NULL) {
    dc.FillRect(&ClientRect, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
    return;
  }

  int blit_width  = m_BlitImage->GetWidth();
  int blit_height = m_BlitImage->GetHeight();

  // draw black rectangle around image
  if (1) {
    RECT rect = ClientRect;
    rect.left += blit_width;
    dc.FillRect(&rect, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
    rect.left -= blit_width;
    rect.top += blit_height;
    dc.FillRect(&rect, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
    rect.top -= blit_height;
  }

  if (m_SelectedDirection >= 0 && m_SelectedDirection < m_Spriteset->GetNumDirections())
  {
	// draw the frame
	// fill the DIB section
    BGRA* pixels = (BGRA*)m_BlitImage->GetPixels();
      
    int iy;

    // make a checkerboard
    for (iy = 0; iy < blit_height; iy++)
      for (int ix = 0; ix < blit_width; ix++)
      {
        pixels[iy * blit_width + ix] = 
          ((ix / 8 + iy / 8) % 2 ?
            CreateBGRA(255, 255, 255, 255) :
            CreateBGRA(255, 192, 192, 255));
      }
    // draw the frame into it
	int frame_index =  m_Spriteset->GetFrameIndex(m_SelectedDirection, m_CurrentFrame);
    RGBA* source = m_Spriteset->GetImage(frame_index).GetPixels();
    for (iy = 0; iy < blit_height; iy++) {
      for (int ix = 0; ix < blit_width; ix++)
      {
        int ty = (int)(iy / m_ZoomFactor.GetZoomFactor());
        int tx = (int)(ix / m_ZoomFactor.GetZoomFactor());
        // this here would crash if the spriteset has been resized
        // and the spriteset animation palette hasn't been informed of the resize
        if (tx >= 0 && tx < m_Spriteset->GetFrameWidth()
          && ty >= 0 && ty < m_Spriteset->GetFrameHeight()) {
  
          int t = ty * m_Spriteset->GetFrameWidth() + tx;    
          int d = iy * blit_width + ix;
          int alpha = source[t].alpha;
          pixels[d].red   = (source[t].red   * alpha + pixels[d].red   * (255 - alpha)) / 256;
          pixels[d].green = (source[t].green * alpha + pixels[d].green * (255 - alpha)) / 256;
          pixels[d].blue  = (source[t].blue  * alpha + pixels[d].blue  * (255 - alpha)) / 256;
        }
			}
    }
      
    // blit the frame
    CDC* tile = CDC::FromHandle(m_BlitImage->GetDC());
    dc.BitBlt(ClientRect.left, ClientRect.top,
              ClientRect.right - ClientRect.left, ClientRect.bottom - ClientRect.top,
              tile, 0, 0, SRCCOPY);
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetAnimationPalette::OnRButtonUp(UINT flags, CPoint point)
{
  /* //no menu needed for now
  // show pop-up menu
  ClientToScreen(&point);
  HMENU menu = ::LoadMenu(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_SPRITESET_ANIMATION_PALETTE));
  HMENU submenu = GetSubMenu(menu, 0);
  TrackPopupMenu(submenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, point.x, point.y, 0, m_hWnd, NULL);
  DestroyMenu(menu);
  */
}
///////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetAnimationPalette::OnZoom(double zoom) {
  m_ZoomFactor.SetZoomFactor(zoom);
  if (m_BlitImage != NULL)
    delete m_BlitImage;
  int width = (int) (m_Spriteset->GetFrameWidth() * m_ZoomFactor.GetZoomFactor());
  int height = (int) (m_Spriteset->GetFrameHeight() * m_ZoomFactor.GetZoomFactor()); 
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
///////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritesetAnimationPalette::OnTimer(UINT event)
{
	if (m_SelectedDirection >= 0 && m_SelectedDirection < m_Spriteset->GetNumDirections())
	
	if (m_TicksLeft == 0)
	{
		m_CurrentFrame++;
		
		if (m_CurrentFrame >= m_Spriteset->GetNumFrames(m_SelectedDirection))
			m_CurrentFrame = 0;
		m_TicksLeft = m_Spriteset->GetFrameDelay(m_SelectedDirection, m_CurrentFrame);
		Invalidate();
	}
	else
	{
    //if (!m_MenuShown)
  		m_TicksLeft--;
	}
}
//////////////////////////////////////////////////////////////////////////////
void
CSpritesetAnimationPalette::ResetAnimation()
{
	if (m_SelectedDirection != -1)
	{
		m_CurrentFrame = 0;
		m_TicksLeft = m_Spriteset->GetFrameDelay(m_SelectedDirection, m_CurrentFrame);
		Invalidate();
	}
}
