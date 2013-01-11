#pragma warning(disable : 4786)  // identifier too long
#include <windows.h>
#include <set>
#include <stdio.h>
#include <string.h>
#include "Project.hpp"
#include "../common/configfile.hpp"
#include "../common/types.h"
#include "../common/system.hpp"
//#include "FileSystem.hpp"

////////////////////////////////////////////////////////////////////////////////

CProject::CProject()
: m_ScreenWidth(320)
, m_ScreenHeight(240)
{
  m_GameTitle   = "Untitled";
  m_Author      = "Unknown";
}

////////////////////////////////////////////////////////////////////////////////

bool
CProject::Create(const char* games_directory, const char* project_name)
{
  // create the project
  if (SetCurrentDirectory(games_directory) == FALSE)
    return false;
  
  // if creating the directory failed, it may already exist
  CreateDirectory(project_name, NULL);
  // now create all of the subdirectories
  for (int i = 0; i < NUM_GROUP_TYPES; i++)
  {
    if (i == GT_TILESETS || i == GT_PLAYLISTS)
      continue;
    std::string directory = project_name;
    directory += "/";
    directory += GetGroupDirectory(i);
    CreateDirectory(directory.c_str(), NULL);
  }
  // create an 'other' subdirectory too
  std::string directory = project_name;
  directory += "/other";
  CreateDirectory(directory.c_str(), NULL);
  
  // wait to see if SetCurrentDirectory() fails
  if (SetCurrentDirectory(project_name) == FALSE)
    return false;
  // set the project directory
  char path[MAX_PATH];
  if (GetCurrentDirectory(MAX_PATH, path) == FALSE)
    return false;
  m_Directory = path;
    
  // set the project filename
  m_Filename = path;
  m_Filename += "\\game.sgm";
  // set default values in project
  m_GameTitle   = "Untitled";
  m_Author      = "Unknown";
  m_Description = "";
  m_GameScript = "";
  m_ScreenWidth = 320;
  m_ScreenHeight = 240;
  RefreshItems();
  return Save();
}

////////////////////////////////////////////////////////////////////////////////

