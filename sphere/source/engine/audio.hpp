#include "../common/platform.h"

#if SPHERE_MAC_OSX 
# include "mac/mac_audio.h"
#elif SPHERE_WIN32
# include "win32/win32_audio.hpp"
#elif SPHERE_UNIX
# include "unix/unix_audio.h"
#endif
