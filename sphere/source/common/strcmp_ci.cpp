#include <ctype.h>
#include <string.h>
#include "strcmp_ci.hpp"
/* VC++ has _stricmp, and it's probably faster, so use it... */
#ifdef _MSC_VER
#else
int strcmp_ci(const char* str1, const char* str2)
{
    char c1, c2;
    while (*str1 && *str2)
    {

        c1 = tolower(*str1);
        c2 = tolower(*str2);
        if (c1 != c2)
        {

            return c1 - c2;
        }
        str1++;
        str2++;
    }
    c1 = tolower(*str1);
    c2 = tolower(*str2);
    return c1 - c2;
}
#endif
