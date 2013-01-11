#ifndef NO_WRITE_MNG
#include <libmng.h>
#include <stdio.h>
#include <stdlib.h>

#include "write_mng.hpp"
#include "Image32.hpp"
#include "rgb.hpp"

#include "../common/sphere_version.h"
#ifndef MAX_PATH
#define MAX_PATH 1024
#endif

const char* mng_get_error_message(mng_retcode code)
{
    switch (code)
    {
    case (MNG_NOERROR):          return "er.. indicates all's well";
        break;
    case (MNG_OUTOFMEMORY):      return "oops, buy some megabytes!";
        break;
    case (MNG_INVALIDHANDLE):    return "call mng_initialize first";
        break;
    case (MNG_NOCALLBACK):       return "set the callbacks please";
        break;
    case (MNG_UNEXPECTEDEOF):    return "what'd ya do with the data?";
        break;
    case (MNG_ZLIBERROR):        return "zlib burped";
        break;
    case (MNG_JPEGERROR):        return "jpglib complained";
        break;
    case (MNG_LCMSERROR):        return "little cms stressed out";
        break;
    case (MNG_NOOUTPUTPROFILE):  return "no output-profile defined";
        break;
    case (MNG_NOSRGBPROFILE):    return "no sRGB-profile defined";
        break;
    case (MNG_BUFOVERFLOW):      return "zlib output-buffer overflow";
        break;
    case (MNG_FUNCTIONINVALID):  return "ay, totally inappropriate";
        break;
    case (MNG_OUTPUTERROR):      return "disk full ?";
        break;
    case (MNG_JPEGBUFTOOSMALL):  return "can't handle buffer overflow";
        break;
    case (MNG_NEEDMOREDATA):     return "I'm hungry, give me more";
        break;
    case (MNG_NEEDTIMERWAIT):    return "Sleep a while then wake me";
        break;
    case (MNG_NEEDSECTIONWAIT):  return "just processed a SEEK";
        break;
    case (MNG_APPIOERROR):       return "application I/O error";
        break;
    case (MNG_APPTIMERERROR):    return "application timing error";
        break;
    case (MNG_APPCMSERROR):      return "application CMS error";
        break;
    case (MNG_APPMISCERROR):     return "application other error";
        break;
    case (MNG_APPTRACEABORT):    return "application aborts on trace";
        break;
    case (MNG_INTERNALERROR):    return "internal inconsistancy";
        break;
    case (MNG_INVALIDSIG):       return "invalid graphics file";
        break;
    case (MNG_INVALIDCRC):       return "crc check failed";
        break;
    case (MNG_INVALIDLENGTH):    return "chunklength mystifies me";
        break;
    case (MNG_SEQUENCEERROR):    return "invalid chunk sequence";
        break;
    case (MNG_CHUNKNOTALLOWED):  return "completely out-of-place";
        break;
    case (MNG_MULTIPLEERROR):    return "only one occurence allowed";
        break;
    case (MNG_PLTEMISSING):      return "indexed-color requires PLTE";
        break;
    case (MNG_IDATMISSING):      return "IHDR-block requires IDAT";
        break;
    case (MNG_CANNOTBEEMPTY):    return "must contain some data";
        break;
    case (MNG_GLOBALLENGTHERR):  return "global data incorrect";
        break;
    case (MNG_INVALIDBITDEPTH):  return "bitdepth out-of-range";
        break;
    case (MNG_INVALIDCOLORTYPE): return "colortype out-of-range";
        break;
    case (MNG_INVALIDCOMPRESS):  return "compression method invalid";
        break;
    case (MNG_INVALIDFILTER):    return "filter method invalid";
        break;
    case (MNG_INVALIDINTERLACE): return "interlace method invalid";
        break;
    case (MNG_NOTENOUGHIDAT):    return "ran out of compressed data";
        break;
    case (MNG_PLTEINDEXERROR):   return "palette-index out-of-range";
        break;
    case (MNG_NULLNOTFOUND):     return "couldn't find null-separator";
        break;
    case (MNG_KEYWORDNULL):      return "keyword cannot be empty";
        break;
    case (MNG_OBJECTUNKNOWN):    return "the object can't be found";
        break;
    case (MNG_OBJECTEXISTS):     return "the object already exists";
        break;
    case (MNG_TOOMUCHIDAT):      return "got too much compressed data";
        break;
    case (MNG_INVSAMPLEDEPTH):   return "sampledepth out-of-range";
        break;
    case (MNG_INVOFFSETSIZE):    return "invalid offset-size";
        break;
    case (MNG_INVENTRYTYPE):     return "invalid entry-type";
        break;
    case (MNG_ENDWITHNULL):      return "may not end with NULL";
        break;
    case (MNG_INVIMAGETYPE):     return "invalid image_type";
        break;
    case (MNG_INVDELTATYPE):     return "invalid delta_type";
        break;
    case (MNG_INVALIDINDEX):     return "index-value invalid";
        break;
    case (MNG_TOOMUCHJDAT):      return "got too much compressed data";
        break;
    case (MNG_JPEGPARMSERR):     return "JHDR/JPEG parms do not match";
        break;
    case (MNG_INVFILLMETHOD):    return "invalid fill_method";
        break;
    case (MNG_OBJNOTCONCRETE):   return "object must be concrete";
        break;
    case (MNG_TARGETNOALPHA):    return "object has no alpha-channel";
        break;
    case (MNG_MNGTOOCOMPLEX):    return "can't handle complexity";
        break;
    case (MNG_UNKNOWNCRITICAL):  return "unknown critical chunk found";
        break;
    case (MNG_UNSUPPORTEDNEED):  return "nEED requirement unsupported";
        break;
    case (MNG_INVALIDDELTA):     return "Delta operation illegal";
        break;
    case (MNG_INVALIDMETHOD):    return "invalid MAGN method";
        break;
    case (MNG_INVALIDCNVSTYLE):  return "can't make anything of this";
        break;
    case (MNG_WRONGCHUNK):       return "accessing the wrong chunk";
        break;
    case (MNG_INVALIDENTRYIX):   return "accessing the wrong entry";
        break;
    case (MNG_NOHEADER):         return "must have had header first";
        break;
    case (MNG_NOCORRCHUNK):      return "can't find parent chunk";
        break;
    case (MNG_NOMHDR):           return "no MNG header available";
        break;
    case (MNG_IMAGETOOLARGE):    return "input-image way too big";
        break;
    case (MNG_NOTANANIMATION):   return "file not a MNG";
        break;
    case (MNG_FRAMENRTOOHIGH):   return "frame-nr out-of-range";
        break;
    case (MNG_LAYERNRTOOHIGH):   return "layer-nr out-of-range";
        break;
    case (MNG_PLAYTIMETOOHIGH):  return "playtime out-of-range";
        break;
    case (MNG_FNNOTIMPLEMENTED): return "function not yet available";
        break;
    case (MNG_IMAGEFROZEN):      return "stopped displaying";
        break;
    }
    return "Unknown error code";
}

