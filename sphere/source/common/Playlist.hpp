#ifndef PLAYLIST_HPP
#define PLAYLIST_HPP
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif
#include "../common/DefaultFileSystem.hpp"
#include <string>
#include <vector>
class CPlaylist
{

public:
    CPlaylist();
    ~CPlaylist();
private:
    std::vector<std::string> m_Filenames;
public:
    const char* GetFile(int index) const;
    int GetNumFiles() const;
    void Clear();
    bool AppendFile(const char* filename);
    bool LoadFromFile(const char* filename, IFileSystem& fs = g_DefaultFileSystem);
};
#endif
