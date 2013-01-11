#ifndef FILTERS_HPP
#define FILTERS_HPP


#include "rgb.hpp"


extern void FlipHorizontally(int width, int height, RGBA* pixels);
extern void FlipVertically(int width, int height, RGBA* pixels);
extern void Translate(int width, int height, RGBA* pixels, int dx, int dy);
extern void RotateCW(int width, RGBA* pixels);
extern void RotateCCW(int width, RGBA* pixels);
extern void Blur(int width, int height, RGBA* pixels);
extern void Noise(int width, int height, RGBA* pixels);


#endif
