#include "MoveLayerDialog.hpp"
////////////////////////////////////////////////////////////////////////////////
static inline std::string itos(int i)
{
  char s[20];
  sprintf(s, "%d", i);
  return s;
}
////////////////////////////////////////////////////////////////////////////////
CLayerMoveDialog::CLayerMoveDialog(const sMap* m_Map)
: CListDialog()
{
  SetCaption("Move Entities To Layer");
  AddItem("No change");
  for (int i = 0; i < m_Map->GetNumLayers(); i++)
  {
    std::string layer_info = itos(i) + " - " + m_Map->GetLayer(i).GetName();
    if (m_Map->GetStartLayer() == i) {
      layer_info += " - ST";
    }
    AddItem(layer_info.c_str());
  }
}
////////////////////////////////////////////////////////////////////////////////
