#ifndef TILESET_SELECTION_DIALOG_HPP
#define TILESET_SELECTION_DIALOG_HPP
#include <afxwin.h>
class CTilesetSelectionDialog : public CDialog
{
public:
  CTilesetSelectionDialog(const char* tileset);
  ~CTilesetSelectionDialog();
  const char* GetTilesetPath() const;
private:
  BOOL OnInitDialog();
  void OnOK();
  afx_msg void OnBrowseForTileset();
private:
  char* m_pSelectedTileset;
  DECLARE_MESSAGE_MAP()
};
#endif
