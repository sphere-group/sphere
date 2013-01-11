// defines keys, which are names associated with data types
#ifndef KEYS_HPP
#define KEYS_HPP

#include <string>

// template class for starting a POD type with zeroed members
template<typename T>
class zeroed
{
public:
  zeroed() { memset(&t, 0, sizeof(t)); }
  operator T() { return t; }

private:
  T t;
};

// if this were gcc, we could use the static const initializer in the struct, but no, I have to hack something in :\
// Down with VC++!  Long live gcc!

#define DEFINE_KEY(name, T, def)        \
static struct _##name {                 \
    static const char* const keyname;   \
    static const T default_value;       \
    typedef T type;                     \
} name;

// Define the keys
#include "Keys.table"

#undef DEFINE_KEY
#endif
