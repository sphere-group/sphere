#ifndef H_SCROLL_WINDOW_HPP
#define H_SCROLL_WINDOW_HPP
#include <afxwin.h>
class CHScrollWindow : public CWnd
{
public:
  CHScrollWindow();
protected:
  void SetHScrollPosition(int position);
  void SetHScrollRange(int range, int page_size);
private:
  afx_msg void OnHScroll(UINT code, UINT pos, CScrollBar* scroll_bar);
  virtual void OnHScrollChanged(int x) = 0;
  void SetHScrollInfo();
 
private:
  int m_Range;
  int m_PageSize;
  int m_Position;
  DECLARE_MESSAGE_MAP()
};
#endif