///////////////////////////////////////////////////////////
static mng_bool image_has_alpha(const void* image, const int width, const int height,
                                const int x, const int y, const int w, const int h)
{
    const RGBA* pixels = (RGBA*) image;
    mng_bool has_alpha = MNG_FALSE;
    for (int j = y; j < y + h; j++)
    {
        for (int i = x; i < x + w; i++)
        {
            if (pixels[j * width + i].alpha != 255)
            {
                has_alpha = MNG_TRUE;
                break;
            }
        }
    }
    return has_alpha;
}

static mng_bool image_is_grayscale(const void* image, const int width, const int height,
                                   const int x, const int y, const int w, const int h)
{
    const RGBA* pixels = (RGBA*) image;
    mng_bool grayscale = MNG_TRUE;
    for (int j = y; j < y + h; j++)
    {
        for (int i = x; i < x + w; i++)
        {
            if (pixels[j * width + i].red != pixels[j * width + i].green
                    && pixels[j * width + i].green != pixels[j * width + i].blue)
            {
                grayscale = MNG_FALSE;
                break;
            }
        }
    }
    return grayscale;
}

///////////////////////////////////////////////////////////
typedef struct userdata
{
    FILE* file;
    char filename[MAX_PATH];
}
userdata;

typedef userdata* userdatap;
mng_ptr MNG_DECL mng_alloc (mng_size_t iLen)
{
    return (mng_ptr)calloc (1, iLen);
}
void MNG_DECL mng_free (mng_ptr pPtr, mng_size_t iLen)
{
    if (iLen) free (pPtr);
}

mng_bool MNG_DECL
mng_write_stream (mng_handle mng, mng_ptr buffer, mng_uint32 size, mng_uint32p iWritten)
{
    userdatap userdata = (userdatap) mng_get_userdata(mng);
    *iWritten = fwrite(buffer, 1, size, userdata->file);
    return MNG_TRUE;
}

mng_bool MNG_DECL mng_open_stream (mng_handle mng)
{
    userdatap userdata = (userdatap) mng_get_userdata(mng);
    userdata->file = fopen(userdata->filename, "wb+");
    return (userdata->file) ? MNG_TRUE : MNG_FALSE;
}

