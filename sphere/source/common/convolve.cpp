#include <stdlib.h>
#include <string.h>
#include "convolve.hpp"
#define CONVOLUTION_PIXEL_TYPE RGBA
#define CONVOLUTION_PIXEL_SIZE sizeof(RGBA)
#define CONVOLUTION_PIXEL_CHANNEL_1 old_pixels[py * width + px].red
#define CONVOLUTION_PIXEL_CHANNEL_2 old_pixels[py * width + px].green
#define CONVOLUTION_PIXEL_CHANNEL_3 old_pixels[py * width + px].blue
#define CONVOLUTION_PIXEL_CHANNEL_4 old_pixels[py * width + px].alpha
#define CONVOLUTION_CURRENT_PIXEL_CHANNEL_1 pixels[sy * width + sx].red
#define CONVOLUTION_CURRENT_PIXEL_CHANNEL_2 pixels[sy * width + sx].green
#define CONVOLUTION_CURRENT_PIXEL_CHANNEL_3 pixels[sy * width + sx].blue
#define CONVOLUTION_CURRENT_PIXEL_CHANNEL_4 pixels[sy * width + sx].alpha
#define CONVOLUTION_TYPE int
#define CONVOLUTION_NAME int_convolve_rgba
#include "convolve.c"
#undef CONVOLUTION_TYPE
#undef CONVOLUTION_NAME
#define CONVOLUTION_TYPE double
#define CONVOLUTION_NAME double_convolve_rgba
#include "convolve.c"
#undef CONVOLUTION_TYPE
#undef CONVOLUTION_NAME
#undef CONVOLUTION_PIXEL_TYPE
#undef CONVOLUTION_PIXEL_SIZE
#undef CONVOLUTION_PIXEL_CHANNEL_1
#undef CONVOLUTION_PIXEL_CHANNEL_2
#undef CONVOLUTION_PIXEL_CHANNEL_3
#undef CONVOLUTION_PIXEL_CHANNEL_4
#undef CONVOLUTION_CURRENT_PIXEL_CHANNEL_1
#undef CONVOLUTION_CURRENT_PIXEL_CHANNEL_2
#undef CONVOLUTION_CURRENT_PIXEL_CHANNEL_3
#undef CONVOLUTION_CURRENT_PIXEL_CHANNEL_4
