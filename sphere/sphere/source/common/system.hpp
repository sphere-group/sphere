#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#ifdef WIN32
#include <windows.h>
#endif
#include <vector>
#include <cstring>
#include <string>
#include <list>
#ifndef MAX_PATH
#define MAX_PATH 255
#endif
#ifndef WIN32
// non win32 definitions
bool SetCurrentDirectory(const char* directory);
bool CopyFile(const char* src, const char* dest, bool force);
bool GetCurrentDirectory(int max_path, char* directory);
bool CreateDirectory(const char* directory, const char* options);

#endif // WIN32
std::vector<std::string> GetFileList(const char* filter);
std::vector<std::string> GetFolderList(const char* filter);

std::string GetFileTitle(const char* filename);
std::string GetFilePath(const char* filename);
bool IsDirectory(const char* filename);
bool IsFile(const char* filename);
///
extern bool PathExists(const char* pathname);
extern bool FileExists(const char* filename);
extern unsigned long  FileSize(const char* filename);
extern bool EnumerateFiles(const char* filter, const char* directory, const char* directory_prefix, std::list<std::string>& files);
#endif // SYSTEM_HPP
