/*--------------------------------------------------------------------------*
 * hq2x filter                                                              *
 * Copyright (c) 2003 Maxim Stepin aka MaxSt ( maxst@hiend3d.com )          *
 *--------------------------------------------------------------------------*
/* Rewritten by Anatoli Steinmark on 9. February 2008                       */
/* for use in the Sphere RPG Engine.                                        */
/*--------------------------------------------------------------------------*/


#include "hq2x.h"

#define        R_SHIFT_565  8
#define        G_SHIFT_565  3
#define         R_MASK_565  0x00F800
#define         G_MASK_565  0x0007E0
#define         B_MASK_565  0x00001F
#define        RB_MASK_565  0x00F81F

#define      D1_G_MASK_565  0x000FC0
#define      D2_G_MASK_565  0x001F80
#define      D3_G_MASK_565  0x003F00
#define      D4_G_MASK_565  0x007E00
#define     D1_RB_MASK_565  0x01F03E
#define     D2_RB_MASK_565  0x03E07C
#define     D3_RB_MASK_565  0x07C0F8
#define     D4_RB_MASK_565  0x0F81F0

#define        R_SHIFT_555  7
#define        G_SHIFT_555  2
#define         R_MASK_555  0x007C00
#define         G_MASK_555  0x0003E0
#define         B_MASK_555  0x00001F
#define        RB_MASK_555  0x007C1F

#define      D1_G_MASK_555  0x0007C0
#define      D2_G_MASK_555  0x000F80
#define      D3_G_MASK_555  0x001F00
#define      D4_G_MASK_555  0x003E00
#define     D1_RB_MASK_555  0x00F83E
#define     D2_RB_MASK_555  0x01F07C
#define     D3_RB_MASK_555  0x03E0F8
#define     D4_RB_MASK_555  0x07C1F0

dword     R_SHIFT;
dword     G_SHIFT;
dword      R_MASK;
dword      G_MASK;
dword      B_MASK;
dword     RB_MASK;
dword   D1_G_MASK;
dword   D2_G_MASK;
dword   D3_G_MASK;
dword   D4_G_MASK;
dword  D1_RB_MASK;
dword  D2_RB_MASK;
dword  D3_RB_MASK;
dword  D4_RB_MASK;

inline void Interp1(word* pc, word c1, word c2)
{
  *pc = (word)(((( (c1 &  G_MASK)*3 + (c2 &  G_MASK) ) &  D2_G_MASK)  +
                (( (c1 & RB_MASK)*3 + (c2 & RB_MASK) ) & D2_RB_MASK)) >> 2);
}

inline void Interp2(word* pc, word c1, word c2, word c3)
{
  *pc = (word)(((( (c1 &  G_MASK)*2 + (c2 &  G_MASK) + (c3 &  G_MASK) ) &  D2_G_MASK)  +
                (( (c1 & RB_MASK)*2 + (c2 & RB_MASK) + (c3 & RB_MASK) ) & D2_RB_MASK)) >> 2);
}

inline void Interp5(word* pc, word c1, word c2)
{
  *pc = (word)(((( (c1 &  G_MASK) + (c2 &  G_MASK) ) &  D1_G_MASK)  +
                (( (c1 & RB_MASK) + (c2 & RB_MASK) ) & D1_RB_MASK)) >> 1);
}

inline void Interp6(word* pc, word c1, word c2, word c3)
{
  *pc = (word)(((((c1 &  G_MASK)*5 + (c2 &  G_MASK)*2 + (c3 &  G_MASK) ) &  D3_G_MASK)  +
                (((c1 & RB_MASK)*5 + (c2 & RB_MASK)*2 + (c3 & RB_MASK) ) & D3_RB_MASK)) >> 3);
}

inline void Interp7(word* pc, word c1, word c2, word c3)
{
  *pc = (word)(((( (c1 &  G_MASK)*6 + (c2 &  G_MASK) + (c3 &  G_MASK) ) &  D3_G_MASK)  +
                (( (c1 & RB_MASK)*6 + (c2 & RB_MASK) + (c3 & RB_MASK) ) & D3_RB_MASK)) >> 3);
}

inline void Interp9(word* pc, word c1, word c2, word c3)
{
  *pc = (word)(((( (c1 &  G_MASK)*2 + ((c2 &  G_MASK) + (c3 &  G_MASK))*3 ) &  D3_G_MASK)  +
                (( (c1 & RB_MASK)*2 + ((c2 & RB_MASK) + (c3 & RB_MASK))*3 ) & D3_RB_MASK)) >> 3);
}

inline void Interp10(word* pc, word c1, word c2, word c3)
{
  *pc = (word)(((( (c1 &  G_MASK)*14 + (c2 &  G_MASK) + (c3 &  G_MASK) ) &  D4_G_MASK)  +
                (( (c1 & RB_MASK)*14 + (c2 & RB_MASK) + (c3 & RB_MASK) ) & D4_RB_MASK)) >> 4);
}

