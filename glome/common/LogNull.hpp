#ifndef __LOGNULL_HPP
#define __LOGNULL_HPP


#include "Log.hpp"


class CLogNull : public CLog
{
private:
  virtual void WriteString(const char* string);
};


#endif
