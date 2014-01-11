#include <stdlib.h>
#include <string.h>
#include "Filters.hpp"


////////////////////////////////////////////////////////////////////////////////

void FlipHorizontally(int width, int height, RGBA* pixels)
{
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width / 2; x++) {
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
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height / 2; y++) {
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

  for (int iy = 0; iy < height; iy++) {
    for (int ix = 0; ix < width; ix++) {
      int sx = ix - dx;
      int sy = iy - dy;

      if (sx < 0) {
        sx += width;
      } else if (sx > width - 1) {
        sx -= width;
      }

      if (sy < 0) {
        sy += height;
      } else if (sy > height - 1) {
        sy -= height;
      }

      pixels[iy * width + ix] = old_pixels[sy * width + sx];
    }
  }

  delete[] old_pixels;
}

////////////////////////////////////////////////////////////////////////////////

void RotateCW(int width, RGBA* pixels)
{
  RGBA* old_pixels = new RGBA[width * width];
  memcpy(old_pixels, pixels, width * width * sizeof(RGBA));

  for (int iy = 0; iy < width; iy++) {
    for (int ix = 0; ix < width; ix++) {
      int dx = width - iy - 1;
      int dy = ix;
      pixels[dy * width + dx] = old_pixels[iy * width + ix];
    }
  }

  delete[] old_pixels;  
}

////////////////////////////////////////////////////////////////////////////////

void RotateCCW(int width, RGBA* pixels)
{
  RGBA* old_pixels = new RGBA[width * width];
  memcpy(old_pixels, pixels, width * width * sizeof(RGBA));

  for (int iy = 0; iy < width; iy++) {
    for (int ix = 0; ix < width; ix++) {
      int dx = iy;
      int dy = width - ix - 1;
      pixels[dy * width + dx] = old_pixels[iy * width + ix];
    }
  }

  delete[] old_pixels;  
}

////////////////////////////////////////////////////////////////////////////////

inline RGBA BlurPixel(int width, int height, RGBA* pixels, int x, int y)
{
  if (x < 0) {
    x += width;
  } else if (x >= width) {
    x -= width;
  }

  if (y < 0) {
    y += height;
  } else if (y >= height) {
    y -= height;
  }

  return pixels[y * width + x];
}

void Blur(int width, int height, RGBA* pixels)
{
  RGBA* old_pixels = new RGBA[width * height];
  memcpy(old_pixels, pixels, width * height * sizeof(RGBA));

#define Blur_pixel(x, y) BlurPixel(width, height, pixels, ix + x, iy + y)

  for (int ix = 0; ix < width; ix++) {
    for (int iy = 0; iy < height; iy++) {
      int destr = 0;
      int destg = 0;
      int destb = 0;
      int desta = 0;

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

      pixels[iy * width + ix].red   = destr / 9;
      pixels[iy * width + ix].green = destg / 9;
      pixels[iy * width + ix].blue  = destb / 9;
      pixels[iy * width + ix].alpha = desta / 9;
    }
  }

#undef Blur_pixel

  delete[] old_pixels;  
}

////////////////////////////////////////////////////////////////////////////////

void Noise(int width, int height, RGBA* pixels)
{
  for (int i = 0; i < width * height; i++) {
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
