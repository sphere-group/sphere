#include "LayerPropertiesDialog.hpp"
#include "resource.h"
#include "../common/Map.hpp"

const int MIN_RANGE = -160;
const int MAX_RANGE = 160;
const int PAGE_SIZE = 10;

BEGIN_MESSAGE_MAP(CLayerPropertiesDialog, CDialog)
  ON_BN_CLICKED(IDC_HAS_PARALLAX, OnEnableParallax)
  ON_WM_HSCROLL()
  
END_MESSAGE_MAP()

static inline int FloatToScrollValue(float32 f) {
  return (int) ((f + 0.05) * 10); // sadly, there is roundoff error
}

static inline float32 ScrollValueToFloat(int value) {
  return (float32)value / 10;
}

////////////////////////////////////////////////////////////////////////////////
CLayerPropertiesDialog::CLayerPropertiesDialog(sLayer& layer, bool parallax_allowed)
: CDialog(IDD_LAYER_PROPERTIES)
, m_Layer(layer)
, m_ParallaxAllowed(parallax_allowed)
{
}

////////////////////////////////////////////////////////////////////////////////
BOOL
CLayerPropertiesDialog::OnInitDialog()
{
  // set the default values in the dialog box
  SetDlgItemText(IDC_NAME,  m_Layer.GetName());
  SetDlgItemInt(IDC_WIDTH,  m_Layer.GetWidth());
  SetDlgItemInt(IDC_HEIGHT, m_Layer.GetHeight());

  CEdit* name = (CEdit*)GetDlgItem(IDC_NAME);
  name->SetFocus();
  name->SetSel(0, -1); // select all of the text

  // check "reflective" button
  CheckDlgButton(IDC_REFLECTIVE, (m_Layer.IsReflective() ? BST_CHECKED : BST_UNCHECKED));

  // check "has parallax" button
  CheckDlgButton(IDC_HAS_PARALLAX, (m_Layer.HasParallax() ? BST_CHECKED : BST_UNCHECKED));

  // if parallax is not allowed, disable "enable parallax" button
  if (m_ParallaxAllowed == false) {
    GetDlgItem(IDC_HAS_PARALLAX)->EnableWindow(FALSE);
    CheckDlgButton(IDC_HAS_PARALLAX, BST_UNCHECKED);
  }

  // set scroller ranges
  SendDlgItemMessage(IDC_HORIZONTAL_PARALLAX,  TBM_SETRANGE, TRUE, MAKELONG(MIN_RANGE, MAX_RANGE));
  SendDlgItemMessage(IDC_VERTICAL_PARALLAX,    TBM_SETRANGE, TRUE, MAKELONG(MIN_RANGE, MAX_RANGE));
  SendDlgItemMessage(IDC_HORIZONTAL_SCROLLING, TBM_SETRANGE, TRUE, MAKELONG(MIN_RANGE, MAX_RANGE));
  SendDlgItemMessage(IDC_VERTICAL_SCROLLING,   TBM_SETRANGE, TRUE, MAKELONG(MIN_RANGE, MAX_RANGE));

  // set scroller positions
  SendDlgItemMessage(IDC_HORIZONTAL_PARALLAX,  TBM_SETPOS, TRUE, FloatToScrollValue(m_Layer.GetXParallax()));
  SendDlgItemMessage(IDC_VERTICAL_PARALLAX,    TBM_SETPOS, TRUE, FloatToScrollValue(m_Layer.GetYParallax()));
  SendDlgItemMessage(IDC_HORIZONTAL_SCROLLING, TBM_SETPOS, TRUE, FloatToScrollValue(m_Layer.GetXScrolling()));
  SendDlgItemMessage(IDC_VERTICAL_SCROLLING,   TBM_SETPOS, TRUE, FloatToScrollValue(m_Layer.GetYScrolling()));

  UpdateParallaxEnabled();
  UpdateLabels();

  return FALSE;  // we've set the focus
}

////////////////////////////////////////////////////////////////////////////////
void
CLayerPropertiesDialog::OnOK()
{
  CString name;
  GetDlgItemText(IDC_NAME, name);
  m_Layer.SetName(name);

  int width = GetDlgItemInt(IDC_WIDTH);
  int height = GetDlgItemInt(IDC_HEIGHT);

  if (width < 1 || width > 4096 ||
      height < 1 || height > 4096) {
    MessageBox("width and height must be between 1 and 4096");
    return;
  }

  m_Layer.Resize(width, height);
  m_Layer.SetReflective(IsDlgButtonChecked(IDC_REFLECTIVE) == BST_CHECKED);
  // close the dialog
  CDialog::OnOK();
}

////////////////////////////////////////////////////////////////////////////////
void
CLayerPropertiesDialog::OnEnableParallax()
{
  m_Layer.EnableParallax(!m_Layer.HasParallax());
  UpdateParallaxEnabled();
}

////////////////////////////////////////////////////////////////////////////////
void
CLayerPropertiesDialog::OnHScroll(UINT code, UINT pos, CScrollBar* sb)
{
  float32 f = ScrollValueToFloat(sb->SendMessage(TBM_GETPOS));

  // put the modified value back
  if (sb == GetDlgItem(IDC_HORIZONTAL_PARALLAX)) {
    m_Layer.SetXParallax(f);
  } else if (sb == GetDlgItem(IDC_VERTICAL_PARALLAX)) {
    m_Layer.SetYParallax(f);
  } else if (sb == GetDlgItem(IDC_HORIZONTAL_SCROLLING)) {
    m_Layer.SetXScrolling(f);
  } else if (sb == GetDlgItem(IDC_VERTICAL_SCROLLING)) {
    m_Layer.SetYScrolling(f);
  }

  UpdateLabels();
}

////////////////////////////////////////////////////////////////////////////////
void
CLayerPropertiesDialog::UpdateParallaxEnabled()
{
  bool enabled = (m_ParallaxAllowed && m_Layer.HasParallax());

  GetDlgItem(IDC_HORIZONTAL_PARALLAX) ->EnableWindow(enabled);
  GetDlgItem(IDC_VERTICAL_PARALLAX)   ->EnableWindow(enabled);
  GetDlgItem(IDC_HORIZONTAL_SCROLLING)->EnableWindow(enabled);
  GetDlgItem(IDC_VERTICAL_SCROLLING)  ->EnableWindow(enabled);
}

////////////////////////////////////////////////////////////////////////////////
void
CLayerPropertiesDialog::UpdateLabels()
{
  char label[80];

  sprintf(label, "%.1f", m_Layer.GetXParallax());  SetDlgItemText(IDC_HORIZONTAL_PARALLAX_VALUE,  label);
  sprintf(label, "%.1f", m_Layer.GetYParallax());  SetDlgItemText(IDC_VERTICAL_PARALLAX_VALUE,    label);
  sprintf(label, "%.1f", m_Layer.GetXScrolling()); SetDlgItemText(IDC_HORIZONTAL_SCROLLING_VALUE, label);
  sprintf(label, "%.1f", m_Layer.GetYScrolling()); SetDlgItemText(IDC_VERTICAL_SCROLLING_VALUE,   label);
}

////////////////////////////////////////////////////////////////////////////////
