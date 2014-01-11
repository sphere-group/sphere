#ifndef ENTITY_PERSON_DIALOG_HPP
#define ENTITY_PERSON_DIALOG_HPP
#include <afxwin.h>
#include "../common/Entities.hpp"
class sMap; // #include "../common/Map.hpp"
class CEntityPersonDialog : public CDialog
{
public:
  CEntityPersonDialog(sPersonEntity& person, sMap* map);
private:
  afx_msg void OnSize(UINT type, int cx, int cy);
  afx_msg void OnSizing(UINT side, LPRECT rect);
private:
  BOOL OnInitDialog();
  void OnOK();
  afx_msg void OnBrowseSpriteset();
  afx_msg void OnCheckSyntax();
  afx_msg void OnScriptChanged();
  afx_msg void OnGenerateName();
  void SetScript();
  void StoreScript();
private:
  sPersonEntity& m_Person;
  sMap* m_Map;

  int     m_CurrentScript;
  CString m_Scripts[5];
  DECLARE_MESSAGE_MAP()
};
#endif
