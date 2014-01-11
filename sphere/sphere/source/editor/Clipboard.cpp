#include "Clipboard.hpp"
#include <windows.h>
static unsigned int s_ClipboardFormat;

///////////////////////////////////////////////////////////////////////////////

CClipboard::CClipboard()
{
  s_ClipboardFormat = RegisterClipboardFormat("FlatImage32");
}

///////////////////////////////////////////////////////////////////////////////

bool
CClipboard::IsFlatImageOnClipbard()
{
  if (IsClipboardFormatAvailable(s_ClipboardFormat))
    return true;
  return false;
}

///////////////////////////////////////////////////////////////////////////////

RGBA*
CClipboard::GetFlatImageFromClipboard(int& width, int& height)
{
  width = 0;
  height = 0;
  HGLOBAL memory = (HGLOBAL)GetClipboardData(s_ClipboardFormat);
  if (memory == NULL) {
    return NULL;
  }
  // get the height and pixels from the clipboard
  dword* ptr = (dword*)GlobalLock(memory);
  if (ptr == NULL) {
    return NULL;
  }
  width = *ptr++;
  height = *ptr++;
  if (width <= 0 || height <= 0) {
    width = 0;
    height = 0;
    return NULL;
  }
  RGBA* clipboard = (RGBA*)ptr;
  RGBA* pixels = new RGBA[width * height];
  if (pixels == NULL) {
    width = 0;
    height = 0;
    return NULL;
  }
  memcpy(pixels, clipboard, width * height * sizeof(RGBA));
  GlobalUnlock(memory);
  return pixels;
}

///////////////////////////////////////////////////////////////////////////////

bool
CClipboard::PutFlatImageOntoClipboard(int width, int height, const RGBA* flat_pixels)
{
  if (width <= 0 || height <= 0) {
    return false;
  }
  // copy the image as a flat 32-bit color image
  HGLOBAL memory = GlobalAlloc(GHND, 8 + width * height * 4);
  if (memory == NULL) {
    return false;
  }
  dword* ptr = (dword*)GlobalLock(memory);
  if (ptr == NULL) {
    return false;
  }
  *ptr++ = width;
  *ptr++ = height;
  memcpy(ptr, flat_pixels, width * height * sizeof(RGBA));
  // put the image on the clipboard
  GlobalUnlock(memory);
  SetClipboardData(s_ClipboardFormat, memory);
  return true;
}

///////////////////////////////////////////////////////////////////////////////

bool
CClipboard::IsBitmapImageOnClipboard() {
  if (IsClipboardFormatAvailable(CF_BITMAP))
    return true;
  return false;
}

///////////////////////////////////////////////////////////////////////////////

RGBA*
CClipboard::GetBitmapImageFromClipboard(int& width, int& height)
{
  width = 0;
  height = 0;
  HBITMAP bitmap = (HBITMAP)GetClipboardData(CF_BITMAP);
  if (bitmap == NULL) {
    return NULL;
  }
  BITMAP b;
  GetObject(bitmap, sizeof(b), &b);
  HDC dc = CreateCompatibleDC(NULL);
  HBITMAP oldbitmap = (HBITMAP)SelectObject(dc, bitmap);
  width  = b.bmWidth;
  height = b.bmHeight;
  if (width <= 0 || height <= 0) {
    width = 0;
    height = 0;
    return NULL;
  }
  RGBA* pixels = new RGBA[width * height];
  if (pixels == NULL) {
    width = 0;
    height = 0;
    return NULL;
  }
  memset(pixels, 255, sizeof(RGBA) * width * height);
  // todo: GetPixel/GetR/G/BValue are extremely slow, fix it!
  RGBA* ptr = pixels;
  for (int iy = 0; iy < height; iy++) {
    for (int ix = 0; ix < width; ix++)
    {
      COLORREF pixel = GetPixel(dc, ix, iy);
      if (pixel != CLR_INVALID) {
        ptr->red   = GetRValue(pixel);
        ptr->green = GetGValue(pixel);
        ptr->blue  = GetBValue(pixel);
      }
      ptr++;
    }
  }
  SelectObject(dc, oldbitmap);
  DeleteDC(dc);
  return pixels;
}

///////////////////////////////////////////////////////////////////////////////

bool
CClipboard::PutBitmapImageOntoClipboard(int width, int height, const RGBA* source)
{
  // ADD DIB
  // create a pixel array to initialize the bitmap
  BGRA* pixels = new BGRA[width * height];
  if (pixels == NULL) {
    return false;
  }
  for (int iy = 0; iy < height; iy++) {
    for (int ix = 0; ix < width; ix++)
    {
      pixels[(height - iy - 1) * (width) + ix].red   = source[iy * width + ix].red;
      pixels[(height - iy - 1) * (width) + ix].green = source[iy * width + ix].green;
      pixels[(height - iy - 1) * (width) + ix].blue  = source[iy * width + ix].blue;
      pixels[(height - iy - 1) * (width) + ix].alpha = source[iy * width + ix].alpha;
    }
  }
  // create the bitmap
  HBITMAP bitmap = CreateBitmap(width, height, 1, 32, pixels);
  BITMAPINFOHEADER header;
  memset((void*)&header, 0, sizeof(header));
  header.biSize = sizeof(header);
  header.biWidth = width;
  header.biHeight = height;
  header.biPlanes = 1;
  header.biBitCount = 32;
  header.biCompression = BI_RGB;
  header.biSizeImage = 0;
  header.biXPelsPerMeter = 0;
  header.biYPelsPerMeter = 0;
  header.biClrUsed = 0;
  header.biClrImportant = 0;
  HGLOBAL hDIB = GlobalAlloc(GHND, sizeof(header) + width * height * 4);
  char* dibPtr = (char*)GlobalLock(hDIB);
  if (dibPtr == NULL) {
    CloseClipboard();
    delete[] pixels;
    return false;
  }
  
  memcpy(dibPtr, &header, sizeof(header));
  memcpy(dibPtr+sizeof(header), pixels, width * height * 4);
  GlobalUnlock(hDIB);
  // put the bitmap in the clipboard
  SetClipboardData(CF_DIB, hDIB);
	delete[] pixels;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
