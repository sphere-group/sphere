#ifndef V_SCROLL_WINDOW_HPP
#define V_SCROLL_WINDOW_HPP
#include <afxwin.h>
class CVScrollWindow : public CWnd
{
public:
  CVScrollWindow();
protected:
  void SetVScrollPosition(int position);
  void SetVScrollRange(int range, int page_size);
private:
  afx_msg void OnVScroll(UINT code, UINT pos, CScrollBar* scroll_bar);
  virtual void OnVScrollChanged(int y) = 0;
  void SetVScrollInfo();
private:
  int m_Range;
  int m_PageSize;
  int m_Position;
  DECLARE_MESSAGE_MAP()
};
#endif
