#ifndef PACKAGE_FILE_HPP
#define PACKAGE_FILE_HPP
#include <stdio.h>
#include <zlib.h>
#include "../common/IFile.hpp"
#include "../common/types.h"
class CPackageFileSystem;
// macros are bad!
#define PACKAGE_BUFFER_SIZE 4096
class CPackageFile : public IFile
{
    friend class CPackageFileSystem;
public:
    virtual void Close();
    virtual int  Read(void* bytes, int size);
    virtual int  Write(const void* bytes, int size);
    virtual int  Size();
    virtual int  Tell();
    virtual void Seek(int position);
private:
    CPackageFile(FILE* file);
    CPackageFile(FILE* file, int start, int length, int compressed_length);
private:
    bool m_in_package;
    FILE* m_file;
    // following are only valid if (m_in_package == true)
    int m_start;
    int m_length;
    int m_compressed_length;
    int m_position; // position in uncompressed file
    long m_next_read; // position in compressed file
    z_stream m_stream;
    byte m_in_buffer[PACKAGE_BUFFER_SIZE];  // from file (compressed)
};
#endif
