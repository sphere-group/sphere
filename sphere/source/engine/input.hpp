#if   defined(MAC)
#include "mac/mac_input.h"

#elif defined(WIN32)
#include "win32/win32_input.hpp"

#elif defined(unix)
#include "unix/unix_input.h"

#else
#error unsupported platform

#endif