#define PIXEL00_0     dstA[0] = src1[0];
#define PIXEL00_10    Interp1( dstA, src1[0], src0[-1]);
#define PIXEL00_11    Interp1( dstA, src1[0], src1[-1]);
#define PIXEL00_12    Interp1( dstA, src1[0], src0[0]);
#define PIXEL00_20    Interp2( dstA, src1[0], src1[-1], src0[0]);
#define PIXEL00_21    Interp2( dstA, src1[0], src0[-1], src0[0]);
#define PIXEL00_22    Interp2( dstA, src1[0], src0[-1], src1[-1]);
#define PIXEL00_60    Interp6( dstA, src1[0], src0[0],  src1[-1]);
#define PIXEL00_61    Interp6( dstA, src1[0], src1[-1], src0[0]);
#define PIXEL00_70    Interp7( dstA, src1[0], src1[-1], src0[0]);
#define PIXEL00_90    Interp9( dstA, src1[0], src1[-1], src0[0]);
#define PIXEL00_100   Interp10(dstA, src1[0], src1[-1], src0[0]);
#define PIXEL01_0     dstA[1] = src1[0];
#define PIXEL01_10    Interp1( dstA+1, src1[0], src0[1]);
#define PIXEL01_11    Interp1( dstA+1, src1[0], src0[0]);
#define PIXEL01_12    Interp1( dstA+1, src1[0], src1[1]);
#define PIXEL01_20    Interp2( dstA+1, src1[0], src0[0], src1[1]);
#define PIXEL01_21    Interp2( dstA+1, src1[0], src0[1], src1[1]);
#define PIXEL01_22    Interp2( dstA+1, src1[0], src0[1], src0[0]);
#define PIXEL01_60    Interp6( dstA+1, src1[0], src1[1], src0[0]);
#define PIXEL01_61    Interp6( dstA+1, src1[0], src0[0], src1[1]);
#define PIXEL01_70    Interp7( dstA+1, src1[0], src0[0], src1[1]);
#define PIXEL01_90    Interp9( dstA+1, src1[0], src0[0], src1[1]);
#define PIXEL01_100   Interp10(dstA+1, src1[0], src0[0], src1[1]);
#define PIXEL10_0     dstB[0] = src1[0];
#define PIXEL10_10    Interp1( dstB, src1[0], src2[-1]);
#define PIXEL10_11    Interp1( dstB, src1[0], src2[0]);
#define PIXEL10_12    Interp1( dstB, src1[0], src1[-1]);
#define PIXEL10_20    Interp2( dstB, src1[0], src2[0],  src1[-1]);
#define PIXEL10_21    Interp2( dstB, src1[0], src2[-1], src1[-1]);
#define PIXEL10_22    Interp2( dstB, src1[0], src2[-1], src2[0]);
#define PIXEL10_60    Interp6( dstB, src1[0], src1[-1], src2[0]);
#define PIXEL10_61    Interp6( dstB, src1[0], src2[0],  src1[-1]);
#define PIXEL10_70    Interp7( dstB, src1[0], src2[0],  src1[-1]);
#define PIXEL10_90    Interp9( dstB, src1[0], src2[0],  src1[-1]);
#define PIXEL10_100   Interp10(dstB, src1[0], src2[0],  src1[-1]);
#define PIXEL11_0     dstB[1] = src1[0];
#define PIXEL11_10    Interp1( dstB+1, src1[0], src2[1]);
#define PIXEL11_11    Interp1( dstB+1, src1[0], src1[1]);
#define PIXEL11_12    Interp1( dstB+1, src1[0], src2[0]);
#define PIXEL11_20    Interp2( dstB+1, src1[0], src1[1], src2[0]);
#define PIXEL11_21    Interp2( dstB+1, src1[0], src2[1], src2[0]);
#define PIXEL11_22    Interp2( dstB+1, src1[0], src2[1], src1[1]);
#define PIXEL11_60    Interp6( dstB+1, src1[0], src2[0], src1[1]);
#define PIXEL11_61    Interp6( dstB+1, src1[0], src1[1], src2[0]);
#define PIXEL11_70    Interp7( dstB+1, src1[0], src1[1], src2[0]);
#define PIXEL11_90    Interp9( dstB+1, src1[0], src1[1], src2[0]);
#define PIXEL11_100   Interp10(dstB+1, src1[0], src1[1], src2[0]);

inline bool Diff(word a, word b)
{
  int R = abs((a & R_MASK) - (b & R_MASK)) >> R_SHIFT;
  int G = abs((a & G_MASK) - (b & G_MASK)) >> G_SHIFT;
  int B = abs((a & B_MASK) - (b & B_MASK)) << 3;

  return ( ( (( R +   G + B) >> 2) > 0x20 ) ||
           ( (( R -   B)     >> 2) > 0x10 ) ||
           ( ((-R + 2*G - B) >> 3) > 0x10 ) );
}

