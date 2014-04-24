/******************************************************************************
 * @file common/platform.h
 * @author Jos 'Rahkiin' Kuijpers
 *
 * Platform macros. This cleans up code elsewhere and is easy to keep up
 * to date.
 * 
 * Use by including platform.h.
 * When having specific code for both linux and unix apart,
 * place linux above unix when comparing, as unix is also defined for linux.
 *****************************************************************************/

#ifndef SPHERE_PLATFORM_H
#define SPHERE_PLATFORM_H

#ifdef _WIN32
# define SPHERE_WIN32 1
#elif __APPLE__ && __MACH__
# define SPHERE_MAC_OSX 1
// # define SPHERE_UNIX 1
#elif __linux
# define SPHERE_LINUX 1
# define SPHERE_UNIX 1
#elif __unix
# define SPHERE_UNIX 1
#else
# error unsupported platform
#endif

#ifdef _MSC_VER
# define SPHERE_MSC 1
#elif __clang__
# define SPHERE_CLANG 1
#elif __GNUC__
# define SPHERE_GCC 1
#elif __MINGW32__
# define SPHERE_MINGW 1
#endif

// GNUC menas we have a GNU C compatible compiler, so it can be
// many compilers. Always define GNUC if it is that case.
#if __GNUC__ && !SPHERE_GNUC
# define SPHERE_GNUC 1
#endif

#ifdef SPHERE_WIN32
# define SPHERE_STDCALL __attribute__((stdcall))
#else
# define SPHERE_STDCALL
#endif

#ifndef __has_feature
# define __has_feature(x) 0
#endif
#ifndef __has_extension
# define __has_extension __has_feature
#endif

#endif // SPHERE_PLATFORM_H
