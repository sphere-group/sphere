#include "SpritePropertiesDialog.hpp"
#include "../common/Spriteset.hpp"
#include "resource.h"
BEGIN_MESSAGE_MAP(CSpritePropertiesDialog, CDialog)
  
  ON_BN_CLICKED(IDC_NEXT,     OnNextTile)
  ON_BN_CLICKED(IDC_PREVIOUS, OnPrevTile)
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////
CSpritePropertiesDialog::CSpritePropertiesDialog(sSpriteset* spriteset, int direction, int frame)
: CDialog(IDD_SPRITE_PROPERTIES)
, m_Spriteset(spriteset)
, m_Direction(direction)
, m_Frame(frame)
, m_DelayList(NULL)
{
}
////////////////////////////////////////////////////////////////////////////////
CSpritePropertiesDialog::~CSpritePropertiesDialog()
{
  delete m_DelayList;
  m_DelayList = NULL;
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritePropertiesDialog::UpdateDialog()
{
  GetDlgItem(IDC_NEXT)->EnableWindow(m_Spriteset->GetNumFrames(m_Direction) > 1 ? TRUE : FALSE);
  GetDlgItem(IDC_PREVIOUS)->EnableWindow(m_Spriteset->GetNumFrames(m_Direction) > 1 ? TRUE : FALSE);
  int delay = m_DelayList ? m_DelayList[m_Frame] : m_Spriteset->GetFrameDelay(m_Direction, m_Frame);
  SetDlgItemInt(IDC_DELAY, delay);
  CString title;
  title.Format("Frame Properties - %d/%d", m_Frame, m_Spriteset->GetNumFrames(m_Direction));
  SetWindowText(title);
}
////////////////////////////////////////////////////////////////////////////////
BOOL
CSpritePropertiesDialog::OnInitDialog()
{
  m_DelayList = new int[m_Spriteset->GetNumFrames(m_Direction)];
  if (m_DelayList) {
    for (int i = 0; i < m_Spriteset->GetNumFrames(m_Direction); i++) {
      m_DelayList[i] = m_Spriteset->GetFrameDelay(m_Direction, i);
    }
  }
  UpdateDialog();
  CEdit* edit = (CEdit*)GetDlgItem(IDC_DELAY);
  edit->SetSel(0, -1);
  edit->SetFocus();
  return FALSE;
}
////////////////////////////////////////////////////////////////////////////////
bool
CSpritePropertiesDialog::StoreFrame() {
  int delay = GetDlgItemInt(IDC_DELAY);
  if (delay <= 0 || delay > 4096) {
    MessageBox("Delay must be inbetween 1 and 4096", "Frame Properties");
    return false;
  }
  m_DelayList[m_Frame] = delay;
  return true;
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritePropertiesDialog::OnNextTile() {
  if (!m_DelayList) return;
  if (!StoreFrame()) return;
  m_Frame += 1;
  if (m_Frame >= m_Spriteset->GetNumFrames(m_Direction))
    m_Frame = 0;
  UpdateDialog();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CSpritePropertiesDialog::OnPrevTile() {
  if (!m_DelayList) return;
  if (!StoreFrame()) return;
  m_Frame -= 1;
  if (m_Frame < 0)
    m_Frame = m_Spriteset->GetNumFrames(m_Direction) - 1;
  UpdateDialog();
}
////////////////////////////////////////////////////////////////////////////////
void
CSpritePropertiesDialog::OnOK()
{
  if (!StoreFrame()) return;
  int delay = GetDlgItemInt(IDC_DELAY);
  m_Spriteset->SetFrameDelay(m_Direction, m_Frame, delay);
  if (m_DelayList) {
    m_DelayList[m_Frame] = delay;
    for (int i = 0; i < m_Spriteset->GetNumFrames(m_Direction); i++) {
      m_Spriteset->SetFrameDelay(m_Direction, i, m_DelayList[i]);
    }
  }
  CDialog::OnOK();
}
////////////////////////////////////////////////////////////////////////////////
