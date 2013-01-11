#ifndef SPRITE_PROPERTIES_DIALOG_HPP
#define SPRITE_PROPERTIES_DIALOG_HPP
#include <afxwin.h>
class sSpriteset;  // #include "../common/Spriteset.hpp"
class CSpritePropertiesDialog : public CDialog
{
public:
  CSpritePropertiesDialog(sSpriteset* spriteset, int direction, int frame);
  ~CSpritePropertiesDialog();
  BOOL OnInitDialog();
  void OnOK();
  afx_msg void OnNextTile();
  afx_msg void OnPrevTile();
private:
  void UpdateDialog();
  bool StoreFrame();
private:
  sSpriteset* m_Spriteset;
  int         m_Direction;
  int         m_Frame;
  int* m_DelayList;
  DECLARE_MESSAGE_MAP()
};
#endif
