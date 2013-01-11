#ifndef I_FILE_SYSTEM_HPP
#define I_FILE_SYSTEM_HPP

#include "../common/IFile.hpp"
struct IFileSystem
{
    // file open modes
    enum { read = 0x1, write = 0x2 };

    virtual ~IFileSystem()								{}
    virtual IFile* Open(const char* filename, int mode) =0;
    virtual const char* GetFileSystemName()				=0;
};

#endif
