#include <stddef.h>
#include "LogDispatcher.hpp"
////////////////////////////////////////////////////////////////////////////////
CLogDispatcher::CLogDispatcher()
        : m_Head(NULL)
{
}

////////////////////////////////////////////////////////////////////////////////
CLogDispatcher::~CLogDispatcher()
{
    // step along linked list destroying the nodes (and the logs if necessary)
    SLogNode* p = m_Head;
    while (p != NULL)
    {
        // get a pointer to this node and move the cursor along
        SLogNode* q = p;
        p = p->next;
        // delete this node and the log attached to it
        if (q->auto_delete)
            delete q->log;
        delete q;
    }
}
////////////////////////////////////////////////////////////////////////////////
void
CLogDispatcher::AddLog(CLogEvent* log, bool auto_delete)
{
    SLogNode* p = new SLogNode;
    if (!p)
        return;
    p->next        = NULL;
    p->log         = log;
    p->auto_delete = auto_delete;
    if (m_Head == NULL)
        m_Head = p;
    else
    {
        // step along until end of list
        SLogNode* q = m_Head;
        while (q->next)
            q = q->next;
        q->next = p;
    }
}
////////////////////////////////////////////////////////////////////////////////
void
CLogDispatcher::WriteString(const char* string)
{
    // write the string to all of the logs
    SLogNode* p = m_Head;
    while (p != NULL)
    {
        *(p->log) << string;
        p = p->next;
    }
}
////////////////////////////////////////////////////////////////////////////////
