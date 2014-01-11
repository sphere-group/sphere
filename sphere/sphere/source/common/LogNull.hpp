#ifndef __LOGNULL_HPP
#define __LOGNULL_HPP
#include "LogEvent.hpp"
class CLogNull : public CLogEvent
{
private:
    virtual void WriteString(const char* string);
};
#endif
