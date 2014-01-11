#include <stdio.h>
#include "Scripting.hpp"
// reserved word list
static const char* Keywords[] = {
  // keywords
  "break", "else", "new", "var",
  "case", "finally", "return", "void",
  "catch", "for", "switch", "while",
  "continue", "function", "this", "with",
  "default", "if", "throw",
  "delete", "in", "try",
  "do", "instanceof", "typeof",
  // future reserved words
  "abstract", "enum", "int", "short",
  "boolean", "export", "interface", "static",
  "byte", "extends", "long", "super",
  "char", "final", "native", "synchronized",
  "class", "float", "package", "throws",
  "const", "goto", "private", "transient",
  "debugger", "implements", "protected", "volatile",
  "double", "import", "public",
  // null literal
  "null",
  // boolean literals
  "true", "false",
};
////////////////////////////////////////////////////////////////////////////////
sScripting::sScripting()
{
  rt = NULL;
  cx = NULL;
  global = NULL;
  m_Script     = NULL;
  m_HasError   = false;
  m_IsRunning  = false;
  m_ShouldExit = false;
  m_IsCreated  = false;
  m_private_data = NULL;
}
////////////////////////////////////////////////////////////////////////////////
sScripting::~sScripting()
{
  Destroy();
}
////////////////////////////////////////////////////////////////////////////////
static std::string GetToken(const char* tokenptr)
{
  // hack
  return " ";
}
////////////////////////////////////////////////////////////////////////////////
int
sScripting::GetTokenStart(int line, int offset)
{
  const char* p = m_Script;
  for (int i = 0; i < line; i++)
  {
    while (*p && *p != '\n')
      p++;
    p++;
  }
  return (p - m_Script) + offset;
}
////////////////////////////////////////////////////////////////////////////////
void
ErrorReporter(JSContext* cx, const char* message, JSErrorReport* report)
{
  sScripting* scripter = (sScripting*) JS_GetContextPrivate(cx);
  if (scripter) {
    scripter->m_HasError   = true;
    scripter->m_ShouldExit = true;
    // fill error information
    scripter->m_Error.m_Message    = message;
    scripter->m_Error.m_Token      = GetToken(report->tokenptr);
    scripter->m_Error.m_TokenLine  = report->lineno;
    scripter->m_Error.m_TokenStart = scripter->GetTokenStart(report->lineno, report->tokenptr - report->linebuf);
  }
}
////////////////////////////////////////////////////////////////////////////////
JSBool BranchCallback(JSContext* cx, JSScript* script)
{
  bool m_IsRunning = false;
  bool m_ShouldExit = false;
  sScripting* scripter = (sScripting*) JS_GetContextPrivate(cx);
  if (scripter) {
    m_IsRunning  = scripter->m_IsRunning;
    m_ShouldExit = scripter->m_ShouldExit;
  }
  // handle garbage collection
  // garbage collect!
  JS_GC(cx);
  return (!m_IsRunning || m_ShouldExit) ? JS_FALSE : JS_TRUE;
}
////////////////////////////////////////////////////////////////////////////////
bool
sScripting::Create()
{
  Destroy();
  rt = JS_NewRuntime(4 * 1024 * 1024);
  if (rt == NULL) {
    return false;
  }
  cx = JS_NewContext(rt, 8 * 1024);
  if (cx == NULL) {
    JS_DestroyRuntime(rt);
    rt = NULL;
    return false;
  }
  static JSClass global_class = {
    "global", 0,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
  };
  global = JS_NewObject(cx, &global_class, NULL, NULL);
  if (global == NULL) {
    JS_DestroyContext(cx);
    cx = NULL;
    JS_DestroyRuntime(rt);
    rt = NULL;
    return false;
  }
  JS_SetContextPrivate(cx, (void*)this);
  JS_InitStandardClasses(cx, global);
  JS_SetErrorReporter(cx,  ErrorReporter);
  JS_SetBranchCallback(cx, BranchCallback);
  m_IsCreated = true;
  return true;
}
////////////////////////////////////////////////////////////////////////////////
void
sScripting::Destroy()
{
  m_ShouldExit = true;
  while (m_IsRunning == true && m_ShouldExit == true) { }
  if (cx) { JS_DestroyContext(cx); cx = NULL; }
  if (rt) { JS_DestroyRuntime(rt); rt = NULL; }
  if (m_Script) {
    delete[] m_Script;
    m_Script = NULL;
  }
  m_private_data = NULL;
  m_IsCreated = false;
}
////////////////////////////////////////////////////////////////////////////////
bool
sScripting::IsKeyword(const char* token)
{
  for (int i = 0; i < sizeof(Keywords) / sizeof(*Keywords); i++)
    if (strcmp(token, Keywords[i]) == 0)
      return true;
  return false;
}
////////////////////////////////////////////////////////////////////////////////
bool
sScripting::VerifyScript(const char* script, sCompileError& error)
{
  sScripting scripter;
  if (!scripter.Create())
    return false;
  bool v = scripter.__VerifyScript__(script, error);
  scripter.Destroy();
  return v;
}
////////////////////////////////////////////////////////////////////////////////
void*
sScripting::GetPrivate() {
  return m_private_data;
}
////////////////////////////////////////////////////////////////////////////////
void
sScripting::SetPrivate(void* data) {
  m_private_data = data;
}
////////////////////////////////////////////////////////////////////////////////
bool
sScripting::SetScript(const char* script)
{
  if (m_Script) {
    delete[] m_Script;
    m_Script = NULL;
  }
  if (!m_IsCreated)
    return false;
  int size = strlen(script);
  m_Script = new char[size + 1];
  if (!m_Script)
    return false;
  strcpy(m_Script, script);
  return true;
}
////////////////////////////////////////////////////////////////////////////////
bool
sScripting::__VerifyScript__(const char* script, sCompileError& error)
{
  if (!m_IsCreated)
    return false;
  if (rt == NULL || cx == NULL || global == NULL)
    return false;
  if (script != NULL) {
    if (!SetScript(script))
      return false;
  }
  if (m_Script == NULL)
    return false;
  m_ShouldExit = false;
  m_HasError = false;
  m_IsRunning = true;
  // compile the script (checks for syntax errors)
  // time to actually compile
  if (error.m_TokenLine != -2) {
    JSScript* compiled_script = JS_CompileScript(cx, global, m_Script, strlen(m_Script), "", 0);
    if (compiled_script) {
      if (!m_HasError && error.m_TokenLine == -1) {
        jsval val;
        if (JS_ExecuteScript(cx, global, compiled_script, &val) == JS_TRUE) {
          JSString* str = JS_ValueToString(cx, val);
          if (str) {
            const char* s = JS_GetStringBytes(str);
            error.m_Message = (s ? s : "");
          } else {
            error.m_Message = "";
          }
        }
      }
      JS_DestroyScript(cx, compiled_script);
      compiled_script = NULL;
    }
  }
  else {
    jsval val;
    if (JS_EvaluateScript(cx, global, m_Script, strlen(m_Script), NULL, 0, &val) == JS_TRUE)
    {
      if (JS_EvaluateScript(cx, global, "main()", strlen("main()"), NULL, 0, &val) == JS_TRUE)
      {
        JSString* str = JS_ValueToString(cx, val);
        if (str) {
          const char* s = JS_GetStringBytes(str);
          error.m_Message = (s ? s : "");
        } else {
          error.m_Message = "";
        }          
      }
    }
  }
  if (m_HasError) {
    error = m_Error;
  }
  // we're done
  delete[] m_Script;
  m_Script = NULL;
  m_IsRunning = false;
  return !m_HasError;
}
////////////////////////////////////////////////////////////////////////////////
