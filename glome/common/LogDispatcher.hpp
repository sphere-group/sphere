#ifndef __LOGDISPATCHER_HPP
#define __LOGDISPATCHER_HPP


#include "Log.hpp"


class CLogDispatcher : public CLog
{
public:
  CLogDispatcher();
  ~CLogDispatcher();

  void AddLog(CLog* log, bool auto_delete = true); 

private:
  struct SLogNode
  {
    SLogNode* next;
    CLog*     log;
    bool      auto_delete;
  };

private:
  virtual void WriteString(const char* string);

private:
  SLogNode* m_Head;
};


#endif
