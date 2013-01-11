/* main.cpp
   glome app intialization with project window */

#include <wx/wx.h>
#include "main.h"
#include "project.h"

////////////////////////////////////////////////////////////////////////////////

const wxChar *gs_PROJTITLE = "Glome";
const wxChar *gs_PROJVERS  = "v.01a (2002.04.27)";
const wxChar *gs_PROJAUTH  = "Ted Reed, Brandon Mechtley";

IMPLEMENT_APP(GlomeApp)

////////////////////////////////////////////////////////////////////////////////

bool GlomeApp::OnInit(){ // sde opens
  Projects.push_back(new ProjectFrame(gs_PROJTITLE, 50, 50, 150, 200));
  
  Projects[0]->Show(true);
  SetTopWindow(Projects[0]);
  
  return true;
}

////////////////////////////////////////////////////////////////////////////////