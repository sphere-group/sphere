#include "../common/platform.h"

#if SPHERE_HAS_NETWORKING

#if SPHERE_MAC_OSX
# include "mac/mac_network.h"
#elif SPHERE_WIN32
# include "win32/win32_network.hpp"
#elif SPHERE_UNIX
# include "unix/unix_network.h"
#endif

#endif