/*---------------------------------------------------------------------*
 * The following (piece of) code, (part of) the 2xSaI engine,          *
 * copyright (c) 1999 by Derek Liauw Kie Fa.                           *
 * Non-Commercial use of the engine is allowed and is encouraged,      *
 * provided that appropriate credit be given and that this copyright   *
 * notice will not be removed under any circumstance.                  *
 * You may freely modify this code, but I request                      *
 * that any improvements to the engine be submitted to me, so          *
 * that I can implement these improvements in newer versions of        *
 * the engine.                                                         *
 * If you need more information, have any comments or suggestions,     *
 * you can e-mail me. My e-mail: derek-liauw@usa.net.                  *
 *---------------------------------------------------------------------*/
/* Rewritten by Anatoli Steinmark on 9. February 2008                  */
/* for use in the Sphere RPG Engine.                                   */
/*---------------------------------------------------------------------*/

#include "2xSaI.h"


static dword colorMask;
static dword lowPixelMask;
static dword qcolorMask;
static dword qlowpixelMask;


inline int GetResult(dword A, dword B, dword C, dword D)
{
 int x = 0;
 int y = 0;
 int r = 0;
 if (A == C) x+=1; else if (B == C) y+=1;
 if (A == D) x+=1; else if (B == D) y+=1;
 if (x <= 1) r+=1;
 if (y <= 1) r-=1;
 return r;
}

inline dword INTERPOLATE(dword A, dword B)
{
    if (A !=B)
    {
       return ( ((A & colorMask) >> 1) +
                ((B & colorMask) >> 1) +
                 (A & B & lowPixelMask) );
    }
    else return A;
}

inline dword Q_INTERPOLATE(dword A, dword B, dword C, dword D)
{
        register dword x = ((A & qcolorMask) >> 2) +
                           ((B & qcolorMask) >> 2) +
                           ((C & qcolorMask) >> 2) +
                           ((D & qcolorMask) >> 2);

        register dword y = (A & qlowpixelMask) +
                           (B & qlowpixelMask) +
                           (C & qlowpixelMask) +
                           (D & qlowpixelMask);

        y = (y >> 2) & qlowpixelMask;

        return x + y;
}




