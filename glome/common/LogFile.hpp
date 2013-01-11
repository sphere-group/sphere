#ifndef __LOGFILE_HPP
#define __LOGFILE_HPP


#include <stdio.h>
#include "Log.hpp"


class CLogFile : public CLog
{
public:
  CLogFile(const char* filename);
  ~CLogFile();

private:
  virtual void WriteString(const char* string);

private:
  FILE* m_File;
};


#endif
