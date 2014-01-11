#ifndef LOG_HPP
#define LOG_HPP

// identifier too long
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include <stack>
#include <string>
#include <cstdio>

class CLog
{
public:
    CLog();
    ~CLog();

    bool Open(const char* filename);
    bool Close();
    bool Send(const char* text);
    void BeginBlock(const char* name);
    void EndBlock();

private:
    bool SendNoNewLine(const char* text);
    const char* GetTime();

private:
    std::stack<std::string> m_Blocks;
    FILE* m_file;
};

#endif
