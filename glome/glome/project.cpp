/* project.cpp
  defines project windows */

#include <iostream.h>
#include <wx/wx.h>
#include <wx/filedlg.h>
#include "types.h"
#include "../common/configfile.hpp"
#include "project.h"


////////////////////////////////////////////////////////////////////////////////

vector<ProjectFrame*> Projects;

////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(SettingsDialog, wxDialog) // project settings dialog
  EVT_BUTTON(wxID_OK, SettingsDialog::OnOk) 
END_EVENT_TABLE();

BEGIN_EVENT_TABLE(ProjectFrame, wxFrame) // project window
  EVT_MENU(PROJFILENEW,           ProjectFrame::OnFileNew          )
  EVT_MENU(PROJFILEOPEN,          ProjectFrame::OnFileOpen         )
  EVT_MENU(PROJFILECLOSE,         ProjectFrame::OnFileClose        )
  EVT_MENU(PROJFILESETTINGS,      ProjectFrame::OnFileSettings     )
  EVT_MENU(PROJFILEABOUT,         ProjectFrame::OnFileAbout        )
  EVT_MENU(PROJFILEEXIT,          ProjectFrame::OnFileExit         )
  
  EVT_MENU(PROJNEWFONT,           ProjectFrame::OnNewFont          )
  EVT_MENU(PROJNEWIMAGE,          ProjectFrame::OnNewImage         )
  EVT_MENU(PROJNEWMAP,            ProjectFrame::OnNewMap           )
  EVT_MENU(PROJNEWSCRIPT,         ProjectFrame::OnNewScript        )
  EVT_MENU(PROJNEWSPRITESET,      ProjectFrame::OnNewSpriteset     )
  EVT_MENU(PROJNEWWINDOWSTYLE,    ProjectFrame::OnNewWindowStyle   )
  
  EVT_MENU(PROJIMPORTANIM,        ProjectFrame::OnImportAnim       )
  EVT_MENU(PROJIMPORTFONT,        ProjectFrame::OnImportFont       )
  EVT_MENU(PROJIMPORTIMAGE,       ProjectFrame::OnImportImage      )
  EVT_MENU(PROJIMPORTMAP,         ProjectFrame::OnImportMap        )
  EVT_MENU(PROJIMPORTSCRIPT,      ProjectFrame::OnImportScript     )
  EVT_MENU(PROJIMPORTSOUND,       ProjectFrame::OnImportSound      )
  EVT_MENU(PROJIMPORTSPRITESET,   ProjectFrame::OnImportSpriteset  )
  EVT_MENU(PROJIMPORTWINDOWSTYLE, ProjectFrame::OnImportWindowStyle)
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////////////////

ProjectFrame::ProjectFrame(const wxChar *title, int xpos, int ypos, int width, 
    int height) : wxFrame((wxFrame *)NULL, -1, title, wxPoint(xpos, ypos), 
    wxSize(width, height)) // project window initializes
{
  // set up the project tree control
  m_ProjectTree = (wxTreeCtrl *) NULL;
  m_ProjectTree = new wxTreeCtrl(this, -1, wxDefaultPosition, wxDefaultSize, -1);
  
  // set up the menubar
  m_FileMenu = new wxMenu;
  m_FileMenu->Append(PROJFILENEW,      "&New Project"     );
  m_FileMenu->Append(PROJFILEOPEN,     "&Open Project"    );
  m_FileMenu->Append(PROJFILECLOSE,    "&Close Project"   );
  m_FileMenu->AppendSeparator();
  m_FileMenu->Append(PROJFILESETTINGS, "&Project Settings");
  m_FileMenu->AppendSeparator();
  m_FileMenu->Append(PROJFILEABOUT,    "&About"           );
  m_FileMenu->Append(PROJFILEEXIT,     "E&xit"            );
  m_NewMenu = new wxMenu;
  m_NewMenu->Append(PROJNEWFONT,        "&Font"        );
  m_NewMenu->Append(PROJNEWIMAGE,       "&Image"       );
  m_NewMenu->Append(PROJNEWMAP,         "&Map"         );
  m_NewMenu->Append(PROJNEWSCRIPT,      "&Script"      );
  m_NewMenu->Append(PROJNEWSPRITESET,   "S&priteset"   );
  m_NewMenu->Append(PROJNEWWINDOWSTYLE, "&Window Style");
  m_ImportMenu = new wxMenu;
  m_ImportMenu->Append(PROJIMPORTANIM,        "&Animation"   );
  m_ImportMenu->Append(PROJIMPORTFONT,        "&Font"        );
  m_ImportMenu->Append(PROJIMPORTIMAGE,       "&Image"       );
  m_ImportMenu->Append(PROJIMPORTMAP,         "&Map"         );
  m_ImportMenu->Append(PROJIMPORTSCRIPT,      "&Script"      );
  m_ImportMenu->Append(PROJIMPORTSOUND,       "S&ound"       );
  m_ImportMenu->Append(PROJIMPORTSPRITESET,   "S&priteset"   );
  m_ImportMenu->Append(PROJIMPORTWINDOWSTYLE, "&Window Style");
  m_TopMenu = new wxMenuBar;
  m_TopMenu->Append(m_FileMenu,   "&File"  );
  m_TopMenu->Append(m_NewMenu,    "&New"   );
  m_TopMenu->Append(m_ImportMenu, "&Import");
  
  SetOpened(false);
  
  SetMenuBar(m_TopMenu);
}

