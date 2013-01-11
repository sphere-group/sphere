#ifndef DEFAULT_FILE_HPP
#define DEFAULT_FILE_HPP


#include <stdio.h>
#include "IFile.hpp"


class DefaultFileSystem;


class DefaultFile : public IFile
{
  friend class DefaultFileSystem;

public:
  virtual void Close();

  virtual int  Read(void* bytes, int size);
  virtual int  Write(const void* bytes, int size);
  virtual int  Size();
  virtual int  Tell();
  virtual void Seek(int position);
  
private:
  DefaultFile(FILE* file);

private:
  FILE* m_file;
};


#endif
