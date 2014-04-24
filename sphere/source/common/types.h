// portable type definitions
// chad austin
// 7.26.1999

// Some of these conflict with JS.
#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

// boolean (VC++'s bool is 1 byte)
#ifndef __cplusplus
#define bool  unsigned char
#define true  1
#define false 0
#endif

// sized types
typedef uint8_t    byte;
typedef int8_t    sbyte;
typedef uint8_t    ubyte;

typedef uint16_t   word;
typedef int16_t   sword;
typedef uint16_t   uword;

typedef uint32_t    dword;
typedef int32_t    sdword;
typedef uint32_t    udword;

#ifdef _MSC_VER
typedef unsigned __int64 qword;
typedef signed   __int64 sqword;
typedef unsigned __int64 uqword;

typedef unsigned short   ushort;
typedef signed   short   sshort;

typedef unsigned long    ulong;
typedef signed   long    slong;

// typedef  signed char    int8;
typedef          short   int16;
typedef          long    int32;
//typedef          __int64 int64;

typedef signed   int     sint;
typedef signed   char    sint8;
typedef signed   short   sint16;
typedef signed   long    sint32;

typedef unsigned int     uint;
typedef unsigned char    uint8;
typedef unsigned short   uint16;
typedef unsigned long    uint32;

typedef signed   char    schar;
typedef unsigned char    uchar;

typedef          char    char8;
typedef signed   char    schar8;
typedef unsigned char    uchar8;

typedef          short   char16;
typedef signed   short   schar16;
typedef unsigned short   uchar16;

typedef          float   float32;
typedef          double  float64;

#elif defined(CYGWIN)
typedef          long long qword;
typedef signed   long long sqword;
typedef unsigned long long uqword;

typedef unsigned short   ushort;
typedef signed   short   sshort;

typedef unsigned long    ulong;
typedef signed   long    slong;

// typedef  signed char    int8;
typedef          short     int16;
typedef          long      int32;
//typedef          long long int64;

typedef signed   int     sint;
typedef signed   char    sint8;
typedef signed   short   sint16;
typedef signed   long    sint32;

typedef unsigned int     uint;
typedef unsigned char    uint8;
typedef unsigned short   uint16;
typedef unsigned long    uint32;

typedef signed   char    schar;
typedef unsigned char    uchar;

typedef          char    char8;
typedef signed   char    schar8;
typedef unsigned char    uchar8;

typedef          short   char16;
typedef signed   short   schar16;
typedef unsigned short   uchar16;

typedef          float   float32;
typedef          double  float64;

#else

typedef int64_t qword;
typedef int64_t sqword;
typedef uint64_t uqword;

typedef uint16_t ushort;
typedef int16_t sshort;

typedef unsigned long    ulong;
typedef signed   long    slong;

typedef  int8_t    int8;
typedef  int16_t   int16;
// typed long int32;

//typedef          long long int64;
typedef signed   int     sint;
typedef int8_t    sint8;
typedef int16_t   sint16;
typedef int32_t   sint32;

typedef unsigned int     uint;
typedef uint8_t    uint8;
typedef uint16_t   uint16;
typedef uint32_t   uint32;

typedef signed   char    schar;
typedef unsigned char    uchar;

typedef int8_t    char8;
typedef int8_t    schar8;
typedef uint8_t   uchar8;

typedef uint16_t  char16;
typedef int16_t   schar16;
typedef uint16_t  uchar16;

typedef float   float32;
typedef double float64;

#endif
#endif /* TYPES_H__ */
