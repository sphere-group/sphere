#ifndef DEFAULT_FILE_SYSTEM_HPP
#define DEFAULT_FILE_SYSTEM_HPP

#include "IFileSystem.hpp"
class DefaultFileSystem : public IFileSystem
{
public:
    IFile* Open(const char* filename, int mode);
    const char* GetFileSystemName()
    {
        return "default";
    }
};

extern DefaultFileSystem g_DefaultFileSystem;
#endif
