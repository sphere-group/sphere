#ifndef SPRITE_BASE_VIEW_HPP
#define SPRITE_BASE_VIEW_HPP
#include <afxwin.h>
#include "../common/Spriteset.hpp"
class ISpriteBaseViewHandler
{
public:
  virtual void SBV_SpritesetModified() = 0;
};
class CSpriteBaseView : public CWnd
{
public:
  CSpriteBaseView();
  ~CSpriteBaseView();
  BOOL Create(CWnd* parent, ISpriteBaseViewHandler* handler, sSpriteset* spriteset);
  void SetSprite(const CImage32* sprite);
private:
  void BeginDrag(int x, int y);
  void Drag(int x, int y);
  void WindowToImage(int* x, int* y);
  void UpdateStatusBar();
  afx_msg void OnSize(UINT type, int cx, int cy);
  afx_msg void OnPaint();
  afx_msg void OnLButtonDown(UINT flags, CPoint point);
  afx_msg void OnLButtonUp(UINT flags, CPoint point);
  afx_msg void OnMouseMove(UINT flags, CPoint point);
private:
  ISpriteBaseViewHandler* m_Handler;
  
  sSpriteset*     m_Spriteset;
  const CImage32* m_Sprite;
  bool m_MouseDown;
  DECLARE_MESSAGE_MAP()
};
#endif
