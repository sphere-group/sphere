
#include <wx/statline.h>

#include "DialogSphere32.h"


enum
{
    SCALE_NO = 0,
    SCALE_2X,
    SCALE_3X,
    SCALE_4X,
};


enum
{
    ID_FULLSCREEN = 0,
    ID_VSYNC,
    ID_VEXPAND,
    ID_HEXPAND,
    ID_SCALE,
    ID_FILTER,
};


CDialogSphere32::CDialogSphere32(wxWindow* parent, const wxString& title, const wxString &drv_name)
               : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize,
                          wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX)
{
    const wxString scale_opts[] =
    {
          _("No"),
        wxT("2x"),
        wxT("3x"),
        wxT("4x"),
    };


    const wxString filter_opts[] =
    {
          _("None"),
        wxT("Scale2x"),
        wxT("Eagle"),
        wxT("hq2x"),
        wxT("2xSaI"),
        wxT("Super 2xSaI"),
        wxT("Super Eagle"),
    };

    m_drv_name = drv_name;

    wxStaticLine* sline = new wxStaticLine(this, wxID_ANY);

    wxBoxSizer* vbox    = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* vbox0   = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* vbox1   = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* hbox1   = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* hbox2   = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* hbox3   = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* hbox4   = new wxBoxSizer(wxHORIZONTAL);

    wxStaticBox*      sbox_exp_res = new wxStaticBox(this,  wxID_ANY, _("Expand Resolution"));
    wxStaticBoxSizer* sboxs        = new wxStaticBoxSizer(sbox_exp_res, wxVERTICAL);

    m_ok_button     = new wxButton(this,   wxID_OK,       _("OK"));
    m_cancel_button = new wxButton(this,   wxID_CANCEL,   _("Cancel"));

    m_fullscreen    = new wxCheckBox(this, ID_FULLSCREEN, _("Fullscreen"));
    m_vsync         = new wxCheckBox(this, ID_VSYNC,      _("VSync"));

    m_vexpand_str   = new wxStaticText(this, wxID_ANY, _("pixels vertically"));
    m_vexpand       = new wxTextCtrl(this, ID_VEXPAND,    wxT("0"), wxDefaultPosition, wxSize(50, -1), wxTE_CENTRE);
    m_vexpand->SetMaxLength(4);
    m_hexpand_str   = new wxStaticText(this, wxID_ANY, _("pixels horizontally"));
    m_hexpand       = new wxTextCtrl(this, ID_HEXPAND,    wxT("0"), wxDefaultPosition, wxSize(50, -1), wxTE_CENTRE);
    m_hexpand->SetMaxLength(4);

    m_scale         = new wxCheckBox(this, ID_SCALE,      _("Scale") + wxString(wxT(" (2x)")));
    m_filter        = new wxRadioBox(this, ID_FILTER,     _("Filter"), wxDefaultPosition, wxDefaultSize, 7, filter_opts, 4, wxRA_SPECIFY_ROWS);

    vbox1->Add(-1, 10);
    vbox1->Add(m_fullscreen);
    vbox1->Add(-1, 5);
    vbox1->Add(m_vsync);
    vbox1->Add(-1, 5);
    vbox1->Add(m_scale);

    hbox3->Add(m_vexpand);
    hbox3->Add(5, -1);
    hbox3->Add(m_vexpand_str, 0, wxALIGN_CENTER_VERTICAL);

    hbox4->Add(m_hexpand);
    hbox4->Add(5, -1);
    hbox4->Add(m_hexpand_str, 0, wxALIGN_CENTER_VERTICAL);

    sboxs->Add(hbox4);
    sboxs->Add(-1, 5);
    sboxs->Add(hbox3);

    hbox1->Add(vbox1);
    hbox1->Add(20, -1);
    hbox1->Add(sboxs);

    hbox2->Add(m_cancel_button);
    hbox2->Add(10, -1);
    hbox2->Add(m_ok_button);

    vbox0->Add(hbox1);
    vbox0->Add(-1, 10);
    vbox0->Add(m_filter);
    vbox0->Add(-1, 10);
    vbox0->Add(sline, 1, wxEXPAND);
    vbox0->Add(-1, 10);
    vbox0->Add(hbox2, 0, wxALIGN_RIGHT);

    vbox->Add(vbox0, 1, wxEXPAND | wxALL, 10);

    SetSizer(vbox);
    Fit();

    Center();

    Connect(wxID_OK,         wxEVT_COMMAND_BUTTON_CLICKED,    wxCommandEventHandler(CDialogSphere32::OnClickOk));
    Connect(wxID_CANCEL,     wxEVT_COMMAND_BUTTON_CLICKED,    wxCommandEventHandler(CDialogSphere32::OnClickCancel));
    Connect(ID_FULLSCREEN,   wxEVT_COMMAND_CHECKBOX_CLICKED,  wxCommandEventHandler(CDialogSphere32::OnClickFullscreen));
    Connect(ID_SCALE,        wxEVT_COMMAND_CHECKBOX_CLICKED,  wxCommandEventHandler(CDialogSphere32::OnClickScale));


    LoadConfiguration();

}

