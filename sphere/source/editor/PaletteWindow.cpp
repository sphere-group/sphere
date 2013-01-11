#ifdef USE_SIZECBAR
	#undef NOMINMAX
#endif
#include "PaletteWindow.hpp"
#include "DocumentWindow.hpp"
#ifdef USE_SIZECBAR
#define BAR_ID_START 5000
//#include "MainWindow.hpp"
//class MainWindow;
// This technique is used, because the original files should remain unchanged,
// and though compile conditionally.
// ATTENTION: The only change I had to made to sizecbar.cpp and scbarg.cpp 
// was to remove the #include "stdafx.h"
#include "sizecbar.cpp"
#ifdef _DEBUG
#undef _DEBUG //to avoid THIS_FILE redefinition
#include "scbarg.cpp"
#define _DEBUG
#else
#include "scbarg.cpp"
#endif
#endif
////////////////////////////////////////////////////////////////////////////////
//					actual	CPaletteWindow class
////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CPaletteWindow, baseCPaletteWindow)
  ON_WM_DESTROY()
  ON_WM_CLOSE()
  ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////
CPaletteWindow::CPaletteWindow(CDocumentWindow* owner, const char* name, RECT rect, bool visible)
{
  m_Owner = owner;
#ifdef USE_SIZECBAR
  m_Name = name;
  m_pBarParent = NULL;
  m_PaletteNumber = (owner) ? owner->GetNumPalettes() : -1;
#endif
  DWORD styles;
#ifndef USE_SIZECBAR
  styles = WS_THICKFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_CLIPCHILDREN;
  if (visible)
    styles |= WS_VISIBLE;
#else
  styles = WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN;
#endif
	if (owner)
	{
		owner->AttachPalette(this);
	}
  // make sure the palette is visible!
  if (rect.right <= rect.left ||
      rect.bottom <= rect.top ||
      rect.left < 0 ||
      rect.top < 0 ||
      rect.left >= GetSystemMetrics(SM_CXSCREEN) - 1 ||
      rect.top >= GetSystemMetrics(SM_CYSCREEN) - 1) {
    rect.left = 64;
    rect.top = 64;
    rect.right = 256;
    rect.bottom = 256;
  }
  // !!!!
  // for now, all palettes are shown by default
  // change this in the future
  
#ifdef USE_SIZECBAR
	CreateBar(true);	
	Create(
    AfxRegisterWndClass(
      0,
      ::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW))
    ),
    name,
    styles,
    rect,
		m_pBarParent,
   0
  );
#else
  Create(
    AfxRegisterWndClass(
      0,
      ::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW))
    ),
    name,
    styles,
    rect,
    AfxGetApp()->m_pMainWnd,
    0
  );
  if (visible) {
  	ShowWindow(SW_SHOW);
  }
#endif
}
////////////////////////////////////////////////////////////////////////////////
CPaletteWindow::CPaletteWindow() {
  m_Owner = NULL;
#ifdef USE_SIZECBAR
  m_pBarParent = NULL;
  m_Name = "";
  m_PaletteNumber = 0;
#endif
}
////////////////////////////////////////////////////////////////////////////////
CPaletteWindow::~CPaletteWindow()
{
	if (m_Owner)
	{
		m_Owner->DetachPalette(this);
	}
#ifdef USE_SIZECBAR
  delete m_pBarParent;
  m_pBarParent = NULL;
#endif
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CPaletteWindow::OnDestroy()
{
  //::SendMessage(::GetParent(m_hWnd), WM_UPDATE_PALETTE_MENU, 0, 0);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CPaletteWindow::OnClose()
{
  ShowWindow(SW_HIDE);
  //::SendMessage(::GetParent(m_hWnd), WM_UPDATE_PALETTE_MENU, 0, 0);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CPaletteWindow::OnShowWindow(BOOL show, UINT status)
{
  //::SendMessage(::GetParent(m_hWnd), WM_UPDATE_PALETTE_MENU, 0, 0);
  baseCPaletteWindow::OnShowWindow(show, status);
}
////////////////////////////////////////////////////////////////////////////////
void
CPaletteWindow::ShowPalette(bool bShow)
{
#ifdef USE_SIZECBAR
	CFrameWnd* parent = (CFrameWnd*)AfxGetApp()->m_pMainWnd;	
  parent->ShowControlBar(m_pBarParent, bShow, TRUE);	
#else
	ShowWindow(bShow ? SW_SHOW : SW_HIDE);
#endif
}
////////////////////////////////////////////////////////////////////////////////
#ifdef USE_SIZECBAR
void
CPaletteWindow::CreateBar(bool bCreate)
{
	// should only be called if there is really need to create/destroy the bar
	ASSERT(bCreate == (m_pBarParent == NULL));
	CFrameWnd* parent = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
	if (bCreate)
	{	
		m_pBarParent = new CPaletteBar;
    if (!m_pBarParent)
      return;
    if (m_pBarParent->Create(m_Name, parent, BAR_ID_START + m_PaletteNumber) == FALSE) {
      delete m_pBarParent;
      m_pBarParent = NULL;
      return;
    }
		
		//move the palette window into the bar
		if (m_hWnd != 0)
		{		
			SetParent(m_pBarParent);
			ShowWindow(SW_SHOW);
		}
		//set various styles
		parent->AddControlBar(m_pBarParent);
		m_pBarParent->SetBarStyle(m_pBarParent->GetBarStyle() |  CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
		m_pBarParent->SetSCBStyle(SCBS_SIZECHILD);
		m_pBarParent->EnableDocking(CBRS_ALIGN_ANY);		
		parent->DockControlBar(m_pBarParent, AFX_IDW_DOCKBAR_LEFT);		
	}
	else
	{
		//move the palette window out the bar (onto the desktop), not to destroy the palette
		ASSERT(m_hWnd != 0);
		ShowWindow(SW_HIDE);
		SetParent(NULL);
		parent->RemoveControlBar(m_pBarParent);
		delete m_pBarParent;
		m_pBarParent = NULL;
	}
}
#endif
////////////////////////////////////////////////////////////////////////////////
