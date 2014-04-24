#include "../common/platform.h"

#if SPHERE_MAC_OSX 
# include "mac/mac_input.h"
#elif SPHERE_WIN32
# include "win32/win32_input.hpp"
#elif SPHERE_UNIX
# include "unix/unix_input.h"
#endif