void SuperEagle(word* dst, int dst_pitch, word* src, int src_width, int src_height, int bpp)
{
    if (bpp == 16)
    {
            colorMask     = 0xF7DEF7DE; // 1111011111011110 1111011111011110
            lowPixelMask  = 0x08210821; // 0000100000100001 0000100000100001
            qcolorMask    = 0xE79CE79C; // 1110011110011100 1110011110011100
            qlowpixelMask = 0x18631863; // 0001100001100011 0001100001100011
    }
    else if (bpp == 15)
    {
            colorMask     = 0x7BDE7BDE; // 0111101111011110 0111101111011110
            lowPixelMask  = 0x04210421; // 0000010000100001 0000010000100001
            qcolorMask    = 0x739C739C; // 0111001110011100 0111001110011100
            qlowpixelMask = 0x0C630C63; // 0000110001100011 0000110001100011
    }

    dword product1a;
    dword product1b;
    dword product2a;
    dword product2b;

    dword color4;
    dword color5;
    dword color6;
    dword color1;
    dword color2;
    dword color3;

    dword colorA1;
    dword colorA2;
    dword colorB1;
    dword colorB2;
    dword colorS1;
    dword colorS2;

    word*  src0 = src + 1;
    word*  src1 = src + 1 + src_width;
    word*  src2 = src + 1 + src_width * 2;
    word*  src3 = src + 1 + src_width * 3;

    dword* dst0 = (dword*)(dst + 2 + dst_pitch * 2);
    dword* dst1 = (dword*)(dst + 2 + dst_pitch * 3);

    int iy = src_height - 2;
    int ix, r;

    while (iy--)
    {
        ix = src_width - 2;
        dword* dstA = dst0;
        dword* dstB = dst1;

        while (ix--)
        {

            //      B1 B2
            //   4  5  6  S2
            //   1  2  3  S1
            //      A1 A2

            colorB1 = src0[0];
            colorB2 = src0[1];

            color4  = src1[-1];
            color5  = src1[0];
            color6  = src1[1];

            color1  = src2[-1];
            color2  = src2[0];
            color3  = src2[1];

            if (ix)
            {
                colorS2 = src1[2];
                colorS1 = src2[2];
            }
            else
            {
                colorS2 = src1[1];
                colorS1 = src2[1];
            }

            if (iy)
            {
                colorA1 = src3[0];
                colorA2 = src3[1];
            }
            else
            {
                colorA1 = src2[0];
                colorA2 = src2[1];
            }

            if (color2 == color6 && color5 != color3)
            {
               product1b = product2a = color2;

               if ((color1 == color2) ||
                   (color6 == colorB2))
               {
                   product1a = INTERPOLATE (color2, color5);
                   product1a = INTERPOLATE (color2, product1a);
               }
               else
               {
                  product1a = INTERPOLATE (color5, color6);
               }

               if ((color6 == colorS2) || (color2 == colorA1))
               {
                   product2b = INTERPOLATE (color2, color3);
                   product2b = INTERPOLATE (color2, product2b);
               }
               else
               {
                  product2b = INTERPOLATE (color2, color3);
               }
            }
            else if (color5 == color3 && color2 != color6)
            {
               product2b = product1a = color5;

               if ((colorB1 == color5) || (color3 == colorS1))
               {
                   product1b = INTERPOLATE (color5, color6);
                   product1b = INTERPOLATE (color5, product1b);
               }
               else
               {
                  product1b = INTERPOLATE (color5, color6);
               }

               if ((color3 == colorA2) || (color4 == color5))
               {
                   product2a = INTERPOLATE (color5, color2);
                   product2a = INTERPOLATE (color5, product2a);
               }
               else
               {
                  product2a = INTERPOLATE (color2, color3);
               }

            }
            else if (color5 == color3 && color2 == color6)
            {
               r = 0;
               r += GetResult (color6, color5, color1,  colorA1);
               r += GetResult (color6, color5, color4,  colorB1);
               r += GetResult (color6, color5, colorA2, colorS1);
               r += GetResult (color6, color5, colorB2, colorS2);

               if (r > 0)
               {
                  product1b = product2a = color2;
                  product1a = product2b = INTERPOLATE (color5, color6);
               }
               else if (r < 0)
               {
                  product2b = product1a = color5;
                  product1b = product2a = INTERPOLATE (color5, color6);
               }
               else
               {
                  product2b = product1a = color5;
                  product1b = product2a = color2;
               }
            }
            else
            {
              product2b = product1a = INTERPOLATE (color2, color6);
              product2b = Q_INTERPOLATE (color3, color3, color3, product2b);
              product1a = Q_INTERPOLATE (color5, color5, color5, product1a);

              product2a = product1b = INTERPOLATE (color5, color3);
              product2a = Q_INTERPOLATE (color2, color2, color2, product2a);
              product1b = Q_INTERPOLATE (color6, color6, color6, product1b);
            }


            dstA[0] = product1a | (product1b << 16);
            dstB[0] = product2a | (product2b << 16);

            ++src0;
            ++src1;
            ++src2;
            ++src3;
            ++dstA;
            ++dstB;
        }

        src0 += 2;
        src1 += 2;
        src2 += 2;
        src3 += 2;

        dst0 += dst_pitch;
        dst1 += dst_pitch;
    }
}



