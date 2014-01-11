#include <stdlib.h>
#include <string.h>
#include "Filters.hpp"
#include <math.h>

////////////////////////////////////////////////////////////////////////////////

void FlipHorizontally(int width, int height, RGBA* pixels)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width / 2; x++)
        {
            RGBA p = pixels[x];
            pixels[x] = pixels[width - x - 1];
            pixels[width - x - 1] = p;
        }
        pixels += width;
    }
}

////////////////////////////////////////////////////////////////////////////////

void FlipVertically(int width, int height, RGBA* pixels)
{
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height / 2; y++)
        {
            RGBA p = pixels[y * width + x];
            pixels[y * width + x] = pixels[(height - y - 1) * width + x];
            pixels[(height - y - 1) * width + x] = p;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Translate(int width, int height, RGBA* pixels, int dx, int dy)
{
    RGBA* old_pixels = new RGBA[width * height];
    memcpy(old_pixels, pixels, width * height * sizeof(RGBA));

    for (int iy = 0; iy < height; iy++)
    {
        for (int ix = 0; ix < width; ix++)
        {
            int sx = ix - dx;
            int sy = iy - dy;

            if (sx < 0)
            {
                sx += width;
            }
            else if (sx > width - 1)
            {
                sx -= width;
            }

            if (sy < 0)
            {
                sy += height;
            }
            else if (sy > height - 1)
            {
                sy -= height;
            }

            pixels[iy * width + ix] = old_pixels[sy * width + sx];
        }
    }

    delete[] old_pixels;
}

////////////////////////////////////////////////////////////////////////////////

bool RotateCW(int src_width, int src_height, RGBA* pixels)
{
    RGBA* old_pixels = new RGBA[src_width * src_height];
    if (old_pixels == NULL)
        return false;

    memcpy(old_pixels, pixels, src_width * src_height * sizeof(RGBA));
    for (int iy = 0; iy < src_height; iy++)
    {
        for (int ix = 0; ix < src_width; ix++)
        {
            int dx = src_width - iy - 1;
            int dy = ix;
            pixels[dy * src_width + dx] = old_pixels[iy * src_width + ix];
        }
    }

    delete[] old_pixels;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool RotateCCW(int src_width, int src_height, RGBA* pixels)
{
    RGBA* old_pixels = new RGBA[src_width * src_height];
    if (old_pixels == NULL)
        return false;

    memcpy(old_pixels, pixels, src_width * src_height * sizeof(RGBA));
    for (int iy = 0; iy < src_height; iy++)
    {
        for (int ix = 0; ix < src_width; ix++)
        {
            int dx = iy;
            int dy = src_width - ix - 1;
            pixels[dy * src_width + dx] = old_pixels[iy * src_width + ix];
        }
    }

    delete[] old_pixels;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

inline RGBA BlurPixel(int width, int height, RGBA* pixels, int x, int y)
{
    if (x < 0)
    {
        x += width;
    }
    else if (x >= width)
    {
        x -= width;
    }

    if (y < 0)
    {
        y += height;
    }
    else if (y >= height)
    {
        y -= height;
    }

    return pixels[y * width + x];
}

////////////////////////////////////////////////////////////////////////////////

void Blur(int width, int height, RGBA* pixels, int mask_width, int mask_height)
{
    RGBA* old_pixels = new RGBA[width * height];
    memcpy(old_pixels, pixels, width * height * sizeof(RGBA));

    // the greater width/height is, the more blurry the effect
    int mask_xoffset = mask_width / 2;
    int mask_yoffset = mask_height / 2;
    int mask_size = mask_width * mask_height;

#define Blur_pixel(x, y) BlurPixel(width, height, pixels, ix + x, iy + y)
    for (int ix = 0; ix < width; ix++)
    {
        for (int iy = 0; iy < height; iy++)
        {
            int destr = 0;
            int destg = 0;
            int destb = 0;
            int desta = 0;

            for (int my = 0; my < mask_height; my++)
            {
                for (int mx = 0; mx < mask_width; mx++)
                {
                    destr += Blur_pixel(mx - mask_xoffset, my - mask_yoffset).red;
                    destg += Blur_pixel(mx - mask_xoffset, my - mask_yoffset).green;
                    destb += Blur_pixel(mx - mask_xoffset, my - mask_yoffset).blue;
                    desta += Blur_pixel(mx - mask_xoffset, my - mask_yoffset).alpha;
                }
            }
            /*
            destr += Blur_pixel(-1, -1).red;
            destr += Blur_pixel(-1,  0).red;
            destr += Blur_pixel(-1,  1).red;
            destr += Blur_pixel( 0, -1).red;
            destr += Blur_pixel( 0,  0).red;
            destr += Blur_pixel( 0,  1).red;
            destr += Blur_pixel( 1, -1).red;
            destr += Blur_pixel( 1,  0).red;
            destr += Blur_pixel( 1,  1).red;

            destg += Blur_pixel(-1, -1).green;
            destg += Blur_pixel(-1,  0).green;
            destg += Blur_pixel(-1,  1).green;
            destg += Blur_pixel( 0, -1).green;
            destg += Blur_pixel( 0,  0).green;
            destg += Blur_pixel( 0,  1).green;
            destg += Blur_pixel( 1, -1).green;
            destg += Blur_pixel( 1,  0).green;
            destg += Blur_pixel( 1,  1).green;

            destb += Blur_pixel(-1, -1).blue;
            destb += Blur_pixel(-1,  0).blue;
            destb += Blur_pixel(-1,  1).blue;
            destb += Blur_pixel( 0, -1).blue;
            destb += Blur_pixel( 0,  0).blue;
            destb += Blur_pixel( 0,  1).blue;
            destb += Blur_pixel( 1, -1).blue;
            destb += Blur_pixel( 1,  0).blue;
            destb += Blur_pixel( 1,  1).blue;

            desta += Blur_pixel(-1, -1).alpha;
            desta += Blur_pixel(-1,  0).alpha;
            desta += Blur_pixel(-1,  1).alpha;
            desta += Blur_pixel( 0, -1).alpha;
            desta += Blur_pixel( 0,  0).alpha;
            desta += Blur_pixel( 0,  1).alpha;
            desta += Blur_pixel( 1, -1).alpha;
            desta += Blur_pixel( 1,  0).alpha;
            desta += Blur_pixel( 1,  1).alpha;
            */

            pixels[iy * width + ix].red   = destr / mask_size;
            pixels[iy * width + ix].green = destg / mask_size;
            pixels[iy * width + ix].blue  = destb / mask_size;
            pixels[iy * width + ix].alpha = desta / mask_size;
        }
    }

#undef Blur_pixel
    delete[] old_pixels;
}

////////////////////////////////////////////////////////////////////////////////

void Noise(int width, int height, RGBA* pixels)
{
    for (int i = 0; i < width * height; i++)
    {
        int red   = pixels[i].red   + rand() % 33 - 16;
        int green = pixels[i].green + rand() % 33 - 16;
        int blue  = pixels[i].blue  + rand() % 33 - 16;

        // clamp to [0,255]
        red   = (red   < 0 ? 0 : (red   > 255 ? 255 : red));
        green = (green < 0 ? 0 : (green > 255 ? 255 : green));
        blue  = (blue  < 0 ? 0 : (blue  > 255 ? 255 : blue));

        pixels[i].red   = red;
        pixels[i].green = green;
        pixels[i].blue  = blue;
    }
}

////////////////////////////////////////////////////////////////////////////////

void GenerateNegativeImageLookup(unsigned char lookup[256])
{
    for (int i = 0; i < 256; i++)
    {
        lookup[i] = 255 - i;
    }
}

////////////////////////////////////////////////////////////////////////////////

void NegativeImage(int width, int height, bool red, bool green, bool blue, bool alpha, RGBA* pixels)
{
    unsigned char lookup[256];
    GenerateNegativeImageLookup(lookup);

    for (int i = 0; i < width * height; ++i)
    {
        if (red)   pixels[i].red   = lookup[pixels[i].red];
        if (green) pixels[i].green = lookup[pixels[i].green];
        if (blue)  pixels[i].blue  = lookup[pixels[i].blue];
        if (alpha) pixels[i].alpha = lookup[pixels[i].alpha];
    }
}

////////////////////////////////////////////////////////////////////////////////

void GenerateSolarizeLookup(unsigned char lookup[256], int value)
{
    for (int j = 0; j < 256; ++j)
    {
        lookup[j] = (j > value) ? (255 - j) : j;
    }

}

////////////////////////////////////////////////////////////////////////////////

void Solarize(int width, int height, int value, RGBA* pixels)
{
    unsigned char lookup[256];
    GenerateSolarizeLookup(lookup, value);

    for (int i = 0; i < width * height; i++)
    {
        pixels[i].red =   lookup[pixels[i].red];
        pixels[i].green = lookup[pixels[i].green];
        pixels[i].blue =  lookup[pixels[i].blue];
    }
}

////////////////////////////////////////////////////////////////////////////////

void BlendImage(int dest_width, int dest_height, int src_width, int src_height, RGBA* dest_pixels, RGBA* src_pixels)
{
    for (int dx = 0; dx < dest_width; dx++)
    {
        for (int dy = 0; dy < dest_height; dy++)
        {
            if (dx > src_width - 1)
                break;

            if (dy > src_height - 1)
                break;

            // Blend4(dest_pixels[dy * dest_width + dx], src_pixels[dy * src_width + dx], src_pixels[dy * src_width + dx].alpha);
            Blend4(dest_pixels[dy * dest_width + dx], src_pixels[dy * src_width + dx], src_pixels[dy * src_width + dx].alpha);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void GenerateAdjustBrightnessLookup(unsigned char lookup[256], int value)
{
    for (int i = 0; i < 256; ++i)
    {
        int v = i + value;
        // clamp to [0, 255]
        v = (v < 0 ? 0 : (v > 255 ? 255 : v));
        lookup[i] = v;
    }

}

////////////////////////////////////////////////////////////////////////////////

void AdjustBrightness(int width, int height, RGBA* pixels, int dred, int dgreen, int dblue, int dalpha)
{
    unsigned char rlookup[256];
    unsigned char glookup[256];
    unsigned char blookup[256];
    unsigned char alookup[256];

    GenerateAdjustBrightnessLookup(rlookup, dred);
    GenerateAdjustBrightnessLookup(glookup, dgreen);
    GenerateAdjustBrightnessLookup(blookup, dblue);
    GenerateAdjustBrightnessLookup(alookup, dalpha);

    for (int dx = 0; dx < width; ++dx)
    {
        for (int dy = 0; dy < height; ++dy)
        {
            pixels[dy * width + dx].red   = rlookup[pixels[dy * width + dx].red];
            pixels[dy * width + dx].green = glookup[pixels[dy * width + dx].green];
            pixels[dy * width + dx].blue  = blookup[pixels[dy * width + dx].blue];
            pixels[dy * width + dx].alpha = alookup[pixels[dy * width + dx].alpha];
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void GenerateGammaLookup(unsigned char lookup[256], double gamma)
{

    for (int i = 0; i < 256; ++i)
    {
        if (gamma)
        {
            int v = (int) (255.0 * (double) (pow((double) i / (double) 255.0, (double) (1.0/gamma))));
            // clamp to [0, 255]
            v = (v < 0 ? 0 : (v > 255 ? 255 : v));
            lookup[i] = v;
        }
        else
        {
            lookup[i] = 0;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void AdjustGamma(int width, int height, RGBA* pixels, double dred, double dgreen, double dblue, double dalpha)
{
    unsigned char rlookup[256];
    unsigned char glookup[256];
    unsigned char blookup[256];
    unsigned char alookup[256];

    GenerateGammaLookup(rlookup, dred);
    GenerateGammaLookup(glookup, dgreen);
    GenerateGammaLookup(blookup, dblue);
    GenerateGammaLookup(alookup, dalpha);

    for (int dx = 0; dx < width; ++dx)
    {
        for (int dy = 0; dy < height; ++dy)
        {
            pixels[dy * width + dx].red   = rlookup[pixels[dy * width + dx].red];
            pixels[dy * width + dx].green = glookup[pixels[dy * width + dx].green];
            pixels[dy * width + dx].blue  = blookup[pixels[dy * width + dx].blue];
            pixels[dy * width + dx].alpha = alookup[pixels[dy * width + dx].alpha];
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

unsigned long CountColorsUsed(const RGBA* pixels, const int width, const int height,
                              const int x, const int y, const int w, const int h)
{
    unsigned long num_colors = 0;
    unsigned long max_colors = 1 + ((255 * 255) * 255) + (256 * 255) + 255;

    bool* color_map = new bool[max_colors];
    if (color_map)
    {

        for (unsigned long i = 0; i < max_colors; i++)
        {
            color_map[i] = false;
        }

        for (int sy = y; sy < y + h; sy++)
        {
            for (int sx = x; sx < x + w; sx++)
            {
                const RGBA* p = &pixels[(sy * width) + sx];
                bool* found = &color_map[((255 * 255) * p->red) + (256 * p->green) + (p->blue)];
                if (*found == false)
                {
                    num_colors += 1;
                    *found = true;
                }
            }
        }

        delete[] color_map;
    }

    return num_colors;
}

////////////////////////////////////////////////////////////////////////////////
