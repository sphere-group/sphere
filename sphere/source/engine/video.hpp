#include "../common/platform.h"

#if SPHERE_WIN32
# include "win32/win32_video.hpp"
#elif SPHERE_MAC_OSX
# include "mac/mac_video.h"
#elif SPHERE_UNIX
# include "unix/unix_video.h"
#endif
