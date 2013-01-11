#ifndef TOOL_PALETTE_HPP
#define TOOL_PALETTE_HPP
#pragma warning(disable : 4786)
#include <afxcmn.h>
#include <vector>
#include "PaletteWindow.hpp"
class CMainWindow;
class IToolPaletteHandler
{
public:
  virtual void TP_ToolSelected(int tool, int tool_index) = 0;
};
class CToolPalette : public CPaletteWindow
{
public:
  CToolPalette(CDocumentWindow* owner, IToolPaletteHandler* handler, const char* name, RECT rect, bool visible);
  virtual void Destroy();
  void AddTool(int icon_id, const char* label);
private:
  afx_msg void OnSize(UINT type, int cx, int cy);
  afx_msg void OnPaint();
  afx_msg BOOL OnNeedText(UINT id, NMHDR* nmhdr, LRESULT* result);
  afx_msg void OnToolSelected(UINT id);
private:
  IToolPaletteHandler* m_Handler;
  std::vector<CButton*> m_Buttons;
  std::vector<std::string> m_Labels;
  int m_CurrentTool;
  
  friend CMainWindow;
  DECLARE_MESSAGE_MAP()
};
#endif
