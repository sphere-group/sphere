#ifndef I_FILE_HPP
#define I_FILE_HPP


struct IFile
{
  virtual void Close() = 0;

  virtual int  Read(void* bytes, int size) = 0;
  virtual int  Write(const void* bytes, int size) = 0;
  virtual int  Size() = 0;
  virtual int  Tell() = 0;
  virtual void Seek(int position) = 0;
};


#endif
