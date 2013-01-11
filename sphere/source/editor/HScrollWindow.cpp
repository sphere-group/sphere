#include "HScrollWindow.hpp"
BEGIN_MESSAGE_MAP(CHScrollWindow, CWnd)
  ON_WM_HSCROLL()
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////
CHScrollWindow::CHScrollWindow()
: m_Range(0)
, m_PageSize(0)
, m_Position(0)
{
}
////////////////////////////////////////////////////////////////////////////////
void
CHScrollWindow::SetHScrollPosition(int position)
{
  m_Position = position;
  if (m_Position < 0) {
    m_Position = 0;
  } else if (m_Position > m_Range - m_PageSize) {
    m_Position = m_Range - m_PageSize;
  }
  SetHScrollInfo();
}
////////////////////////////////////////////////////////////////////////////////
void
CHScrollWindow::SetHScrollRange(int range, int page_size)
{
  m_Range    = range;
  m_PageSize = page_size;
  // make sure position is optimized
  if (m_Position > 0) {
    if (m_Position > m_Range - m_PageSize) {
      m_Position = m_Range - m_PageSize;
      if (m_Position < 0) {
        m_Position = 0;
      }
      OnHScrollChanged(m_Position);
    }
  }
  SetHScrollInfo();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CHScrollWindow::OnHScroll(UINT code, UINT pos, CScrollBar* scroll_bar)
{
  switch (code) {
    case SB_LEFT:          m_Position = 0;           break;
    case SB_RIGHT:         m_Position = m_Range - 1; break;
    case SB_PAGELEFT:      m_Position -= m_PageSize; break;
    case SB_PAGERIGHT:     m_Position += m_PageSize; break;
    case SB_LINELEFT:      m_Position--;             break;
    case SB_LINERIGHT:     m_Position++;             break;
    case SB_THUMBPOSITION: m_Position = pos;         break;
    case SB_THUMBTRACK:    m_Position = pos;         break;
  }
  if (m_Position > m_Range - m_PageSize) {
    m_Position = m_Range - m_PageSize;
  }
  if (m_Position < 0) {
    m_Position = 0;
  }
  SetHScrollRange(m_Range, m_PageSize);
  OnHScrollChanged(m_Position);
}
////////////////////////////////////////////////////////////////////////////////
void
CHScrollWindow::SetHScrollInfo()
{
  SCROLLINFO si;
  si.cbSize = sizeof(si);
  si.fMask  = SIF_ALL;
  // set the scrollbar info
  if (m_Range > m_PageSize) {
    si.nMin  = 0;
    si.nMax  = m_Range - 1;
    si.nPage = m_PageSize;
    si.nPos  = m_Position;
    SetScrollInfo(SB_HORZ, &si);
  } else {
    si.nMin  = 0;
    si.nMax  = 0xFFFF;
    si.nPage = 0xFFFE;
    si.nPos  = 0;
    SetScrollInfo(SB_HORZ, &si);
  }
}
////////////////////////////////////////////////////////////////////////////////
