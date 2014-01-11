#ifndef DIALOG_SPHERE8_H
#define DIALOG_SPHERE8_H


#include <wx/wx.h>
#include <wx/wfstream.h>
#include <wx/fileconf.h>
#include <wx/dir.h>


class CDialogSphere8 : public wxDialog
{
    public:

        CDialogSphere8(wxWindow* parent, const wxString &title, const wxString &drv_name);

    private:

        void OnClickOk(wxCommandEvent& event);
        void OnClickCancel(wxCommandEvent& event);
        void OnClickFullscreen(wxCommandEvent& event);

        void BuildPalettesList();
        void LoadDefaultConfiguration();
        void LoadConfiguration();
        void SaveConfiguration();

        wxString    m_drv_name;

        wxButton*   m_ok_button;
        wxButton*   m_cancel_button;
        wxCheckBox* m_fullscreen;
        wxCheckBox* m_vsync;
        wxChoice*   m_palettes;

        wxArrayString m_palettes_list;

};



#endif