mng_bool MNG_DECL mng_close_stream (mng_handle mng)
{
    userdatap userdata = (userdatap) mng_get_userdata(mng);
    if (userdata->file)
        fclose(userdata->file);
    return MNG_TRUE;
}

///////////////////////////////////////////////////////////
void rgba_image_add_filter_byte(const RGBA* pixels, int sx, int sy, int sw, int sh, const int width, const int height, unsigned char* filtered)
{
    unsigned char* ptr = filtered;
    int x;
    int y;

    for (y = sy; y < sy + sh; y++)
    {
        for (x = sx; x < sx + sw; x++)
        {
            if (x == sx)
            {
                *ptr++ = 0;
            }

            *ptr++ = pixels[(y * width) + x].red;
            *ptr++ = pixels[(y * width) + x].green;
            *ptr++ = pixels[(y * width) + x].blue;
            *ptr++ = pixels[(y * width) + x].alpha;
        }
    }
}

void rgb_image_add_filter_byte(const RGBA* pixels,
                               const int sx, const int sy,
                               const int sw, const int sh,
                               const int width, const int height,
                               unsigned char* filtered)
{
    int x;
    int y;
    unsigned char* ptr = filtered;

    for (y = sy; y < sy + sh; y++)
    {
        for (x = sx; x < sx + sw; x++)
        {
            if (x == sx)
            {
                *ptr++ = 0;
            }

            *ptr++ = pixels[(y * width) + x].red;
            *ptr++ = pixels[(y * width) + x].green;
            *ptr++ = pixels[(y * width) + x].blue;
        }
    }
}

void gray_image_add_filter_byte(const RGBA* pixels,
                                const int sx, const int sy,
                                const int sw, const int sh,
                                const int width, const int height,
                                unsigned char* filtered)
{
    int x;
    int y;
    unsigned char* ptr = filtered;

    for (y = sy; y < sy + sh; y++)
    {
        for (x = sx; x < sx + sw; x++)
        {
            if (x == sx)
            {
                *ptr++ = 0;
            }
            *ptr++ = pixels[(y * width) + x].red;
        }
    }
}

void grayalpha_image_add_filter_byte(const RGBA* pixels,
                                     const int sx, const int sy,
                                     const int sw, const int sh,
                                     const int width, const int height,
                                     unsigned char* filtered)
{
    int x;
    int y;
    unsigned char* ptr = filtered;

    for (y = sy; y < sy + sh; y++)
    {
        for (x = sx; x < sx + sw; x++)
        {
            if (x == sx)
            {
                *ptr++ = 0;
            }
            *ptr++ = pixels[(y * width) + x].red;
            *ptr++ = pixels[(y * width) + x].alpha;
        }
    }
}

int findcolorfrompalette(const mng_palette8 palette,
                         int palette_size, int red, int green, int blue)
{
    for (int i = 0; i < palette_size; i++)
    {
        if (palette[i].iRed   == red
                && palette[i].iGreen == green
                && palette[i].iBlue  == blue)
            return i;
    }

    return -1;
}

void rgb_palette_image_add_filter_byte(const mng_palette8 palette, int palette_size,
                                       const RGBA* pixels,
                                       const int sx, const int sy, const int sw, const int sh,
                                       const int width, const int height,
                                       unsigned char* filtered)
{
    int x;
    int y;
    unsigned char* ptr = filtered;

    for (y = sy; y < sy + sh; y++)
    {
        for (x = sx; x < sx + sw; x++)
        {
            if (x == sx)
            {
                *ptr++ = 0;
            }

            const RGBA* p = &pixels[y * width + x];
            *ptr++ = findcolorfrompalette(palette, palette_size, p->red, p->green, p->blue);
        }
    }
}

///////////////////////////////////////////////////////////
mng_bool image_to_palette(mng_palette8 palette, int* palette_size,
                          const int width, const int height, const RGBA* pixels)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            const RGBA* p = &pixels[y * width + x];
            int index = findcolorfrompalette(palette, *palette_size, p->red, p->green, p->blue);
            if (index == -1)
            {
                if (*palette_size <= 255)
                {
                    palette[*palette_size].iRed   = p->red;
                    palette[*palette_size].iGreen = p->green;
                    palette[*palette_size].iBlue  = p->blue;
                    *palette_size += 1;
                }
                else
                    return MNG_FALSE;
            }
        }
    }

    return MNG_TRUE;
}

