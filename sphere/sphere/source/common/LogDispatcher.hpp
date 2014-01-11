#ifndef __LOGDISPATCHER_HPP
#define __LOGDISPATCHER_HPP
#include "LogEvent.hpp"
class CLogDispatcher : public CLogEvent
{
public:
    CLogDispatcher();
    ~CLogDispatcher();
    void AddLog(CLogEvent* log, bool auto_delete = true);
private:
    struct SLogNode
    {
        SLogNode* next;
        CLogEvent*     log;
        bool      auto_delete;
    };
private:
    virtual void WriteString(const char* string);
private:
    SLogNode* m_Head;
};
#endif
