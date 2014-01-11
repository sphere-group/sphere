#ifndef DOCUMENT_WINDOW_HPP
#define DOCUMENT_WINDOW_HPP

#include <afxwin.h>
#include <vector>

const int WM_DW_CLOSING		= (WM_APP + 1000);
const int WA_DOCUMENT_WINDOW= 0x0001;
const int WA_SAVEABLE       = 0x0002;
const int WA_MAP			= 0x0010;
const int WA_IMAGE			= 0x0020;
const int WA_SCRIPT			= 0x0030;
const int WA_SPRITESET		= 0x0040;
const int WA_SOUND			= 0x0050;
const int WA_WINDOWSTYLE	= 0x0060;
const int WA_ANIMATION		= 0x0070;
const int WA_FONT			= 0x0080;

class CPaletteWindow;  // #include "PaletteWindow.hpp"
///////////////////////////////////////////////////////////
class CDocumentWindow : public CMDIChildWnd
{
public:
	CDocumentWindow(const char* document_path, int menu_resource, const CSize& min_size = CSize(0, 0));
	~CDocumentWindow();
	virtual bool Close();

	const char* GetFilename() const;
	void AttachPalette(CPaletteWindow* palette);
	void DetachPalette(CPaletteWindow* palette);

#ifdef USE_SIZECBAR
	void LoadPaletteStates();
#endif
	int GetNumPalettes() const;
	CPaletteWindow* GetPalette(int i) const;
	virtual bool IsSaveable() const;
	virtual void OnToolChanged(UINT id, int tool_index);
	virtual BOOL IsToolAvailable(UINT id);

protected:
	BOOL Create(LPCTSTR class_name, DWORD style);
	BOOL Create(LPCTSTR class_name);

private:
	static const char* GetDefaultWindowClass();

protected:
	void        SetDocumentPath(const char* path);
	const char* GetDocumentPath() const;
	virtual const char* GetDocumentTitle() const;

	void SetCaption(const char* caption);
	const char* GetCaption();
	virtual void UpdateWindowCaption();

	afx_msg int OnCreate(LPCREATESTRUCT cs);
	afx_msg void OnClose();
	afx_msg void OnSizing(UINT side, LPRECT rect);
	afx_msg void OnMDIActivate(BOOL activate, CWnd* active_window, CWnd* inactive_window);
	afx_msg void OnUpdateSaveableCommand(CCmdUI* cmdui);

protected:
	int m_DocumentType;

private:
	char  m_DocumentPath[MAX_PATH];
	char  m_Caption[520];
	int   m_MenuResource;
	CSize m_MinSize;
	std::vector<CPaletteWindow*> m_AttachedPalettes;

#if 1
	std::vector<bool> m_AttachedPalettesStates;
#endif
	DECLARE_MESSAGE_MAP()
	static BOOL DistributeAccelerator(CWnd* pWnd, MSG* pMsg);

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

///////////////////////////////////////////////////////////
CDocumentWindow* GetCurrentDocumentWindow();
///////////////////////////////////////////////////////////
#endif
