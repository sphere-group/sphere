#ifndef LIST_DIALOG_HPP
#define LIST_DIALOG_HPP
#pragma warning(disable: 4786)
#include <afxwin.h>
#include <vector>
#include <string>
///////////////////////////////////////////////////////////
class CListDialog : public CDialog
{
public:
  CListDialog();
  ~CListDialog();
  void SetCaption(const char* caption);
  int AddItem(const char* string);
  
  int GetSelection() const;
  void SetSelection(int i);
  const char* GetItemText(int index) const;
private:
  BOOL OnInitDialog();
  void OnOK();
  void UpdateButtons();
private:
  void OnSelectionChanged();
  int m_Value;
  std::vector<std::string> m_Items;
  std::string m_Caption;
  //std::string m_Text;
private:
  DECLARE_MESSAGE_MAP()
};
///////////////////////////////////////////////////////////
#endif
