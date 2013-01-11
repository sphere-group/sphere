// disable 'identifier too long' warning
#pragma warning(disable : 4786)
#include "DocumentWindow.hpp"
#include "MainWindow.hpp"
#include "WindowCommands.hpp"
#include "PaletteWindow.hpp"
#include "Editor.hpp"
#include "resource.h"

static CDocumentWindow* s_CurrentDocumentWindow = NULL;
CDocumentWindow* GetCurrentDocumentWindow() {
  return s_CurrentDocumentWindow;
}

////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDocumentWindow, CMDIChildWnd)
  ON_WM_CREATE()
  ON_WM_CLOSE()
  ON_WM_SIZING()
  ON_WM_MDIACTIVATE()
  ON_UPDATE_COMMAND_UI(ID_FILE_SAVE,       OnUpdateSaveableCommand)
  ON_UPDATE_COMMAND_UI(ID_FILE_SAVEAS,     OnUpdateSaveableCommand)
  ON_UPDATE_COMMAND_UI(ID_FILE_SAVECOPYAS, OnUpdateSaveableCommand)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////
CDocumentWindow::CDocumentWindow(const char* document_path, int menu_resource, const CSize& min_size)
: m_MenuResource(menu_resource)
, m_MinSize(min_size)
, m_DocumentType(0)
{
  /*
  if (m_MenuResource != IDR_IRC
   && m_MenuResource != IDR_ANIMATION
   && m_MenuResource != IDR_BROWSE
   && m_MenuResource != IDR_SOUND
   && m_MenuResource != IDR_WINDOWSTYLE) {
    m_MenuResource = -1;
  }
  */
  if (document_path) {
    strcpy(m_DocumentPath, document_path);
  } else {
    strcpy(m_DocumentPath, "");
  }
  strcpy(m_Caption, GetDocumentTitle());
}
////////////////////////////////////////////////////////////////////////////////
CDocumentWindow::~CDocumentWindow()
{
  // this destructor can be called from a constructor, and the main window would
  // try to remove the window pointer before it was added
  if (this == s_CurrentDocumentWindow)
    s_CurrentDocumentWindow = NULL;
  m_DocumentType = 0;
  AfxGetApp()->m_pMainWnd->PostMessage(WM_DW_CLOSING, 0, (LPARAM)this);
}
////////////////////////////////////////////////////////////////////////////////
bool
CDocumentWindow::Close()
{
  return true;
}
////////////////////////////////////////////////////////////////////////////////
const char*
CDocumentWindow::GetFilename() const
{
  return m_DocumentPath;
}
////////////////////////////////////////////////////////////////////////////////
void
CDocumentWindow::AttachPalette(CPaletteWindow* palette)
{
  if (palette == NULL)
    return;
  m_AttachedPalettes.push_back(palette);
#if 1
  m_AttachedPalettesStates.push_back(false);
#endif
}
////////////////////////////////////////////////////////////////////////////////
void
CDocumentWindow::DetachPalette(CPaletteWindow* palette)
{
  for (int i = 0; i < int(m_AttachedPalettes.size()); i++) {
    if (m_AttachedPalettes[i] == palette) {
      m_AttachedPalettes.erase(m_AttachedPalettes.begin() + i);
#if 1
      m_AttachedPalettesStates.erase(m_AttachedPalettesStates.begin() + i);
#endif
      return;
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
#ifdef USE_SIZECBAR
void 
CDocumentWindow::LoadPaletteStates()
{
  // load the bar state based on the document class
	CDockState state; 
	CFrameWnd* pFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
	state.LoadState(GetRuntimeClass()->m_lpszClassName);
	// take care of bars not avaible at the moment (e.g. project window)
	// or not available anymore
	// HINT: Bars removed here lose their states and next time opened remain 
	//			 at the default positions
	for (int i = 0; i < state.m_arrBarInfo.GetSize(); i++)
  {
		CControlBarInfo* pInfo = (CControlBarInfo*)state.m_arrBarInfo[i];
		ASSERT(pInfo != NULL);
    
		int nDockedCount = pInfo->m_arrBarID.GetSize();
    if (nDockedCount > 0)
		{
			// dockbar
      for (int j = 0; j < nDockedCount; j++)
      {
				UINT nID = (UINT) pInfo->m_arrBarID[j];
        if (nID == 0) continue; // row separator
        if (nID > 0xFFFF)
					nID &= 0xFFFF; // placeholder - get the ID
				
				if (pFrame->GetControlBar(nID) == NULL)
				{
					// bar is not present
					pInfo->m_arrBarID.RemoveAt(j);						
					nDockedCount--;
					j--;
				}
      }
		}
        
    if (!pInfo->m_bFloating) // floating dockbars can be created later
		{
			if (pFrame->GetControlBar(pInfo->m_nBarID) == NULL)
			{
				// bar is not present
				state.m_arrBarInfo.RemoveAt(i);
				i--;
			}
		}
  }
	pFrame->SetDockState(state);
  CSizingControlBar::GlobalLoadState(pFrame, GetRuntimeClass()->m_lpszClassName);  
}
#endif
////////////////////////////////////////////////////////////////////////////////
int
CDocumentWindow::GetNumPalettes() const
{
  return int(m_AttachedPalettes.size());
}
////////////////////////////////////////////////////////////////////////////////
CPaletteWindow*
CDocumentWindow::GetPalette(int i) const
{
  return m_AttachedPalettes[i];
}
////////////////////////////////////////////////////////////////////////////////
BOOL
CDocumentWindow::Create(LPCTSTR class_name)
{
  return Create(class_name, WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW);
}
////////////////////////////////////////////////////////////////////////////////
BOOL
CDocumentWindow::Create(LPCTSTR class_name, DWORD style)
{
	BOOL maximized = FALSE;
	((CMainWindow*)AfxGetMainWnd())->MDIGetActive(&maximized);
  if (maximized) style |= WS_MAXIMIZE;
 
  BOOL result = CMDIChildWnd::Create(class_name, "", style);
  if (result) {
    UpdateWindowCaption();
    //RECT client_rect;
    //GetClientRect(&client_rect);
    //OnSize(0, client_rect.right, client_rect.bottom);
  }
  return result;
}
////////////////////////////////////////////////////////////////////////////////
const char*
CDocumentWindow::GetDefaultWindowClass()
{
  return AfxRegisterWndClass(0, ::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)), NULL, NULL);
}
////////////////////////////////////////////////////////////////////////////////
void
CDocumentWindow::SetDocumentPath(const char* path)
{
  if (strlen(path) < sizeof(m_DocumentPath)) {
    strcpy(m_DocumentPath, path);
  }
}
////////////////////////////////////////////////////////////////////////////////
const char*
CDocumentWindow::GetDocumentPath() const
{
  return m_DocumentPath;
}
////////////////////////////////////////////////////////////////////////////////
const char*
CDocumentWindow::GetDocumentTitle() const
{

  const char* ptr = strrchr(m_DocumentPath, '\\');
  if (ptr == NULL)
    return (const char*)m_DocumentPath;
  else
    return (ptr + 1);
}

////////////////////////////////////////////////////////////////////////////////
void
CDocumentWindow::SetCaption(const char* caption)
{
  if (strlen(caption) < sizeof(m_Caption)) {
    strcpy(m_Caption, caption);
  }
  UpdateWindowCaption();
}

////////////////////////////////////////////////////////////////////////////////
const char*
CDocumentWindow::GetCaption()
{
  return m_Caption;
}
////////////////////////////////////////////////////////////////////////////////
void
CDocumentWindow::UpdateWindowCaption()
{
  SetWindowText(m_Caption);
#ifdef TABBED_WINDOW_LIST
  CFrameWnd* pFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
  ((CMainWindow*)pFrame)->OnUpdateFrameTitle(TRUE);
#endif
}
////////////////////////////////////////////////////////////////////////////////
bool
CDocumentWindow::IsSaveable() const
{
  return false;
}
////////////////////////////////////////////////////////////////////////////////
void
CDocumentWindow::OnToolChanged(UINT id, int tool_index)
{
}
////////////////////////////////////////////////////////////////////////////////
BOOL
CDocumentWindow::IsToolAvailable(UINT id)
{
  return FALSE;
}
////////////////////////////////////////////////////////////////////////////////
afx_msg int
CDocumentWindow::OnCreate(LPCREATESTRUCT cs)
{
  SetWindowLong(m_hWnd, GWL_USERDATA, WA_DOCUMENT_WINDOW | m_DocumentType | (IsSaveable() ? WA_SAVEABLE : 0));
  return 0;
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CDocumentWindow::OnClose()
{
  if (Close())
    CMDIChildWnd::OnClose();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CDocumentWindow::OnSizing(UINT side, LPRECT rect)
{
  CMDIChildWnd::OnSizing(side, rect);
  if (m_MinSize == CSize(0, 0))
    return;
  int width = rect->right - rect->left;
  int height = rect->bottom - rect->top;
  // check sides; if they are too small, resize them
  if (side == WMSZ_TOPRIGHT ||
      side == WMSZ_RIGHT ||
      side == WMSZ_BOTTOMRIGHT)
  {
    if (width < m_MinSize.cx)
      rect->right = rect->left + m_MinSize.cx;
  }
  if (side == WMSZ_TOPLEFT ||
      side == WMSZ_LEFT ||
      side == WMSZ_BOTTOMLEFT)
  {
    if (width < m_MinSize.cx)
      rect->left = rect->right - m_MinSize.cx;
  }
  if (side == WMSZ_BOTTOMLEFT ||
      side == WMSZ_BOTTOM ||
      side == WMSZ_BOTTOMRIGHT)
  {
    if (height < m_MinSize.cy)
      rect->bottom = rect->top + m_MinSize.cy;
  }
  if (side == WMSZ_TOPLEFT ||
      side == WMSZ_TOP ||
      side == WMSZ_TOPRIGHT)
  {
    if (height < m_MinSize.cy)
      rect->top = rect->bottom - m_MinSize.cy;
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CDocumentWindow::OnMDIActivate(BOOL activate, CWnd* active_window, CWnd* inactive_window)
{
  CMDIChildWnd::OnMDIActivate(activate, active_window, inactive_window);
  CFrameWnd* pFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
  if (activate) s_CurrentDocumentWindow = this;
  if (activate)
  {
    // set the child menu resource and update the palette menu
    pFrame->SendMessage(WM_SET_CHILD_MENU, m_MenuResource);
    //OnToolCommand( ((CMainWindow*)pFrame)->GetImageTool() );
    //OnToolCommand( ((CMainWindow*)pFrame)->GetMapTool() );
    // pFrame->SendMessage(WM_UPDATE_TOOLBARS);
    OnToolChanged( ((CMainWindow*)pFrame)->GetImageTool(0),   0);
    OnToolChanged( ((CMainWindow*)pFrame)->GetMapTool(0),     0);
    OnToolChanged( ((CMainWindow*)pFrame)->GetMapZoomTool(0), 0);
    OnToolChanged( ((CMainWindow*)pFrame)->GetMapGridTool(0), 0);
    OnToolChanged( ((CMainWindow*)pFrame)->GetImageTool(1),   1);
    OnToolChanged( ((CMainWindow*)pFrame)->GetMapTool(1),     1);
    OnToolChanged( ((CMainWindow*)pFrame)->GetMapZoomTool(1), 1);
    OnToolChanged( ((CMainWindow*)pFrame)->GetMapGridTool(1), 1);
#ifdef USE_SIZECBAR
    // have to create the bars 
    for (int i = 0; i < int(m_AttachedPalettes.size()); i++) {
      m_AttachedPalettes[i]->CreateBar(true);
    } 
    
    if (m_AttachedPalettes.size() > 0)
    {
#ifdef FLOATING_PROJECT_WINDOW
			// the project palette should stay visible/invisible
      CPaletteWindow* pProjectWindow = (CPaletteWindow*)((CMainWindow*)pFrame)->GetProjectWindow();
			if (pProjectWindow != NULL)
			{
				bool bProjectVisible = pProjectWindow->IsVisible();
				LoadPaletteStates();
				pProjectWindow->ShowPalette(bProjectVisible);		
			}
#else
      if (0) {
      }
#endif // FLOATING_PROJECT_WINDOW
			else
			{
				LoadPaletteStates();
			}
    }
#else
    // display the palettes     
    for (int i = 0; i < int(m_AttachedPalettes.size()); i++) {
#if 1 
      if (m_AttachedPalettesStates[i])
#endif
        m_AttachedPalettes[i]->ShowPalette(true);        
    }
#endif // USE_SIZECBAR
  }
  else
  {
    // clear the child menu and update the palette menu
    pFrame->SendMessage(WM_CLEAR_CHILD_MENU);
#ifdef USE_SIZECBAR
    // save the bar state based on the document class
    pFrame->SaveBarState(GetRuntimeClass()->m_lpszClassName);
    CSizingControlBar::GlobalSaveState(pFrame, GetRuntimeClass()->m_lpszClassName);
    // have to remove the bars actually to free the IDs
    for (int i = 0; i < int(m_AttachedPalettes.size()); i++) {
      m_AttachedPalettes[i]->CreateBar(false);
    }
    pFrame->RecalcLayout();
#else
    // hide the palettes
    for (int i = 0; i < int(m_AttachedPalettes.size()); i++) {
#if 1
      m_AttachedPalettesStates[i] = m_AttachedPalettes[i]->IsVisible();
#endif
      m_AttachedPalettes[i]->ShowPalette(false);
    }
#endif
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CDocumentWindow::OnUpdateSaveableCommand(CCmdUI* cmdui)
{
  cmdui->Enable(IsSaveable() == true);
}
////////////////////////////////////////////////////////////////////////////////
BOOL CDocumentWindow::DistributeAccelerator(CWnd* pWnd, MSG* pMsg) 
{
	if (pWnd == NULL)
    return FALSE;
  pWnd = pWnd->GetWindow(GW_CHILD);	
	
	while (pWnd != NULL)
	{
		if (pWnd->IsWindowVisible())
		{				
			HACCEL hCustomAccel = NULL;
			pWnd->SendMessage(WM_GETACCELERATOR, (WPARAM)&hCustomAccel);					
			if (hCustomAccel != NULL)
			{
				if (::TranslateAccelerator(pWnd->m_hWnd, hCustomAccel, pMsg))
				{
					return TRUE;
				}
			}
			if (DistributeAccelerator(pWnd, pMsg))
			{
				return TRUE;
			}		
		}
		pWnd = pWnd->GetNextWindow();
	}
	return FALSE;
}
////////////////////////////////////////////////////////////////////////////////
BOOL CDocumentWindow::PreTranslateMessage(MSG* pMsg)
{
	BOOL processed = FALSE;
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	{
		//distribute the accelerator among the windows childs		
		processed = DistributeAccelerator(this, pMsg);		
	}
	return processed || CMDIChildWnd::PreTranslateMessage(pMsg);
}
////////////////////////////////////////////////////////////////////////////////
