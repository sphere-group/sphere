#ifndef RGB_HPP
#define RGB_HPP


#include "types.h"


#define STRUCT_NAME RGB
#include "begin_packed_struct.h"
  byte red;
  byte green;
  byte blue;
#include "end_packed_struct.h"

#define STRUCT_NAME BGR
#include "begin_packed_struct.h"
  byte blue;
  byte green;
  byte red;
#include "end_packed_struct.h"

#define STRUCT_NAME RGBA
#include "begin_packed_struct.h"
  byte red;
  byte green;
  byte blue;
  byte alpha;
#include "end_packed_struct.h"

#define STRUCT_NAME BGRA
#include "begin_packed_struct.h"
  byte blue;
  byte green;
  byte red;
  byte alpha;
#include "end_packed_struct.h"


// premultiply alpha
template<typename destT>
void PremultiplyAlpha(destT& d, int alpha) {
  d.red   = d.red   * alpha / 256;
  d.green = d.green * alpha / 256;
  d.blue  = d.blue  * alpha / 256;
}


// blends two colors together
template<typename destT, typename srcT>
void Blend3(destT& d, srcT s, int alpha) {
  d.red   = (s.red   * alpha + d.red   * (256 - alpha)) / 256;
  d.green = (s.green * alpha + d.green * (256 - alpha)) / 256;
  d.blue  = (s.blue  * alpha + d.blue  * (256 - alpha)) / 256;
}


template<typename destT, typename srcT>
void Blend4(destT& d, srcT s, int alpha) {
  d.red   = (s.red   * alpha + d.red   * (256 - alpha)) / 256;
  d.green = (s.green * alpha + d.green * (256 - alpha)) / 256;
  d.blue  = (s.blue  * alpha + d.blue  * (256 - alpha)) / 256;
  d.alpha = (s.alpha * alpha + d.alpha * (256 - alpha)) / 256;
}


// blends two colors if source has premultiplied alpha
template<typename destT, typename srcT>
void BlendPA(destT& d, srcT s, int alpha) {
  d.red   = s.red   + d.red   * (256 - alpha) / 256;
  d.green = s.green + d.green * (256 - alpha) / 256;
  d.blue  = s.blue  + d.blue  * (256 - alpha) / 256;
}


// creates red-green-blue color
template<typename colorT>
colorT CreateColor3(byte red, byte green, byte blue) {
  colorT c;
  c.red   = red;
  c.green = green;
  c.blue  = blue;
  return c;
}


// creates red-green-blue-alpha color
template<typename colorT>
colorT CreateColor4(byte red, byte green, byte blue, byte alpha) {
  colorT c;
  c.red   = red;
  c.green = green;
  c.blue  = blue;
  c.alpha = alpha;
  return c;
}

inline RGBA CreateRGBA(byte red, byte green, byte blue, byte alpha) {
  return CreateColor4<RGBA>(red, green, blue, alpha);
}

inline BGRA CreateBGRA(byte red, byte green, byte blue, byte alpha) {
  return CreateColor4<BGRA>(red, green, blue, alpha);
}

inline RGB CreateRGB(byte red, byte green, byte blue) {
  return CreateColor3<RGB>(red, green, blue);
}


#endif