void Super2xSaI(word* dst, int dst_pitch, word* src, int src_width, int src_height, int bpp)
{

    if (bpp == 16)
    {
            colorMask     = 0xF7DEF7DE; // 1111011111011110 1111011111011110
            lowPixelMask  = 0x08210821; // 0000100000100001 0000100000100001
            qcolorMask    = 0xE79CE79C; // 1110011110011100 1110011110011100
            qlowpixelMask = 0x18631863; // 0001100001100011 0001100001100011
    }
    else if (bpp == 15)
    {
            colorMask     = 0x7BDE7BDE; // 0111101111011110 0111101111011110
            lowPixelMask  = 0x04210421; // 0000010000100001 0000010000100001
            qcolorMask    = 0x739C739C; // 0111001110011100 0111001110011100
            qlowpixelMask = 0x0C630C63; // 0000110001100011 0000110001100011
    }

    dword product1a;
    dword product1b;
    dword product2a;
    dword product2b;

    dword color4;
    dword color5;
    dword color6;
    dword color1;
    dword color2;
    dword color3;

    dword colorA0;
    dword colorA1;
    dword colorA2;
    dword colorA3;
    dword colorB0;
    dword colorB1;
    dword colorB2;
    dword colorB3;
    dword colorS1;
    dword colorS2;

    word*  src0 = src + 1;
    word*  src1 = src + 1 + src_width;
    word*  src2 = src + 1 + src_width * 2;
    word*  src3 = src + 1 + src_width * 3;

    dword* dst0 = (dword*)(dst + 2 + dst_pitch * 2);
    dword* dst1 = (dword*)(dst + 2 + dst_pitch * 3);

    int iy = src_height - 2;
    int ix, r;

    while (iy--)
    {
        ix = src_width - 2;
        dword* dstA = dst0;
        dword* dstB = dst1;

        while (ix--)
        {

            //   B0 B1 B2 B3
            //   4  5  6  S2
            //   1  2  3  S1
            //   A0 A1 A2 A3

            colorB0 = src0[-1];
            colorB1 = src0[0];
            colorB2 = src0[1];

            color4  = src1[-1];
            color5  = src1[0];
            color6  = src1[1];

            color1  = src2[-1];
            color2  = src2[0];
            color3  = src2[1];

            if (ix && iy)
                colorA3 = src3[2];
            else if (!ix && iy)
                colorA3 = src3[1];
            else
                colorA3 = src2[1];

            if (ix)
            {
                colorB3 = src0[2];
                colorS2 = src1[2];
                colorS1 = src2[2];
            }
            else
            {
                colorB3 = src0[1];
                colorS2 = src1[1];
                colorS1 = src2[1];
            }

            if (iy)
            {
                colorA0 = src3[-1];
                colorA1 = src3[0];
                colorA2 = src3[1];
            }
            else
            {
                colorA0 = src2[-1];
                colorA1 = src2[0];
                colorA2 = src2[1];
            }

            if (color2 == color6 && color5 != color3)
            {
               product2b = product1b = color2;
            }
            else if (color5 == color3 && color2 != color6)
            {
               product2b = product1b = color5;
            }
            else if (color5 == color3 && color2 == color6)
            {
               r = 0;
               r += GetResult (color6, color5, color1, colorA1);
               r += GetResult (color6, color5, color4, colorB1);
               r += GetResult (color6, color5, colorA2, colorS1);
               r += GetResult (color6, color5, colorB2, colorS2);

               if (r > 0)
                  product2b = product1b = color6;
               else if (r < 0)
                  product2b = product1b = color5;
               else
               {
                  product2b = product1b = INTERPOLATE (color5, color6);
               }

            }
            else
            {

               if (color6 == color3 && color3 == colorA1 && color2 != colorA2 && color3 != colorA0)
                  product2b = Q_INTERPOLATE (color3, color3, color3, color2);
               else if (color5 == color2 && color2 == colorA2 && colorA1 != color3 && color2 != colorA3)
                  product2b = Q_INTERPOLATE (color2, color2, color2, color3);
               else
                  product2b = INTERPOLATE (color2, color3);

               if (color6 == color3 && color6 == colorB1 && color5 != colorB2 && color6 != colorB0)
                  product1b = Q_INTERPOLATE (color6, color6, color6, color5);
               else if (color5 == color2 && color5 == colorB2 && colorB1 != color6 && color5 != colorB3)
                  product1b = Q_INTERPOLATE (color6, color5, color5, color5);
               else
                  product1b = INTERPOLATE (color5, color6);
            }

            if (color5 == color3 && color2 != color6 && color4 == color5 && color5 != colorA2)
               product2a = INTERPOLATE (color2, color5);
            else if (color5 == color1 && color6 == color5 && color4 != color2 && color5 != colorA0)
               product2a = INTERPOLATE(color2, color5);
            else
               product2a = color2;

            if (color2 == color6 && color5 != color3 && color1 == color2 && color2 != colorB2)
               product1a = INTERPOLATE (color2, color5);
            else if (color4 == color2 && color3 == color2 && color1 != color5 && color2 != colorB0)
               product1a = INTERPOLATE(color2, color5);
            else
               product1a = color5;


            dstA[0] = product1a | (product1b << 16);
            dstB[0] = product2a | (product2b << 16);

            ++src0;
            ++src1;
            ++src2;
            ++src3;
            ++dstA;
            ++dstB;
        }

        src0 += 2;
        src1 += 2;
        src2 += 2;
        src3 += 2;

        dst0 += dst_pitch;
        dst1 += dst_pitch;
    }
}



