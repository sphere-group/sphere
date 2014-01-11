#ifndef ALPHA_VIEW_HPP
#define ALPHA_VIEW_HPP
#include <afxwin.h>
#include <afxcmn.h>
#include "../common/types.h"
////////////////////////////////////////////////////////////////////////////////
class CCustomSliderCtrl: public CSliderCtrl
{
public:
  CCustomSliderCtrl();
  ~CCustomSliderCtrl();
protected:
  void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
  DECLARE_MESSAGE_MAP()
};
////////////////////////////////////////////////////////////////////////////////
class IAlphaViewHandler
{
public:
  virtual void AV_AlphaChanged(byte alpha) = 0;
};
////////////////////////////////////////////////////////////////////////////////
class CAlphaView : public CWnd
{
public:
  CAlphaView();
  ~CAlphaView();
  BOOL Create(IAlphaViewHandler* handler, CWnd* parent);
  void SetAlpha(byte alpha);
  byte GetAlpha() const;
private:
  void UpdateSlider();
  afx_msg void OnSize(UINT type, int cx, int cy);
  afx_msg void OnVScroll(UINT code, UINT pos, CScrollBar* scrollbar);
  void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
private:
  IAlphaViewHandler* m_Handler;
  CCustomSliderCtrl m_AlphaSlider;
  CStatic     m_AlphaStatic;
  bool m_Created;
  byte m_Alpha;
  DECLARE_MESSAGE_MAP()
};
#endif
