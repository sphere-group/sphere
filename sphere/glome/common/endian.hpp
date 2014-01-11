// the functions are named as following:
//   <initial format> to <result format> _ <data size>
// where format can be one of:
//   m = machine
//   l = little-endian
//   b = big-endian
// and data size can be:
//   w = word
//   d = dword


#ifndef ENDIAN_HPP
#define ENDIAN_HPP


#include "types.h"


#define LITTLE_ENDIAN 0
#define BIG_ENDIAN    1

#define ENDIANNESS LITTLE_ENDIAN
//#define ENDIANNESS BIG_ENDIAN


#if ENDIANNESS == LITTLE_ENDIAN


  inline word ltom_w(word in) {
    return in;
  }
  inline word mtol_w(word in) {
    return in;
  }
  inline word btom_w(word in) {
    return (in & 0xFF00 >> 8) + (in & 0x00FF << 8);
  }
  inline word mtob_w(word in) {
    return (in & 0xFF00 >> 8) + (in & 0x00FF << 8);
  }

  inline dword ltom_d(dword in) {
    return in;
  }
  inline dword mtol_d(dword in) {
    return in;
  }
  inline dword btom_d(dword in) {
    return (in & 0xFF000000 >> 24) +
           (in & 0x00FF0000 >> 8)  +
           (in & 0x0000FF00 << 8)  +
           (in & 0x000000FF << 24);
  }
  inline dword mtob_d(dword in) {
    return (in & 0xFF000000 >> 24) +
           (in & 0x00FF0000 >> 8)  +
           (in & 0x0000FF00 << 8)  +
           (in & 0x000000FF << 24);
  }


#elif ENDIANNESS == BIG_ENDIAN


  inline word ltom_w(word in) {
    return (in & 0xFF00 >> 8) + (in & 0x00FF << 8);
  }
  inline word mtol_w(word in) {
    return (in & 0xFF00 >> 8) + (in & 0x00FF << 8);
  }
  inline word btom_w(word in) {
    return in;
  }
  inline word mtob_w(word in) {
    return in;
  }

  inline dword ltom_d(dword in) {
    return (in & 0xFF000000 >> 24) +
           (in & 0x00FF0000 >> 8)  +
           (in & 0x0000FF00 << 8)  +
           (in & 0x000000FF << 24);
  }
  inline dword mtol_d(dword in) {
    return (in & 0xFF000000 >> 24) +
           (in & 0x00FF0000 >> 8)  +
           (in & 0x0000FF00 << 8)  +
           (in & 0x000000FF << 24);
  }
  inline dword btom_d(dword in) {
    return in;
  }
  inline dword mtob_d(dword in) {
    return in;
  }


#else

  #error unknown endianness

#endif


#endif
