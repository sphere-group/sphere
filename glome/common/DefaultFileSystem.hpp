#ifndef DEFAULT_FILE_SYSTEM_HPP
#define DEFAULT_FILE_SYSTEM_HPP


#include "IFileSystem.hpp"


class DefaultFileSystem : public IFileSystem
{
public:
  virtual IFile* Open(const char* filename, int mode);
};


extern DefaultFileSystem g_DefaultFileSystem;


#endif
