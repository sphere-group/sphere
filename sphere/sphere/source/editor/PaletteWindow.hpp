#ifndef PALETTE_WINDOW_HPP
#define PALETTE_WINDOW_HPP
#include <afxwin.h>
class CDocumentWindow;  // #include "DocumentWindow.hpp"
const int WM_UPDATE_PALETTE_MENU = (WM_APP + 800);
#ifdef USE_SIZECBAR
	// since no version is available in the original header
	#define SIZECBAR_VERSION "2.44" 
	#include "sizecbar.h"
	#include "scbarg.h"
	#define baseCPaletteWindow CWnd
	#define CPaletteBar CSizingControlBarG
#else
	#define baseCPaletteWindow CMiniFrameWnd
#endif
///////////////////////////////////////////////////////////////////////////////
class CPaletteWindow : public baseCPaletteWindow
{
public:
	void ShowPalette(bool bShow);
#ifdef USE_SIZECBAR
	void CreateBar(bool bCreate);
#endif
protected:
  // protected so only derived classes can construct
  CPaletteWindow();
  CPaletteWindow(CDocumentWindow* owner, const char* name, RECT rect, bool visible);
  ~CPaletteWindow();
private:
  afx_msg void OnDestroy();
  afx_msg void OnClose();
  afx_msg void OnShowWindow(BOOL show, UINT status);
private:
  CDocumentWindow* m_Owner;
#ifdef USE_SIZECBAR
	CString			 m_Name;
	CPaletteBar* m_pBarParent;
	int					 m_PaletteNumber;
#endif
public:
	inline bool IsVisible() {
#ifdef USE_SIZECBAR
    return (m_pBarParent != NULL && m_pBarParent->IsVisible()) ? true : false;
#else
    return IsWindowVisible() != FALSE ? true : false;
#endif
  }
  DECLARE_MESSAGE_MAP()
};
///////////////////////////////////////////////////////////////////////////////
#endif
