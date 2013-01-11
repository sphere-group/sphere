#if   defined(MAC)
#include "mac/mac_network.h"

#elif defined(WIN32)
#include "win32/win32_network.hpp"

#elif defined(unix)
#include "unix/unix_network.h"

#else
#error unsupported platform

#endif