//////////////////////////////////////////////////////////////////////////////////////
void
CDialogSphere32::OnClickOk(wxCommandEvent& event)
{
    SaveConfiguration();
    EndModal(1);
}

//////////////////////////////////////////////////////////////////////////////////////
void
CDialogSphere32::OnClickCancel(wxCommandEvent& event)
{
    EndModal(0);
}

//////////////////////////////////////////////////////////////////////////////////////
void
CDialogSphere32::OnClickFullscreen(wxCommandEvent& event)
{
    if (m_fullscreen->IsChecked())
    {
        m_vexpand->Enable();
        m_hexpand->Enable();
        m_vexpand_str->Enable();
        m_hexpand_str->Enable();

        m_vsync->Enable();
    }
    else
    {
        m_vexpand->Enable(false);
        m_hexpand->Enable(false);
        m_vexpand_str->Enable(false);
        m_hexpand_str->Enable(false);

        m_vsync->Enable(false);
    }
}

//////////////////////////////////////////////////////////////////////////////////////
void
CDialogSphere32::OnClickScale(wxCommandEvent& event)
{
    if (m_scale->IsChecked())
        m_filter->Enable();
    else
        m_filter->Enable(false);
}

//////////////////////////////////////////////////////////////////////////////////////
void
CDialogSphere32::LoadDefaultConfiguration()
{
    m_fullscreen->SetValue(false);
    m_vsync->SetValue(false);

    m_vexpand->SetValue(wxT("0"));
    m_hexpand->SetValue(wxT("0"));

    m_scale->SetValue(false);
    m_filter->SetSelection(0);

    m_vexpand->Enable(false);
    m_hexpand->Enable(false);
    m_vexpand_str->Enable(false);
    m_hexpand_str->Enable(false);
    m_vsync->Enable(false);
    m_filter->Enable(false);
}

//////////////////////////////////////////////////////////////////////////////////////
void
CDialogSphere32::LoadConfiguration()
{
    wxString filename(wxGetCwd() + wxT("/system/video/") + m_drv_name + wxT(".cfg"));

    if (!wxFile::Exists(filename))
    {
        LoadDefaultConfiguration();
        return;
    }

    wxFileInputStream is(filename);

    if (!is.IsOk())
    {
        LoadDefaultConfiguration();
        return;
    }

    wxFileConfig file(is);

    wxString sDummy;
    long     lDummy;
    bool     bDummy;

    file.Read(m_drv_name + wxT("/Fullscreen"), &bDummy, false);
    m_fullscreen->SetValue(bDummy);

    file.Read(m_drv_name + wxT("/VSync"), &bDummy, false);
    m_vsync->SetValue(bDummy);

    file.Read(m_drv_name + wxT("/Scale"), &bDummy, false);
    m_scale->SetValue(bDummy);

    file.Read(m_drv_name + wxT("/Filter"), &lDummy, (long)0);

    if (lDummy < 0 || lDummy >= 7)
        lDummy = 0;

    m_filter->SetSelection((int)lDummy);

    file.Read(m_drv_name + wxT("/VExpand"), &sDummy, wxT("0"));
    m_vexpand->SetValue(sDummy);

    file.Read(m_drv_name + wxT("/HExpand"), &sDummy, wxT("0"));
    m_hexpand->SetValue(sDummy);


    if (!m_fullscreen->IsChecked())
    {
        m_vexpand->Enable(false);
        m_hexpand->Enable(false);
        m_vexpand_str->Enable(false);
        m_hexpand_str->Enable(false);

        m_vsync->Enable(false);
    }

    if (!m_scale->IsChecked())
        m_filter->Enable(false);

}

//////////////////////////////////////////////////////////////////////////////////////
void
CDialogSphere32::SaveConfiguration()
{
    wxString filename(wxGetCwd() + wxT("/system/video/") + m_drv_name + wxT(".cfg"));

    wxFileConfig file(wxEmptyString, wxEmptyString, filename, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);

    long dummy;

    file.Write(m_drv_name + wxT("/Fullscreen"),  m_fullscreen->IsChecked());
    file.Write(m_drv_name + wxT("/VSync"),       m_vsync->IsChecked());

    file.Write(m_drv_name + wxT("/Scale"),       m_scale->IsChecked());
    file.Write(m_drv_name + wxT("/Filter"),      (long)m_filter->GetSelection());

    if (!m_vexpand->GetValue().ToLong(&dummy))
        dummy = 0;

    if (dummy < 0 || dummy > 1024)
        dummy = 0;

    file.Write(m_drv_name + wxT("/VExpand"), dummy);

    if (!m_hexpand->GetValue().ToLong(&dummy))
        dummy = 0;

    if (dummy < 0 || dummy > 1024)
        dummy = 0;

    file.Write(m_drv_name + wxT("/HExpand"), dummy);

}








