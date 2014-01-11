#ifndef SPRITESET_ANIMATION_PALETTE_HPP
#define SPRITESET_ANIMATION_PALETTE_HPP
#include "PaletteWindow.hpp"
#include "DIBSection.hpp"
#include "../common/Spriteset.hpp"
#include "Zoomer.hpp"
class CDocumentWindow;
class CSpritesetAnimationPalette : public CPaletteWindow
{
public:
  CSpritesetAnimationPalette(CDocumentWindow* owner, sSpriteset* spriteset);
  
  virtual void Destroy();
	void SetCurrentDirection(int direction);
	void OnZoom(double zoom);
private:
  afx_msg void OnPaint();
  afx_msg void OnRButtonUp(UINT flags, CPoint point);
	afx_msg void OnTimer(UINT event);
  void ResetAnimation();
private:
	sSpriteset* m_Spriteset;    
	Zoomer m_ZoomFactor;
	int m_SelectedDirection;
	int m_CurrentFrame;
	int m_TicksLeft;
  UINT m_Timer;
  CDIBSection* m_BlitImage;
  DECLARE_MESSAGE_MAP()
};
#endif
