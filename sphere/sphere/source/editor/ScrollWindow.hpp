#ifndef SCROLL_WINDOW_HPP
#define SCROLL_WINDOW_HPP
#include <afxwin.h>
class CScrollWindow : public CWnd
{
public:
  CScrollWindow();
protected:
  void SetHScrollPosition(int position);
  void SetHScrollRange(int range, int page_size);
  void SetVScrollPosition(int position);
  void SetVScrollRange(int range, int page_size);
private:
  afx_msg void OnHScroll(UINT code, UINT pos, CScrollBar* scroll_bar);
  afx_msg void OnVScroll(UINT code, UINT pos, CScrollBar* scroll_bar);
  void SetHScrollInfo();
  void SetVScrollInfo();
  virtual void OnHScrollChanged(int x) = 0;
  virtual void OnVScrollChanged(int y) = 0;
private:
  int m_XRange;
  int m_XPageSize;
  int m_XPosition;
  int m_YRange;
  int m_YPageSize;
  int m_YPosition;
  
  DECLARE_MESSAGE_MAP()
};
#endif
