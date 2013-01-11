#ifndef FILTERS_HPP
#define FILTERS_HPP

#include "rgb.hpp"
extern void FlipHorizontally		(int width, int height, RGBA* pixels);
extern void FlipVertically			(int width, int height, RGBA* pixels);
extern void Translate				(int width, int height, RGBA* pixels, int dx, int dy);
extern bool RotateCW				(int src_width, int src_height, RGBA* pixels);
extern bool RotateCCW				(int src_width, int src_height, RGBA* pixels);
extern void Blur					(int width, int height, RGBA* pixels, int mask_width = 3,
									 int mask_height = 3);

extern void Noise					(int width, int height, RGBA* pixels);
extern void NegativeImage			(int width, int height, bool red, bool green, bool blue, 
									 bool alpha, RGBA* pixels);

extern void Solarize				(int width, int height, int value, RGBA* pixels);
extern void BlendImage				(int dest_width, int dest_height, int src_width, int src_height, 
									 RGBA* dest_pixels, RGBA* src_pixels);

extern void AdjustBrightness		(int width, int height, RGBA* pixels, int dred, int dgreen, 
									 int dblue, int dalpha = 0);

extern void AdjustGamma				(int width, int height, RGBA* pixels, double dred, 
									 double dgreen, double dblue, double dalpha = 1);

extern unsigned long CountColorsUsed(const RGBA* pixels, const int width, const int height, 
									 const int x, const int y, const int w, const int h);

#endif
