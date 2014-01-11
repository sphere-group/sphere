#include "EntityTriggerDialog.hpp"
#include "Scripting.hpp"
#include "resource.h"
#include "../common/Map.hpp"

BEGIN_MESSAGE_MAP(CEntityTriggerDialog, CDialog)
  ON_BN_CLICKED(IDC_CHECK_SYNTAX, OnCheckSyntax)

  ON_WM_SIZE()
  ON_WM_SIZING()

END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////
static inline std::string itos(int i)
{
  char s[20];
  sprintf(s, "%d", i);
  return s;
}

////////////////////////////////////////////////////////////////////////////////
CEntityTriggerDialog::CEntityTriggerDialog(sTriggerEntity& trigger, sMap* map)
: CDialog(IDD_ENTITY_TRIGGER)
, m_Trigger(trigger)
, m_Map(map)
{
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CEntityTriggerDialog::OnSizing(UINT side, LPRECT rect)
{
  if (!rect)
    return;

  if (rect->right - rect->left < 520)
    rect->right = rect->left + 520;
  if (rect->bottom - rect->top < 280)
    rect->bottom = rect->top + 280;

  CDialog::OnSizing(side, rect);
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CEntityTriggerDialog::OnSize(UINT type, int cx, int cy)
{

  if (GetDlgItem(IDC_SCRIPT))
    GetDlgItem(IDC_SCRIPT)->MoveWindow(16, 70, cx - 34, cy - 81, TRUE);

  CDialog::OnSize(type, cx, cy);
}

////////////////////////////////////////////////////////////////////////////////
BOOL
CEntityTriggerDialog::OnInitDialog()
{
  SetDlgItemText(IDC_SCRIPT, m_Trigger.script.c_str());
  // add layer names in "layer_index - layer_name" style to dropdown layer list
  for (int i = 0; i < m_Map->GetNumLayers(); i++) {
    std::string layer_info = itos(i) + " - " + m_Map->GetLayer(i).GetName();
    if (m_Map->GetStartLayer() == i) {
      layer_info += " - ST";
    }
    SendDlgItemMessage(IDC_LAYER, CB_ADDSTRING, 0, (LPARAM)layer_info.c_str());
    if (i == m_Trigger.layer)
      SendDlgItemMessage(IDC_LAYER, CB_SETCURSEL, m_Trigger.layer);
  }

  return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
void
CEntityTriggerDialog::OnOK()
{
  CString script;
  GetDlgItemText(IDC_SCRIPT, script);
  m_Trigger.script = script;

  m_Trigger.layer = SendDlgItemMessage(IDC_LAYER, CB_GETCURSEL);
  CDialog::OnOK();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CEntityTriggerDialog::OnCheckSyntax()
{
  CString script;
  GetDlgItemText(IDC_SCRIPT, script);
  // check for errors
  sCompileError error;
  if (sScripting::VerifyScript(script, error)) {
    MessageBox("No syntax errors", "Check Syntax");
  } else {
    // show error
    MessageBox(("Script error:\n" + error.m_Message).c_str(), "Check Syntax");
    // select text
    SendDlgItemMessage(
      IDC_SCRIPT,
      EM_SETSEL,
      error.m_TokenStart,
      error.m_TokenStart + error.m_Token.length() + 1
    );
    GetDlgItem(IDC_SCRIPT)->SetFocus();
  }
}
////////////////////////////////////////////////////////////////////////////////