///////////////////////////////////////////////////////////
void calc_different_area(const RGBA* one, const RGBA* two, const int width, const int height, int* x, int* y, int* w, int* h)
{
    int px;
    int py;

    int done;
    *x = 0;
    *y = 0;
    *w = width;
    *h = height;

    done = 0;
    for (py = 0; !done && py < height; py++)
    {
        for (px = 0; px < width; px++)
        {
            const RGBA* p1 = &one[py * width + px];
            const RGBA* p2 = &two[py * width + px];

            if (p1->red   != p2->red
                    || p1->green != p2->green
                    || p1->blue  != p2->blue
                    || p1->alpha != p2->alpha)
            {
                done = 1;
                break;
            }
        }
        if (!done)
            *y += 1;
    }

    done = 0;
    for (px = 0; !done && px < width; px++)
    {
        for (py = 0; py < height; py++)
        {
            const RGBA* p1 = &one[py * width + px];
            const RGBA* p2 = &two[py * width + px];

            if (p1->red   != p2->red
                    || p1->green != p2->green
                    || p1->blue  != p2->blue
                    || p1->alpha != p2->alpha)
            {
                done = 1;
                break;
            }
        }
        if (!done)
            *x += 1;
    }

    *w = width - *x;
    *h = height - *y;

    done = 0;
    for (px = width - 1; !done && px > *x; px--)
    {
        for (py = height - 1; py > *y; py--)
        {
            const RGBA* p1 = &one[py * width + px];
            const RGBA* p2 = &two[py * width + px];

            if (p1->red   != p2->red
                    || p1->green != p2->green
                    || p1->blue  != p2->blue
                    || p1->alpha != p2->alpha)
            {
                done = 1;
                break;
            }
        }
        if (!done)
            *w -= 1;
    }

    done = 0;
    for (py = height - 1; !done && py > *y; py--)
    {
        for (px = width - 1; px > *x; px--)
        {
            const RGBA* p1 = &one[py * width + px];
            const RGBA* p2 = &two[py * width + px];

            if (p1->red   != p2->red
                    || p1->green != p2->green
                    || p1->blue  != p2->blue
                    || p1->alpha != p2->alpha)
            {
                done = 1;
                break;
            }
        }
        if (!done)
            *h -= 1;
    }

}
/*
mng_retcode mng_putjpgimage(mng_handle hMNG, const RGBA* pixels, const int width, const int height,
                            const int x, const int y, const int w, const int h) {

  mng_retcode iRC = mng_putchunk_jhdr(hMNG,
                                      w, // mng_uint32       iWidth,
                                      h, // mng_uint32       iHeight,
                                      MNG_COLORTYPE_JPEGCOLOR,   // mng_uint8        iColortype,
                                      MNG_BITDEPTH_JPEG8,        // mng_uint8        iImagesampledepth,
                                      MNG_COMPRESSION_BASELINEJPEG,   // mng_uint8        iImagecompression,
                                      0, // MNG_INTERLACE_SEQUENTIAL,  // mng_uint8        iImageinterlace,
                                      0, // MNG_BITDEPTH_JPEG8,        // mng_uint8        iAlphasampledepth,
                                      0, // MNG_COMPRESSION_DEFLATE,   // mng_uint8        iAlphacompression,
                                      0, // MNG_FILTER_ADAPTIVE,       // mng_uint8        iAlphafilter,
                                      0); // MNG_INTERLACE_SEQUENTIAL); // mng_uint8        iAlphainterlace);
  if (iRC != 0)
    return iRC;

  int pixel_size = sizeof(RGB);
  if (1) {
    mng_memalloc __mng_alloc__ = mng_getcb_memalloc(hMNG);
    mng_memfree  __mng_free__  = mng_getcb_memfree(hMNG);

    unsigned char* buffer;
    unsigned char* compressed;

    mng_uint32 filter_len     = (pixel_size * w * h) + h;
    mng_uint32 compressed_len = (pixel_size * w * h) + h;
               compressed_len += compressed_len / 100 + 12 + 8;	// extra 8 for safety

    buffer = (unsigned char*) __mng_alloc__(filter_len);
    if (buffer == NULL)
      return MNG_OUTOFMEMORY;

    compressed = (unsigned char*) __mng_alloc__(compressed_len);
    if (compressed == NULL) {
      __mng_free__(buffer, filter_len);
      return MNG_OUTOFMEMORY;
    }

    rgb_image_add_filter_byte(pixels, x, y, w, h, width, height, buffer);
    uLong dstLen = compressed_len;
    uLong srcLen = filter_len;
    if (compress2(compressed, &dstLen, buffer, srcLen, 9) != Z_OK) {
      __mng_free__(buffer, filter_len);
      __mng_free__(compressed, compressed_len);
      return MNG_ZLIBERROR;
    }

    iRC = mng_putchunk_idat(hMNG, dstLen, compressed);
    __mng_free__(buffer, filter_len);
    __mng_free__(compressed, compressed_len);

    if (iRC != 0) return iRC;
  }

  if (iRC != 0)
    return iRC;

  iRC = mng_putchunk_iend(hMNG);
  return iRC;
}
*/

