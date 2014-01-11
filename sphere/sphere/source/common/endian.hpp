#ifndef ENDIAN_HPP
#define ENDIAN_HPP

#include "types.h"

#define SPHERE_LITTLE_ENDIAN 0
#define SPHERE_BIG_ENDIAN    1

#if defined(MAC) && defined(__BIG_ENDIAN__)
#define SPHERE_BYTEORDER SPHERE_BIG_ENDIAN
#else
#define SPHERE_BYTEORDER SPHERE_LITTLE_ENDIAN
#endif

// the functions are named as following:
//   <initial format> to <result format> _ <data size>
// where format can be one of:
//   m = machine
//   l = little-endian
//   b = big-endian
// and data size can be:
//   w = word
//   d = dword
//   f = float32


#if SPHERE_BYTEORDER == SPHERE_LITTLE_ENDIAN

inline word ltom_w(word in)
{
    return in;
}

inline word mtol_w(word in)
{
    return in;
}

inline word btom_w(word in)
{
    return (in & 0xFF00 >> 8) + (in & 0x00FF << 8);
}

inline word mtob_w(word in)
{
    return (in & 0xFF00 >> 8) + (in & 0x00FF << 8);
}

inline dword ltom_d(dword in)
{
    return in;
}

inline dword mtol_d(dword in)
{
    return in;
}

inline dword btom_d(dword in)
{
    return (in & 0xFF000000 >> 24) +
           (in & 0x00FF0000 >> 8)  +
           (in & 0x0000FF00 << 8)  +
           (in & 0x000000FF << 24);
}

inline dword mtob_d(dword in)
{
    return (in & 0xFF000000 >> 24) +
           (in & 0x00FF0000 >> 8)  +
           (in & 0x0000FF00 << 8)  +
           (in & 0x000000FF << 24);
}

inline float32 ltom_f(char* in)
{
    float32 out;
    char* fp = (char*)&out;

    fp[0] = in[0];
    fp[1] = in[1];
    fp[2] = in[2];
    fp[3] = in[3];

    return out;
}

inline void mtol_f(char* out, float32 in)
{
    char* fp = (char*)&in;

    out[0] = fp[0];
    out[1] = fp[1];
    out[2] = fp[2];
    out[3] = fp[3];

}


#elif SPHERE_BYTEORDER == SPHERE_BIG_ENDIAN

inline word ltom_w(word in)
{
  return ((in >>8) | (in << 8));
}

inline word mtol_w(word in)
{
  return ((in >>8) | (in << 8));
}

inline word btom_w(word in)
{
    return in;
}

inline word mtob_w(word in)
{
    return in;
}

inline dword ltom_d(dword in)
{
    return ((in >> 24) |
            ((in & 0x00FF0000) >> 8)  |
            ((in & 0x0000FF00) << 8)  |
            ((in & 0x000000FF) << 24));
}

inline dword mtol_d(dword in)
{
    return ((in >> 24) |
            ((in & 0x00FF0000) >> 8)  |
            ((in & 0x0000FF00) << 8)  |
            ((in & 0x000000FF) << 24));
}

inline dword btom_d(dword in)
{
    return in;
}

inline dword mtob_d(dword in)
{
    return in;
}

inline float32 ltom_f(char* in)
{
    float32 out;
    char* fp = (char*)&out;

    fp[0] = in[3];
    fp[1] = in[2];
    fp[2] = in[1];
    fp[3] = in[0];

    return out;
}

inline void mtol_f(char* out, float32 in)
{
    char* fp = (char*)&in;

    out[0] = fp[3];
    out[1] = fp[2];
    out[2] = fp[1];
    out[3] = fp[0];

}

#else
#error unknown byteorder
#endif

#endif
