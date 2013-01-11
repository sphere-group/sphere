#pragma warning(disable: 4786)
#include "OptionsDialog.hpp"
#include "resource.h"
#include "FileTypes.hpp"
#include "Editor.hpp"
#include "Configuration.hpp"
#include "Keys.hpp"
#include <afxwin.h>
#include <afxcmn.h>
static int s_PropertyPage = 0;
///////////////////////////////////////////////////////////
static
const char* __getfiletype__(const char* ext)
{
  const char* filetype = ext;
  if (strcmp(ext, "rmp") == 0) filetype = "SphereMap";
  if (strcmp(ext, "rss") == 0) filetype = "SphereSpriteset";
  if (strcmp(ext, "rts") == 0) filetype = "SphereTileset";
  if (strcmp(ext, "rws") == 0) filetype = "SphereWindowStyle";
  if (strcmp(ext, "rfn") == 0) filetype = "SphereFont";
  if (strcmp(ext, "spk") == 0) filetype = "SpherePackage";
  if (strcmp(ext, "sgm") == 0) filetype = "SphereGame";
  return filetype;
}
static
void
__get_sphere_command__(const char* ext, char* command)
{
  char sphere_editor[MAX_PATH];
  GetModuleFileName(AfxGetApp()->m_hInstance, sphere_editor, MAX_PATH);
  std::string sphere_engine = GetSphereDirectory() + "\\engine.exe";
  if (strcmp(ext, "spk") == 0) {
    sprintf (command, "\"%s\" \"%%1\"", sphere_engine.c_str());
  }
  else {
    sprintf (command, "\"%s\" \"%%1\"", sphere_editor);
  }
}
static
void
__get_sphere_icon__(const char* ext, char* icon)
{
  char sphere_editor[MAX_PATH];
  GetModuleFileName(AfxGetApp()->m_hInstance, sphere_editor, MAX_PATH);
  std::string sphere_engine = GetSphereDirectory() + "\\engine.exe";
  if (strcmp(ext, "sgm") == 0) {
    sprintf (icon, "%s,1", sphere_editor);
  }
  if (strcmp(ext, "png") == 0) {
    sprintf (icon, "%s,7", sphere_editor);
  }
  if (strcmp(ext, "rts") == 0) {
    sprintf (icon, "%s,4", sphere_editor);
  }
  if (strcmp(ext, "spk") == 0) {
    sprintf (icon, "%s,8", sphere_editor);
  }
  if (strcmp(ext, "rmp") == 0) {
    sprintf (icon, "%s,3", sphere_editor);
  }
  if (strcmp(ext, "rss") == 0) {
    sprintf (icon, "%s,5", sphere_editor);
  }
  if (strcmp(ext, "js") == 0) {
    sprintf (icon, "%s,6", sphere_editor);
  }
  if (strcmp(ext, "rws") == 0) {
    sprintf (icon, "%s,12", sphere_editor);
  }
  if (strcmp(ext, "rfn") == 0) {
    sprintf (icon, "%s,13", sphere_editor);
  }
}
static
bool IsRegistered(const char* ext)
{
  const char* filetype = __getfiletype__(ext);
  char extension[1024] = {0};
  sprintf (extension, ".%s", ext);
  char command[MAX_PATH + 1024];
  __get_sphere_command__(ext, command);
	bool registered = false;
  if (1)
  {
    HKEY key;
    int retval = RegOpenKey(HKEY_CLASSES_ROOT, extension, &key);
    RegCloseKey(key);
    
    if (retval == 0)
    {
      retval = RegOpenKey(HKEY_CLASSES_ROOT, filetype, &key);
	    char buffer[MAX_PATH + 1024];
	    long size = sizeof(buffer);
	    if (RegQueryValue(key, "shell\\open\\command", buffer, &size) == 0) {
		    if (strcmp(buffer, command) == 0) {
		      registered = true;
        }
      }
      RegCloseKey(key);
    }
  }
  return registered;
}
static
void RegisterFile(const char* ext)
{
  char extension[1024] = {0};
  sprintf (extension, ".%s", ext);
  const char* filetype = __getfiletype__(ext);
  char icon[MAX_PATH + 1024] = {0};
  __get_sphere_icon__(ext, icon);
  if (1)
  {
    HKEY key;
    char command[MAX_PATH + 1024];
    __get_sphere_command__(ext, command);
    // register the extension
    if (RegCreateKey(HKEY_CLASSES_ROOT, extension, &key) == 0) {
      RegSetValue(key, NULL, REG_SZ, filetype, strlen(filetype));
      RegCloseKey(key);
    }
    // register the type
    if (RegCreateKey(HKEY_CLASSES_ROOT, filetype, &key) == 0) {
      // register the default command
      RegSetValue(key, "shell\\open\\command", REG_SZ, command, strlen(command));
      // register the default icon
      RegSetValue(key, "DefaultIcon", REG_SZ, icon, strlen(icon));
      RegCloseKey(key);
    }
  }
}
static
void UnregisterFile(const char* ext)
{
  if (!IsRegistered(ext))
    return;
  return;
  const char* filetype = __getfiletype__(ext);
  char extension[1024] = {0};
  sprintf (extension, ".%s", ext);
  if (1)
  {
  	HKEY key;
	  int retval = RegOpenKey(HKEY_CLASSES_ROOT, filetype, &key);
	  if (retval == 0) {
      RegDeleteKey(key, "shell\\open\\command");
    }
	  RegCloseKey(key);
  }
}
///////////////////////////////////////////////////////////
static
BOOL CALLBACK FileTypeRegisterDialogProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
  struct Local {
    static void RefreshFileTypes(HWND window, bool first) {
      // add filetypes
      int index = 0;
      for (int filetype = 0; filetype < FTL.GetNumFileTypes(); filetype++) {
        std::vector<std::string> extensions;
        FTL.GetFileTypeExtensions(filetype, false, extensions);
        for (unsigned int i = 0; i < extensions.size(); i++, index++) {
          if (first) SendDlgItemMessage(window, IDC_FILETYPE_LIST, LB_ADDSTRING, 0, (LPARAM)extensions[i].c_str());
          SendDlgItemMessage(window, IDC_FILETYPE_LIST, (UINT) LB_SETSEL, (WPARAM) IsRegistered(extensions[i].c_str()) ? TRUE : FALSE, (LPARAM)index);
        }
      }
      int sel_count = SendDlgItemMessage(window, IDC_FILETYPE_LIST, LB_GETSELCOUNT, 0, 0);
      EnableWindow(GetDlgItem(window, IDC_FILETYPES_REGISTER_TYPES), sel_count > 0 ? TRUE : FALSE);
      EnableWindow(GetDlgItem(window, IDC_FILETYPES_UNREGISTER_TYPES), sel_count > 0 ? TRUE : FALSE);
    }
    static void SelectedFileTypes(HWND window, void (*SelectedFileTypeOperation)(const char* ext))
    {
      for (int i = 0; i < SendDlgItemMessage(window, IDC_FILETYPE_LIST, LB_GETCOUNT, 0, 0); i++) {
        if (SendDlgItemMessage(window, IDC_FILETYPE_LIST, (UINT) LB_GETSEL, (WPARAM) i, 0) > 0) {
          int len = SendDlgItemMessage(window, IDC_FILETYPE_LIST, LB_GETTEXTLEN, i, 0);
          if (len > 0 && len < 1020) {
            char text[1024];
            SendDlgItemMessage(window, IDC_FILETYPE_LIST, LB_GETTEXT, i, (WPARAM)text);
            text[len] = '\0';
            SelectedFileTypeOperation(text);
          }
        }
      }
    }
  };
  switch (message)
  {
    case WM_INITDIALOG:
    {
      Local::RefreshFileTypes(window, true);
      SetFocus(GetDlgItem(window, IDC_FILETYPE_LIST));
      return TRUE;
    }
    case WM_COMMAND:
    {
      if (LOWORD(wparam) == IDC_FILETYPE_LIST && HIWORD(wparam) == LBN_SELCHANGE)
      {
        if (SendDlgItemMessage(window, IDC_FILETYPE_LIST, LB_GETCOUNT, 0, 0) >= 1) {
          EnableWindow(GetDlgItem(window, IDC_FILETYPES_REGISTER_TYPES), TRUE);
          EnableWindow(GetDlgItem(window, IDC_FILETYPES_UNREGISTER_TYPES), TRUE);
        }
        return TRUE;
      }
      if (LOWORD(wparam) == IDC_FILETYPES_RESTORE_DEFAULTS)
      {
        for (int filetype = 0; filetype < FTL.GetNumFileTypes(); filetype++) {
          std::vector<std::string> extensions;
          FTL.GetFileTypeExtensions(filetype, false, extensions);
          for (unsigned int i = 0; i < extensions.size(); i++) {
            UnregisterFile(extensions[i].c_str());
          }
        }
        RegisterFile("sgm");
        RegisterFile("rmp");
        RegisterFile("rss");
        RegisterFile("rws");
        RegisterFile("rfn");
        RegisterFile("rts");
        RegisterFile("spk");
        Local::RefreshFileTypes(window, false);
        return TRUE;
      }
      if (LOWORD(wparam) == IDC_FILETYPES_REGISTER_TYPES)
      {
        Local::SelectedFileTypes(window, RegisterFile);
        return TRUE;
      }
      if (LOWORD(wparam) == IDC_FILETYPES_UNREGISTER_TYPES)
      {
        Local::SelectedFileTypes(window, UnregisterFile);
        return TRUE;
      }
      return FALSE;
    }
    case WM_NOTIFY:
    {
      PSHNOTIFY* psn = (PSHNOTIFY*)lparam;
      if (psn->hdr.code == UINT(PSN_APPLY))
      {
        
      }
    }
    default:
      return FALSE;
  }
  return FALSE;
}
///////////////////////////////////////////////////////////
static
BOOL CALLBACK FileTypeGeneralDialogProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
  switch (message)
  {
    case WM_INITDIALOG:
    {
      CheckDlgButton(window, IDC_OPENUNKNOWNFILETYPES,      (Configuration::Get(OPEN_UNKNOWN_FILETYPES_AS_TEXT) ? BST_CHECKED : BST_UNCHECKED));
      CheckDlgButton(window, IDC_USE_COMMON_TEXT_FILETYPES, (Configuration::Get(USE_COMMON_TEXT_FILETYPES)      ? BST_CHECKED : BST_UNCHECKED));
      return TRUE;
    }
    case WM_NOTIFY:
    {
      PSHNOTIFY* psn = (PSHNOTIFY*)lparam;
      if (psn->hdr.code == UINT(PSN_APPLY))
      {
        Configuration::Set(OPEN_UNKNOWN_FILETYPES_AS_TEXT, (IsDlgButtonChecked(window, IDC_OPENUNKNOWNFILETYPES)      == BST_CHECKED));
        Configuration::Set(USE_COMMON_TEXT_FILETYPES,      (IsDlgButtonChecked(window, IDC_USE_COMMON_TEXT_FILETYPES) == BST_CHECKED));
        return TRUE;
      }
      return FALSE;
    }
    
  }
  return FALSE;
}
///////////////////////////////////////////////////////////
static
BOOL CALLBACK TitlebarDialogProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
  switch (message)
  {
    case WM_INITDIALOG:
    {
      bool use_defaulttext = Configuration::Get(KEY_MAINWINDOW_USE_DEFAULTTEXT);
      if (use_defaulttext) {
        CheckDlgButton(window, IDC_DEFAULT_TEXT, BST_CHECKED);
      } else {
        CheckDlgButton(window, IDC_CUSTOM_TEXT, BST_CHECKED);
      }
      
      std::string text = Configuration::Get(KEY_MAINWINDOW_CUSTOM_TEXT);
      SetDlgItemText(window, IDC_TITLEBAR, text.c_str());
      return TRUE;
    }
    case WM_NOTIFY:
    {
      PSHNOTIFY* psn = (PSHNOTIFY*)lparam;
      if (psn->hdr.code == UINT(PSN_APPLY))
      {
        char title[1024] = {0};
        bool use_defaulttext = (IsDlgButtonChecked(window, IDC_DEFAULT_TEXT) == BST_CHECKED);
        GetDlgItemText(window, IDC_TITLEBAR, title, sizeof(title));
       
        if (use_defaulttext) {
          GetMainWindow()->SetWindowText(CMainWindow::GetDefaultWindowText());
        } else {
          GetMainWindow()->SetWindowText(title);
        }
        Configuration::Set(KEY_MAINWINDOW_CUSTOM_TEXT, title);
        Configuration::Set(KEY_MAINWINDOW_USE_DEFAULTTEXT, use_defaulttext);
        return TRUE;
      }
      return FALSE;
    }
    
  }
  return FALSE;
}
    
