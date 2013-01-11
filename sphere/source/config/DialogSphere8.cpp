
#include <wx/statline.h>

#include "DialogSphere8.h"


enum
{
    ID_FULLSCREEN = 0,
    ID_VSYNC,
    ID_PALETTES,
};


CDialogSphere8::CDialogSphere8(wxWindow* parent, const wxString& title, const wxString &drv_name)
              : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize,
                         wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX)
{
    m_drv_name = drv_name;

    wxStaticLine* sline = new wxStaticLine(this, wxID_ANY);
    wxStaticBox* box    = new wxStaticBox(this, wxID_ANY, _("Palette"));

    m_ok_button      = new wxButton(this,   wxID_OK,         _("OK"));
    m_cancel_button  = new wxButton(this,   wxID_CANCEL,     _("Cancel"));
    m_fullscreen     = new wxCheckBox(this, ID_FULLSCREEN,   _("Fullscreen"));
    m_vsync          = new wxCheckBox(this, ID_VSYNC,        _("VSync"));

    BuildPalettesList();
    m_palettes             = new wxChoice(this, ID_PALETTES, wxDefaultPosition, wxDefaultSize, m_palettes_list);

    wxStaticBoxSizer* sbox = new wxStaticBoxSizer(box, wxVERTICAL);
    wxBoxSizer* vbox       = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* vbox0      = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* vbox1      = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* hbox1      = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* hbox2      = new wxBoxSizer(wxHORIZONTAL);

    vbox1->Add(-1, 10);
    vbox1->Add(m_fullscreen);
    vbox1->Add(-1, 5);
    vbox1->Add(m_vsync);

    sbox->Add(m_palettes);

    hbox1->Add(5, -1);
    hbox1->Add(vbox1);
    hbox1->Add(20, -1);
    hbox1->Add(sbox);

    hbox2->Add(m_cancel_button);
    hbox2->Add(10, -1);
    hbox2->Add(m_ok_button);

    vbox0->Add(hbox1);
    vbox0->Add(-1, 10);
    vbox0->Add(sline, 1, wxEXPAND);
    vbox0->Add(-1, 10);
    vbox0->Add(hbox2, 0, wxALIGN_RIGHT);

    vbox->Add(vbox0, 1, wxEXPAND | wxALL, 10);

    SetSizer(vbox);
    Fit();

    Center();

    Connect(wxID_OK,         wxEVT_COMMAND_BUTTON_CLICKED,   wxCommandEventHandler(CDialogSphere8::OnClickOk));
    Connect(wxID_CANCEL,     wxEVT_COMMAND_BUTTON_CLICKED,   wxCommandEventHandler(CDialogSphere8::OnClickCancel));
    Connect(ID_FULLSCREEN,   wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CDialogSphere8::OnClickFullscreen));

    LoadConfiguration();

}

//////////////////////////////////////////////////////////////////////////////////////
void
CDialogSphere8::OnClickOk(wxCommandEvent& event)
{
    SaveConfiguration();
    EndModal(1);
}

//////////////////////////////////////////////////////////////////////////////////////
void
CDialogSphere8::OnClickCancel(wxCommandEvent& event)
{
    EndModal(0);
}

//////////////////////////////////////////////////////////////////////////////////////
void
CDialogSphere8::OnClickFullscreen(wxCommandEvent& event)
{
    if (m_fullscreen->IsChecked())
        m_vsync->Enable();
    else
        m_vsync->Enable(false);
}

//////////////////////////////////////////////////////////////////////////////////////
void
CDialogSphere8::BuildPalettesList()
{
    m_palettes_list.Clear();

    // add default palette
    m_palettes_list.Add(wxT("(default)"));

    // try to find additional palettes and add them
    wxString filename;
    wxDir dir(wxGetCwd() + wxT("/system/video/"));

    if (!dir.IsOpened())
        return;

    bool succeeded = dir.GetFirst(&filename, wxT("*.pal"), wxDIR_FILES);
    while (succeeded)
    {
        m_palettes_list.Add(filename);
        succeeded = dir.GetNext(&filename);
    }

}

//////////////////////////////////////////////////////////////////////////////////////
void
CDialogSphere8::LoadDefaultConfiguration()
{
    m_fullscreen->SetValue(false);
    m_vsync->SetValue(false);
    m_palettes->SetSelection(0);

    m_vsync->Enable(false);
}

//////////////////////////////////////////////////////////////////////////////////////
void
CDialogSphere8::LoadConfiguration()
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
    bool     bDummy;

    file.Read(m_drv_name + wxT("/Fullscreen"), &bDummy, false);
    m_fullscreen->SetValue(bDummy);

    file.Read(m_drv_name + wxT("/VSync"), &bDummy, false);
    m_vsync->SetValue(bDummy);

    file.Read(m_drv_name + wxT("/Palette"), &sDummy, wxT("(default)"));
    m_palettes->SetStringSelection(sDummy);

    if (!m_fullscreen->IsChecked())
        m_vsync->Enable(false);

}

//////////////////////////////////////////////////////////////////////////////////////
void
CDialogSphere8::SaveConfiguration()
{
    wxString filename(wxGetCwd() + wxT("/system/video/") + m_drv_name + wxT(".cfg"));

    wxFileConfig file(wxEmptyString, wxEmptyString, filename, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);

    file.Write(m_drv_name + wxT("/Fullscreen"),  m_fullscreen->IsChecked());
    file.Write(m_drv_name + wxT("/VSync"),       m_vsync->IsChecked());
    file.Write(m_drv_name + wxT("/Palette"),     m_palettes->GetStringSelection());

}








