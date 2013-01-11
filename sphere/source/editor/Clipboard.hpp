#ifndef CCLIPBOARD_HPP
#define CCLIPBOARD_HPP
#include "../common/rgb.hpp"
class CClipboard
{
public:
  CClipboard();
public:
  bool IsBitmapImageOnClipboard();
  bool IsFlatImageOnClipbard();
  RGBA* GetFlatImageFromClipboard(int& width, int& height);
  bool PutFlatImageOntoClipboard(int width, int height, const RGBA* flat_pixels);
  RGBA* GetBitmapImageFromClipboard(int& width, int& height);
  bool PutBitmapImageOntoClipboard(int width, int height, const RGBA* pixels);
};
#endif