///////////////////////////////////////////////////////////
void
COptionsDialog::Execute()
{
  PROPSHEETPAGE Pages[3];
  const int num_pages = sizeof(Pages) / sizeof(*Pages);
  // default values
  for (unsigned int i = 0; i < num_pages; i++)
  {
    Pages[i].dwSize    = sizeof(Pages[i]);
    Pages[i].dwFlags   = PSP_DEFAULT;
    Pages[i].hInstance = AfxGetApp()->m_hInstance;
    Pages[i].hIcon     = NULL;
  }
  
  int current_page = 0;
  Pages[current_page].pszTemplate = MAKEINTRESOURCE(IDD_FILETYPES_REGISTER_PAGE);
  Pages[current_page].pfnDlgProc  = FileTypeRegisterDialogProc;
  current_page += 1;
  Pages[current_page].pszTemplate = MAKEINTRESOURCE(IDD_FILETYPES_GENERAL_PAGE);
  Pages[current_page].pfnDlgProc  = FileTypeGeneralDialogProc;
  current_page += 1;
  Pages[current_page].pszTemplate = MAKEINTRESOURCE(IDD_TITLEBAR_PAGE);
  Pages[current_page].pfnDlgProc  = TitlebarDialogProc;
  current_page += 1;
  /*
  // get the previously shown page index
  current_page = Configuration::Get(KEY_OPTIONS_PAGE);
  if ( !(current_page >= 0 && current_page < num_pages) )
    current_page = 0;
  */
  // create the dialog box
  PROPSHEETHEADER psh;
  memset(&psh, 0, sizeof(psh));
  psh.dwSize      = sizeof(psh);
  psh.dwFlags     = PSH_NOAPPLYNOW | PSH_PROPSHEETPAGE;
  psh.hwndParent  = NULL;
  psh.hInstance   = AfxGetApp()->m_hInstance;
  psh.hIcon       = NULL;
  psh.pszCaption  = "Sphere Options";
  psh.nPages      = num_pages;
  psh.nStartPage  = current_page;
  psh.ppsp        = Pages;
  PropertySheet(&psh);
  //  Configuration::Set(KEY_OPTIONS_PAGE, s_PropertyPage);
}
///////////////////////////////////////////////////////////