////////////////////////////////////////////////////////////////////////////////

ProjectFrame::~ProjectFrame(){ // project window destructs
}

////////////////////////////////////////////////////////////////////////////////

SettingsDialog::SettingsDialog(ProjectFrame *parent, wxWindowID id, 
  const wxString & title, const wxPoint & position, const wxSize & size, 
  long style) : wxDialog(parent, id, title, position, size, style)
{ // project settings
  ppointer = parent; // this is so disgusting.
  wxPoint p;
  wxSize sz;
  sz.SetWidth(size.GetWidth() - 20); sz.SetHeight(20);
  
  wxString screen_width; screen_width.Printf("%d", parent->m_ScreenWidth);
  wxString screen_height; screen_height.Printf("%d", parent->m_ScreenHeight);
  
  p.x = 6; p.y = 2;
  m_TextGameTitle = new wxTextCtrl(this, -1, parent->m_GameTitle.c_str(), p, sz);
  p.y += sz.GetHeight() + 5;
  m_TextGameScript = new wxTextCtrl(this, -1, parent->m_GameScript.c_str(), p, sz);
  p.y += sz.GetHeight() + 5;
  m_SpinScreenWidth = new wxSpinCtrl(this, -1, screen_width, p, sz, wxSP_ARROW_KEYS, 0, 1600, parent->m_ScreenWidth);
  p.y += sz.GetHeight() + 5;
  m_SpinScreenHeight = new wxSpinCtrl(this, -1, screen_height, p, sz, wxSP_ARROW_KEYS, 0, 1200, parent->m_ScreenHeight);
  
  p.y += sz.GetHeight() + 10;
  wxButton *button_ok = new wxButton(this, wxID_OK, "OK", p, wxDefaultSize);
  p.x += 110;
  wxButton *button_cancel = new wxButton(this, wxID_CANCEL, "Cancel", p, wxDefaultSize);
}

////////////////////////////////////////////////////////////////////////////////

void ProjectFrame::Refresh(){
  if(m_ProjectTree->GetCount())
    m_ProjectTree->DeleteAllItems();

  wxTreeItemId Item_ProjectName  = m_ProjectTree->AddRoot(m_GameTitle.c_str());
 
  wxTreeItemId MapsFolder         = m_ProjectTree->AppendItem(Item_ProjectName, "Maps"         );
  wxTreeItemId SpritesetsFolder   = m_ProjectTree->AppendItem(Item_ProjectName, "Spritesets"   );
  wxTreeItemId ScriptsFolder      = m_ProjectTree->AppendItem(Item_ProjectName, "Scripts"      );
  wxTreeItemId SoundsFolder       = m_ProjectTree->AppendItem(Item_ProjectName, "Sounds"       );
  wxTreeItemId ImagesFolder       = m_ProjectTree->AppendItem(Item_ProjectName, "Images"       );
  wxTreeItemId WindowStylesFolder = m_ProjectTree->AppendItem(Item_ProjectName, "Window Styles");
  wxTreeItemId FontsFolder        = m_ProjectTree->AppendItem(Item_ProjectName, "Fonts"        );
  wxTreeItemId AnimationsFolder   = m_ProjectTree->AppendItem(Item_ProjectName, "Animations"   );
  
  m_ProjectTree->SortChildren(Item_ProjectName);
}

