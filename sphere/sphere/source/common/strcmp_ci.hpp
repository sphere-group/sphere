#ifndef STRCMP_CI_HPP
#define STRCMP_CI_HPP
/* VC++ has _stricmp, and it's probably faster, so use it... */
#ifdef _MSC_VER
#define strcmp_ci _stricmp
#else
int strcmp_ci(const char* str1, const char* str2);
#endif
#endif
