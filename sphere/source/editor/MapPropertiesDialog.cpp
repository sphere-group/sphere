#include "Editor.hpp"
#include "DocumentWindow.hpp"
#include "MapPropertiesDialog.hpp"
#include "Scripting.hpp"
#include "../common/Map.hpp"
#include "FileDialogs.hpp"
#include "resource.h"
BEGIN_MESSAGE_MAP(CMapPropertiesDialog, CDialog)

  ON_BN_CLICKED(IDC_BROWSE_MUSIC,   OnBrowseBackgroundMusic)
  ON_BN_CLICKED(IDC_BROWSE_TILESET, OnBrowseTileset)
  ON_BN_CLICKED(IDC_CHECK_SYNTAX,   OnCheckSyntax)

  ON_CBN_SELCHANGE(IDC_SCRIPT_TYPE, OnScriptChanged)

  ON_WM_SIZE()
  ON_WM_SIZING()

END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////
CMapPropertiesDialog::CMapPropertiesDialog(sMap* map, const char* document_path)
: CDialog(IDD_MAP_PROPERTIES)
, m_Map(map)
, m_CurrentScript(ENTRY)
, m_DocumentPath(document_path)
{
  m_EntryScript = m_Map->GetEntryScript();
  m_ExitScript  = m_Map->GetExitScript();
  m_NorthScript = m_Map->GetEdgeScript(sMap::NORTH);
  m_EastScript  = m_Map->GetEdgeScript(sMap::EAST);
  m_SouthScript = m_Map->GetEdgeScript(sMap::SOUTH);
  m_WestScript  = m_Map->GetEdgeScript(sMap::WEST);
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CMapPropertiesDialog::OnSizing(UINT side, LPRECT rect)
{
  if (!rect)
    return;

  if (rect->right - rect->left < 660)
    rect->right = rect->left + 660;
  if (rect->bottom - rect->top < 350)
    rect->bottom = rect->top + 350;

  CDialog::OnSizing(side, rect);
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CMapPropertiesDialog::OnSize(UINT type, int cx, int cy)
{

  if (GetDlgItem(IDC_SCRIPT))
    GetDlgItem(IDC_SCRIPT)->MoveWindow(15, 164, cx - 30, cy - 176, TRUE);

  CDialog::OnSize(type, cx, cy);
}

////////////////////////////////////////////////////////////////////////////////
BOOL
CMapPropertiesDialog::OnInitDialog()
{
  if (m_DocumentPath)
  {
    std::string full_path = m_DocumentPath;
    std::string term("maps");
    size_t pos = full_path.rfind(term);
    std::string filename = full_path.substr(pos + 5);

    SetDlgItemText(IDC_MAPPATH, filename.c_str());
  }
  else
  {
    SetDlgItemText(IDC_MAPPATH, "");
  }

  // set music file
  SetDlgItemText(IDC_MUSIC,   m_Map->GetMusicFile());
  SetDlgItemText(IDC_TILESET, m_Map->GetTilesetFile());

  // set script controls
  SendDlgItemMessage(IDC_SCRIPT_TYPE, CB_ADDSTRING, 0, (LPARAM)"On Enter Map");
  SendDlgItemMessage(IDC_SCRIPT_TYPE, CB_ADDSTRING, 0, (LPARAM)"On Leave Map");
  SendDlgItemMessage(IDC_SCRIPT_TYPE, CB_ADDSTRING, 0, (LPARAM)"On Leave Map North");
  SendDlgItemMessage(IDC_SCRIPT_TYPE, CB_ADDSTRING, 0, (LPARAM)"On Leave Map East");
  SendDlgItemMessage(IDC_SCRIPT_TYPE, CB_ADDSTRING, 0, (LPARAM)"On Leave Map South");
  SendDlgItemMessage(IDC_SCRIPT_TYPE, CB_ADDSTRING, 0, (LPARAM)"On Leave Map West");
  SendDlgItemMessage(IDC_SCRIPT_TYPE, CB_SETCURSEL, 0);
  LoadNewScript();

  // check "repeating" button
  CheckDlgButton(IDC_REPEATING, (m_Map->IsRepeating() ? BST_CHECKED : BST_UNCHECKED));

  // give focus to the script combo box
  GetDlgItem(IDC_SCRIPT)->SetFocus();

  return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
void
CMapPropertiesDialog::OnOK()
{
  // get music file
  CString str;

  GetDlgItemText(IDC_MUSIC, str);
  m_Map->SetMusicFile(str);

  GetDlgItemText(IDC_TILESET, str);
  m_Map->SetTilesetFile(str);

  m_Map->SetRepeating(IsDlgButtonChecked(IDC_REPEATING) == BST_CHECKED);

  StoreCurrentScript();

  m_Map->SetEntryScript(            m_EntryScript);
  m_Map->SetExitScript(             m_ExitScript);
  m_Map->SetEdgeScript(sMap::NORTH, m_NorthScript);
  m_Map->SetEdgeScript(sMap::EAST,  m_EastScript);
  m_Map->SetEdgeScript(sMap::SOUTH, m_SouthScript);
  m_Map->SetEdgeScript(sMap::WEST,  m_WestScript);

  CDialog::OnOK();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CMapPropertiesDialog::OnBrowseBackgroundMusic()
{
  char old_directory[MAX_PATH] = {0};
  GetCurrentDirectory(MAX_PATH, old_directory);
  std::string directory = GetMainWindow()->GetDefaultFolder(WA_SOUND);
  SetCurrentDirectory(directory.c_str());
  CSoundFileDialog dialog(FDM_OPEN);

  if (dialog.DoModal() == IDOK)
  {
    std::string full_path = LPCTSTR(dialog.GetPathName());
    std::string term("sounds");
    size_t pos = full_path.rfind(term);

    if (pos != std::string::npos)
    {
      CString path = full_path.substr(pos + 7).c_str();
      path.Replace('\\', '/');
      SetDlgItemText(IDC_MUSIC, path);
    }

  }

  SetCurrentDirectory(old_directory);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CMapPropertiesDialog::OnBrowseTileset()
{
  char old_directory[MAX_PATH] = {0};
  GetCurrentDirectory(MAX_PATH, old_directory);
  std::string directory = GetMainWindow()->GetDefaultFolder(WA_MAP);
  SetCurrentDirectory(directory.c_str());
  CTilesetFileDialog dialog(FDM_OPEN);
  if (dialog.DoModal() == IDOK) {
    SetDlgItemText(IDC_TILESET, dialog.GetFileName());
  }
  SetCurrentDirectory(old_directory);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CMapPropertiesDialog::OnCheckSyntax()
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
void
CMapPropertiesDialog::OnScriptChanged()
{
  StoreCurrentScript();

  int index = SendDlgItemMessage(IDC_SCRIPT_TYPE, CB_GETCURSEL);

  switch (index)
  {
    case 0: m_CurrentScript = ENTRY; break;
    case 1: m_CurrentScript = EXIT;  break;
    case 2: m_CurrentScript = NORTH; break;
    case 3: m_CurrentScript = EAST;  break;
    case 4: m_CurrentScript = SOUTH; break;
    case 5: m_CurrentScript = WEST;  break;
  }

  LoadNewScript();
}

////////////////////////////////////////////////////////////////////////////////
void
CMapPropertiesDialog::StoreCurrentScript()
{
  CString* string = NULL;

  switch (m_CurrentScript) {
    case ENTRY: string = &m_EntryScript; break;
    case EXIT:  string = &m_ExitScript;  break;
    case NORTH: string = &m_NorthScript; break;
    case EAST:  string = &m_EastScript;  break;
    case SOUTH: string = &m_SouthScript; break;
    case WEST:  string = &m_WestScript;  break;
    default:    return;
  }
  GetDlgItemText(IDC_SCRIPT, *string);
}
////////////////////////////////////////////////////////////////////////////////
void
CMapPropertiesDialog::LoadNewScript()
{
  CString* string = NULL;
  switch (m_CurrentScript) {
    case ENTRY: string = &m_EntryScript; break;
    case EXIT:  string = &m_ExitScript;  break;
    case NORTH: string = &m_NorthScript; break;
    case EAST:  string = &m_EastScript;  break;
    case SOUTH: string = &m_SouthScript; break;
    case WEST:  string = &m_WestScript;  break;
    default:    return;
  }
  SetDlgItemText(IDC_SCRIPT, *string);
}
////////////////////////////////////////////////////////////////////////////////
