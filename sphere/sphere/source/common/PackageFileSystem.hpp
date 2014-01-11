#ifndef PACKAGE_FILE_SYSTEM_HPP
#define PACKAGE_FILE_SYSTEM_HPP
#include <vector>
#include <string>
#include <stdio.h>
#include "../common/IFileSystem.hpp"
#include "../common/types.h"
class CPackageFileSystem : public IFileSystem
{
public:
    CPackageFileSystem(const char* filename, bool load_from_filesystem = true);
    ~CPackageFileSystem();
    IFile* Open(const char* filename, int mode);
    int GetNumFiles() const;
    const char* GetFileName(int i) const;
    const char* GetFileSystemName()
    {
        return "package";
    }

private:
    struct fileinfo
    {

        std::string name;
        dword file_offset;
        dword file_size;
        dword compressed_size;
    };
private:
    FILE* m_file;
    std::vector<fileinfo> m_directory;
    bool m_LoadFromFileSystem;
};
#endif