////////////////////////////////////////////////////////////////////////////////

void ProjectFrame::Save(){
  CConfigFile config;
  
  config.WriteString("", "name", m_GameTitle.c_str());
  config.WriteString("", "script", m_GameScript.c_str());
  
  config.WriteInt("", "screen_width", m_ScreenWidth);
  config.WriteInt("", "screen_height", m_ScreenHeight);
  
  config.Save(m_GamePath.c_str());
  
  Refresh();
}

////////////////////////////////////////////////////////////////////////////////

void ProjectFrame::SetOpened(bool opened){
    m_FileMenu->Enable(m_FileMenu->FindItem("&Close Project"), opened);
    m_FileMenu->Enable(m_FileMenu->FindItem("&Project Settings"), opened);
    m_TopMenu->EnableTop(1, opened); //Find a less absolute way to do this.
    m_TopMenu->EnableTop(2, opened);
}

////////////////////////////////////////////////////////////////////////////////

void ProjectFrame::OnFileNew(wxCommandEvent & event){ //create a new project
  // ... create the project
}

////////////////////////////////////////////////////////////////////////////////

void ProjectFrame::OnFileOpen(wxCommandEvent & event){ //open a project
  const wxChar *proj_file_types = _T("Project Files|*.sgm;*.inf|"
                                "All files|*");
  wxFileDialog *OpenFileDialog = new wxFileDialog(this, "Open Project", "", "", proj_file_types, wxOPEN, wxDefaultPosition);
  
  ProjectFrame *where_to;
  
  // do we need to create a new window?
  if(m_GameFile != ""){
    Projects.push_back(new ProjectFrame(gs_PROJTITLE, 50, 50, 150, 200));
    where_to = Projects[Projects.size()-1];
  } else {
    where_to = this;
  }

  if (OpenFileDialog->ShowModal() == wxID_OK){
    where_to->m_GamePath = OpenFileDialog->GetPath(); // the file we open (full path)
    where_to->m_GameFile = OpenFileDialog->GetFilename(); // the file we open (just filename)
    where_to->m_GameDir = OpenFileDialog->GetDirectory(); //the file's parent directory
    
    CConfigFile config(where_to->m_GamePath.c_str()); // open the file as a config file
    
    // set variables for the project
    where_to->m_GameTitle = config.ReadString("", "name", ""); 
    where_to->m_GameScript = config.ReadString("", "script", ""); 
    where_to->m_ScreenWidth = config.ReadInt("", "screen_width", m_ScreenWidth); 
    where_to->m_ScreenHeight = config.ReadInt("", "screen_height", m_ScreenHeight); 
    
    // debug
    cout << "\"" << (where_to->m_GamePath) << "\" loaded.\n";
    cout << "\tGame filename: " << (where_to->m_GameFile) << endl;
    cout << "\tGame title: " << (where_to->m_GameTitle) << endl;
    cout << "\tGame script: " << (where_to->m_GameScript) << endl;
    cout << "\tScreen width: " << (where_to->m_ScreenWidth) << endl;
    cout << "\tScreen height: " << (where_to->m_ScreenHeight) << endl;
    
    // Refresh the project tree, enable the menus
    where_to->SetOpened(true);
    
    wxString frame_title = gs_PROJTITLE;
    frame_title += " - ";
    frame_title += where_to->m_GameFile.c_str();
    where_to->SetTitle(frame_title);
    
    where_to->Refresh();
    
    if(!where_to->IsShown())
      where_to->Show(true);
  }
}

////////////////////////////////////////////////////////////////////////////////

void ProjectFrame::OnFileClose(wxCommandEvent & event){ //close the project, disable stuff
    SetOpened(false);

    if(m_ProjectTree->GetCount())
      m_ProjectTree->DeleteAllItems();  
    
    SetTitle(gs_PROJTITLE);
    
    m_GameFile = "";
    m_GameTitle = "";
    m_GameScript = "";
    m_GamePath = "";
    m_GameDir = "";
    m_ScreenWidth = 0;
    m_ScreenHeight = 0;
}

