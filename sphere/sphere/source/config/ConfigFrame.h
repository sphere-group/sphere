#ifndef CONFIG_FRAME_H
#define CONFIG_FRAME_H


#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/dir.h>
#include <wx/dynlib.h>
#include <wx/wfstream.h>
#include <wx/fileconf.h>
#include <wx/filename.h>
#include <wx/timer.h>

#include "DialogSphereGL.h"
#include "DialogSphere32.h"
#include "DialogSphere8.h"


// platform dependent initialization
#if   defined(WIN32)
    #define FILESPEC_DYNLIB "*.dll"
    #define STDCALL __stdcall

    #define MAX_AUDIO_DRIVERS 2
    const wxString audio_drivers[] =
    {
        wxT("directsound"),
        wxT("winmm"),
    };

#elif defined(UNIX)
    #define FILESPEC_DYNLIB "*.so"
    #define STDCALL __attribute__((stdcall))

    #define MAX_AUDIO_DRIVERS 1
    const wxString audio_drivers[] =
    {
        wxT("oss"),
    };

#elif defined(MAC)
    #include <dlfcn.h>

    #define FILESPEC_DYNLIB "*.dylib"
    #define STDCALL __attribute__((stdcall))

    #define MAX_AUDIO_DRIVERS 1
    const wxString audio_drivers[] =
    {
        wxT("sdl_audio"),
    };

#else
    #error unknown platform

#endif


struct DRIVERINFO
{
    const char* name;
    const char* author;
    const char* date;
    const char* version;
    const char* description;
};


struct PLAYERCONFIG
{
    wxString key_menu;
    wxString key_up;
    wxString key_down;
    wxString key_left;
    wxString key_right;
    wxString key_a;
    wxString key_b;
    wxString key_x;
    wxString key_y;

    bool     allow_keyboard_input;
    bool     allow_joypad_input;
};


struct SPHERECONFIG
{
    // main
    int language;

    // video
    wxString video_driver;

    // audio
    int sound_preference;
    wxString audio_driver;

    // input
    PLAYERCONFIG players[4];

    // network
    bool allow_networking;

};


#define MIN_LANGUAGE 1
#define MAX_LANGUAGE 9
static const wxLanguage LanguageIDs[] =
{
    wxLANGUAGE_DEFAULT,
    wxLANGUAGE_ENGLISH,
    wxLANGUAGE_GERMAN,
    wxLANGUAGE_FRENCH,
    wxLANGUAGE_DUTCH,
    wxLANGUAGE_JAPANESE,
    wxLANGUAGE_CHINESE_SIMPLIFIED,
    wxLANGUAGE_RUSSIAN,
    wxLANGUAGE_SPANISH,
    wxLANGUAGE_INDONESIAN,
};


static const wxString LanguageNames[] =
{
    wxT("Default"),
    wxT("English"),
    wxT("German"),
    wxT("French"),
    wxT("Dutch"),
    wxT("Japanese"),
    wxT("Chinese"),
    wxT("Russian"),
    wxT("Spanish"),
    wxT("Indonesian"),
};


static const wxString PlayerIDs[] =
{
    wxT("1"),
    wxT("2"),
    wxT("3"),
    wxT("4"),
};


enum
{
    IDL_DEFAULT = 0,
    IDL_ENGLISH,
    IDL_GERMAN,
    IDL_FRENCH,
    IDL_DUTCH,
    IDL_JAPANESE,
    IDL_CHINESE,
    IDL_RUSSIAN,
    IDL_SPANISH,
    IDL_INDONESIAN,

    IDM_RESTORE_DEFAULTS = 20,

    IDV_CONFIG_BUTTON,
    IDV_DRIVER_LIST,

    IDA_SOUND_AUTO,
    IDA_SOUND_ON,
    IDA_SOUND_OFF,
    IDA_DRIVER,

    IDI_PLAYER_INDEX,
    IDI_MENU,
    IDI_UP,
    IDI_DOWN,
    IDI_LEFT,
    IDI_RIGHT,
    IDI_A,
    IDI_B,
    IDI_X,
    IDI_Y,
    IDI_ALLOW_KEYBOARD_INPUT,
    IDI_ALLOW_JOYPAD_INPUT,
    IDI_INPUT_MANAGER,

