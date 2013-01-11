#include "AlphaView.hpp"
////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CCustomSliderCtrl, CWnd)
  
  ON_WM_KEYDOWN()
  ON_WM_CHAR()
END_MESSAGE_MAP()
CCustomSliderCtrl::CCustomSliderCtrl()
{  
  CSliderCtrl::CSliderCtrl();
}
CCustomSliderCtrl::~CCustomSliderCtrl()
{  
  CSliderCtrl::~CSliderCtrl();
}
void CCustomSliderCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  if(nFlags&0x100)
  {
    CSliderCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
    return;
  }
  GetParent()->SendMessage(WM_KEYDOWN, nChar, nRepCnt + (((DWORD)nFlags)<<16));
}
void CCustomSliderCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{   
  if(nFlags&0x100)
  {
    CSliderCtrl::OnChar(nChar, nRepCnt, nFlags);
    return;
  }
  GetParent()->SendMessage(WM_CHAR, nChar, nRepCnt + (((DWORD)nFlags)<<16));
}
////////////////////////////////////////////////////////////////////////////////
static int s_AlphaViewID = 700;
BEGIN_MESSAGE_MAP(CAlphaView, CWnd)
  ON_WM_SIZE()
  ON_WM_VSCROLL()
  ON_WM_KEYDOWN()
  ON_WM_CHAR()
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////
CAlphaView::CAlphaView()
: m_Created(false)
, m_Alpha(0)
{
}
////////////////////////////////////////////////////////////////////////////////
CAlphaView::~CAlphaView()
{
  m_AlphaSlider.DestroyWindow();
  m_AlphaStatic.DestroyWindow();
  DestroyWindow();
}
////////////////////////////////////////////////////////////////////////////////
BOOL
CAlphaView::Create(IAlphaViewHandler* handler, CWnd* parent)
{
  m_Handler = handler;
  BOOL result = CWnd::Create(
    AfxRegisterWndClass(0, LoadCursor(NULL, IDC_ARROW), NULL, NULL),
    "",
    WS_CHILD | WS_VISIBLE,
    CRect(0, 0, 0, 0),
    parent,
    s_AlphaViewID++);
  m_AlphaSlider.Create(WS_CHILD | WS_VISIBLE | TBS_VERT, CRect(0, 0, 0, 0), this, 800);
  m_AlphaSlider.SetRange(0, 255);
  m_AlphaStatic.Create("", WS_CHILD | WS_VISIBLE | SS_CENTER, CRect(0, 0, 0, 0), this);
  m_Created = true;
  // make sure everything is in the correct place
  CRect rect;
  GetClientRect(rect);
  OnSize(0, rect.right, rect.bottom);
  UpdateSlider();
  return result;
}
////////////////////////////////////////////////////////////////////////////////
void
CAlphaView::SetAlpha(byte alpha)
{
  m_Alpha = 255 - alpha;
  UpdateSlider();
}
////////////////////////////////////////////////////////////////////////////////
byte
CAlphaView::GetAlpha() const
{
  return 255 - m_Alpha;
}
////////////////////////////////////////////////////////////////////////////////
void
CAlphaView::UpdateSlider()
{
  m_AlphaSlider.SetPos(m_Alpha);
  char str[80];
  sprintf(str, "%d", 255 - m_Alpha);
  m_AlphaStatic.SetWindowText(str);
}
////////////////////////////////////////////////////////////////////////////////
void
CAlphaView::OnSize(UINT type, int cx, int cy)
{
  const int STATIC_HEIGHT = 16;
  if (m_Created)
  {
    m_AlphaSlider.MoveWindow(0, 0, cx, cy - STATIC_HEIGHT);
    m_AlphaSlider.Invalidate();
    m_AlphaStatic.MoveWindow(0, cy - STATIC_HEIGHT, cx, STATIC_HEIGHT);
    m_AlphaStatic.Invalidate();
  }
}
////////////////////////////////////////////////////////////////////////////////
void
CAlphaView::OnVScroll(UINT code, UINT pos, CScrollBar* scrollbar)
{
  int value = m_Alpha;
  switch (code)
  {
    case SB_TOP:           value = 0;   break;
    case SB_BOTTOM:        value = 255; break;
    case SB_LINEDOWN:      value++;     break;
    case SB_LINEUP:        value--;     break;
    case SB_PAGEDOWN:      value += 16; break;
    case SB_PAGEUP:        value -= 16; break;
    case SB_THUMBPOSITION: value = pos; break;
    case SB_THUMBTRACK:    value = pos; break;
  }
  if (value < 0)
    value = 0;
  else if (value > 255)
    value = 255;
  m_Alpha = value;
  UpdateSlider();
  m_Handler->AV_AlphaChanged(255 - m_Alpha);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void 
CAlphaView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{ 
  //MessageBox("AlphaView::OnChar"); 
  GetParent()->SendMessage(WM_CHAR, nChar, nRepCnt + (((DWORD)nFlags)<<16));
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void 
CAlphaView::OnKeyDown(UINT vk, UINT nRepCnt, UINT nFlags) 
{ 
  //MessageBox("AlphaView::OnKeyDown"); 
  GetParent()->SendMessage(WM_KEYDOWN, vk, nRepCnt + (((DWORD)nFlags)<<16));
}
////////////////////////////////////////////////////////////////////////////////
