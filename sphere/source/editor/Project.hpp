#ifndef PROJECT_HPP
#define PROJECT_HPP
// disable 'identifier too long'
#pragma warning(disable : 4786)
#include <vector>
#include <string>
#include "FileTypes.hpp"
class CProject
{
public:
  CProject();
  bool Create(const char* games_directory, const char* project_name);
  bool Open(const char* filename);
  bool Save() const;
  const char* GetDirectory() const;        // returns full path to game
  const char* GetGameSubDirectory() const;
  const char* GetGameTitle() const;
  const char* GetAuthor() const;
  const char* GetDescription() const;
  const char* GetGameScript() const;
  int         GetScreenWidth() const;
  int         GetScreenHeight() const;
  void SetGameTitle(const char* game_title);
  void SetAuthor(const char* author);
  void SetDescription(const char* description);
  void SetGameScript(const char* game_script);
  void SetScreenWidth(int width);
  void SetScreenHeight(int height);
  static const char* GetGroupDirectory(int grouptype); // returns relative path
  void RefreshItems();
  int         GetItemCount(const char* groupname) const;
  int         GetItemCount(int grouptype) const;
  const char* GetItem(const char* groupname, int index) const;
  const char* GetItem(int grouptype, int index) const;
  bool        HasItem(const char* groupname, const char* item) const;
  bool        HasItem(int grouptype, const char* item) const;
public:
  class Group {
  public:
    std::string FolderName;
    std::vector<std::string> Files;
  };
private:
  //void AddItem(int grouptype, const char* filename);
  void Destroy();
private:
  std::string m_Directory;
  std::string m_Filename;
  std::string m_GameTitle;
  std::string m_Author;
  std::string m_Description;
  std::string m_GameScript;
  int m_ScreenWidth;
  int m_ScreenHeight;
  std::vector<Group> m_Groups;
};
#endif