////////////////////////////////////////////////////////////////////////////////

void ProjectFrame::OnFileSettings(wxCommandEvent & event){ // settings dialog
  // ... configure the project
  SettingsDialog *dialog_settings = new SettingsDialog(this, -1, "Project Settings", wxDefaultPosition, wxSize(300, 200));
  dialog_settings->ShowModal();
}

////////////////////////////////////////////////////////////////////////////////

void ProjectFrame::OnFileAbout(wxCommandEvent & event){ // about dialog
  // set up caption
  wxString pt = _T(gs_PROJTITLE);
  pt.append(_T(gs_PROJVERS));
  pt.append(_T(gs_PROJAUTH));
  
  // display dialog
  wxMessageDialog dialog_about(this, pt, "About SDE", wxOK);
  dialog_about.ShowModal();
}

////////////////////////////////////////////////////////////////////////////////

void ProjectFrame::OnFileExit(wxCommandEvent & event){ //exit sde via menu
  Close(true);
}

////////////////////////////////////////////////////////////////////////////////

void ProjectFrame::OnNewFont(wxCommandEvent & event){ //new font
  // ... new font dialog
}

////////////////////////////////////////////////////////////////////////////////

void ProjectFrame::OnNewImage(wxCommandEvent & event){ //new image
  // ... new image dialog
}

////////////////////////////////////////////////////////////////////////////////

void ProjectFrame::OnNewMap(wxCommandEvent & event){ //new map
  // ... new map dialog
}

////////////////////////////////////////////////////////////////////////////////

void ProjectFrame::OnNewScript(wxCommandEvent & event){ //new script
  // ... new script dialog
}

////////////////////////////////////////////////////////////////////////////////

void ProjectFrame::OnNewSpriteset(wxCommandEvent & event){ //new spriteset
  // ... new spriteset dialog
}

////////////////////////////////////////////////////////////////////////////////

void ProjectFrame::OnNewWindowStyle(wxCommandEvent & event){ //new windowstyle
  // ... new windowstyle dialog
}

////////////////////////////////////////////////////////////////////////////////

void ProjectFrame::OnImportAnim(wxCommandEvent & event){ // import animation
  // ... import animation dialog
}

////////////////////////////////////////////////////////////////////////////////

void ProjectFrame::OnImportFont(wxCommandEvent & event){ // import font
  // ... import font dialog
}

////////////////////////////////////////////////////////////////////////////////

void ProjectFrame::OnImportImage(wxCommandEvent & event){ // import image
  // ... import image dialog
}

////////////////////////////////////////////////////////////////////////////////

void ProjectFrame::OnImportMap(wxCommandEvent & event){ // import map
  // ... import map dialog
}

////////////////////////////////////////////////////////////////////////////////

void ProjectFrame::OnImportScript(wxCommandEvent & event){ // import script
  // ... import script dialog
}

////////////////////////////////////////////////////////////////////////////////

void ProjectFrame::OnImportSound(wxCommandEvent & event){ // import sound
  // ... import sound dialog
}

////////////////////////////////////////////////////////////////////////////////

void ProjectFrame::OnImportSpriteset(wxCommandEvent & event){ // import spriteset
  // ... import spriteset dialog
}

////////////////////////////////////////////////////////////////////////////////

void ProjectFrame::OnImportWindowStyle(wxCommandEvent & event){ // import windowstyle
  // ... import windowstyle dialog
}

////////////////////////////////////////////////////////////////////////////////

void SettingsDialog::OnOk(wxCommandEvent & event){ // Settings Dialog
  ppointer->m_GameTitle = m_TextGameTitle->GetValue().c_str();
  ppointer->m_GameScript = m_TextGameScript->GetValue().c_str();
  
  ppointer->m_ScreenWidth = m_SpinScreenWidth->GetValue();
  ppointer->m_ScreenHeight = m_SpinScreenHeight->GetValue();
  
  EndModal(1);
  
  ppointer->Save();
}

////////////////////////////////////////////////////////////////////////////////