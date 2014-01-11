#ifndef NEW_MAP_DIALOG_HPP
#define NEW_MAP_DIALOG_HPP
#include <afxwin.h>
#include <string>
class CNewMapDialog : public CDialog
{
public:
  CNewMapDialog();
  CNewMapDialog(int w, int h, const char* tileset);
  int GetMapWidth() const;
  int GetMapHeight() const;
  const char* GetTileset() const;
  bool ShouldTilesetBeExternalToMap();
private:
  BOOL OnInitDialog();
  void OnOK();
  void UpdateButtons();
  afx_msg void OnTilesetBrowse();
  afx_msg void OnOptionChanged();
  bool ValidateValues(std::string& error);
private:
  int         m_MapWidth;
  int         m_MapHeight;
  std::string m_Tileset;
  bool m_ShouldTilesetBeExternalToMap;
  DECLARE_MESSAGE_MAP()
};
#endif
