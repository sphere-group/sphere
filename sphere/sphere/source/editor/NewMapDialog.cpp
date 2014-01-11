#include "NewMapDialog.hpp"
#include "Project.hpp"
#include "FileDialogs.hpp"
#include "resource.h"
#include "translate.hpp"
#include "Editor.hpp"
BEGIN_MESSAGE_MAP(CNewMapDialog, CDialog)
  ON_COMMAND(IDC_TILESET_BROWSE, OnTilesetBrowse)
  ON_EN_CHANGE(IDC_WIDTH, OnOptionChanged)
  ON_EN_CHANGE(IDC_HEIGHT, OnOptionChanged)
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////
CNewMapDialog::CNewMapDialog()
: CDialog(IDD_NEW_MAP)
, m_MapWidth(64)
, m_MapHeight(64)
{
}
////////////////////////////////////////////////////////////////////////////////
CNewMapDialog::CNewMapDialog(int w, int h, const char* tileset)
: CDialog(IDD_NEW_MAP)
, m_MapWidth(w)
, m_MapHeight(h)
, m_ShouldTilesetBeExternalToMap(false)
{
  if (tileset != NULL)
    m_Tileset = tileset;
}
////////////////////////////////////////////////////////////////////////////////
int
CNewMapDialog::GetMapWidth() const
{
  return m_MapWidth;
}
////////////////////////////////////////////////////////////////////////////////
int
CNewMapDialog::GetMapHeight() const
{
  return m_MapHeight;
}
////////////////////////////////////////////////////////////////////////////////
const char*
CNewMapDialog::GetTileset() const
{
  return m_Tileset.c_str();
}
////////////////////////////////////////////////////////////////////////////////
bool
CNewMapDialog::ShouldTilesetBeExternalToMap() {
  return m_ShouldTilesetBeExternalToMap;
}
////////////////////////////////////////////////////////////////////////////////
BOOL
CNewMapDialog::OnInitDialog()
{
  CDialog::OnInitDialog();
  TranslateDialog(m_hWnd);
  // put default values into the edit boxes
  SetDlgItemInt(IDC_WIDTH, m_MapWidth);
  SetDlgItemInt(IDC_HEIGHT, m_MapHeight);
  GetDlgItem(IDC_TILESET)->SetWindowText(m_Tileset.c_str());
  // set the focus and selection and tell the dialog not to set the focus
  GetDlgItem(IDC_WIDTH)->SetFocus();
  ((CEdit*)GetDlgItem(IDC_WIDTH))->SetSel(0, -1);
  return FALSE;
}
////////////////////////////////////////////////////////////////////////////////
bool
CNewMapDialog::ValidateValues(std::string& error)
{
  int width  = GetDlgItemInt(IDC_WIDTH);
  int height = GetDlgItemInt(IDC_HEIGHT);
  if (width < 1 || height < 1)
  {
    error = "Width and height must be at least 1.";
    return false;
  }
  if (width > 4096 || height > 4096)
  {
    error = "Width and height must be at most 4096.";
    return false;
  }
  m_MapWidth  = width;
  m_MapHeight = height;
  return true;
}
////////////////////////////////////////////////////////////////////////////////
void
CNewMapDialog::UpdateButtons()
{
  BOOL enabled = TRUE;
  std::string error;
  if (!ValidateValues(error)) {
    enabled = FALSE;
    GetStatusBar()->SetWindowText(error.c_str());
  } else {
    GetStatusBar()->SetWindowText("");
  }
  if (GetDlgItem(IDOK)) {
    GetDlgItem(IDOK)->EnableWindow(enabled); 
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CNewMapDialog::OnOptionChanged()
{
  UpdateButtons();
}
////////////////////////////////////////////////////////////////////////////////
void
CNewMapDialog::OnOK()
{
  std::string error;
  if (!ValidateValues(error)) {
    MessageBox(error.c_str());
    return;
  }
  char tileset[MAX_PATH];
  GetDlgItemText(IDC_TILESET, tileset, MAX_PATH);
  if (strlen(tileset) == 0)
  {
    if (MessageBox("Are you sure you want to use an empty tileset?", NULL, MB_YESNO) == IDNO)
      return;
  } else {
    if (MessageBox("Store the tileset as part of the map?", NULL, MB_YESNO) == IDNO) {
      m_ShouldTilesetBeExternalToMap = true;
    }
  }
  m_Tileset   = tileset;
  CDialog::OnOK();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CNewMapDialog::OnTilesetBrowse()
{
  CTilesetFileDialog dialog(FDM_OPEN);
  if (dialog.DoModal() == IDOK)
    SetDlgItemText(IDC_TILESET, dialog.GetPathName());
}
////////////////////////////////////////////////////////////////////////////////
