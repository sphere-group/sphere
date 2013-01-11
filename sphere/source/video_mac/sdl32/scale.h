#ifndef __SCALE_H
#define __SCALE_H


template<typename T>
void DirectScale(T* dst, int dst_pitch, T* src, int src_width, int src_height)
{
    /*
    +--+
    |E |
    +--+

    +--+--+
    |E0|E1|
    +--+--+
    |E2|E3|
    +--+--+

    E: src[0]

    E0: dstA[0]
    E2: dstB[0]
    */

    int dst_pitch2 = dst_pitch * 2;

    T* dst0 = dst;
    T* dst1 = dst + dst_pitch;

    int iy = src_height;
    int ix;

    while (iy--)
    {
        ix = src_width;
        T* dstA = dst0;
        T* dstB = dst1;

        while (ix--)
        {
            dstA[0] = src[0];
            dstA[1] = src[0];
            dstB[0] = src[0];
            dstB[1] = src[0];

            ++src;
            dstA += 2;
            dstB += 2;
        }

        dst0 += dst_pitch2;
        dst1 += dst_pitch2;
    }
}

template<typename T>
void Scale2x(T* dst, int dst_pitch, T* src, int src_width, int src_height)
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

    int dst_pitch2 = dst_pitch * 2;

    T* dst0 = dst + 2 + dst_pitch * 2;
    T* dst1 = dst + 2 + dst_pitch * 3;
    T* src0 = src + 1;
    T* src1 = src + 1 + src_width;
    T* src2 = src + 1 + src_width * 2;

    int iy = src_height - 2;
    int ix;

    // middle
    while (iy--)
    {
        ix = src_width - 2;
        T* dstA = dst0;
        T* dstB = dst1;

        while (ix--)
        {
            if (src0[0] != src2[0] && src1[-1] != src1[1])
            {
                dstA[0] = src1[-1] == src0[0] ? src1[-1] : src1[0];
                dstA[1] = src0[0]  == src1[1] ? src1[1]  : src1[0];
                dstB[0] = src1[-1] == src2[0] ? src1[-1] : src1[0];
                dstB[1] = src2[0]  == src1[1] ? src1[1]  : src1[0];
            }
            else
            {
                dstA[0] = src1[0];
                dstA[1] = src1[0];
                dstB[0] = src1[0];
                dstB[1] = src1[0];
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

template<typename T>
void Eagle(T* dst, int dst_pitch, T* src, int src_width, int src_height)
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

    int dst_pitch2 = dst_pitch * 2;

    T* dst0 = dst + 2 + dst_pitch * 2;
    T* dst1 = dst + 2 + dst_pitch * 3;
    T* src0 = src + 1;
    T* src1 = src + 1 + src_width;
    T* src2 = src + 1 + src_width * 2;

    int iy = src_height - 2;
    int ix;

    // middle
    while (iy--)
    {
        ix = src_width - 2;
        T* dstA = dst0;
        T* dstB = dst1;

        while (ix--)
        {

            if (src0[0] == src0[-1] && src0[-1] == src1[-1])
                dstA[0] = src1[-1];
            else
                dstA[0] = src1[0];

            if (src0[0] == src0[1] && src0[1] == src1[1])
                dstA[1] = src1[1];
            else
                dstA[1] = src1[0];

            if (src2[0] == src2[-1] && src2[-1] == src1[-1])
                dstB[0] = src1[-1];
            else
                dstB[0] = src1[0];

            if (src2[0] == src2[1] && src2[1] == src1[1])
                dstB[1] = src1[1];
            else
                dstB[1] = src1[0];

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

#endif
