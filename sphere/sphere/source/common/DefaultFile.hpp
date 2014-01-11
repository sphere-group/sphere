#ifndef DEFAULT_FILE_HPP
#define DEFAULT_FILE_HPP

#include <stdio.h>
#include "IFile.hpp"

class DefaultFileSystem;
class DefaultFile : public IFile
{
    friend class DefaultFileSystem;

public:
    ~DefaultFile();
    int  Read(void* bytes, int size);
    int  Write(const void* bytes, int size);
    int  Size();
    int  Tell();
    void Seek(int position);

private:
    DefaultFile(FILE* file);

private:
    FILE* m_file;
};

#endif
