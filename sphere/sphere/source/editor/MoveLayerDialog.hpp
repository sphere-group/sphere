#include "ListDialog.hpp"
#include "../common/Map.hpp"
class CLayerMoveDialog : public CListDialog {
public:
  CLayerMoveDialog(const sMap* m_Map);
  int GetLayer() { return GetSelection() - 1; }
  bool LayerChanged() { return GetSelection() != 0; }
};
