#ifndef CHECKLIST_DIALOG_HPP
#define CHECKLIST_DIALOG_HPP
#pragma warning(disable: 4786)
#include <afxwin.h>
#include <vector>
#include <string>
///////////////////////////////////////////////////////////
class CCheckListDialog : public CDialog
{
public:
  CCheckListDialog();
  ~CCheckListDialog();
  void SetCaption(const char* caption);
  bool AddItem(const char* string, bool checked); 
  bool IsChecked(int i) const;
  void SetMinChecked(int num);
  void SetMaxChecked(int num);
private:
  afx_msg void OnSize(UINT type, int cx, int cy);
  afx_msg void OnSizing(UINT side, LPRECT rect);
private:
  BOOL OnInitDialog();
  void OnOK();
  void UpdateButtons();
  bool ValidateValues(std::string& error);
private:
  CCheckListBox m_CheckList;
  int m_MinChecked;
  int m_MaxChecked;
  std::vector<std::string> m_Items;
  std::vector<bool> m_CheckStates;
  std::string m_Caption;
  //std::string m_Text;
private:
  DECLARE_MESSAGE_MAP()
};
///////////////////////////////////////////////////////////
#endif
