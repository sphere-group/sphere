#ifndef LAYER_PROPERTIES_DIALOG_HPP
#define LAYER_PROPERTIES_DIALOG_HPP
#include <afxwin.h>
class sLayer;  // #include "../sphere/common/Map.hpp"
class CLayerPropertiesDialog : public CDialog
{
public:
  CLayerPropertiesDialog(sLayer& layer, bool parallax_allowed);
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnEnableParallax();
  afx_msg void OnHScroll(UINT code, UINT pos, CScrollBar* sb);
  void UpdateParallaxEnabled();
  void UpdateLabels();
private:
  sLayer& m_Layer;
  bool m_ParallaxAllowed;
  DECLARE_MESSAGE_MAP()
};
#endif
