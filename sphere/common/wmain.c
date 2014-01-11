#include <windows.h>
#include "wmain.h"


extern int __cdecl main(int argc, char* argv[]);


static int iswhitespace(char c);


static HINSTANCE MainInstance;


////////////////////////////////////////////////////////////////////////////////

// Stub WinMain
// It calls main() with the standard argc and argv parameters

int WINAPI WinMain(HINSTANCE inst, HINSTANCE previnst, LPSTR cmdline, int cmdshow)
{
  int argc = 1;
  char** argv;

  char* p;

  // so everything else can have it
  MainInstance = inst;

  // allocate space for first entry
  argv = (char**)malloc(1 * sizeof(char*));

  // copy executable name into first entry
  argv[0] = (char*)malloc(sizeof(char) * 520);
  GetModuleFileName(inst, argv[0], 512);

  // convert cmdline into arguments
  p = cmdline;
  while (*p)
  {
    // skip whitespace
    while (iswhitespace(*p))
      p++;

    if (*p)
    {
      int size = 0;

      // allocate space for new argument
      argv = (char**)realloc(argv, (argc + 1) * sizeof(char*));
      argv[argc] = NULL;

      if (*p == '\"') // go until next " or end of string
      {
        p++;
        while (*p && *p != '\"')
        {
          argv[argc] = (char*)realloc(argv[argc], size + 1);
          argv[argc][size] = *p;
          size++;
          p++;
        }
        if (*p) // if we hit a ", skip it
          p++;
      }
      else            // go until there is no more whitespace
      {
        while (*p && !iswhitespace(*p))
        {
          argv[argc] = (char*)realloc(argv[argc], size + 1);
          argv[argc][size] = *p;
          size++;
          p++;
        }
      }

      // append a terminating zero
      argv[argc] = (char*)realloc(argv[argc], size + 1);
      argv[argc][size] = 0;

      argc++;
    }
  }
  MessageBox(NULL, "Test.", "Your car is on fire!", MB_OK);
  return main(argc, argv);
}

////////////////////////////////////////////////////////////////////////////////

int iswhitespace(char c)
{
  return (c == ' ' || c == '\n' || c == '\r' || c == '\t');
}

////////////////////////////////////////////////////////////////////////////////

HINSTANCE GetMainInstance(void)
{
  return MainInstance;
}

////////////////////////////////////////////////////////////////////////////////
