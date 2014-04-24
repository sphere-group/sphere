#include "../common/platform.h"

#if SPHERE_MAC_OSX
# include "mac/mac_time.h"
#elif SPHERE_WIN32
# include "win32/win32_time.hpp"
#elif SPHERE_UNIX
# include "unix/unix_time.h"
#endif
