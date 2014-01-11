
#include "../common/configfile.hpp"
#include "Editor.hpp"
#include "Configuration.hpp"
#include "Keys.hpp"
static std::string s_LanguageName = "English";
static CConfigFile s_LanguageConfig;

////////////////////////////////////////////////////////////////////////////////

const char* GetLanguage() {
  return s_LanguageName.c_str();
}

////////////////////////////////////////////////////////////////////////////////

void SetLanguage(const char* language)
{
  s_LanguageName = language;
  if (1) {
    char directory[MAX_PATH] = {0};
    GetCurrentDirectory(MAX_PATH, directory);
    if (SetCurrentDirectory(GetSphereDirectory().c_str()) != 0) {
      s_LanguageConfig.Save("_language_.ini");
      s_LanguageConfig.Load("language.ini");
      SetCurrentDirectory(directory);
    }
  }
  Configuration::Set(KEY_LANGUAGE, language);
}

////////////////////////////////////////////////////////////////////////////////

const char* TranslateString(const char* string)
{
  const char* language = GetLanguage();
  if (language == NULL || strlen(language) == 0) {
    return string;
  }
  if (strcmp(language, "English") == 0) {
    return string;
  }
  return s_LanguageConfig.ReadString(language, string, string).c_str();
}
////////////////////////////////////////////////////////////////////////////////

void TranslateMenu(HMENU menu)
{
  int i;
  const char* language = GetLanguage();
  if (language == NULL || strlen(language) == 0) {
    return;
  }

  if (strcmp(language, "English") == 0) {
    return;
  }

  int count = GetMenuItemCount(menu);
  for (i = 0; i < count; i++)
  {
    char buffer[1024] = {0};
    GetMenuString(menu, i, buffer, 1000, MF_BYPOSITION);
    if (strlen(buffer) > 0) {
      ModifyMenu(menu, i, MF_BYPOSITION, GetMenuItemID(menu, i), TranslateString(buffer));
    }
  }

  for (i = 0; i < count; i++) {
    HMENU sub_menu = GetSubMenu(menu, i);
    if (sub_menu != NULL) {
      TranslateMenu(sub_menu);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

void TranslateDialog(HWND hWnd)
{
  const char* language = GetLanguage();
  if (language == NULL || strlen(language) == 0) {
    return;
  }

  if (strcmp(language, "English") == 0) {
    return;
  }

  for (int i = 0; i < 10; i++)
  {
    char buffer[1024] = {0};
    GetDlgItemText(hWnd, i, buffer, 1000);
    if (strlen(buffer) > 0) {
      SetDlgItemText(hWnd, i, TranslateString(buffer));
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
