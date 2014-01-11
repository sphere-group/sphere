#include "ToolPalette.hpp"
#include "Configuration.hpp"
#include "Keys.hpp"

const int TOOL_WIDTH  = 24;
const int TOOL_HEIGHT = 24;
const int BUTTON_BASE = 256;
const int MAX_BUTTONS = 256;

BEGIN_MESSAGE_MAP(CToolPalette, CPaletteWindow)
  ON_WM_SIZE()
  ON_WM_PAINT()

  ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnNeedText)
  ON_COMMAND_RANGE(BUTTON_BASE, BUTTON_BASE + MAX_BUTTONS - 1, OnToolSelected)
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////
CToolPalette::CToolPalette(CDocumentWindow* owner, IToolPaletteHandler* handler, const char* name, RECT rect, bool visible)
: CPaletteWindow(owner, name, rect, visible)
, m_Handler(handler)
, m_CurrentTool(0)
{
  EnableToolTips();
}

////////////////////////////////////////////////////////////////////////////////
void
CToolPalette::Destroy()
{
  // destroy buttons
  for (unsigned int i = 0; i < m_Buttons.size(); i++)
    delete m_Buttons[i];

  DestroyWindow();
}

////////////////////////////////////////////////////////////////////////////////
void
CToolPalette::AddTool(int icon_id, const char* label)
{
  CButton* button = new CButton;
  button->Create("",
    BS_ICON | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_CHILD | WS_VISIBLE,
    CRect(0, 0, 0, 0), this, BUTTON_BASE + m_Buttons.size());
  button->SetIcon(AfxGetApp()->LoadIcon(MAKEINTRESOURCE(icon_id)));

  if (m_CurrentTool == m_Buttons.size())
    button->SetCheck(BST_CHECKED);

  CRect rect(0, 0, 0, 0);
  m_Buttons.push_back(button);
  m_Labels.push_back(label);

  // make sure the tools are in the correct place now
  RECT cr;
  GetClientRect(&cr);
  OnSize(0, cr.right, cr.bottom);
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CToolPalette::OnSize(UINT type, int cx, int cy)
{
  int nx = cx / TOOL_WIDTH;

  int ix = 0; // current x
  int iy = 0; // current y

  for (unsigned int i = 0; i < m_Buttons.size(); i++) {
    int x = ix * TOOL_WIDTH;
    int y = iy * TOOL_HEIGHT;
    m_Buttons[i]->MoveWindow(x, y, TOOL_WIDTH, TOOL_HEIGHT, FALSE);
    m_Buttons[i]->Invalidate();

    if (++ix >= nx) {
      ix = 0;
      iy++;
    }
  }

  Invalidate();
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CToolPalette::OnPaint()
{
  RECT cr;
  GetClientRect(&cr);

  CPaintDC dc(this);
  dc.FillRect(&cr, CBrush::FromHandle((HBRUSH)(COLOR_MENU + 1)));
}

////////////////////////////////////////////////////////////////////////////////
afx_msg BOOL
CToolPalette::OnNeedText(UINT /*id*/, NMHDR* hdr, LRESULT* result)
{
  TOOLTIPTEXT* ttt = (TOOLTIPTEXT*)hdr;
  UINT id = hdr->idFrom;
  if (ttt->uFlags & TTF_IDISHWND)
    id = ::GetDlgCtrlID((HWND)id);

  if (id >= BUTTON_BASE && id < BUTTON_BASE + m_Labels.size())
    ttt->lpszText = const_cast<char*>(m_Labels[id - BUTTON_BASE].c_str());
  else
    ttt->lpszText = "";

  *result = 0;
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CToolPalette::OnToolSelected(UINT id)
{
  //m_Handler->TP_ToolSelected(id - BUTTON_BASE);
}

////////////////////////////////////////////////////////////////////////////////
