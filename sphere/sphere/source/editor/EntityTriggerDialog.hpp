#ifndef ENTITY_TRIGGER_DIALOG_HPP
#define ENTITY_TRIGGER_DIALOG_HPP
#include <afxwin.h>
#include "../common/Entities.hpp"
class sMap; // #include "../common/Map.hpp"
class CEntityTriggerDialog : public CDialog
{
public:
  CEntityTriggerDialog(sTriggerEntity& trigger, sMap* map);
private:
  afx_msg void OnSize(UINT type, int cx, int cy);
  afx_msg void OnSizing(UINT side, LPRECT rect);
private:
  BOOL OnInitDialog();
  void OnOK();
  afx_msg void OnCheckSyntax();
private:
  sTriggerEntity& m_Trigger;
  sMap* m_Map;
  DECLARE_MESSAGE_MAP()
};
#endif