inline int GetResult1(dword A, dword B, dword C, dword D)
{
 int x = 0;
 int y = 0;
 int r = 0;
 if (A == C) x+=1; else if (B == C) y+=1;
 if (A == D) x+=1; else if (B == D) y+=1;
 if (x <= 1) r+=1;
 if (y <= 1) r-=1;
 return r;
}

inline int GetResult2(dword A, dword B, dword C, dword D)
{
 int x = 0;
 int y = 0;
 int r = 0;
 if (A == C) x+=1; else if (B == C) y+=1;
 if (A == D) x+=1; else if (B == D) y+=1;
 if (x <= 1) r-=1;
 if (y <= 1) r+=1;
 return r;
}



void _2xSaI(word* dst, int dst_pitch, word* src, int src_width, int src_height, int bpp)
{
    if (bpp == 16)
    {
            colorMask     = 0xF7DEF7DE; // 1111011111011110 1111011111011110
            lowPixelMask  = 0x08210821; // 0000100000100001 0000100000100001
            qcolorMask    = 0xE79CE79C; // 1110011110011100 1110011110011100
            qlowpixelMask = 0x18631863; // 0001100001100011 0001100001100011
    }
    else if (bpp == 15)
    {
            colorMask     = 0x7BDE7BDE; // 0111101111011110 0111101111011110
            lowPixelMask  = 0x04210421; // 0000010000100001 0000010000100001
            qcolorMask    = 0x739C739C; // 0111001110011100 0111001110011100
            qlowpixelMask = 0x0C630C63; // 0000110001100011 0000110001100011
    }

    dword product0;
    dword product1;
    dword product2;

    dword colorI;
    dword colorE;
    dword colorF;
    dword colorJ;

    dword colorG;
    dword colorA;
    dword colorB;
    dword colorK;

    dword colorH;
    dword colorC;
    dword colorD;
    dword colorL;

    dword colorM;
    dword colorN;
    dword colorO;

    word*  src0 = src + 1;
    word*  src1 = src + 1 + src_width;
    word*  src2 = src + 1 + src_width * 2;
    word*  src3 = src + 1 + src_width * 3;

    dword* dst0 = (dword*)(dst + 2 + dst_pitch * 2);
    dword* dst1 = (dword*)(dst + 2 + dst_pitch * 3);

    int iy = src_height - 2;
    int ix, r;

    while (iy--)
    {
        ix = src_width - 2;
        dword* dstA = dst0;
        dword* dstB = dst1;

        while (ix--)
        {

            //   I E F J
            //   G A B K
            //   H C D L
            //   M N O

            colorI = src0[-1];
            colorE = src0[0];
            colorF = src0[1];

            colorG  = src1[-1];
            colorA  = src1[0];
            colorB  = src1[1];

            colorH  = src2[-1];
            colorC  = src2[0];
            colorD  = src2[1];

            if (ix)
            {
                colorJ = src0[2];
                colorK = src1[2];
                colorL = src2[2];
            }
            else
            {
                colorJ = src0[1];
                colorK = src1[1];
                colorL = src2[1];
            }

            if (iy)
            {
                colorM = src3[-1];
                colorN = src3[0];
                colorO = src3[1];
            }
            else
            {
                colorM = src2[-1];
                colorN = src2[0];
                colorO = src2[1];
            }

            if ((colorA == colorD) && (colorB != colorC))
            {
               if ( ((colorA == colorE) && (colorB == colorL)) ||
                    ((colorA == colorC) && (colorA == colorF) && (colorB != colorE) && (colorB == colorJ)) )
               {
                  product0 = colorA;
               }
               else
               {
                  product0 = INTERPOLATE(colorA, colorB);
               }

               if (((colorA == colorG) && (colorC == colorO)) ||
                   ((colorA == colorB) && (colorA == colorH) && (colorG != colorC) && (colorC == colorM)) )
               {
                  product1 = colorA;
               }
               else
               {
                  product1 = INTERPOLATE(colorA, colorC);
               }
               product2 = colorA;
            }
            else if ((colorB == colorC) && (colorA != colorD))
            {
               if (((colorB == colorF) && (colorA == colorH)) ||
                   ((colorB == colorE) && (colorB == colorD) && (colorA != colorF) && (colorA == colorI)) )
               {
                  product0 = colorB;
               }
               else
               {
                  product0 = INTERPOLATE(colorA, colorB);
               }

               if (((colorC == colorH) && (colorA == colorF)) ||
                   ((colorC == colorG) && (colorC == colorD) && (colorA != colorH) && (colorA == colorI)) )
               {
                  product1 = colorC;
               }
               else
               {
                  product1 = INTERPOLATE(colorA, colorC);
               }
               product2 = colorB;
            }
            else if ((colorA == colorD) && (colorB == colorC))
            {
               if (colorA == colorB)
               {
                  product0 = colorA;
                  product1 = colorA;
                  product2 = colorA;
               }
               else
               {
                  r = 0;
                  product1 = INTERPOLATE(colorA, colorC);
                  product0 = INTERPOLATE(colorA, colorB);

                  r += GetResult1 (colorA, colorB, colorG, colorE);
                  r += GetResult2 (colorB, colorA, colorK, colorF);
                  r += GetResult2 (colorB, colorA, colorH, colorN);
                  r += GetResult1 (colorA, colorB, colorL, colorO);

                  if (r > 0)
                      product2 = colorA;
                  else if (r < 0)
                      product2 = colorB;
                  else
                  {
                      product2 = Q_INTERPOLATE(colorA, colorB, colorC, colorD);
                  }
               }
            }
            else
            {
               product2 = Q_INTERPOLATE(colorA, colorB, colorC, colorD);

               if ((colorA == colorC) && (colorA == colorF) && (colorB != colorE) && (colorB == colorJ))
               {
                  product0 = colorA;
               }
               else if ((colorB == colorE) && (colorB == colorD) && (colorA != colorF) && (colorA == colorI))
               {
                  product0 = colorB;
               }
               else
               {
                  product0 = INTERPOLATE(colorA, colorB);
               }

               if ((colorA == colorB) && (colorA == colorH) && (colorG != colorC) && (colorC == colorM))
               {
                  product1 = colorA;
               }
               else if ((colorC == colorG) && (colorC == colorD) && (colorA != colorH) && (colorA == colorI))
               {
                  product1 = colorC;
               }
               else
               {
                  product1 = INTERPOLATE(colorA, colorC);
               }
            }

            dstA[0] = colorA   | (product0 << 16);
            dstB[0] = product1 | (product2 << 16);

            ++src0;
            ++src1;
            ++src2;
            ++src3;
            ++dstA;
            ++dstB;
        }

        src0 += 2;
        src1 += 2;
        src2 += 2;
        src3 += 2;

        dst0 += dst_pitch;
        dst1 += dst_pitch;
    }
}



