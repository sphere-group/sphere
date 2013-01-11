#ifndef LINE_SORTER_DIALOG_HPP
#define LINE_SORTER_DIALOG_HPP
#include <afxwin.h>
///////////////////////////////////////////////////////////
class CLineSorterDialog : public CDialog
{
public:
  CLineSorterDialog();
  bool ShouldSortLines();
  bool ShouldReverseLines();
  bool ShouldRemoveDuplicateLines();
  bool ShouldIgnoreCase();
  bool ShouldCompareNumeric();
  int GetStartCharacter();
  int GetStartTab();
private:
  bool m_SortLines;
  bool m_ReverseLines;
  bool m_RemoveDuplicateLines;
  bool m_IgnoreCase;
  bool m_CompareNumeric;
  int m_StartCharacter;
  int m_StartTab;
private:
  void UpdateButtons();
private:
  afx_msg void OnOptionChanged();
private:
  BOOL OnInitDialog();
  void OnOK();
private:
  DECLARE_MESSAGE_MAP()
};
///////////////////////////////////////////////////////////
#endif
