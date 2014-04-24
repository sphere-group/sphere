#include "../common/platform.h"

#if SPHERE_MAC_OSX
# include "mac/mac_filesystem.h"
#elif SPHERE_WIN32
# include "win32/win32_filesystem.hpp"
#elif SPHERE_UNIX
# include "unix/unix_filesystem.h"
#endif
