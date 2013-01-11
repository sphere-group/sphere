#ifndef CONVOLVE_HPP
#define CONVOLVE_HPP
#include "rgb.hpp"
void
int_convolve_rgba(int x, int y, int w, int h, int width, int height,
                  RGBA* pixels, int mask_width, int mask_height,
                  int mask_xoffset, int mask_yoffset, const int* mask,
                  int divisor, int offset, int wrap,
                  int clamp, int clamp_low, int clamp_high, int infinite,
                  int use_red, int use_green, int use_blue, int use_alpha);
void
double_convolve_rgba(int x, int y, int w, int h, int width, int height,
                     RGBA* pixels, int mask_width, int mask_height,
                     int mask_xoffset, int mask_yoffset, const double* mask,
                     int divisor, int offset, int wrap,
                     int clamp, int clamp_low, int clamp_high, int infinite,
                     int use_red, int use_green, int use_blue, int use_alpha);
#endif