    IDN_ALLOW_NETWORKING,
};


// forward declarations
class CConfigNotebook;
class CConfigVideoPage;
class CConfigAudioPage;
class CConfigInputPage;
class CConfigNetworkPage;
class CInputManager;


/************************************************************************************/
// FRAME /////////////////////////////////////////////////////////////////////////////
/************************************************************************************/

class CConfigFrame : public wxFrame
{
    public:

        CConfigFrame(const wxString& title);

        bool Initialize(wxString &error);
        bool InitializeLanguage(int lang_id);

        void OnSelectEnglish(wxCommandEvent& event);
        void OnSelectGerman(wxCommandEvent& event);
        void OnSelectFrench(wxCommandEvent& event);
        void OnSelectDutch(wxCommandEvent& event);
        void OnSelectJapanese(wxCommandEvent& event);
        void OnSelectChinese(wxCommandEvent& event);
        void OnSelectRussian(wxCommandEvent& event);
        void OnSelectSpanish(wxCommandEvent& event);
        void OnSelectIndonesian(wxCommandEvent& event);

        void OnSelectRestoreDefaults(wxCommandEvent& event);
        void OnClickOk(wxCommandEvent& event);
        void OnClickCancel(wxCommandEvent& event);

        void LoadDefaultConfiguration();
        void LoadConfiguration();
        void SaveConfiguration();
        void Translate();
        void RefreshLayout();

        SPHERECONFIG* GetConfig()
        {
            return &m_sphere_config;
        }

    protected:

        wxLocale            m_locale;

    private:

        SPHERECONFIG        m_sphere_config;

        wxPanel*            m_panel;

        wxMenuBar*          m_menu_bar;
        wxMenu*             m_config_menu;
        wxMenu*             m_language_menu;

        wxButton*           m_ok_button;
        wxButton*           m_cancel_button;

        CConfigNotebook*    m_notebook;


};


/************************************************************************************/
// NOTEBOOK //////////////////////////////////////////////////////////////////////////
/************************************************************************************/

class CConfigNotebook : public wxNotebook
{

    public:

        CConfigNotebook(CConfigFrame* frame, wxWindow* parent, wxWindowID id);

        void OnPageChanging(wxNotebookEvent& event);

        CConfigVideoPage*   GetVideoPage()
        {
            return m_video;
        }

        CConfigAudioPage*   GetAudioPage()
        {
            return m_audio;
        }

        CConfigInputPage*   GetInputPage()
        {
            return m_input;
        }

        CConfigNetworkPage* GetNetworkPage()
        {
            return m_network;
        }

    private:

        CConfigFrame*       m_frame;

        CConfigVideoPage*   m_video;
        CConfigAudioPage*   m_audio;
        CConfigInputPage*   m_input;
        CConfigNetworkPage* m_network;

};


/************************************************************************************/
// VIDEO PAGE ////////////////////////////////////////////////////////////////////////
/************************************************************************************/

class CConfigVideoPage : public wxPanel
{

    public:

        CConfigVideoPage(CConfigFrame* frame, wxWindow* parent);

        void OnClickConfigureDriver(wxCommandEvent& event);
        void OnSelectDriver(wxCommandEvent& event);

        bool IsDriverSelected();
        void LoadConfiguration(bool reload = false);
        bool BuildDriverList(wxString &error);
        void LoadDriverInfo(wxString &drv_name);
        void Translate();

        CConfigFrame*  m_frame;

        wxStaticBox*   m_box;

        wxListBox*     m_driver_list;
        wxButton*      m_config_button;
        wxStaticText*  m_info_name;
        wxStaticText*  m_info_author;
        wxStaticText*  m_info_date;
        wxStaticText*  m_info_version;
        wxStaticText*  m_info_desc;

};


/************************************************************************************/
// AUDIO PAGE ////////////////////////////////////////////////////////////////////////
/************************************************************************************/

class CConfigAudioPage : public wxPanel
{

    public:

