#ifndef VIDEO_H
#define VIDEO_H
#define EXPORT(ret) extern "C" ret __stdcall
// Driver info
typedef struct
{
    const char* name;
    const char* author;
    const char* date;
    const char* version;
    const char* description;
}
DRIVERINFO;

EXPORT(void) SetClippingRectangle(int x, int y, int w, int h);
EXPORT(void) GetClippingRectangle(int* x, int* y, int* w, int* h);
extern HINSTANCE DriverInstance;
extern int       ScreenWidth;
extern int       ScreenHeight;
extern RECT      ClippingRectangle;
extern void GetDriverConfigFile(char* config_file);
#define calculate_clipping_metrics(width, height)                    \
  int image_offset_x = 0;                                            \
  int image_offset_y = 0;                                            \
  int image_blit_width = width;                                      \
  int image_blit_height = height;                                    \
                                                                     \
  if (x < ClippingRectangle.left)                                    \
  {                                                                  \
    image_offset_x = (ClippingRectangle.left - x);                   \
    image_blit_width -= image_offset_x;                              \
  }                                                                  \
                                                                     \
  if (y < ClippingRectangle.top)                                     \
  {                                                                  \
    image_offset_y = (ClippingRectangle.top - y);                    \
    image_blit_height -= image_offset_y;                             \
  }                                                                  \
                                                                     \
  if (x + (int)width - 1 > ClippingRectangle.right)                  \
    image_blit_width -= (x + width - ClippingRectangle.right - 1);   \
                                                                     \
  if (y + (int)height - 1 > ClippingRectangle.bottom)                \
    image_blit_height -= (y + height - ClippingRectangle.bottom - 1)
// end #define
#endif

