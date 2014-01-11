#ifndef DIALOG_SPHERE32_H
#define DIALOG_SPHERE32_H


#include <wx/wx.h>
#include <wx/wfstream.h>
#include <wx/fileconf.h>


class CDialogSphere32 : public wxDialog
{
    public:

        CDialogSphere32(wxWindow* parent, const wxString &title, const wxString &drv_name);

    private:

        void OnClickOk(wxCommandEvent& event);
        void OnClickCancel(wxCommandEvent& event);
        void OnClickFullscreen(wxCommandEvent& event);
        void OnClickScale(wxCommandEvent& event);

        void LoadDefaultConfiguration();
        void LoadConfiguration();
        void SaveConfiguration();

        wxString      m_drv_name;

        wxButton*     m_ok_button;
        wxButton*     m_cancel_button;

        wxCheckBox*   m_scale;
        wxRadioBox*   m_filter;

        wxCheckBox*   m_fullscreen;
        wxCheckBox*   m_vsync;
        wxTextCtrl*   m_vexpand;
        wxTextCtrl*   m_hexpand;
        wxStaticText* m_vexpand_str;
        wxStaticText* m_hexpand_str;

};



#endif