        CConfigAudioPage(CConfigFrame* frame, wxWindow* parent);

        void OnSelectSoundAuto(wxCommandEvent& event);
        void OnSelectSoundOn(wxCommandEvent& event);
        void OnSelectSoundOff(wxCommandEvent& event);
        void OnSelectDriver(wxCommandEvent& event);

        void LoadConfiguration(bool reload = false);
        void Translate();

        CConfigFrame*  m_frame;

        wxStaticBox*   m_box;

        wxStaticBox*   m_driver_box;
        wxRadioButton* m_sound_auto;
        wxRadioButton* m_sound_on;
        wxRadioButton* m_sound_off;
        wxChoice*      m_driver;

};


/************************************************************************************/
// INPUT PAGE ////////////////////////////////////////////////////////////////////////
/************************************************************************************/

class CConfigInputPage : public wxPanel
{

    public:

        CConfigInputPage(CConfigFrame* frame, wxWindow* parent);

        void OnSelectPlayer(wxCommandEvent& event);
        void OnClickMenu(wxCommandEvent& event);
        void OnClickUp(wxCommandEvent& event);
        void OnClickDown(wxCommandEvent& event);
        void OnClickLeft(wxCommandEvent& event);
        void OnClickRight(wxCommandEvent& event);
        void OnClickA(wxCommandEvent& event);
        void OnClickB(wxCommandEvent& event);
        void OnClickX(wxCommandEvent& event);
        void OnClickY(wxCommandEvent& event);
        void OnClickAllowKeyboardInput(wxCommandEvent& event);
        void OnClickAllowJoypadInput(wxCommandEvent& event);

        CInputManager* GetManager();
        int  GetCurrentPlayer();
        void LoadPlayerConfiguration(int player_index);
        void LoadConfiguration(bool reload = false);
        void Translate();

        CConfigFrame*  m_frame;

        CInputManager* m_input_manager;

        wxStaticBox*   m_box;

        wxStaticText*  m_player_str;
        wxChoice*      m_player_index;
        wxButton*      m_menu;
        wxButton*      m_up;
        wxButton*      m_down;
        wxButton*      m_left;
        wxButton*      m_right;
        wxButton*      m_a;
        wxButton*      m_b;
        wxButton*      m_x;
        wxButton*      m_y;
        wxStaticText*  m_menu_str;
        wxStaticText*  m_up_str;
        wxStaticText*  m_down_str;
        wxStaticText*  m_left_str;
        wxStaticText*  m_right_str;
        wxStaticText*  m_a_str;
        wxStaticText*  m_b_str;
        wxStaticText*  m_x_str;
        wxStaticText*  m_y_str;
        wxCheckBox*    m_allow_keyboard_input;
        wxCheckBox*    m_allow_joypad_input;

};


/************************************************************************************/
// NETWORK PAGE //////////////////////////////////////////////////////////////////////
/************************************************************************************/

class CConfigNetworkPage : public wxPanel
{

    public:

        CConfigNetworkPage(CConfigFrame* frame, wxWindow* parent);

        void OnClickAllowNetworking(wxCommandEvent& event);

        void LoadConfiguration(bool reload = false);
        void Translate();

        CConfigFrame*  m_frame;

        wxStaticBox*   m_box;

        wxCheckBox*    m_allow_networking;

};


/************************************************************************************/
// INPUT MANAGER /////////////////////////////////////////////////////////////////////
/************************************************************************************/

class CInputManager : public wxWindow
{

    public:

        CInputManager(CConfigInputPage* parent);

        bool IsRunning();
        void Start(wxString* subject, wxStaticText* output, int num_seconds);
        void Stop();
        void Finalize(wxString &new_str);

        void OnKeyUp(wxKeyEvent &event);
        void OnKillFocus(wxFocusEvent &event);
        void OnTimer(wxTimerEvent &event);

    private:

        void CleanUp();
        void UpdateOutput();

        CConfigInputPage* m_parent;

        wxTimer       m_timer;
        int           m_seconds;

        wxString*     m_subject;
        wxStaticText* m_output;
        wxString      m_original;
};




#endif


