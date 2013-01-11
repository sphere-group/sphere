#include "unix_time.h"
#include "SDL.h"

/* dword GetTime () {
  time_t current;

  current = time(NULL);
  if (current == (time_t)(-1))
    return 0;
  return current;
} */

dword GetTime () {
  Uint32 current;

  current = SDL_GetTicks();
  return current;
}