bool
CProject::Open(const char* filename)
{
  Destroy();
  // set the game directory
  m_Directory = filename;
  if (m_Directory.rfind('\\') != std::string::npos)
    m_Directory[m_Directory.rfind('\\')] = 0;
  // set the game filename
  m_Filename = filename;
  if (1) {
    char directory[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, directory);
    if (SetCurrentDirectory(m_Directory.c_str()) == 0)
      return false;
    FILE* file = fopen(m_Filename.c_str(), "a");
    if (!file) return false;
    fclose(file);
    SetCurrentDirectory(directory);
  }
  // load the game.sgm
  CConfigFile config;
  if (!config.Load(m_Filename.c_str()))
    return false;
  m_GameTitle   = config.ReadString("", "name",        "Untitled");
  m_Author      = config.ReadString("", "author",      "Unknown");
  m_Description = config.ReadString("", "description", "");
  m_GameScript = config.ReadString("", "script", "");
  // screen dimensions
  m_ScreenWidth  = config.ReadInt("", "screen_width", 320);
  m_ScreenHeight = config.ReadInt("", "screen_height", 240);
  RefreshItems();
  return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CProject::Save() const
{
  SetCurrentDirectory(m_Directory.c_str());
  
  CConfigFile config;
  config.Load(m_Filename.c_str());
  config.WriteString("", "name", m_GameTitle.c_str());
  config.WriteString("", "author", m_Author.c_str());
  config.WriteString("", "description", m_Description.c_str());
  config.WriteString("", "script", m_GameScript.c_str());
  // screen dimensions
  config.WriteInt("", "screen_width",  m_ScreenWidth);
  config.WriteInt("", "screen_height", m_ScreenHeight);
  config.Save(m_Filename.c_str());
  return true;
}

////////////////////////////////////////////////////////////////////////////////

const char*
CProject::GetDirectory() const
{
  return m_Directory.c_str();
}

////////////////////////////////////////////////////////////////////////////////

const char*
CProject::GetGameSubDirectory() const
{
  if (strrchr(m_Directory.c_str(), '\\'))
    return strrchr(m_Directory.c_str(), '\\') + 1;
  else
    return "";
}

////////////////////////////////////////////////////////////////////////////////

const char*
CProject::GetGameTitle() const
{
  return m_GameTitle.c_str();
}

////////////////////////////////////////////////////////////////////////////////

const char*
CProject::GetAuthor() const
{
  return m_Author.c_str();
}

////////////////////////////////////////////////////////////////////////////////

const char*
CProject::GetDescription() const
{
  return m_Description.c_str();
}

////////////////////////////////////////////////////////////////////////////////

const char*
CProject::GetGameScript() const
{
  return m_GameScript.c_str();
}

////////////////////////////////////////////////////////////////////////////////

int
CProject::GetScreenWidth() const
{
  return m_ScreenWidth;
}

////////////////////////////////////////////////////////////////////////////////

int
CProject::GetScreenHeight() const
{
  return m_ScreenHeight;
}

////////////////////////////////////////////////////////////////////////////////

void
CProject::SetGameTitle(const char* game_title)
{
  m_GameTitle = game_title;
}

////////////////////////////////////////////////////////////////////////////////

void
CProject::SetAuthor(const char* author)
{
  m_Author = author;
}

////////////////////////////////////////////////////////////////////////////////

void
CProject::SetDescription(const char* description)
{
  m_Description = description;
}

////////////////////////////////////////////////////////////////////////////////

void
CProject::SetGameScript(const char* game_script)
{
  m_GameScript = game_script;
}

////////////////////////////////////////////////////////////////////////////////

void
CProject::SetScreenWidth(int width)
{
  m_ScreenWidth = width;
}

////////////////////////////////////////////////////////////////////////////////

void
CProject::SetScreenHeight(int height)
{
  m_ScreenHeight = height;
}

////////////////////////////////////////////////////////////////////////////////

const char*
CProject::GetGroupDirectory(int grouptype)
{
  switch (grouptype)
  {
    case GT_MAPS:         return "maps";
    case GT_SPRITESETS:   return "spritesets";
    case GT_SCRIPTS:      return "scripts";
    case GT_SOUNDS:       return "sounds";
    case GT_FONTS:        return "fonts";
    case GT_WINDOWSTYLES: return "windowstyles";
    case GT_IMAGES:       return "images";
    case GT_ANIMATIONS:   return "animations";
    case GT_TILESETS:     return "tilesets";
    default:              return NULL;
  }
}

////////////////////////////////////////////////////////////////////////////////

static bool UpdateItems(std::vector<CProject::Group>& m_Groups);
static void UpdateGroupItems(std::vector<CProject::Group>& m_Groups)
{
  std::vector<std::string> folderlist = GetFolderList("*");
  for (unsigned int i = 0; i < folderlist.size(); i++) {
    if (!strcmp(folderlist[i].c_str(), ".")
     || !strcmp(folderlist[i].c_str(), "..")) {
      continue;
    }
    // insert files into that folder
    char directory[MAX_PATH] = {0};
    GetCurrentDirectory(MAX_PATH, directory);
    SetCurrentDirectory(folderlist[i].c_str());
    std::vector<std::string> filelist = GetFileList("*");
    if (1) {
      CProject::Group current;
      current.FolderName = folderlist[i];
      current.Files = GetFileList("*");
      m_Groups.push_back(current);
    }
    filelist.clear();
    if (GetFolderList("*").size() > 0) {
      UpdateGroupItems(m_Groups);
    }
    SetCurrentDirectory(directory);
  }
}

////////////////////////////////////////////////////////////////////////////////

void
CProject::RefreshItems()
{
  m_Groups.clear();
  // store the old directory
  char old_directory[MAX_PATH];
  GetCurrentDirectory(MAX_PATH, old_directory);
  if (SetCurrentDirectory(m_Directory.c_str()) != 0)
  {
    UpdateGroupItems(m_Groups);
    if (m_GameScript.empty())
    {
      if (GetItemCount(GT_SCRIPTS) == 1) {
        m_GameScript = GetItem(GT_SCRIPTS, 0);
      }
      if (!m_Filename.empty()) {
        Save();
      }
      SetCurrentDirectory(m_Directory.c_str());
    }
  }
  // restore the old directory
  SetCurrentDirectory(old_directory);
}

////////////////////////////////////////////////////////////////////////////////

int
CProject::GetItemCount(const char* groupname) const {
  for (unsigned int i = 0; i < m_Groups.size(); i++) {
    if (strcmp(groupname, m_Groups[i].FolderName.c_str()) == 0)
      return m_Groups[i].Files.size();
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////

int
CProject::GetItemCount(int group_type) const
{
  const char* groupname = GetGroupDirectory(group_type);
  return GetItemCount(groupname);
}

////////////////////////////////////////////////////////////////////////////////

const char*
CProject::GetItem(int group_type, int index) const
{
  const char* groupname = GetGroupDirectory(group_type);
  return groupname == NULL ? 0 : GetItem(groupname, index);
}

////////////////////////////////////////////////////////////////////////////////

const char*
CProject::GetItem(const char* groupname, int index) const {
  for (unsigned int i = 0; i < m_Groups.size(); i++) {
    if (strcmp(groupname, m_Groups[i].FolderName.c_str()) == 0)
      return m_Groups[i].Files[index].c_str();
  }
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////

bool
CProject::HasItem(const char* groupname, const char* item) const
{
  for (unsigned int i = 0; i < m_Groups.size(); i++) {
    if (strcmp(m_Groups[i].FolderName.c_str(), groupname) == 0) {
      for (unsigned int j = 0; j < m_Groups[i].Files.size(); j++) {
        if (strcmp(m_Groups[i].Files[j].c_str(), item) == 0)
          return true;
      }
    }
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////

bool
CProject::HasItem(int group_type, const char* item) const
{
  return HasItem(GetGroupDirectory(group_type), item);
}

////////////////////////////////////////////////////////////////////////////////

void
CProject::Destroy()
{
  m_Directory  = "";
  m_Filename   = "";
  m_GameTitle  = "";
  m_GameScript = "";
  m_ScreenWidth = 0;
  m_ScreenHeight = 0;
  m_Groups.clear();
}

////////////////////////////////////////////////////////////////////////////////
