#ifndef ZONE_EDIT_DIALOG_HPP
#define ZONE_EDIT_DIALOG_HPP
#include <afxwin.h>
#include "../common/Map.hpp"
class CZoneEditDialog : public CDialog
{
public:
  CZoneEditDialog(sMap::sZone& zone, int zone_id, sMap* map);
private:
  afx_msg void OnSize(UINT type, int cx, int cy);
  afx_msg void OnSizing(UINT side, LPRECT rect);
private:
  BOOL OnInitDialog();
  void OnOK();
  afx_msg void OnCheckSyntax();
private:
  sMap::sZone& m_Zone;
  sMap* m_Map;
  int m_ZoneIndex;
  DECLARE_MESSAGE_MAP()
};
#endif
