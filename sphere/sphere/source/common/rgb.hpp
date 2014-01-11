#ifndef RGB_HPP
#define RGB_HPP

#include "types.h"
#include "endian.hpp"


#define STRUCT_NAME RGB
#define STRUCT_BODY   \
    byte red;         \
    byte green;       \
    byte blue;
#include "packed_struct.h"

#define STRUCT_NAME RGBA
#define STRUCT_BODY   \
    byte red;         \
    byte green;       \
    byte blue;        \
    byte alpha;
#include "packed_struct.h"

#define STRUCT_NAME BGR
#if SPHERE_BYTEORDER == SPHERE_LITTLE_ENDIAN
#define STRUCT_BODY   \
    byte blue;        \
    byte green;       \
    byte red;
#else
#define STRUCT_BODY   \
    byte red;         \
    byte green;       \
    byte blue;
#endif
#include "packed_struct.h"

#define STRUCT_NAME BGRA
#if SPHERE_BYTEORDER == SPHERE_LITTLE_ENDIAN
#define STRUCT_BODY   \
    byte blue;        \
    byte green;       \
    byte red;         \
    byte alpha;
#else
#define STRUCT_BODY   \
    byte alpha;       \
    byte red;         \
    byte green;       \
    byte blue;
#endif
#include "packed_struct.h"

inline bool operator==(const RGB& c1, const RGB& c2)
{
    return (c1.red   == c2.red &&
            c1.green == c2.green &&
            c1.blue  == c2.blue);
}

inline bool operator!=(const RGB& c1, const RGB& c2)
{
    return !(c1 == c2);
}

inline bool operator==(const BGR& c1, const BGR& c2)
{
    return (c1.red   == c2.red &&
            c1.green == c2.green &&
            c1.blue  == c2.blue);
}

inline bool operator!=(const BGR& c1, const BGR& c2)
{
    return !(c1 == c2);
}

inline bool operator==(const RGBA& c1, const RGBA& c2)
{
    return (c1.red   == c2.red &&
            c1.green == c2.green &&
            c1.blue  == c2.blue &&
            c1.alpha == c2.alpha);
}

inline bool operator!=(const RGBA& c1, const RGBA& c2)
{
    return !(c1 == c2);
}

inline bool operator==(const BGRA& c1, const BGRA& c2)
{
    return (c1.red   == c2.red &&
            c1.green == c2.green &&
            c1.blue  == c2.blue &&
            c1.alpha == c2.alpha);
}

inline bool operator!=(const BGRA& c1, const BGRA& c2)
{
    return !(c1 == c2);
}

// premultiply alpha
template<typename destT>
void PremultiplyAlpha(destT& d, int alpha)
{
    d.red   = d.red   * alpha / 255;
    d.green = d.green * alpha / 255;
    d.blue  = d.blue  * alpha / 255;
}

// blends two colors together
template<typename destT, typename srcT>
void Blend3(destT& d, srcT s, int alpha)
{
    d.red   = (s.red   * alpha + d.red   * (255 - alpha)) / 255;
    d.green = (s.green * alpha + d.green * (255 - alpha)) / 255;
    d.blue  = (s.blue  * alpha + d.blue  * (255 - alpha)) / 255;
}

template<typename destT, typename srcT>
void Blend4(destT& d, srcT s, int alpha)
{
    d.red   = (s.red   * alpha + d.red   * (255 - alpha)) / 255;
    d.green = (s.green * alpha + d.green * (255 - alpha)) / 255;
    d.blue  = (s.blue  * alpha + d.blue  * (255 - alpha)) / 255;
    d.alpha = (s.alpha * alpha + d.alpha * (255 - alpha)) / 255;
}

// blends two colors if source has premultiplied alpha
template<typename destT, typename srcT>
void BlendPA(destT& d, srcT s, int alpha)
{
    d.red   = s.red   + d.red   * (255 - alpha) / 255;
    d.green = s.green + d.green * (255 - alpha) / 255;
    d.blue  = s.blue  + d.blue  * (255 - alpha) / 255;
}

// creates red-green-blue color
template<typename colorT>
colorT CreateColor3(byte red, byte green, byte blue)
{
    colorT c;
    c.red   = red;
    c.green = green;
    c.blue  = blue;
    return c;
}

// creates red-green-blue-alpha color
template<typename colorT>
colorT CreateColor4(byte red, byte green, byte blue, byte alpha)
{
    colorT c;
    c.red   = red;
    c.green = green;
    c.blue  = blue;
    c.alpha = alpha;
    return c;
}

inline RGBA CreateRGBA(byte red, byte green, byte blue, byte alpha)
{
    return CreateColor4<RGBA>(red, green, blue, alpha);
}

inline BGRA CreateBGRA(byte red, byte green, byte blue, byte alpha)
{
    return CreateColor4<BGRA>(red, green, blue, alpha);
}

inline RGB CreateRGB(byte red, byte green, byte blue)
{
    return CreateColor3<RGB>(red, green, blue);
}

#endif
