#if   defined(MAC)
#include "mac/mac_filesystem.h"

#elif defined(WIN32)
#include "win32/win32_filesystem.hpp"

#elif defined(unix)
#include "unix/unix_filesystem.h"

#else
#error unsupported platform

#endif
