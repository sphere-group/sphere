
#include <string.h>
#include <time.h>
#include "log.hpp"

////////////////////////////////////////////////////////////////////////////////
CLog::CLog()
        : m_file(NULL)
{}

////////////////////////////////////////////////////////////////////////////////
CLog::~CLog()
{
    Close();
}

////////////////////////////////////////////////////////////////////////////////
bool
CLog::Open(const char* filename)
{
    m_file = fopen(filename, "a");
    if (m_file == NULL)
    {
        return false;
    }

    SendNoNewLine("Log opened: ");
    SendNoNewLine(GetTime());
    SendNoNewLine("\n");

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
CLog::Close()
{
    if (m_file)
    {

        SendNoNewLine("Log closed: ");
        SendNoNewLine(GetTime());
        SendNoNewLine("\n");

        fclose(m_file);
        m_file = NULL;
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
bool
CLog::Send(const char* text)
{
    if (m_file)
    {
        SendNoNewLine(GetTime());
        SendNoNewLine(" -- ");
        for (unsigned int i = 0; i < m_Blocks.size(); i++)
        {
            SendNoNewLine("  ");
        }
        SendNoNewLine(text);
        fputc('\n', m_file);
        fflush(m_file);
        return true;
    }
    else
    {
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////
void
CLog::BeginBlock(const char* name)
{
    m_Blocks.push(name);
    if (m_file)
    {
        SendNoNewLine(GetTime());
        SendNoNewLine(" -- ");
        for (unsigned int i = 0; i < m_Blocks.size() - 1; i++)
            SendNoNewLine("  ");
        SendNoNewLine("+");
        SendNoNewLine(name);
        SendNoNewLine("\n");
        fflush(m_file);
    }
}

////////////////////////////////////////////////////////////////////////////////
void
CLog::EndBlock()
{
    if (m_file && m_Blocks.size() > 0)
    {
        SendNoNewLine(GetTime());
        SendNoNewLine(" -- ");
        for (unsigned int i = 0; i < m_Blocks.size() - 1; i++)
            SendNoNewLine("  ");
        SendNoNewLine("-");
        SendNoNewLine(m_Blocks.top().c_str());
        SendNoNewLine("\n");

        m_Blocks.pop();
        fflush(m_file);
    }
}

////////////////////////////////////////////////////////////////////////////////
bool
CLog::SendNoNewLine(const char* text)
{
    return (fputs(text, m_file) != 0);
}

////////////////////////////////////////////////////////////////////////////////
const char*
CLog::GetTime()
{
    time_t t = time(NULL);
    char* str = asctime(localtime(&t));
    *strchr(str, '\n') = 0;
    return str;
}

////////////////////////////////////////////////////////////////////////////////
