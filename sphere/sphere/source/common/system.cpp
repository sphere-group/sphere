
// disable 'identifier too long' warning

#pragma warning(disable : 4786)
///////////////////////////////////////////////////////////
#include "system.hpp"

#ifdef WIN32
#include <windows.h>
#endif
#include <vector>
#include <string>
#include <list>
#include <stdio.h>
#ifndef WIN32
#include <glob.h>
#include <unistd.h>
#endif
///////////////////////////////////////////////////////////
#ifndef WIN32
bool SetCurrentDirectory(const char* directory)
{
    return (chdir(directory) >= 0);
}

bool CopyFile(const char* src, const char* dest, bool force)
{
    return false;
}

bool GetCurrentDirectory(int max_path, char* directory)
{
    directory = getcwd(NULL, 0);
    if (directory == NULL)
        return false;
    return true;
}

bool CreateDirectory(const char* directory, const char* options)
{
    return false;
}
#endif

///////////////////////////////////////////////////////////
std::vector<std::string> GetFileList(const char* filter)
{
    std::vector<std::string> file_list;
#ifdef WIN32
    WIN32_FIND_DATA ffd;

    HANDLE h = FindFirstFile(filter, &ffd);
    if (h != INVALID_HANDLE_VALUE)
    {

        do
        {
            if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))

                file_list.push_back(ffd.cFileName);
        }
        while (FindNextFile(h, &ffd));

        FindClose(h);
    }
#else // linux
    glob_t matches;
    memset(&matches, 0, sizeof(matches));

    if (glob(filter, GLOB_ERR | GLOB_MARK, NULL, &matches) == 0)
    {
        for (int lcv = 0; lcv < matches.gl_pathc; lcv++)
        {
            file_list.push_back(matches.gl_pathv[lcv]);
        }
        globfree(&matches);
    }
#endif
    return file_list;
}

///////////////////////////////////////////////////////////
std::vector<std::string> GetFolderList(const char* filter)
{
    std::vector<std::string> folder_list;
#ifdef WIN32
    WIN32_FIND_DATA ffd;
    HANDLE h = FindFirstFile(filter, &ffd);
    if (h != INVALID_HANDLE_VALUE)
    {

        do
        {

            if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {

                folder_list.push_back(ffd.cFileName);
            }
        }
        while (FindNextFile(h, &ffd));

        FindClose(h);
    }
#endif
    return folder_list;
}
///////////////////////////////////////////////////////////
std::string GetFilePath(const char* filename)
{

    std::string path = filename;
    if (path.rfind("/") == -1)
    {

        path = "";
    }
    else
    {

        path = path.substr(0, path.rfind("/"));
    }
    return path;
}
///////////////////////////////////////////////////////////
std::string GetFileTitle(const char* filename)
{

    std::string title = filename;
    if (strrchr(filename, '/') != NULL)
        title = strrchr(filename, '/') + 1;
    return title;
}
///////////////////////////////////////////////////////////
bool IsDirectory(const char* filename)
{

    std::string title = GetFileTitle(filename);
    std::string path = GetFilePath(filename);
    std::vector<std::string> folderlist = GetFolderList(path == "" ? "*" : (path + "/" + "*").c_str());
    for (int i = 0; i < int(folderlist.size()); i++)
    {

        if (title == folderlist[i])
        {

            return true;
        }
    }
    return false;
}
///////////////////////////////////////////////////////////
bool IsFile(const char* filename)
{

    std::string title = GetFileTitle(filename);
    std::string path = GetFilePath(filename);
    std::vector<std::string> filelist = GetFileList(path == "" ? "*" : (path + "/" + "*").c_str());
    for (int i = 0; i < int(filelist.size()); i++)
    {

        if (title == filelist[i])
        {

            return true;
        }
    }
    return false;
}
////////////////////////////////////////////////////////////////////////////////
bool PathExists(const char* szPath)
{
#ifdef WIN32
    char szOldDirectory[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, szOldDirectory);
    BOOL bRetVal = SetCurrentDirectory(szPath);
    SetCurrentDirectory(szOldDirectory);
    return (bRetVal ? true : false);
#else
    return false;
#endif
}
////////////////////////////////////////////////////////////////////////////////
bool FileExists(const char* szFile)
{
#ifdef WIN32
    HANDLE hFile = CreateFile(
                       szFile,
                       0,
                       FILE_SHARE_READ | FILE_SHARE_WRITE,
                       NULL,
                       OPEN_EXISTING,
                       0,
                       NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return false;
    CloseHandle(hFile);
    return true;
#else
    FILE* file = fopen(szFile, "rb");
    if (file == NULL)
        return false;
    fclose(file);
    file = NULL;
    return true;
#endif
}
////////////////////////////////////////////////////////////////////////////////
unsigned long FileSize(const char* filename)
{
    FILE* file = fopen(filename, "rb");
    if (file == NULL)
        return 0;
    fseek(file, 0, SEEK_END);
    unsigned long size = ftell(file);
    fclose(file);
    return size;
}
////////////////////////////////////////////////////////////////////////////////
bool EnumerateFiles(const char* filter, const char* directory, const char* directory_prefix, std::list<std::string>& files)
{
#ifdef WIN32
    // store current directory
    char old_directory[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, old_directory);
    if (strcmp(directory, "") != 0)
    {

        if (SetCurrentDirectory(directory) == 0)
        {

            return false;
        }
    }
    // add files in this directory
    WIN32_FIND_DATA ffd;
    HANDLE search = FindFirstFile(filter, &ffd);
    if (search != INVALID_HANDLE_VALUE)
    {

        do
        {

            if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
            {

                std::string root_path = directory_prefix;
                root_path += directory;
                root_path += ffd.cFileName;
                files.push_back(root_path);
            }
        }
        while (FindNextFile(search, &ffd));

        FindClose(search);
    }
    // now add subdirectories
    search = FindFirstFile("*", &ffd);
    if (search != INVALID_HANDLE_VALUE)
    {

        do
        {

            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY &&
                    strcmp(ffd.cFileName, ".") != 0 &&
                    strcmp(ffd.cFileName, "..") != 0)
            {

                std::string target_directory = ffd.cFileName;
                target_directory += "/";
                std::string target_prefix = directory_prefix;
                if (strcmp(directory, "") != 0)
                {

                    target_prefix += directory;
                }
                EnumerateFiles(filter, target_directory.c_str(), target_prefix.c_str(), files);
            }
        }
        while (FindNextFile(search, &ffd));

        FindClose(search);
    }
    // restore directory
    SetCurrentDirectory(old_directory);
    return true;
#else
    return false;
#endif
}
////////////////////////////////////////////////////////////////////////////////
