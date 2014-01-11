#ifndef DEBUG_HPP
#define DEBUG_HPP
#include <string>
#include "../common/LogDispatcher.hpp"

void InitializeLog();
class LogBlock
{
public:
  LogBlock(const char* name);
  ~LogBlock();
private:
  std::string m_Name;
  static int s_NumTabs;
};

extern CLogDispatcher __DebugLog__;
// logs the entry and exit of a block of code, indenting any log strings in the body

#define LOG_BLOCK(name) LogBlock log_block(name);
// logs one line

#define LOG(x) { for (int i = 0; i < LogBlock::s_NumTabs; i++) __DebugLog__ << "    "; __DebugLog__ << x; }
#endif
