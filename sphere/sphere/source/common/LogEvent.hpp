#ifndef __LOG_EVENT_HPP
#define __LOG_EVENT_HPP
class CLogEvent
{
public:
    virtual ~CLogEvent()
    { };

    friend CLogEvent& operator<<(CLogEvent& log, const char* string);
    friend CLogEvent& operator<<(CLogEvent& log, char character);
    friend CLogEvent& operator<<(CLogEvent& log, int integer);
private:
    virtual void WriteString(const char* string) = 0;
};
#endif
