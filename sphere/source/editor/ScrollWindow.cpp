#include "ScrollWindow.hpp"
BEGIN_MESSAGE_MAP(CScrollWindow, CWnd)
  ON_WM_HSCROLL()
  ON_WM_VSCROLL()
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////
CScrollWindow::CScrollWindow()
: m_XRange(0)
, m_XPageSize(0)
, m_XPosition(0)
, m_YRange(0)
, m_YPageSize(0)
, m_YPosition(0)
{
}
////////////////////////////////////////////////////////////////////////////////
void
CScrollWindow::SetHScrollPosition(int position)
{
  m_XPosition = position;
  if (m_XPosition < 0) {
    m_XPosition = 0;
  } else if (m_XPosition > m_XRange - m_XPageSize) {
    m_XPosition = m_XRange - m_XPageSize;
  }
  SetHScrollInfo();
}
////////////////////////////////////////////////////////////////////////////////
void
CScrollWindow::SetHScrollRange(int range, int page_size)
{
  m_XRange    = range;
  m_XPageSize = page_size;
  // make sure position is optimized
  if (m_XPosition > 0) {
    if (m_XPosition > m_XRange - m_XPageSize) {
      m_XPosition = m_XRange - m_XPageSize;
      if (m_XPosition < 0) {
        m_XPosition = 0;
      }
      OnHScrollChanged(m_XPosition);
    }
  }
  SetHScrollInfo();
}
////////////////////////////////////////////////////////////////////////////////
void
CScrollWindow::SetVScrollPosition(int position)
{
  m_YPosition = position;
  if (m_YPosition < 0) {
    m_YPosition = 0;
  } else if (m_YPosition > m_YRange - m_YPageSize) {
    m_YPosition = m_YRange - m_YPageSize;
  }
  SetVScrollInfo();
}
////////////////////////////////////////////////////////////////////////////////
void
CScrollWindow::SetVScrollRange(int range, int page_size)
{
  m_YRange    = range;
  m_YPageSize = page_size;
  // make sure position is optimized
  if (m_YPosition > 0) {
    if (m_YPosition > m_YRange - m_YPageSize) {
      m_YPosition = m_YRange - m_YPageSize;
      if (m_YPosition < 0) {
        m_YPosition = 0;
      }
      OnVScrollChanged(m_YPosition);
    }
  }
  SetVScrollInfo();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScrollWindow::OnHScroll(UINT code, UINT pos, CScrollBar* scroll_bar)
{
  switch (code) {
    case SB_LEFT:          m_XPosition = 0;            break;
    case SB_RIGHT:         m_XPosition = m_XRange - 1; break;
    case SB_PAGELEFT:      m_XPosition -= m_XPageSize; break;
    case SB_PAGERIGHT:     m_XPosition += m_XPageSize; break;
    case SB_LINELEFT:      m_XPosition--;              break;
    case SB_LINERIGHT:     m_XPosition++;              break;
    case SB_THUMBPOSITION: m_XPosition = pos;          break;
    case SB_THUMBTRACK:    m_XPosition = pos;          break;
  }
  if (m_XPosition > m_XRange - m_XPageSize) {
    m_XPosition = m_XRange - m_XPageSize;
  }
  if (m_XPosition < 0) {
    m_XPosition = 0;
  }
  SetHScrollRange(m_XRange, m_XPageSize);
  OnHScrollChanged(m_XPosition);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScrollWindow::OnVScroll(UINT code, UINT pos, CScrollBar* scroll_bar)
{
  switch (code) {
    case SB_LEFT:          m_YPosition = 0;            break;
    case SB_RIGHT:         m_YPosition = m_YRange - 1; break;
    case SB_PAGELEFT:      m_YPosition -= m_YPageSize; break;
    case SB_PAGERIGHT:     m_YPosition += m_YPageSize; break;
    case SB_LINELEFT:      m_YPosition--;              break;
    case SB_LINERIGHT:     m_YPosition++;              break;
    case SB_THUMBPOSITION: m_YPosition = pos;          break;
    case SB_THUMBTRACK:    m_YPosition = pos;          break;
  }
  if (m_YPosition > m_YRange - m_YPageSize) {
    m_YPosition = m_YRange - m_YPageSize;
  }
  if (m_YPosition < 0) {
    m_YPosition = 0;
  }
  SetVScrollRange(m_YRange, m_YPageSize);
  OnVScrollChanged(m_YPosition);
}
////////////////////////////////////////////////////////////////////////////////
void
CScrollWindow::SetHScrollInfo()
{
  SCROLLINFO si;
  si.cbSize = sizeof(si);
  si.fMask  = SIF_ALL;
  // set the scrollbar info
  if (m_XRange > m_XPageSize) {
    si.nMin  = 0;
    si.nMax  = m_XRange - 1;
    si.nPage = m_XPageSize;
    si.nPos  = m_XPosition;
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
void
CScrollWindow::SetVScrollInfo()
{
  SCROLLINFO si;
  si.cbSize = sizeof(si);
  si.fMask  = SIF_ALL;
  // set the scrollbar info
  if (m_YRange > m_YPageSize) {
    si.nMin  = 0;
    si.nMax  = m_YRange - 1;
    si.nPage = m_YPageSize;
    si.nPos  = m_YPosition;
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
