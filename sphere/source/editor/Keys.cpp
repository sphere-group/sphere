#include <windows.h>
// include the struct definitions
#include "Keys.hpp"

// define the static struct members, which we wouldn't have to do if VC++ didn't suck
#define DEFINE_KEY(name, T, def)				\
    const char* const _##name::keyname = #name;	\
    const T _##name::default_value = def;

#include "Keys.table"
