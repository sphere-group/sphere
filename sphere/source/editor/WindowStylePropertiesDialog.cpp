#include "WindowStylePropertiesDialog.hpp"
#include "resource.h"
////////////////////////////////////////////////////////////////////////////////
CWindowStylePropertiesDialog::CWindowStylePropertiesDialog(sWindowStyle* ws)
: CDialog(IDD_WINDOW_STYLE_PROPERTIES)
, m_WindowStyle(ws)
{
}
////////////////////////////////////////////////////////////////////////////////
CWindowStylePropertiesDialog::~CWindowStylePropertiesDialog()
{
}
////////////////////////////////////////////////////////////////////////////////
BOOL
CWindowStylePropertiesDialog::OnInitDialog()
{
  // background mode
  switch (m_WindowStyle->GetBackgroundMode()) {
    case sWindowStyle::TILED:     CheckDlgButton(IDC_TILE_BACKGROUND,     BST_CHECKED); break;
    case sWindowStyle::STRETCHED: CheckDlgButton(IDC_STRETCH_BACKGROUND,  BST_CHECKED); break;
    case sWindowStyle::GRADIENT:  CheckDlgButton(IDC_GRADIENT_BACKGROUND, BST_CHECKED); break;
    case sWindowStyle::TILED_GRADIENT: CheckDlgButton(IDC_TILED_GRADIENT_BACKGROUND, BST_CHECKED); break;
    case sWindowStyle::STRETCHED_GRADIENT: CheckDlgButton(IDC_STRETCHED_GRADIENT_BACKGROUND, BST_CHECKED); break;
  }
  // background corner colors
  InitializeView(m_UpperLeftView,  sWindowStyle::BACKGROUND_UPPER_LEFT,  IDC_UPPER_LEFT);
  InitializeView(m_UpperRightView, sWindowStyle::BACKGROUND_UPPER_RIGHT, IDC_UPPER_RIGHT);
  InitializeView(m_LowerLeftView,  sWindowStyle::BACKGROUND_LOWER_LEFT,  IDC_LOWER_LEFT);
  InitializeView(m_LowerRightView, sWindowStyle::BACKGROUND_LOWER_RIGHT, IDC_LOWER_RIGHT);
  SetDlgItemInt(IDC_EDGEOFFSET_0, m_WindowStyle->GetEdgeOffset(0));
  SetDlgItemInt(IDC_EDGEOFFSET_1, m_WindowStyle->GetEdgeOffset(1));
  SetDlgItemInt(IDC_EDGEOFFSET_2, m_WindowStyle->GetEdgeOffset(2));
  SetDlgItemInt(IDC_EDGEOFFSET_3, m_WindowStyle->GetEdgeOffset(3));
  return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
void
CWindowStylePropertiesDialog::InitializeView(CRGBAView& view, int corner, int control_id)
{
  RECT rect;
  GetDlgItem(control_id)->GetWindowRect(&rect);
  ScreenToClient(&rect);
  view.Create(
    this,
    rect.left,
    rect.top,
    rect.right - rect.left,
    rect.bottom - rect.top,
    m_WindowStyle->GetBackgroundColor(corner)
  );
}
////////////////////////////////////////////////////////////////////////////////
void
CWindowStylePropertiesDialog::OnOK()
{
  if (IsDlgButtonChecked(IDC_TILE_BACKGROUND) == BST_CHECKED) {
    m_WindowStyle->SetBackgroundMode(sWindowStyle::TILED);
  } else if (IsDlgButtonChecked(IDC_STRETCH_BACKGROUND) == BST_CHECKED) {
    m_WindowStyle->SetBackgroundMode(sWindowStyle::STRETCHED);
  } else if (IsDlgButtonChecked(IDC_GRADIENT_BACKGROUND) == BST_CHECKED) {
    m_WindowStyle->SetBackgroundMode(sWindowStyle::GRADIENT);
  } else if (IsDlgButtonChecked(IDC_TILED_GRADIENT_BACKGROUND) == BST_CHECKED) {
    m_WindowStyle->SetBackgroundMode(sWindowStyle::TILED_GRADIENT);
  } else if (IsDlgButtonChecked(IDC_STRETCHED_GRADIENT_BACKGROUND) == BST_CHECKED) {
    m_WindowStyle->SetBackgroundMode(sWindowStyle::STRETCHED_GRADIENT);
  }
  m_WindowStyle->SetBackgroundColor(sWindowStyle::BACKGROUND_UPPER_LEFT,  m_UpperLeftView.GetColor());
  m_WindowStyle->SetBackgroundColor(sWindowStyle::BACKGROUND_UPPER_RIGHT, m_UpperRightView.GetColor());
  m_WindowStyle->SetBackgroundColor(sWindowStyle::BACKGROUND_LOWER_LEFT,  m_LowerLeftView.GetColor());
  m_WindowStyle->SetBackgroundColor(sWindowStyle::BACKGROUND_LOWER_RIGHT, m_LowerRightView.GetColor());
  if (GetDlgItemInt(IDC_EDGEOFFSET_0) < 0 || GetDlgItemInt(IDC_EDGEOFFSET_0) > 255
   || GetDlgItemInt(IDC_EDGEOFFSET_1) < 0 || GetDlgItemInt(IDC_EDGEOFFSET_1) > 255
   || GetDlgItemInt(IDC_EDGEOFFSET_2) < 0 || GetDlgItemInt(IDC_EDGEOFFSET_2) > 255
   || GetDlgItemInt(IDC_EDGEOFFSET_3) < 0 || GetDlgItemInt(IDC_EDGEOFFSET_3) > 255) {
    MessageBox("Edge offsets must be between 0 and 255");
    return;
  }
  m_WindowStyle->SetEdgeOffset(0, GetDlgItemInt(IDC_EDGEOFFSET_0));
  m_WindowStyle->SetEdgeOffset(1, GetDlgItemInt(IDC_EDGEOFFSET_1));
  m_WindowStyle->SetEdgeOffset(2, GetDlgItemInt(IDC_EDGEOFFSET_2));
  m_WindowStyle->SetEdgeOffset(3, GetDlgItemInt(IDC_EDGEOFFSET_3));
  CDialog::OnOK();
}
////////////////////////////////////////////////////////////////////////////////
