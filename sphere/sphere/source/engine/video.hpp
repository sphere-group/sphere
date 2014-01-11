#if   defined(MAC)
#include "mac/mac_video.h"

#elif defined(WIN32)
#include "win32/win32_video.hpp"

#elif defined(unix)
#include "unix/unix_video.h"

#else
#error unsupported platform

#endif