mng_retcode mng_putpngimage(mng_handle hMNG, const RGBA* pixels, const int width, const int height,
                            const int x, const int y, const int w, const int h,
                            mng_palette8 palette, const int palette_size, mng_bool global_palette)
{
    mng_retcode iRC;
    int pixel_size = palette_size == 0 ? sizeof(RGBA) : sizeof(RGB);
    mng_bool grayscale = image_is_grayscale(pixels, width, height, x, y, w, h);
    mng_bool has_alpha = image_has_alpha(pixels, width, height, x, y, w, h);

    // If the number of colors could be MNG_BITDEPTH_1, MNG_BITDEPTH_2 or MNG_BITDEPTH_4
    // we could probably fit more colors per pixel, but I only support MNG_BITDEPTH_8 for now

    /*
    unsigned long num_colors = CountColorsUsed(pixels, width, height, x, y, w, h);
    mng_int8 bit_depth = MNG_BITDEPTH_8;

    if (num_colors != 0) {
      if (num_colors <= 16) {
        bit_depth = MNG_BITDEPTH_4;
      }
      if (num_colors <= 4) {
    //      bit_depth = MNG_BITDEPTH_2;
      }
      if (num_colors <= 2) {
    //      bit_depth = MNG_BITDEPTH_1;
      }
    }
    */

    if (has_alpha)
    {

        grayscale = MNG_FALSE;
        has_alpha = MNG_TRUE;
        pixel_size = sizeof(RGBA);
    }
    if (grayscale)
    {
        pixel_size = has_alpha ? 2 : 1;
    }

    if (pixel_size == sizeof(RGBA) && !has_alpha)
    {
        pixel_size = sizeof(RGB);
    }

    if (pixel_size == sizeof(RGBA))
    {
        iRC = mng_putchunk_ihdr (hMNG, w, h,
                                 MNG_BITDEPTH_8, MNG_COLORTYPE_RGBA, MNG_COMPRESSION_DEFLATE,
                                 MNG_FILTER_NONE, MNG_INTERLACE_NONE);
        if (iRC != 0) return iRC;
    }
    else
        if (pixel_size == sizeof(RGB) && palette_size != 0)
        {
            iRC = mng_putchunk_ihdr (hMNG, w, h,
                                     MNG_BITDEPTH_8, MNG_COLORTYPE_INDEXED, MNG_COMPRESSION_DEFLATE,
                                     MNG_FILTER_ADAPTIVE, MNG_INTERLACE_NONE);
            if (iRC != 0) return iRC;

            iRC = mng_putchunk_plte (hMNG, global_palette ? 0 : palette_size, palette);
            if (iRC != 0) return iRC;

            //RGBA palettewithalpha[256];
            //int palettewithalpha_size = 0;
            //if (image_to_palettewithalpha(palettewithalpha, &palettewithalpha_size, width, height, pixels)) {

            if (false && has_alpha)
            {
                mng_uint8arr alpha_palette;
                int alpha_palette_size = 1;
                for (int i = 0; i < alpha_palette_size; i++)
                {
                    alpha_palette[i] = 255;
                }

                mng_uint8arr raw_data;
                // write transparency stuff
                mng_putchunk_trns(hMNG,
                                  MNG_FALSE,
                                  MNG_FALSE,
                                  0,
                                  alpha_palette_size, // mng_uint32       iCount,
                                  alpha_palette, //  mng_uint8arr     aAlphas,
                                  0, // mng_uint16       iGray,
                                  0, // mng_uint16       iRed,
                                  0, // mng_uint16       iGreen,
                                  0, // mng_uint16       iBlue,
                                  0, // mng_uint32       iRawlen,
                                  raw_data); // mng_uint8arr     aRawdata);
                if (iRC != 0) return iRC;
//      }
            }

        }
        else
            if (pixel_size == sizeof(RGB))
            {
                iRC = mng_putchunk_ihdr (hMNG, w, h,
                                         MNG_BITDEPTH_8, MNG_COLORTYPE_RGB, MNG_COMPRESSION_DEFLATE,
                                         MNG_FILTER_NONE, MNG_INTERLACE_NONE);
                if (iRC != 0) return iRC;
            }
            else
                if (grayscale)
                {
                    if (has_alpha)
                    {
                        iRC = mng_putchunk_ihdr(hMNG, w, h,
                                                MNG_BITDEPTH_8, MNG_COLORTYPE_GRAYA, MNG_COMPRESSION_DEFLATE,
                                                MNG_FILTER_NONE, MNG_INTERLACE_NONE);
                        if (iRC != 0) return iRC;
                    }
                    else
                    {
                        iRC = mng_putchunk_ihdr(hMNG, w, h,
                                                MNG_BITDEPTH_8, MNG_COLORTYPE_GRAY, MNG_COMPRESSION_DEFLATE,
                                                MNG_FILTER_NONE, MNG_INTERLACE_NONE);
                        if (iRC != 0) return iRC;
                    }
                }

    if (1)
    {
        mng_memalloc __mng_alloc__ = mng_getcb_memalloc(hMNG);
        mng_memfree  __mng_free__  = mng_getcb_memfree(hMNG);

        unsigned char* buffer;
        unsigned char* compressed;

        mng_uint32 filter_len     = (pixel_size * w * h) + h;
        mng_uint32 compressed_len = (pixel_size * w * h) + h;
        compressed_len += compressed_len / 100 + 12 + 8;	// extra 8 for safety

        //if (bit_depth == MNG_BITDEPTH_4) {
        //  filter_len /= 2;
        //  compressed_len /= 2;
        //}

        buffer = (unsigned char*) __mng_alloc__(filter_len);
        if (buffer == NULL)
        {
            return MNG_OUTOFMEMORY;

        }
        compressed = (unsigned char*) __mng_alloc__(compressed_len);
        if (compressed == NULL)
        {
            __mng_free__(buffer, filter_len);
            return MNG_OUTOFMEMORY;
        }

        if (pixel_size == sizeof(RGBA))
            rgba_image_add_filter_byte(pixels, x, y, w, h, width, height, buffer);
        else
            if (pixel_size == sizeof(RGB) && palette_size != 0)
            {
//      RGBA palette[256];
//      int palettewithalpha_size = 0;
//      if (image_to_palettewithalpha(palette, &palettewithalpha_size, width, height, pixels)) {
                rgb_palette_image_add_filter_byte(palette, palette_size, pixels, x, y, w, h, width, height, buffer);
//        rgb_palette_with_alpha_image_add_filter_byte(palette, palettewithalpha_size, pixels, x, y, w, h, width, height, buffer);
//      }
            }
            else
                if (pixel_size == sizeof(RGB))
                    rgb_image_add_filter_byte(pixels, x, y, w, h, width, height, buffer);
                else
                    if (grayscale)
                    {
                        if (has_alpha)
                        {
                            grayalpha_image_add_filter_byte(pixels, x, y, w, h, width, height, buffer);
                        }
                        else
                        {
                            gray_image_add_filter_byte(pixels, x, y, w, h, width, height, buffer);
                        }
                    }

        uLong dstLen = compressed_len;
        uLong srcLen = filter_len;
        if (compress2(compressed, &dstLen, buffer, srcLen, 9) != Z_OK)
        {
            __mng_free__(buffer, filter_len);
            __mng_free__(compressed, compressed_len);
            return MNG_ZLIBERROR;
        }

        iRC = mng_putchunk_idat(hMNG, dstLen, compressed);
        __mng_free__(buffer, filter_len);
        __mng_free__(compressed, compressed_len);

        if (iRC != 0) return iRC;
    }

    iRC = mng_putchunk_iend (hMNG);
    if (iRC != 0)
        return iRC;

    return iRC;
}

