#include "ZoneEditDialog.hpp"
#include "Scripting.hpp"
#include "resource.h"
#include "../common/str_util.hpp"

BEGIN_MESSAGE_MAP(CZoneEditDialog, CDialog)
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
CZoneEditDialog::CZoneEditDialog(sMap::sZone& zone, int zone_id, sMap* map)
: CDialog(IDD_ZONE_EDIT)
, m_Zone(zone)
, m_ZoneIndex(zone_id)
, m_Map(map)
{
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CZoneEditDialog::OnSizing(UINT side, LPRECT rect)
{
  if (!rect)
    return;

  if (rect->right - rect->left < 500)
    rect->right = rect->left + 500;
  if (rect->bottom - rect->top < 400)
    rect->bottom = rect->top + 400;

  CDialog::OnSizing(side, rect);
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CZoneEditDialog::OnSize(UINT type, int cx, int cy)
{

  if (GetDlgItem(IDC_SCRIPT))
    GetDlgItem(IDC_SCRIPT)->MoveWindow(20, 163, cx - 38, cy - 179, TRUE);

  CDialog::OnSize(type, cx, cy);
}

////////////////////////////////////////////////////////////////////////////////
BOOL
CZoneEditDialog::OnInitDialog()
{
  SetDlgItemText(IDC_SCRIPT, m_Zone.script.c_str());
  SetDlgItemInt(IDC_STEPS, m_Zone.reactivate_in_num_steps, FALSE);

  // add layer names in "layer_index - layer_name" style to dropdown layer list
  for (int i = 0; i < m_Map->GetNumLayers(); i++) {
    std::string layer_info = itos(i) + " - " + m_Map->GetLayer(i).GetName();
    SendDlgItemMessage(IDC_LAYER, CB_ADDSTRING, 0, (LPARAM)layer_info.c_str());
    if (i == m_Zone.layer)
      SendDlgItemMessage(IDC_LAYER, CB_SETCURSEL, m_Zone.layer);
  }

  char string[1024];
  SetDlgItemInt(IDC_ZONE_ID, m_ZoneIndex, FALSE);
  sprintf (string, "%d", m_Zone.x1);
  SetDlgItemText(IDC_ZONE_X, string);

  sprintf (string, "%d", m_Zone.y1);
  SetDlgItemText(IDC_ZONE_Y, string);

  sprintf (string, "%d", m_Zone.x2 - m_Zone.x1);
  SetDlgItemText(IDC_ZONE_WIDTH, string);

  sprintf (string, "%d", m_Zone.y2 - m_Zone.y1);
  SetDlgItemText(IDC_ZONE_HEIGHT, string);

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
void
CZoneEditDialog::OnOK()
{
  CString script;
  GetDlgItemText(IDC_SCRIPT, script);
  m_Zone.reactivate_in_num_steps = GetDlgItemInt(IDC_STEPS, NULL, FALSE);
  m_Zone.script = script;

  m_Zone.layer = SendDlgItemMessage(IDC_LAYER, CB_GETCURSEL);

  bool floating_point, percentage;
  int x, y, w, h;
  CString string;
  GetDlgItemText(IDC_ZONE_X, string);

  if (IsInvalidNumber(string, floating_point, percentage) || floating_point || percentage) {
    MessageBox("Invalid number format");
    return;
  }

  x = atoi(string);
  if (x < 0) { MessageBox("Invalid zone x"); return; }

  GetDlgItemText(IDC_ZONE_Y, string);
  if (IsInvalidNumber(string, floating_point, percentage) || floating_point || percentage) {
    MessageBox("Invalid number format"); return;
  }

  y = atoi(string);
  if (y < 0) { MessageBox("Invalid zone y"); return; }

  GetDlgItemText(IDC_ZONE_WIDTH, string);
  if (IsInvalidNumber(string, floating_point, percentage) || floating_point || percentage) {
    MessageBox("Invalid number format");
    return;
  }

  w = atoi(string);
  if (w <= 0) { MessageBox("Invalid zone width"); return; }

  GetDlgItemText(IDC_ZONE_HEIGHT, string);
  if (IsInvalidNumber(string, floating_point, percentage) || floating_point || percentage) {
    MessageBox("Invalid number format"); return;
  }

  h = atoi(string);
  if (h <= 0) { MessageBox("Invalid zone height"); return; }

  m_Zone.x1 = x;
  m_Zone.y1 = y;
  m_Zone.x2 = x + w;
  m_Zone.y2 = y + h;

  CDialog::OnOK();
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CZoneEditDialog::OnCheckSyntax()
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
