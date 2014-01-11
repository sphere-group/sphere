#include <SDL.h>

#include "mac_time.h"

dword GetTime ()
{
    return SDL_GetTicks();
}