///////////////////////////////////////////////////////////////////////////////
static mng_retcode
____SaveMNGAnimationFromImages____(mng_handle hMNG,
                                   WriteMNG_GetImage get_image,
                                   WriteMNG_GetDelay get_delay,
                                   WriteMNG_ContinueProcessingImages should_continue,
                                   void* data)
{
    int max_frame_width = 0;
    int max_frame_height = 0;

    mng_palette8 GlobalPalette;
    int GlobalPaletteSize = 0;
    mng_bool can_use_global_palette = MNG_TRUE;

    int max_images = -1;
    int num_images = 0;

    mng_uint32 totaldelay = 0;
    mng_bool playtime_known = MNG_TRUE;

    CImage32 __temp__;
    while (get_image(num_images, __temp__, data) == MNG_TRUE && (max_images == -1 || num_images < max_images))

    {
        if (max_frame_width < __temp__.GetWidth())
            max_frame_width = __temp__.GetWidth();
        if (max_frame_height < __temp__.GetHeight())
            max_frame_height = __temp__.GetHeight();

        if (playtime_known)
        {
            mng_uint32 delay = get_delay(num_images, data);
            if (delay && (delay + totaldelay > totaldelay))
            {
                totaldelay += delay;
            }
            else
            {
                playtime_known = MNG_FALSE;
                totaldelay = 0;
            }
        }

        if (can_use_global_palette
                && !image_to_palette(GlobalPalette, &GlobalPaletteSize, __temp__.GetWidth(), __temp__.GetHeight(), __temp__.GetPixels()))
        {
            can_use_global_palette = false;
        }
        num_images++;

        if (!should_continue(-1, num_images))
            return -1;
    }

    if (max_frame_width <= 0 || max_frame_height <= 0)
    {
        return -1;
    }

    mng_retcode iRC = mng_setcb_writedata(hMNG, mng_write_stream);
    if (iRC != 0) return iRC;
    iRC = mng_setcb_openstream(hMNG, mng_open_stream);
    if (iRC != 0) return iRC;
    iRC = mng_setcb_closestream(hMNG, mng_close_stream);
    if (iRC != 0) return iRC;

    iRC = mng_create (hMNG);
    if (iRC != 0) return iRC;

    iRC = mng_putchunk_mhdr (hMNG, max_frame_width, max_frame_height,
                             1000, 0, num_images, totaldelay, MNG_SIMPLICITY_TRANSPARENCY);

    if (iRC != 0) return iRC;
    mng_bool repeating = MNG_TRUE;
    mng_uint32 repeat_count = (repeating) ? 0x7fffffff : 0;
    mng_putchunk_term(hMNG, MNG_TERMACTION_REPEAT, MNG_ITERACTION_FIRSTFRAME, 0, repeat_count);
    if (iRC != 0) return iRC;

    const char* software = "Sphere - http://sphere.sf.net/";
    iRC = mng_putchunk_text(hMNG,
                            strlen(MNG_TEXT_SOFTWARE), MNG_TEXT_SOFTWARE,
                            strlen(software), (char*) software);
    if (iRC != 0) return iRC;

    iRC = mng_putchunk_text(hMNG,
                            strlen("Version"), "Version",
                            strlen(SPHERE_VERSION), SPHERE_VERSION);
    if (iRC != 0) return iRC;

#if 1
    char text[100] = {0};
    sprintf (text, "%s", repeating ? "Infinite" : "Once");
    iRC = mng_putchunk_text(hMNG,
                            strlen("RepeatType"), "RepeatType",
                            strlen(text), text);
    if (iRC != 0) return iRC;
    sprintf (text, "%d", repeating ? 1 : repeat_count);
    iRC = mng_putchunk_text(hMNG,
                            strlen("RepeatCount"), "RepeatCount",
                            strlen(text), text);
    if (iRC != 0) return iRC;
    iRC = mng_putchunk_text(hMNG,
                            strlen("BuildDate"), "BuildDate",
                            strlen(__DATE__), __DATE__);
    if (iRC != 0) return iRC;
    iRC = mng_putchunk_text(hMNG,
                            strlen("BuildTime"), "BuildTime",
                            strlen(__TIME__), __TIME__);
    if (iRC != 0) return iRC;

    iRC = mng_putchunk_text(hMNG, strlen("libmng_version"), "libmng_version",
                            strlen(MNG_VERSION_TEXT), MNG_VERSION_TEXT);
    if (iRC != 0) return iRC;
#endif

    iRC = mng_set_srgb(hMNG, MNG_TRUE);
    if (iRC != 0) return iRC;

    //iRC = mng_putchunk_back (hMNG, 255, 255, 255, 0, 0, MNG_BACKGROUNDIMAGE_NOTILE);
    //if (iRC != 0) return iRC;

    iRC = mng_putchunk_fram  (hMNG,  MNG_FALSE,  MNG_FRAMINGMODE_3,
                              0,  NULL,  MNG_CHANGEDELAY_NO, 0,  0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0);
    if (iRC != 0) return iRC;

    if (can_use_global_palette)
    {
        iRC = mng_putchunk_plte(hMNG, GlobalPaletteSize, GlobalPalette);
        if (iRC != 0) return iRC;
    }

    CImage32 image;
    CImage32 last;

    int i = 0;
    while (get_image(i, image, data) && (max_images == -1 || i < max_images))
    {
        int x = 0, y = 0, w = image.GetWidth(), h = image.GetHeight();

        iRC = mng_putchunk_fram(hMNG,  MNG_FALSE,  MNG_FRAMINGMODE_NOCHANGE,
                                0,  NULL,  MNG_CHANGEDELAY_NEXTSUBFRAME, MNG_CHANGETIMOUT_NO,
                                MNG_CHANGECLIPPING_NO, MNG_CHANGESYNCID_NO,
                                get_delay(i, data), 0,  0,
                                0,  0, 0, 0,
                                0, 0);
        if (iRC != 0) return iRC;
        mng_putchunk_move(hMNG, 0, 0, 0, 0, 0);
        if (iRC != 0) return iRC;

        if (i > 0)
        {
            if (last.GetWidth() == image.GetWidth()
                    && last.GetHeight() == image.GetHeight())
            {
                //calc_different_area(last.GetPixels(), image.GetPixels(), image.GetWidth(), image.GetHeight(), &x, &y, &w, &h);

                if (x != 0 || y != 0 || w != image.GetWidth() || h != image.GetHeight())
                {
                    iRC = mng_putchunk_fram(hMNG,  MNG_FALSE,  MNG_FRAMINGMODE_3,
                                            0,  NULL,  MNG_CHANGEDELAY_NO, MNG_CHANGETIMOUT_NO,
                                            MNG_CHANGECLIPPING_NEXTSUBFRAME, MNG_CHANGESYNCID_NO,
                                            0, 0,  0,
                                            x,  x+w, y, y+h,
                                            0, 0);
                    if (iRC != 0) return iRC;
                    iRC = mng_putchunk_move(hMNG, 0, 0, 0, x, y);
                    if (iRC != 0) return iRC;
                }
            }
            else
            {
                iRC = mng_putchunk_fram(hMNG,  MNG_FALSE,  MNG_FRAMINGMODE_3,
                                        0,  NULL,  MNG_CHANGEDELAY_NO, MNG_CHANGETIMOUT_NO,
                                        MNG_CHANGECLIPPING_NO, MNG_CHANGESYNCID_NO,
                                        0, 0,  0,
                                        0,  0, 0, 0,
                                        0, 0);
                mng_putchunk_move(hMNG, 0, 0, 0, 0, 0);
            }
        }

        if (can_use_global_palette)
        {
            iRC = mng_putpngimage(hMNG, image.GetPixels(), image.GetWidth(), image.GetHeight(), x, y, w, h,
                                  GlobalPalette, GlobalPaletteSize, MNG_TRUE);
        }
        else
        {
            mng_palette8 palette;
            int palette_size = 0;
            if (image_to_palette(palette, &palette_size, image.GetWidth(), image.GetHeight(), image.GetPixels()))
            {
                iRC = mng_putpngimage(hMNG, image.GetPixels(), image.GetWidth(), image.GetHeight(), x, y, w, h,
                                      palette, palette_size, MNG_FALSE);
            }
            else
            {
                iRC = mng_putpngimage(hMNG, image.GetPixels(), image.GetWidth(), image.GetHeight(), x, y, w, h,
                                      NULL, 0, MNG_FALSE);
            }
        }

        if (iRC != 0)
            return iRC;

        i++;
        if (!should_continue(i, num_images))
            return -1;

        last = image;
    }

    iRC = mng_putchunk_mend (hMNG);
    if (iRC != 0) return iRC;

    iRC = mng_write(hMNG);
    if (iRC != 0) return iRC;

    return iRC;
}

