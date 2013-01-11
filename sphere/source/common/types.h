// portable type definitions
// chad austin
// 7.26.1999

// Some of these conflict with JS.
#ifndef TYPES_H
#define TYPES_H

// boolean (VC++'s bool is 1 byte)
#ifndef __cplusplus
#define bool  unsigned char
#define true  1
#define false 0
#endif

// sized types
typedef unsigned char    byte;
typedef signed   char    sbyte;
typedef unsigned char    ubyte;

typedef unsigned short   word;
typedef signed   short   sword;
typedef unsigned short   uword;

typedef unsigned long    dword;
typedef signed   long    sdword;
typedef unsigned long    udword;

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
typedef          long long qword;
typedef signed   long long sqword;
typedef unsigned long long uqword;

typedef unsigned short   ushort;
typedef signed   short   sshort;

typedef unsigned long    ulong;
typedef signed   long    slong;

// typedef  signed char    int8;
typedef          short     int16;
// typed long int32;

//typedef          long long int64;
typedef signed   int     sint;
typedef signed   char    sint8;
typedef signed   short   sint16;
typedef signed   long    sint32;

typedef unsigned int     uint;
typedef unsigned char    uint8;
typedef unsigned short   uint16;
// typedef unsigned long    uint32;

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

#endif
#endif /* TYPES_H__ */