void hq2x(word* dst, int dst_pitch, word* src, int src_width, int src_height, int bpp)
{
    /*
    +--+--+--+
    |A |B |C |
    +--+--+--+
    |D |E |F |
    +--+--+--+
    |G |H |I |
    +--+--+--+

    +--+--+
    |E0|E1|
    +--+--+
    |E2|E3|
    +--+--+

    B: src0[0]
    E: src1[0]
    H: src2[0]

    E0: dstA[0]
    E2: dstB[0]
    */

    if (bpp == 16)
    {
        R_SHIFT     =    R_SHIFT_565;
        G_SHIFT     =    G_SHIFT_565;
        R_MASK      =     R_MASK_565;
        G_MASK      =     G_MASK_565;
        B_MASK      =     B_MASK_565;
        RB_MASK     =    RB_MASK_565;
        D1_G_MASK   =  D1_G_MASK_565;
        D2_G_MASK   =  D2_G_MASK_565;
        D3_G_MASK   =  D3_G_MASK_565;
        D4_G_MASK   =  D4_G_MASK_565;
        D1_RB_MASK  = D1_RB_MASK_565;
        D2_RB_MASK  = D2_RB_MASK_565;
        D3_RB_MASK  = D3_RB_MASK_565;
        D4_RB_MASK  = D4_RB_MASK_565;
    }
    else
    {
        R_SHIFT     =    R_SHIFT_555;
        G_SHIFT     =    G_SHIFT_555;
        R_MASK      =     R_MASK_555;
        G_MASK      =     G_MASK_555;
        B_MASK      =     B_MASK_555;
        RB_MASK     =    RB_MASK_555;
        D1_G_MASK   =  D1_G_MASK_555;
        D2_G_MASK   =  D2_G_MASK_555;
        D3_G_MASK   =  D3_G_MASK_555;
        D4_G_MASK   =  D4_G_MASK_555;
        D1_RB_MASK  = D1_RB_MASK_555;
        D2_RB_MASK  = D2_RB_MASK_555;
        D3_RB_MASK  = D3_RB_MASK_555;
        D4_RB_MASK  = D4_RB_MASK_555;
    }

    int dst_pitch2 = dst_pitch * 2;

    word* dst0 = dst + 2 + dst_pitch * 2;
    word* dst1 = dst + 2 + dst_pitch * 3;
    word* src0 = src + 1;
    word* src1 = src + 1 + src_width;
    word* src2 = src + 1 + src_width * 2;


    int iy = src_height - 2;
    int ix;
    int pattern;


  while (iy--)
  {
    ix = src_width - 2;
    word* dstA = dst0;
    word* dstB = dst1;

    while (ix--)
    {
        pattern = 0;

        if (src0[-1] != src1[0]) if (Diff(src0[-1], src1[0])) pattern |= 0x01;
        if (src0[0]  != src1[0]) if (Diff(src0[0], src1[0]))  pattern |= 0x02;
        if (src0[1]  != src1[0]) if (Diff(src0[1], src1[0]))  pattern |= 0x04;
        if (src1[-1] != src1[0]) if (Diff(src1[-1], src1[0])) pattern |= 0x08;
        if (src1[1]  != src1[0]) if (Diff(src1[1], src1[0]))  pattern |= 0x10;
        if (src2[-1] != src1[0]) if (Diff(src2[-1], src1[0])) pattern |= 0x20;
        if (src2[0]  != src1[0]) if (Diff(src2[0], src1[0]))  pattern |= 0x40;
        if (src2[1]  != src1[0]) if (Diff(src2[1], src1[0]))  pattern |= 0x80;

      switch (pattern)
      {
        case 0:
        case 1:
        case 4:
        case 32:
        case 128:
        case 5:
        case 132:
        case 160:
        case 33:
        case 129:
        case 36:
        case 133:
        case 164:
        case 161:
        case 37:
        case 165:
        {
          PIXEL00_20
          PIXEL01_20
          PIXEL10_20
          PIXEL11_20
          break;
        }
        case 2:
        case 34:
        case 130:
        case 162:
        {
          PIXEL00_22
          PIXEL01_21
          PIXEL10_20
          PIXEL11_20
          break;
        }
        case 16:
        case 17:
        case 48:
        case 49:
        {
          PIXEL00_20
          PIXEL01_22
          PIXEL10_20
          PIXEL11_21
          break;
        }
        case 64:
        case 65:
        case 68:
        case 69:
        {
          PIXEL00_20
          PIXEL01_20
          PIXEL10_21
          PIXEL11_22
          break;
        }
        case 8:
        case 12:
        case 136:
        case 140:
        {
          PIXEL00_21
          PIXEL01_20
          PIXEL10_22
          PIXEL11_20
          break;
        }
        case 3:
        case 35:
        case 131:
        case 163:
        {
          PIXEL00_11
          PIXEL01_21
          PIXEL10_20
          PIXEL11_20
          break;
        }
        case 6:
        case 38:
        case 134:
        case 166:
        {
          PIXEL00_22
          PIXEL01_12
          PIXEL10_20
          PIXEL11_20
          break;
        }
        case 20:
        case 21:
        case 52:
        case 53:
        {
          PIXEL00_20
          PIXEL01_11
          PIXEL10_20
          PIXEL11_21
          break;
        }
        case 144:
        case 145:
        case 176:
        case 177:
        {
          PIXEL00_20
          PIXEL01_22
          PIXEL10_20
          PIXEL11_12
          break;
        }
        case 192:
        case 193:
        case 196:
        case 197:
        {
          PIXEL00_20
          PIXEL01_20
          PIXEL10_21
          PIXEL11_11
          break;
        }
        case 96:
        case 97:
        case 100:
        case 101:
        {
          PIXEL00_20
          PIXEL01_20
          PIXEL10_12
          PIXEL11_22
          break;
        }
        case 40:
        case 44:
        case 168:
        case 172:
        {
          PIXEL00_21
          PIXEL01_20
          PIXEL10_11
          PIXEL11_20
          break;
        }
        case 9:
        case 13:
        case 137:
        case 141:
        {
          PIXEL00_12
          PIXEL01_20
          PIXEL10_22
          PIXEL11_20
          break;
        }
        case 18:
        case 50:
        {
          PIXEL00_22
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_10
          }
          else
          {
            PIXEL01_20
          }
          PIXEL10_20
          PIXEL11_21
          break;
        }
        case 80:
        case 81:
        {
          PIXEL00_20
          PIXEL01_22
          PIXEL10_21
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_10
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 72:
        case 76:
        {
          PIXEL00_21
          PIXEL01_20
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_10
          }
          else
          {
            PIXEL10_20
          }
          PIXEL11_22
          break;
        }
        case 10:
        case 138:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_10
          }
          else
          {
            PIXEL00_20
          }
          PIXEL01_21
          PIXEL10_22
          PIXEL11_20
          break;
        }
        case 66:
        {
          PIXEL00_22
          PIXEL01_21
          PIXEL10_21
          PIXEL11_22
          break;
        }
        case 24:
        {
          PIXEL00_21
          PIXEL01_22
          PIXEL10_22
          PIXEL11_21
          break;
        }
        case 7:
        case 39:
        case 135:
        {
          PIXEL00_11
          PIXEL01_12
          PIXEL10_20
          PIXEL11_20
          break;
        }
        case 148:
        case 149:
        case 180:
        {
          PIXEL00_20
          PIXEL01_11
          PIXEL10_20
          PIXEL11_12
          break;
        }
        case 224:
        case 228:
        case 225:
        {
          PIXEL00_20
          PIXEL01_20
          PIXEL10_12
          PIXEL11_11
          break;
        }
        case 41:
        case 169:
        case 45:
        {
          PIXEL00_12
          PIXEL01_20
          PIXEL10_11
          PIXEL11_20
          break;
        }
        case 22:
        case 54:
        {
          PIXEL00_22
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          PIXEL10_20
          PIXEL11_21
          break;
        }
        case 208:
        case 209:
        {
          PIXEL00_20
          PIXEL01_22
          PIXEL10_21
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 104:
        case 108:
        {
          PIXEL00_21
          PIXEL01_20
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          PIXEL11_22
          break;
        }
        case 11:
        case 139:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          PIXEL01_21
          PIXEL10_22
          PIXEL11_20
          break;
        }
        case 19:
        case 51:
        {
          if (Diff(src0[0], src1[1]))
          {
            PIXEL00_11
            PIXEL01_10
          }
          else
          {
            PIXEL00_60
            PIXEL01_90
          }
          PIXEL10_20
          PIXEL11_21
          break;
        }
        case 146:
        case 178:
        {
          PIXEL00_22
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_10
            PIXEL11_12
          }
          else
          {
            PIXEL01_90
            PIXEL11_61
          }
          PIXEL10_20
          break;
        }
        case 84:
        case 85:
        {
          PIXEL00_20
          if (Diff(src1[1], src2[0]))
          {
            PIXEL01_11
            PIXEL11_10
          }
          else
          {
            PIXEL01_60
            PIXEL11_90
          }
          PIXEL10_21
          break;
        }
        case 112:
        case 113:
        {
          PIXEL00_20
          PIXEL01_22
          if (Diff(src1[1], src2[0]))
          {
            PIXEL10_12
            PIXEL11_10
          }
          else
          {
            PIXEL10_61
            PIXEL11_90
          }
          break;
        }
        case 200:
        case 204:
        {
          PIXEL00_21
          PIXEL01_20
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_10
            PIXEL11_11
          }
          else
          {
            PIXEL10_90
            PIXEL11_60
          }
          break;
        }
        case 73:
        case 77:
        {
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL00_12
            PIXEL10_10
          }
          else
          {
            PIXEL00_61
            PIXEL10_90
          }
          PIXEL01_20
          PIXEL11_22
          break;
        }
        case 42:
        case 170:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_10
            PIXEL10_11
          }
          else
          {
            PIXEL00_90
            PIXEL10_60
          }
          PIXEL01_21
          PIXEL11_20
          break;
        }
        case 14:
        case 142:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_10
            PIXEL01_12
          }
          else
          {
            PIXEL00_90
            PIXEL01_61
          }
          PIXEL10_22
          PIXEL11_20
          break;
        }
        case 67:
        {
          PIXEL00_11
          PIXEL01_21
          PIXEL10_21
          PIXEL11_22
          break;
        }
        case 70:
        {
          PIXEL00_22
          PIXEL01_12
          PIXEL10_21
          PIXEL11_22
          break;
        }
        case 28:
        {
          PIXEL00_21
          PIXEL01_11
          PIXEL10_22
          PIXEL11_21
          break;
        }
        case 152:
        {
          PIXEL00_21
          PIXEL01_22
          PIXEL10_22
          PIXEL11_12
          break;
        }
        case 194:
        {
          PIXEL00_22
          PIXEL01_21
          PIXEL10_21
          PIXEL11_11
          break;
        }
        case 98:
        {
          PIXEL00_22
          PIXEL01_21
          PIXEL10_12
          PIXEL11_22
          break;
        }
        case 56:
        {
          PIXEL00_21
          PIXEL01_22
          PIXEL10_11
          PIXEL11_21
          break;
        }
        case 25:
        {
          PIXEL00_12
          PIXEL01_22
          PIXEL10_22
          PIXEL11_21
          break;
        }
        case 26:
        case 31:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          PIXEL10_22
          PIXEL11_21
          break;
        }
        case 82:
        case 214:
        {
          PIXEL00_22
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          PIXEL10_21
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 88:
        case 248:
        {
          PIXEL00_21
          PIXEL01_22
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 74:
        case 107:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          PIXEL01_21
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          PIXEL11_22
          break;
        }
        case 27:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          PIXEL01_10
          PIXEL10_22
          PIXEL11_21
          break;
        }
        case 86:
        {
          PIXEL00_22
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          PIXEL10_21
          PIXEL11_10
          break;
        }
        case 216:
        {
          PIXEL00_21
          PIXEL01_22
          PIXEL10_10
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 106:
        {
          PIXEL00_10
          PIXEL01_21
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          PIXEL11_22
          break;
        }
        case 30:
        {
          PIXEL00_10
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          PIXEL10_22
          PIXEL11_21
          break;
        }
        case 210:
        {
          PIXEL00_22
          PIXEL01_10
          PIXEL10_21
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 120:
        {
          PIXEL00_21
          PIXEL01_22
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          PIXEL11_10
          break;
        }
        case 75:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          PIXEL01_21
          PIXEL10_10
          PIXEL11_22
          break;
        }
        case 29:
        {
          PIXEL00_12
          PIXEL01_11
          PIXEL10_22
          PIXEL11_21
          break;
        }
        case 198:
        {
          PIXEL00_22
          PIXEL01_12
          PIXEL10_21
          PIXEL11_11
          break;
        }
        case 184:
        {
          PIXEL00_21
          PIXEL01_22
          PIXEL10_11
          PIXEL11_12
          break;
        }
        case 99:
        {
          PIXEL00_11
          PIXEL01_21
          PIXEL10_12
          PIXEL11_22
          break;
        }
        case 57:
        {
          PIXEL00_12
          PIXEL01_22
          PIXEL10_11
          PIXEL11_21
          break;
        }
        case 71:
        {
          PIXEL00_11
          PIXEL01_12
          PIXEL10_21
          PIXEL11_22
          break;
        }
        case 156:
        {
          PIXEL00_21
          PIXEL01_11
          PIXEL10_22
          PIXEL11_12
          break;
        }
        case 226:
        {
          PIXEL00_22
          PIXEL01_21
          PIXEL10_12
          PIXEL11_11
          break;
        }
        case 60:
        {
          PIXEL00_21
          PIXEL01_11
          PIXEL10_11
          PIXEL11_21
          break;
        }
        case 195:
        {
          PIXEL00_11
          PIXEL01_21
          PIXEL10_21
          PIXEL11_11
          break;
        }
        case 102:
        {
          PIXEL00_22
          PIXEL01_12
          PIXEL10_12
          PIXEL11_22
          break;
        }
        case 153:
        {
          PIXEL00_12
          PIXEL01_22
          PIXEL10_22
          PIXEL11_12
          break;
        }
        case 58:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_10
          }
          else
          {
            PIXEL00_70
          }
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_10
          }
          else
          {
            PIXEL01_70
          }
          PIXEL10_11
          PIXEL11_21
          break;
        }
        case 83:
        {
          PIXEL00_11
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_10
          }
          else
          {
            PIXEL01_70
          }
          PIXEL10_21
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_10
          }
          else
          {
            PIXEL11_70
          }
          break;
        }
        case 92:
        {
          PIXEL00_21
          PIXEL01_11
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_10
          }
          else
          {
            PIXEL10_70
          }
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_10
          }
          else
          {
            PIXEL11_70
          }
          break;
        }
        case 202:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_10
          }
          else
          {
            PIXEL00_70
          }
          PIXEL01_21
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_10
          }
          else
          {
            PIXEL10_70
          }
          PIXEL11_11
          break;
        }
        case 78:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_10
          }
          else
          {
            PIXEL00_70
          }
          PIXEL01_12
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_10
          }
          else
          {
            PIXEL10_70
          }
          PIXEL11_22
          break;
        }
        case 154:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_10
          }
          else
          {
            PIXEL00_70
          }
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_10
          }
          else
          {
            PIXEL01_70
          }
          PIXEL10_22
          PIXEL11_12
          break;
        }
        case 114:
        {
          PIXEL00_22
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_10
          }
          else
          {
            PIXEL01_70
          }
          PIXEL10_12
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_10
          }
          else
          {
            PIXEL11_70
          }
          break;
        }
        case 89:
        {
          PIXEL00_12
          PIXEL01_22
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_10
          }
          else
          {
            PIXEL10_70
          }
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_10
          }
          else
          {
            PIXEL11_70
          }
          break;
        }
        case 90:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_10
          }
          else
          {
            PIXEL00_70
          }
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_10
          }
          else
          {
            PIXEL01_70
          }
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_10
          }
          else
          {
            PIXEL10_70
          }
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_10
          }
          else
          {
            PIXEL11_70
          }
          break;
        }
        case 55:
        case 23:
        {
          if (Diff(src0[0], src1[1]))
          {
            PIXEL00_11
            PIXEL01_0
          }
          else
          {
            PIXEL00_60
            PIXEL01_90
          }
          PIXEL10_20
          PIXEL11_21
          break;
        }
        case 182:
        case 150:
        {
          PIXEL00_22
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_0
            PIXEL11_12
          }
          else
          {
            PIXEL01_90
            PIXEL11_61
          }
          PIXEL10_20
          break;
        }
        case 213:
        case 212:
        {
          PIXEL00_20
          if (Diff(src1[1], src2[0]))
          {
            PIXEL01_11
            PIXEL11_0
          }
          else
          {
            PIXEL01_60
            PIXEL11_90
          }
          PIXEL10_21
          break;
        }
        case 241:
        case 240:
        {
          PIXEL00_20
          PIXEL01_22
          if (Diff(src1[1], src2[0]))
          {
            PIXEL10_12
            PIXEL11_0
          }
          else
          {
            PIXEL10_61
            PIXEL11_90
          }
          break;
        }
        case 236:
        case 232:
        {
          PIXEL00_21
          PIXEL01_20
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_0
            PIXEL11_11
          }
          else
          {
            PIXEL10_90
            PIXEL11_60
          }
          break;
        }
        case 109:
        case 105:
        {
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL00_12
            PIXEL10_0
          }
          else
          {
            PIXEL00_61
            PIXEL10_90
          }
          PIXEL01_20
          PIXEL11_22
          break;
        }
        case 171:
        case 43:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_0
            PIXEL10_11
          }
          else
          {
            PIXEL00_90
            PIXEL10_60
          }
          PIXEL01_21
          PIXEL11_20
          break;
        }
        case 143:
        case 15:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_0
            PIXEL01_12
          }
          else
          {
            PIXEL00_90
            PIXEL01_61
          }
          PIXEL10_22
          PIXEL11_20
          break;
        }
        case 124:
        {
          PIXEL00_21
          PIXEL01_11
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          PIXEL11_10
          break;
        }
        case 203:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          PIXEL01_21
          PIXEL10_10
          PIXEL11_11
          break;
        }
        case 62:
        {
          PIXEL00_10
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          PIXEL10_11
          PIXEL11_21
          break;
        }
        case 211:
        {
          PIXEL00_11
          PIXEL01_10
          PIXEL10_21
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 118:
        {
          PIXEL00_22
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          PIXEL10_12
          PIXEL11_10
          break;
        }
        case 217:
        {
          PIXEL00_12
          PIXEL01_22
          PIXEL10_10
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 110:
        {
          PIXEL00_10
          PIXEL01_12
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          PIXEL11_22
          break;
        }
        case 155:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          PIXEL01_10
          PIXEL10_22
          PIXEL11_12
          break;
        }
        case 188:
        {
          PIXEL00_21
          PIXEL01_11
          PIXEL10_11
          PIXEL11_12
          break;
        }
        case 185:
        {
          PIXEL00_12
          PIXEL01_22
          PIXEL10_11
          PIXEL11_12
          break;
        }
        case 61:
        {
          PIXEL00_12
          PIXEL01_11
          PIXEL10_11
          PIXEL11_21
          break;
        }
        case 157:
        {
          PIXEL00_12
          PIXEL01_11
          PIXEL10_22
          PIXEL11_12
          break;
        }
        case 103:
        {
          PIXEL00_11
          PIXEL01_12
          PIXEL10_12
          PIXEL11_22
          break;
        }
        case 227:
        {
          PIXEL00_11
          PIXEL01_21
          PIXEL10_12
          PIXEL11_11
          break;
        }
        case 230:
        {
          PIXEL00_22
          PIXEL01_12
          PIXEL10_12
          PIXEL11_11
          break;
        }
        case 199:
        {
          PIXEL00_11
          PIXEL01_12
          PIXEL10_21
          PIXEL11_11
          break;
        }
        case 220:
        {
          PIXEL00_21
          PIXEL01_11
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_10
          }
          else
          {
            PIXEL10_70
          }
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 158:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_10
          }
          else
          {
            PIXEL00_70
          }
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          PIXEL10_22
          PIXEL11_12
          break;
        }
        case 234:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_10
          }
          else
          {
            PIXEL00_70
          }
          PIXEL01_21
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          PIXEL11_11
          break;
        }
        case 242:
        {
          PIXEL00_22
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_10
          }
          else
          {
            PIXEL01_70
          }
          PIXEL10_12
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 59:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_10
          }
          else
          {
            PIXEL01_70
          }
          PIXEL10_11
          PIXEL11_21
          break;
        }
        case 121:
        {
          PIXEL00_12
          PIXEL01_22
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_10
          }
          else
          {
            PIXEL11_70
          }
          break;
        }
        case 87:
        {
          PIXEL00_11
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          PIXEL10_21
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_10
          }
          else
          {
            PIXEL11_70
          }
          break;
        }
        case 79:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          PIXEL01_12
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_10
          }
          else
          {
            PIXEL10_70
          }
          PIXEL11_22
          break;
        }
        case 122:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_10
          }
          else
          {
            PIXEL00_70
          }
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_10
          }
          else
          {
            PIXEL01_70
          }
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_10
          }
          else
          {
            PIXEL11_70
          }
          break;
        }
        case 94:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_10
          }
          else
          {
            PIXEL00_70
          }
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_10
          }
          else
          {
            PIXEL10_70
          }
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_10
          }
          else
          {
            PIXEL11_70
          }
          break;
        }
        case 218:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_10
          }
          else
          {
            PIXEL00_70
          }
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_10
          }
          else
          {
            PIXEL01_70
          }
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_10
          }
          else
          {
            PIXEL10_70
          }
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 91:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_10
          }
          else
          {
            PIXEL01_70
          }
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_10
          }
          else
          {
            PIXEL10_70
          }
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_10
          }
          else
          {
            PIXEL11_70
          }
          break;
        }
        case 229:
        {
          PIXEL00_20
          PIXEL01_20
          PIXEL10_12
          PIXEL11_11
          break;
        }
        case 167:
        {
          PIXEL00_11
          PIXEL01_12
          PIXEL10_20
          PIXEL11_20
          break;
        }
        case 173:
        {
          PIXEL00_12
          PIXEL01_20
          PIXEL10_11
          PIXEL11_20
          break;
        }
        case 181:
        {
          PIXEL00_20
          PIXEL01_11
          PIXEL10_20
          PIXEL11_12
          break;
        }
        case 186:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_10
          }
          else
          {
            PIXEL00_70
          }
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_10
          }
          else
          {
            PIXEL01_70
          }
          PIXEL10_11
          PIXEL11_12
          break;
        }
        case 115:
        {
          PIXEL00_11
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_10
          }
          else
          {
            PIXEL01_70
          }
          PIXEL10_12
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_10
          }
          else
          {
            PIXEL11_70
          }
          break;
        }
        case 93:
        {
          PIXEL00_12
          PIXEL01_11
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_10
          }
          else
          {
            PIXEL10_70
          }
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_10
          }
          else
          {
            PIXEL11_70
          }
          break;
        }
        case 206:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_10
          }
          else
          {
            PIXEL00_70
          }
          PIXEL01_12
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_10
          }
          else
          {
            PIXEL10_70
          }
          PIXEL11_11
          break;
        }
        case 205:
        case 201:
        {
          PIXEL00_12
          PIXEL01_20
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_10
          }
          else
          {
            PIXEL10_70
          }
          PIXEL11_11
          break;
        }
        case 174:
        case 46:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_10
          }
          else
          {
            PIXEL00_70
          }
          PIXEL01_12
          PIXEL10_11
          PIXEL11_20
          break;
        }
        case 179:
        case 147:
        {
          PIXEL00_11
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_10
          }
          else
          {
            PIXEL01_70
          }
          PIXEL10_20
          PIXEL11_12
          break;
        }
        case 117:
        case 116:
        {
          PIXEL00_20
          PIXEL01_11
          PIXEL10_12
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_10
          }
          else
          {
            PIXEL11_70
          }
          break;
        }
        case 189:
        {
          PIXEL00_12
          PIXEL01_11
          PIXEL10_11
          PIXEL11_12
          break;
        }
        case 231:
        {
          PIXEL00_11
          PIXEL01_12
          PIXEL10_12
          PIXEL11_11
          break;
        }
        case 126:
        {
          PIXEL00_10
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          PIXEL11_10
          break;
        }
        case 219:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          PIXEL01_10
          PIXEL10_10
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 125:
        {
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL00_12
            PIXEL10_0
          }
          else
          {
            PIXEL00_61
            PIXEL10_90
          }
          PIXEL01_11
          PIXEL11_10
          break;
        }
        case 221:
        {
          PIXEL00_12
          if (Diff(src1[1], src2[0]))
          {
            PIXEL01_11
            PIXEL11_0
          }
          else
          {
            PIXEL01_60
            PIXEL11_90
          }
          PIXEL10_10
          break;
        }
        case 207:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_0
            PIXEL01_12
          }
          else
          {
            PIXEL00_90
            PIXEL01_61
          }
          PIXEL10_10
          PIXEL11_11
          break;
        }
        case 238:
        {
          PIXEL00_10
          PIXEL01_12
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_0
            PIXEL11_11
          }
          else
          {
            PIXEL10_90
            PIXEL11_60
          }
          break;
        }
        case 190:
        {
          PIXEL00_10
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_0
            PIXEL11_12
          }
          else
          {
            PIXEL01_90
            PIXEL11_61
          }
          PIXEL10_11
          break;
        }
        case 187:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_0
            PIXEL10_11
          }
          else
          {
            PIXEL00_90
            PIXEL10_60
          }
          PIXEL01_10
          PIXEL11_12
          break;
        }
        case 243:
        {
          PIXEL00_11
          PIXEL01_10
          if (Diff(src1[1], src2[0]))
          {
            PIXEL10_12
            PIXEL11_0
          }
          else
          {
            PIXEL10_61
            PIXEL11_90
          }
          break;
        }
        case 119:
        {
          if (Diff(src0[0], src1[1]))
          {
            PIXEL00_11
            PIXEL01_0
          }
          else
          {
            PIXEL00_60
            PIXEL01_90
          }
          PIXEL10_12
          PIXEL11_10
          break;
        }
        case 237:
        case 233:
        {
          PIXEL00_12
          PIXEL01_20
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_100
          }
          PIXEL11_11
          break;
        }
        case 175:
        case 47:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_100
          }
          PIXEL01_12
          PIXEL10_11
          PIXEL11_20
          break;
        }
        case 183:
        case 151:
        {
          PIXEL00_11
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_100
          }
          PIXEL10_20
          PIXEL11_12
          break;
        }
        case 245:
        case 244:
        {
          PIXEL00_20
          PIXEL01_11
          PIXEL10_12
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_100
          }
          break;
        }
        case 250:
        {
          PIXEL00_10
          PIXEL01_10
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 123:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          PIXEL01_10
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          PIXEL11_10
          break;
        }
        case 95:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          PIXEL10_10
          PIXEL11_10
          break;
        }
        case 222:
        {
          PIXEL00_10
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          PIXEL10_10
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 252:
        {
          PIXEL00_21
          PIXEL01_11
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_100
          }
          break;
        }
        case 249:
        {
          PIXEL00_12
          PIXEL01_22
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_100
          }
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 235:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          PIXEL01_21
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_100
          }
          PIXEL11_11
          break;
        }
        case 111:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_100
          }
          PIXEL01_12
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          PIXEL11_22
          break;
        }
        case 63:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_100
          }
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          PIXEL10_11
          PIXEL11_21
          break;
        }
        case 159:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_100
          }
          PIXEL10_22
          PIXEL11_12
          break;
        }
        case 215:
        {
          PIXEL00_11
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_100
          }
          PIXEL10_21
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 246:
        {
          PIXEL00_22
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          PIXEL10_12
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_100
          }
          break;
        }
        case 254:
        {
          PIXEL00_10
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_100
          }
          break;
        }
        case 253:
        {
          PIXEL00_12
          PIXEL01_11
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_100
          }
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_100
          }
          break;
        }
        case 251:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          PIXEL01_10
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_100
          }
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 239:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_100
          }
          PIXEL01_12
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_100
          }
          PIXEL11_11
          break;
        }
        case 127:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_100
          }
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          PIXEL11_10
          break;
        }
        case 191:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_100
          }
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_100
          }
          PIXEL10_11
          PIXEL11_12
          break;
        }
        case 223:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_100
          }
          PIXEL10_10
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 247:
        {
          PIXEL00_11
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_100
          }
          PIXEL10_12
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_100
          }
          break;
        }
        case 255:
        {
          if (Diff(src1[-1], src0[0]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_100
          }
          if (Diff(src0[0], src1[1]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_100
          }
          if (Diff(src2[0], src1[-1]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_100
          }
          if (Diff(src1[1], src2[0]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_100
          }
          break;
        }
      }

      ++src0;
      ++src1;
      ++src2;
      dstA += 2;
      dstB += 2;
    }

    src0 += 2;
    src1 += 2;
    src2 += 2;
    dst0 += dst_pitch2;
    dst1 += dst_pitch2;
  }
}

