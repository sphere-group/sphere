#include "ImageRender.hpp"
#include "Editor.hpp"
#include "../common/minmax.hpp"

void
StretchedBlit(CPaintDC& dc, CDIBSection* blit_tile,
    const double pixel_width, const double pixel_height,
    const int image_width, const int image_height,
    const RGBA* src_pixels, bool show_alpha_mask, RECT* ClientRect,
    int redraw_x, int redraw_y, int redraw_width, int redraw_height,
    int current_x, int current_y)
{
  if (pixel_width <= 0 || pixel_height <= 0
      || image_width <= 0 || image_height <= 0 || !src_pixels)
    return;

  const int totalx = (int)((double)pixel_width * (double)image_width);
  const int totaly = (int)((double)pixel_height * (double)image_height);
  const int offsetx = ClientRect ? (ClientRect->right  - totalx) / 2 - current_x : current_x;
  const int offsety = ClientRect ? (ClientRect->bottom - totaly) / 2 - current_y : current_y;
  const int dib_width  = blit_tile->GetWidth();
  const int dib_height = blit_tile->GetHeight();
  /*
    char string[10000];
    sprintf (string, "%d %d %d %d -- ", redraw_x, redraw_y, redraw_width, redraw_height);
    redraw_width += redraw_x % dib_width;
    redraw_height += redraw_y % dib_height;
    redraw_x -= redraw_x % dib_width;
    redraw_y -= redraw_y % dib_height;
    redraw_width  += dib_width;
    redraw_width  -= redraw_width  % dib_width;
    redraw_height += dib_height;
    redraw_height -= redraw_height % dib_height;
    sprintf (string + strlen(string), "%d %d %d %d -- ", redraw_x, redraw_y, redraw_width, redraw_height);
//    GetStatusBar()->SetWindowText(string);
  */

  // clamp redraw values within image
  if (redraw_x < 0) redraw_x = 0;
  if (redraw_y < 0) redraw_y = 0;
  if (redraw_x + redraw_width > image_width)
    redraw_width = image_width - redraw_x;
  if (redraw_y + redraw_height > image_height)
    redraw_height = image_height - redraw_y;

  if (redraw_width * redraw_height <= (16 * 16)) {
    for (int iy = redraw_y; iy < redraw_y + redraw_height; iy++)
      for (int ix = redraw_x; ix < redraw_x + redraw_width; ix++)
      {
        RGBA color = src_pixels[iy * image_width + ix];
        // opaque
        if (color.alpha == 255 || !show_alpha_mask)
        {
          HBRUSH brush = CreateSolidBrush(RGB(color.red, color.green, color.blue));
          RECT Rect = { (LONG) (ix * pixel_width), (LONG) (iy * pixel_height), (LONG) ((ix + 1) * pixel_width), (LONG) ((iy + 1) * pixel_height) };
          OffsetRect(&Rect, offsetx, offsety);
          FillRect(dc, &Rect, brush);
          DeleteObject(brush);
        }
        // translucent
        else
        {
          // calculate background grid colors
          RGB Color1 = CreateRGB(255, 255, 255);
          RGB Color2 = CreateRGB(128, 128, 128);
          Color1.red   = (color.red   * color.alpha + Color1.red   * (256 - color.alpha)) / 256;
          Color1.green = (color.green * color.alpha + Color1.green * (256 - color.alpha)) / 256;
          Color1.blue  = (color.blue  * color.alpha + Color1.blue  * (256 - color.alpha)) / 256;
          Color2.red   = (color.red   * color.alpha + Color2.red   * (256 - color.alpha)) / 256;
          Color2.green = (color.green * color.alpha + Color2.green * (256 - color.alpha)) / 256;
          Color2.blue  = (color.blue  * color.alpha + Color2.blue  * (256 - color.alpha)) / 256;
          HBRUSH Brush1 = CreateSolidBrush(RGB(Color1.red, Color1.green, Color1.blue));
          HBRUSH Brush2 = CreateSolidBrush(RGB(Color2.red, Color2.green, Color2.blue));
          RECT Rect;
          // draw rectangles
          // upper left
          SetRect(&Rect,
              (int) (ix * pixel_width),
              (int) (iy * pixel_height),
              (int) (ix * pixel_width + pixel_width / 2),
              (int) (iy * pixel_height + pixel_height / 2));
          OffsetRect(&Rect, offsetx, offsety);
          FillRect(dc, &Rect, Brush1);
          // upper right
          SetRect(&Rect,
              (int) (ix * pixel_width + pixel_width / 2),
              (int) (iy * pixel_height),
              (int) (ix * pixel_width + pixel_width),
              (int) (iy * pixel_height + pixel_height / 2));
          OffsetRect(&Rect, offsetx, offsety);
          FillRect(dc, &Rect, Brush2);
          // lower left
          SetRect(&Rect,
              (int) (ix * pixel_width),
              (int) (iy * pixel_height + pixel_height / 2),
              (int) (ix * pixel_width + pixel_width / 2),
              (int) (iy * pixel_height + pixel_height));
          OffsetRect(&Rect, offsetx, offsety);
          FillRect(dc, &Rect, Brush2);
          // lower right
          SetRect(&Rect,
              (int) (ix * pixel_width + pixel_width / 2),
              (int) (iy * pixel_height + pixel_height / 2),
              (int) (ix * pixel_width + pixel_width),
              (int) (iy * pixel_height + pixel_height));
          OffsetRect(&Rect, offsetx, offsety);
          FillRect(dc, &Rect, Brush1);
          DeleteObject(Brush1);
          DeleteObject(Brush2);
        }
      }
    return;
  }

  int num_tiles_x = ((int)((double)pixel_width  * (double)image_width) / dib_width);
  int num_tiles_y = ((int)((double)pixel_height * (double)image_height) / dib_height);
  const RGBA color_mask_1 = CreateRGBA(255, 255, 255, 255);
  const RGBA color_mask_2 = CreateRGBA(192, 192, 192, 255);

  for (int ty = 0; ty <= num_tiles_y; ++ty) {
    for (int tx = 0; tx <= num_tiles_x; ++tx) {
      // clear the DIB
      memset(blit_tile->GetPixels(), 255,  dib_width * dib_height * sizeof(BGRA));
      BGRA* dst_pixels = (BGRA*) blit_tile->GetPixels();
      int current_blit_width = 0;
      int current_blit_height = 0;
      for (int iy = 0; iy < dib_height; ++iy) {
        int sy = (int) ((iy + (ty * dib_height)) / pixel_height);
        if (!(sy >= redraw_y && sy < (redraw_y + redraw_height)))
          continue;

        for (int ix = 0; ix < dib_width; ++ix)
        {
          //for (int l = 0; l < size; ++l)
          {
            int sx = (int) ((ix + (tx * dib_width)) / pixel_width);
            if (!(sx >= redraw_x && sx < (redraw_x + redraw_width)))
              continue;
            int counter = (iy * dib_width) + ix;
            RGBA color = src_pixels[((sy * image_width) + sx)];
 
            if (color.alpha == 255 || !show_alpha_mask ) {
              dst_pixels[counter].red   = color.red;
              dst_pixels[counter].green = color.green;
              dst_pixels[counter].blue  = color.blue;
              dst_pixels[counter].alpha = color.alpha;
            } else {
              RGBA Color1 = color_mask_1;
              RGBA Color2 = color_mask_2;
              Color1.red   = (color.red   * color.alpha + Color1.red   * (256 - color.alpha)) / 256;
              Color1.green = (color.green * color.alpha + Color1.green * (256 - color.alpha)) / 256;
              Color1.blue  = (color.blue  * color.alpha + Color1.blue  * (256 - color.alpha)) / 256;
              Color2.red   = (color.red   * color.alpha + Color2.red   * (256 - color.alpha)) / 256;
              Color2.green = (color.green * color.alpha + Color2.green * (256 - color.alpha)) / 256;
              Color2.blue  = (color.blue  * color.alpha + Color2.blue  * (256 - color.alpha)) / 256;
              Color1.alpha = Color2.alpha = color.alpha;
              if (sx % 2 == 0) {
                if (sy % 2 == 0) {
                  dst_pixels[counter].red = Color2.red;
                  dst_pixels[counter].green = Color2.green;
                  dst_pixels[counter].blue = Color2.blue;
                } else {
                  dst_pixels[counter].red = Color1.red;
                  dst_pixels[counter].green = Color1.green;
                  dst_pixels[counter].blue = Color1.blue;
                }
              } else {
                if (sy % 2 == 0) {
                  dst_pixels[counter].red = Color1.red;
                  dst_pixels[counter].green = Color1.green;
                  dst_pixels[counter].blue = Color1.blue;
                } else {
                  dst_pixels[counter].red = Color2.red;
                  dst_pixels[counter].green = Color2.green;
                  dst_pixels[counter].blue = Color2.blue;
                }
              }
  
            }
          }
          if (iy == 0)
            current_blit_width += 1;
        }      
        current_blit_height++;
      }
      if (current_blit_width && current_blit_height) {
        // render the tile
        dc.BitBlt(offsetx + (tx * dib_width), offsety + (ty * dib_height), current_blit_width, current_blit_height,
                  CDC::FromHandle(blit_tile->GetDC()), 0, 0, SRCCOPY);
      }
    }
  }
}
