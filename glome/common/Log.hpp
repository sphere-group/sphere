#ifndef __LOG_HPP
#define __LOG_HPP


class CLog
{
public:
  virtual ~CLog() { };

  friend CLog& operator<<(CLog& log, const char* string);
  friend CLog& operator<<(CLog& log, char character);
  friend CLog& operator<<(CLog& log, int integer);

private:
  virtual void WriteString(const char* string) = 0;
};


#endif