///////////////////////////////////////////////////////////////////////////////
static mng_retcode
__SaveMNGAnimationFromImages__(mng_handle hMNG, const char* filename,
                               WriteMNG_GetImage get_image,
                               WriteMNG_GetDelay get_delay,
                               WriteMNG_ContinueProcessingImages should_continue,
                               void* data)
{
    userdatap pMydata = (userdatap)calloc (1, sizeof (userdata));
    if (!pMydata)
        return -1;

    strcpy(pMydata->filename, filename);
    mng_set_userdata(hMNG, pMydata);

    mng_retcode iRC = ____SaveMNGAnimationFromImages____(hMNG,
                      get_image,
                      get_delay,
                      should_continue,
                      data);

    free(pMydata);
    pMydata = NULL;

    mng_set_userdata(hMNG, NULL);
    return iRC;
}

///////////////////////////////////////////////////////////////////////////////
mng_retcode
SaveMNGAnimationFromImages(const char* filename,
                           WriteMNG_GetImage get_image,
                           WriteMNG_GetDelay get_delay,
                           WriteMNG_ContinueProcessingImages should_continue,
                           void* data)

{
    mng_retcode iRC;
    mng_handle hMNG = mng_initialize (MNG_NULL, mng_alloc, mng_free, NULL);
    if (!hMNG)
        return -1;

    iRC = __SaveMNGAnimationFromImages__(hMNG, filename, get_image, get_delay, should_continue, data);
    if (iRC == 0)
        iRC = mng_cleanup(&hMNG);
    else
        mng_cleanup(&hMNG);

    return iRC;
}

///////////////////////////////////////////////////////////////////////////////
#endif
