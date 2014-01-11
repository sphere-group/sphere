#ifndef ADJUST_BORDERS_DIALOG_HPP
#define ADJUST_BORDERS_DIALOG_HPP
#include <string>
#include <afxwin.h>
///////////////////////////////////////////////////////////
class CAdjustBordersDialog : public CDialog
{
public:
  CAdjustBordersDialog(int current_x, int current_y, int current_width, int current_height,
                       int min_x, int min_y, int max_x, int max_y,
                       int min_width = 1, int min_height = 1, int max_width = 4096, int max_height = 4096);
  int GetTopPixels();
  int GetRightPixels();
  int GetBottomPixels();
  int GetLeftPixels();
private:
  int m_TopPixels;
  int m_RightPixels;
  int m_BottomPixels;
  int m_LeftPixels;
  int m_CurrentX;
  int m_CurrentY;
  int m_CurrentWidth;
  int m_CurrentHeight;
  int m_MinX;
  int m_MinY;
  int m_MaxX;
  int m_MaxY;
  int m_MinWidth;
  int m_MinHeight;
  int m_MaxWidth;
  int m_MaxHeight;
private:
  void UpdateButtons();
  bool ValidateValues(std::string& error);
private:
  afx_msg void OnOptionChanged();
private:
  BOOL OnInitDialog();
  void OnOK();
  DECLARE_MESSAGE_MAP()
};
///////////////////////////////////////////////////////////
#endif
