#include "VScrollWindow.hpp"
BEGIN_MESSAGE_MAP(CVScrollWindow, CWnd)
  ON_WM_VSCROLL()
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////
CVScrollWindow::CVScrollWindow()
: m_Range(0)
, m_PageSize(0)
, m_Position(0)
{
}
////////////////////////////////////////////////////////////////////////////////
void
CVScrollWindow::SetVScrollPosition(int position)
{
  m_Position = position;
  if (m_Position < 0) {
    m_Position = 0;
  } else if (m_Position > m_Range - m_PageSize) {
    m_Position = m_Range - m_PageSize;
  }
  SetVScrollInfo();
}
////////////////////////////////////////////////////////////////////////////////
void
CVScrollWindow::SetVScrollRange(int range, int page_size)
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
      OnVScrollChanged(m_Position);
    }
  }
  SetVScrollInfo();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CVScrollWindow::OnVScroll(UINT code, UINT pos, CScrollBar* scroll_bar)
{
  switch (code)
  {
    case SB_TOP:           m_Position = 0;           break;
    case SB_BOTTOM:        m_Position = m_Range - 1; break;
    case SB_PAGEUP:        m_Position -= m_PageSize; break;
    case SB_PAGEDOWN:      m_Position += m_PageSize; break;
    case SB_LINEUP:        m_Position--;             break;
    case SB_LINEDOWN:      m_Position++;             break;
    case SB_THUMBPOSITION: m_Position = pos;         break;
    case SB_THUMBTRACK:    m_Position = pos;         break;
  }
  if (m_Position > m_Range - m_PageSize) {
    m_Position = m_Range - m_PageSize;
  }
  if (m_Position < 0) {
    m_Position = 0;
  }
  SetVScrollRange(m_Range, m_PageSize);
  OnVScrollChanged(m_Position);
}
////////////////////////////////////////////////////////////////////////////////
void
CVScrollWindow::SetVScrollInfo()
{
  SCROLLINFO si;
  si.cbSize = sizeof(si);
  si.fMask  = SIF_ALL;
  // set the scrollbar info
  if (m_Range > m_PageSize) {
    si.nMin   = 0;
    si.nMax   = m_Range - 1;
    si.nPage  = m_PageSize;
    si.nPos   = m_Position;
    SetScrollInfo(SB_VERT, &si);
  } else {
    si.nMin  = 0;
    si.nMax  = 0xFFFF;
    si.nPage = 0xFFFE;
    si.nPos  = 0;
    SetScrollInfo(SB_VERT, &si);
  }
}
////////////////////////////////////////////////////////////////////////////////
