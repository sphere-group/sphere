#ifndef DIALOG_SPHEREGL_H
#define DIALOG_SPHEREGL_H


#include <wx/wx.h>
#include <wx/wfstream.h>
#include <wx/fileconf.h>


class CDialogSphereGL : public wxDialog
{
    public:

        CDialogSphereGL(wxWindow* parent, const wxString &title, const wxString &drv_name);

    private:

        void OnClickOk(wxCommandEvent& event);
        void OnClickCancel(wxCommandEvent& event);
        void OnClickFullscreen(wxCommandEvent& event);
        void OnClickScale(wxCommandEvent& event);

        void LoadDefaultConfiguration();
        void LoadConfiguration();
        void SaveConfiguration();

        wxString    m_drv_name;

        wxButton*   m_ok_button;
        wxButton*   m_cancel_button;
        wxRadioBox* m_bit_depths;
        wxCheckBox* m_bilinear_filter;
        wxCheckBox* m_scale;
        wxCheckBox* m_fullscreen;
        wxCheckBox* m_vsync;


};



#endif

