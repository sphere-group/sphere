#ifndef PROJECT_H
#define PROJECT_H

#include <string>
#include <vector>
#include <wx/wx.h>
#include <wx/spinctrl.h>
#include "../common/DefaultFileSystem.hpp"
#include "../common/DefaultFile.hpp"

////////////////////////////////////////////////////////////////////////////////

extern const wxChar *gs_PROJTITLE;
extern const wxChar *gs_PROJVERS;
extern const wxChar *gs_PROJAUTH;

////////////////////////////////////////////////////////////////////////////////
  
class ProjectFrame : public wxFrame {
  public:
    ProjectFrame(const wxChar *title, int xpos, int ypos, int width, int height);
    ~ProjectFrame();
  
    // controls
    wxTreeCtrl   *m_ProjectTree;
    wxMenuBar    *m_TopMenu;
    wxMenu       *m_FileMenu;
    wxMenu       *m_ImportMenu;
    wxMenu       *m_NewMenu;
    
    // project stuff
    std::string m_GameTitle;
    std::string m_GameScript;
    std::string m_GameFile; // filename
    std::string m_GamePath; // path+filename (just for convenience)
    std::string m_GameDir; // path
    int m_ScreenHeight;
    int m_ScreenWidth;
    
    void Save();
    void Refresh();
    void SetOpened(bool); 
    
    // menu functions
    void OnFileNew           (wxCommandEvent & event);
    void OnFileOpen          (wxCommandEvent & event);
    void OnFileClose         (wxCommandEvent & event);
    void OnFileSettings      (wxCommandEvent & event);
    void OnFileExit          (wxCommandEvent & event);
    void OnFileAbout         (wxCommandEvent & event);
    void OnNewFont           (wxCommandEvent & event);
    void OnNewImage          (wxCommandEvent & event);
    void OnNewMap            (wxCommandEvent & event);
    void OnNewScript         (wxCommandEvent & event);
    void OnNewSpriteset      (wxCommandEvent & event);
    void OnNewWindowStyle    (wxCommandEvent & event);
    void OnImportAnim        (wxCommandEvent & event);
    void OnImportFont        (wxCommandEvent & event);
    void OnImportImage       (wxCommandEvent & event);
    void OnImportMap         (wxCommandEvent & event);
    void OnImportScript      (wxCommandEvent & event);
    void OnImportSound       (wxCommandEvent & event);
    void OnImportSpriteset   (wxCommandEvent & event);
    void OnImportWindowStyle (wxCommandEvent & event);
    
  // menubar event table
  DECLARE_EVENT_TABLE();
};

////////////////////////////////////////////////////////////////////////////////

class SettingsDialog : public wxDialog {
  public:
    SettingsDialog( ProjectFrame *parent, wxWindowID id, const wxString & title, 
      const wxPoint & pos = wxDefaultPosition, const wxSize & size = wxDefaultSize, 
      long style = wxDEFAULT_DIALOG_STYLE);
    
    // our controls
    wxTextCtrl * m_TextGameTitle;
    wxTextCtrl * m_TextGameScript;
    wxSpinCtrl * m_SpinScreenWidth;
    wxSpinCtrl * m_SpinScreenHeight;
    
    ProjectFrame *ppointer;
  
  private:
    void OnOk(wxCommandEvent & event);
    
    DECLARE_EVENT_TABLE();
};

////////////////////////////////////////////////////////////////////////////////

enum { // menu events
  PROJFILEEXIT = wxID_HIGHEST + 1, // make sure these aren't used
  PROJFILEOPEN,
  PROJFILECLOSE,
  PROJFILEABOUT,
  PROJFILENEW,
  PROJFILESETTINGS,
  
  PROJNEWFONT,
  PROJNEWIMAGE,
  PROJNEWMAP,
  PROJNEWSCRIPT,
  PROJNEWSPRITESET,
  PROJNEWWINDOWSTYLE,
  
  PROJIMPORTANIM,
  PROJIMPORTFONT,
  PROJIMPORTIMAGE,
  PROJIMPORTMAP,
  PROJIMPORTSCRIPT,
  PROJIMPORTSOUND,
  PROJIMPORTSPRITESET,
  PROJIMPORTWINDOWSTYLE,
};

////////////////////////////////////////////////////////////////////////////////

extern vector<ProjectFrame*> Projects;

////////////////////////////////////////////////////////////////////////////////

#endif
