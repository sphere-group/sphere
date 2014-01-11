#ifndef __2xSAIWIN_H
#define __2xSAIWIN_H

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


#include "../../common/types.h"
#include "../../common/rgb.hpp"


#define COLORMASK       0x00FEFEFE   // 00000000 11111110 11111110 11111110
#define LOWPIXELMASK    0x00010101   // 00000000 00000001 00000001 00000001
#define QCOLORMASK      0x00FCFCFC   // 00000000 11111100 11111100 11111100
#define QLOWPIXELMASK   0x00030303   // 00000000 00000011 00000011 00000011


template<typename T>
inline int GetResult(T A, T B, T C, T D)
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
       return ( ((A & COLORMASK) >> 1) +
                ((B & COLORMASK) >> 1) +
                 (A & B & LOWPIXELMASK) );
    }
    else return A;
}

inline dword Q_INTERPOLATE(dword A, dword B, dword C, dword D)
{
        register dword x = ((A & QCOLORMASK) >> 2) +
                            ((B & QCOLORMASK) >> 2) +
                            ((C & QCOLORMASK) >> 2) +
                            ((D & QCOLORMASK) >> 2);

        register dword y = (A & QLOWPIXELMASK) +
                            (B & QLOWPIXELMASK) +
                            (C & QLOWPIXELMASK) +
                            (D & QLOWPIXELMASK);

        y = (y >> 2) & QLOWPIXELMASK;

        return x + y;
}


template<typename T>
void SuperEagle(T* dst, int dst_pitch, T* src, int src_width, int src_height)
{

    int dst_pitch2 = dst_pitch * 2;

    T product1a;
    T product1b;
    T product2a;
    T product2b;

    T color4;
    T color5;
    T color6;
    T color1;
    T color2;
    T color3;

    T colorA1;
    T colorA2;
    T colorB1;
    T colorB2;
    T colorS1;
    T colorS2;

    T* src0 = src + 1;
    T* src1 = src + 1 + src_width;
    T* src2 = src + 1 + src_width * 2;
    T* src3 = src + 1 + src_width * 3;

    T* dst0 = dst + 2 + dst_pitch * 2;
    T* dst1 = dst + 2 + dst_pitch * 3;

    int iy = src_height - 2;
    int ix, r;

    while (iy--)
    {
        ix = src_width - 2;
        T* dstA = dst0;
        T* dstB = dst1;

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


            dstA[0] = product1a;
            dstA[1] = product1b;
            dstB[0] = product2a;
            dstB[1] = product2b;

            ++src0;
            ++src1;
            ++src2;
            ++src3;
            dstA += 2;
            dstB += 2;
        }

        src0 += 2;
        src1 += 2;
        src2 += 2;
        src3 += 2;

        dst0 += dst_pitch2;
        dst1 += dst_pitch2;
    }
}

template<typename T>
void Super2xSaI(T* dst, int dst_pitch, T* src, int src_width, int src_height)
{

    int dst_pitch2 = dst_pitch * 2;

    T product1a;
    T product1b;
    T product2a;
    T product2b;

    T color4;
    T color5;
    T color6;
    T color1;
    T color2;
    T color3;

    T colorA0;
    T colorA1;
    T colorA2;
    T colorA3;
    T colorB0;
    T colorB1;
    T colorB2;
    T colorB3;
    T colorS1;
    T colorS2;

    T* src0 = src + 1;
    T* src1 = src + 1 + src_width;
    T* src2 = src + 1 + src_width * 2;
    T* src3 = src + 1 + src_width * 3;

    T* dst0 = dst + 2 + dst_pitch * 2;
    T* dst1 = dst + 2 + dst_pitch * 3;

    int iy = src_height - 2;
    int ix, r;

    while (iy--)
    {
        ix = src_width - 2;
        T* dstA = dst0;
        T* dstB = dst1;

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


            dstA[0] = product1a;
            dstA[1] = product1b;
            dstB[0] = product2a;
            dstB[1] = product2b;

            ++src0;
            ++src1;
            ++src2;
            ++src3;
            dstA += 2;
            dstB += 2;
        }

        src0 += 2;
        src1 += 2;
        src2 += 2;
        src3 += 2;

        dst0 += dst_pitch2;
        dst1 += dst_pitch2;
    }
}

template<typename T>
inline int GetResult1(T A, T B, T C, T D)
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

template<typename T>
inline int GetResult2(T A, T B, T C, T D)
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

template<typename T>
void _2xSaI(T* dst, int dst_pitch, T* src, int src_width, int src_height)
{

    int dst_pitch2 = dst_pitch * 2;

    T product0;
    T product1;
    T product2;

    T colorI;
    T colorE;
    T colorF;
    T colorJ;

    T colorG;
    T colorA;
    T colorB;
    T colorK;

    T colorH;
    T colorC;
    T colorD;
    T colorL;

    T colorM;
    T colorN;
    T colorO;

    T* src0 = src + 1;
    T* src1 = src + 1 + src_width;
    T* src2 = src + 1 + src_width * 2;
    T* src3 = src + 1 + src_width * 3;

    T* dst0 = dst + 2 + dst_pitch * 2;
    T* dst1 = dst + 2 + dst_pitch * 3;

    int iy = src_height - 2;
    int ix, r;

    while (iy--)
    {
        ix = src_width - 2;
        T* dstA = dst0;
        T* dstB = dst1;

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


            dstA[0] = colorA;
            dstA[1] = product0;
            dstB[0] = product1;
            dstB[1] = product2;

            ++src0;
            ++src1;
            ++src2;
            ++src3;
            dstA += 2;
            dstB += 2;
        }

        src0 += 2;
        src1 += 2;
        src2 += 2;
        src3 += 2;

        dst0 += dst_pitch2;
        dst1 += dst_pitch2;
    }
}


#endif
