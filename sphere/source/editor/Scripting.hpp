#ifndef SCRIPTING_HPP
#define SCRIPTING_HPP
#pragma warning(disable: 4786)
#include <string>
#include <vector>
#include <jsapi.h>
struct sCompileError
{
  sCompileError(
    const char* message = "",
    const char* token = "",
    int token_start = 0,
    int token_line = 0)
  : m_Message(message)
  , m_Token(token)
  , m_TokenStart(token_start)
  , m_TokenLine(token_line)
  {
  }
  std::string m_Message;
  std::string m_Token;
  int         m_TokenStart;
  int         m_TokenLine;
};
class sScripting {
public:
  char* m_Script;
  bool  m_HasError;
  sCompileError m_Error;
  bool m_IsRunning;
  bool m_ShouldExit;
  bool m_IsCreated;
  JSRuntime* rt;
  JSContext* cx;
  JSObject* global;
  void* m_private_data;
  sScripting();
  ~sScripting();
public:
  int GetTokenStart(int line, int offset);
public:
  bool SetScript(const char* script);
  void SetPrivate(void* data);
  void* GetPrivate();
public:
  bool Create();
  void Destroy();
  static bool IsKeyword(const char* token);
  static bool VerifyScript(const char* script, sCompileError& error);
  bool __VerifyScript__(const char* script, sCompileError& error);
};
#endif
