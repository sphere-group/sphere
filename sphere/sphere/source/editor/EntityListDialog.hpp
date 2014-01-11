#ifndef CONVOLVE_LIST_DIALOG_HPP
#define CONVOLVE_LIST_DIALOG_HPP
#pragma warning(disable: 4786)
#include <afxwin.h>
#include <vector>
#include "../common/map.hpp"
///////////////////////////////////////////////////////////
class CEntityListDialog : public CDialog
{
public:
  CEntityListDialog(sMap* map);
  ~CEntityListDialog();
private:
  afx_msg void OnSize(UINT type, int cx, int cy);
  afx_msg void OnSizing(UINT side, LPRECT rect);
private:
  sMap* m_Map;
  std::vector<sEntity*> m_Entities;
private:
  afx_msg void OnEntityChanged();
  afx_msg void OnEditEntity();
  afx_msg void OnRenameEntities();
  afx_msg void OnDeleteEntities();
  afx_msg void OnMoveEntities();
  afx_msg void OnMoveEntities(int dx, int dy, int layer);
  std::string GenerateUniquePersonName(std::string current_name, std::string new_name);
  void UpdateButtons();
  void UpdateEntityDetails(char string[2048], int entity_index, sMap* map);
  std::vector<int> GetSelectedEntities();
private:
  BOOL OnInitDialog();
  void OnOK();
  DECLARE_MESSAGE_MAP()
};
///////////////////////////////////////////////////////////
#endif
