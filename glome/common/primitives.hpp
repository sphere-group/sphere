// graphics primitives algorithms


#ifndef PRIMITIVES_HPP
#define PRIMITIVES_HPP


#include <algorithm>
#include <math.h>


namespace primitives
{


template<typename destT, typename srcT, typename clipT, typename renderT>
void Point(
  destT* surface,
  int pitch,
  int x,
  int y,
  srcT color,
  clipT clipper,
  renderT renderer)
{
  if (x >= clipper.left && x <= clipper.right &&
      y >= clipper.top  && y <= clipper.bottom) {
    renderer(surface[y * pitch + x], color);
  }
}


template<typename T>
T bracket(T t, T min, T max) {
  if (t < min) {
    return min;
  } else if (t > max) {
    return max;
  } else {
    return t;
  }
}


template<typename T>
T square(T a) {
  return a * a;
}


inline void clip_segment(int& x1, int& y1, int& x2, int& y2, float slope, int left, int right)
{
  // this function assumes x1 < x2
  bool swapped = false;
  if (x1 > x2) {
    std::swap(x1, x2);
    std::swap(y1, y2);
    swapped = true;
  }

  if (x1 < left) {
    y1 -= (int)((x1 - left) * slope);
    x1 = left;
  }

  if (x2 > right) {
    y2 -= (int)((x2 - right) * slope);
    x2 = right;
  }

  // swap them back
  if (swapped) {
    std::swap(x1, x2);
    std::swap(y1, y2);
  }
}


template<typename clipT>
bool is_clipped(int x1, int y1, int x2, int y2, clipT clipper)
{
  return (
    (x1 < clipper.left   && x2 < clipper.left) ||
    (x1 > clipper.right  && x2 > clipper.right) ||
    (y1 < clipper.top    && y2 < clipper.top) ||
    (y1 > clipper.bottom && y2 > clipper.bottom)
  );
}


template<typename destT, typename clipT, typename renderT, typename getcolorT>
void Line(
  destT* surface,
  int pitch,
  int x1,
  int y1,
  int x2,
  int y2,
  getcolorT getcolor,
  clipT clipper,
  renderT renderer)
{
  // if line is completely off screen, don't draw it
  if (is_clipped(x1, y1, x2, y2, clipper)) {
    return;
  }

  // store original coordinates for correct color interpolation
  int o_x1 = x1;
  int o_x2 = x2;
  int o_y1 = y1;
  int o_y2 = y2;

  // vertical (simplified clipping)
  if (x1 == x2) {

    y1 = bracket<int>(y1, clipper.top, clipper.bottom - 1);
    y2 = bracket<int>(y2, clipper.top, clipper.bottom - 1);

    // draw the line
    if (y1 < y2) {
      for (int iy = y1; iy <= y2; iy++) {
        renderer(surface[iy * pitch + x1], getcolor(iy - o_y1, o_y2 - o_y1));
      }
    } else {
      for (int iy = y1; iy >= y2; iy--) {
        renderer(surface[iy * pitch + x1], getcolor(iy - o_y2, o_y1 - o_y2));
      }
    }

  }

  // horizontal (simplified clipping)
  else if (y1 == y2) {

    x1 = bracket<int>(x1, clipper.left, clipper.right - 1);
    x2 = bracket<int>(x2, clipper.left, clipper.right - 1);

    // draw the line
    if (x1 < x2) {
      for (int ix = x1; ix <= x2; ix++) {
        renderer(surface[y1 * pitch + ix], getcolor(ix - o_x1, o_x2 - o_x1));
      }
    } else {
      for (int ix = x1; ix >= x2; ix--) {
        renderer(surface[y1 * pitch + ix], getcolor(ix - o_x2, o_x1 - o_x2));
      }
    }

  }

  // other lines (diagonal)
  else {
    
    float slope = float(y2 - y1) / (x2 - x1);

    clip_segment(x1, y1, x2, y2, slope,     clipper.left, clipper.right);  // x
    if (is_clipped(x1, y1, x2, y2, clipper)) { return; }
    clip_segment(y1, x1, y2, x2, 1 / slope, clipper.top, clipper.bottom);  // y
    if (is_clipped(x1, y1, x2, y2, clipper)) { return; }

    // don't draw it if any points are outside of the clipper (intersection failed)
    if (x1 < clipper.left || x1 > clipper.right ||
        x2 < clipper.left || x2 > clipper.right ||
        y1 < clipper.top || y1 > clipper.bottom ||
        y2 < clipper.top || y2 > clipper.bottom) {
      return;
    }

    // recalculate slope (could be inaccurate)
    slope = float(y2 - y1) / (x2 - x1);

    // if line is more horizontal than vertical...
    if (fabs(slope) < 1) {

      if (x1 < x2) {
        float cy = y1;
        for (int ix = x1; ix <= x2; ix++) {
          renderer(surface[(int)cy * pitch + ix], getcolor(o_x2 - ix, o_x2 - o_x1));
          cy += slope;
        }
      } else {
        float cy = y1;
        for (int ix = x1; ix >= x2; ix--) {
          renderer(surface[(int)cy * pitch + ix], getcolor(ix - o_x2, o_x1 - o_x2));
          cy -= slope;
        }
      }
    
    } else {   // more vertical than horizontal...

      if (y1 < y2) {
        float cx = x1;
        for (int iy = y1; iy <= y2; iy++) {
          renderer(surface[iy * pitch + (int)cx], getcolor(o_y1 - iy, o_y2 - o_y1));
          cx += 1 / slope;
        }
      } else {
        float cx = x1;
        for (int iy = y1; iy >= y2; iy--) {
          renderer(surface[iy * pitch + (int)cx], getcolor(iy - o_y2, o_y1 - o_y2));
          cx -= 1 / slope;
        }
      }
    
    }
  }

}


template<typename T>
T minimum(T a, T b) {
  return (a < b ? a : b);
}

template<typename T>
T maximum(T a, T b) {
  return (a > b ? a : b);
}


template<typename destT, typename sourceT, typename clipT, typename renderT>
void Triangle(
  destT* surface,
  int pitch,
  const int x[3],
  const int y[3],
  sourceT color,
  clipT clipper,
  renderT renderer)
{
  // sort the vertices (ascending with respect to y)
  int sorted_x[3] = { x[0], x[1], x[2] };
  int sorted_y[3] = { y[0], y[1], y[2] };

  // completely unrolled three-element bubble sort
  if (sorted_y[0] > sorted_y[1]) {
    std::swap(sorted_y[0], sorted_y[1]);
    std::swap(sorted_x[0], sorted_x[1]);
  }
  if (sorted_y[1] > sorted_y[2]) {
    std::swap(sorted_y[1], sorted_y[2]);
    std::swap(sorted_x[1], sorted_x[2]);
  }
  if (sorted_y[0] > sorted_y[1]) {
    std::swap(sorted_y[0], sorted_y[1]);
    std::swap(sorted_x[0], sorted_x[1]);
  }

  bool has_top    = (sorted_y[0] != sorted_y[1]);
  bool has_bottom = (sorted_y[1] != sorted_y[2]);

  if (has_top) {
    // find top point
    int top_x = sorted_x[0];
    int top_y = sorted_y[0];

    // find middle points (bottom of top half)
    int mid_x1;
    int mid_x2;
    if (sorted_y[1] == sorted_y[2]) {
      mid_x1 = sorted_x[1];
      mid_x2 = sorted_x[2];
    } else {
      float slope = float(sorted_x[2] - sorted_x[0]) / (sorted_y[2] - sorted_y[0]);
      mid_x1 = sorted_x[1];
      mid_x2 = (int)(sorted_x[0] + slope * (sorted_y[1] - sorted_y[0]));
    }
    int mid_y = sorted_y[1];

    // make sure mid_x1 < mid_x2
    if (mid_x1 > mid_x2) {
      std::swap(mid_x1, mid_x2);
    }

    // render all scanlines
    int min_y = maximum<int>(clipper.top, top_y);
    int max_y = minimum<int>(clipper.bottom, mid_y);
    for (int iy = min_y; iy < max_y; iy++) {

      // calculate edges of scanline
      int right_round = max_y - min_y - 1;
      int left_x  = top_x + (mid_x1 - top_x) * (iy - min_y) / (max_y - min_y);
      int right_x = top_x + ((mid_x2 - top_x) * (iy - min_y) + right_round) / (max_y - min_y);

      // clip edges of scanline
      left_x = maximum<int>(left_x, clipper.left);
      right_x = minimum<int>(right_x, clipper.right);

      // render the scanline
      for (int ix = left_x; ix <= right_x; ix++) {
        renderer(surface[iy * pitch + ix], color);
      }
    }

  }

  if (has_bottom) {
    // find middle points (top of bottom half)
    int mid_x1;
    int mid_x2;
    if (sorted_y[0] == sorted_y[1]) {
      mid_x1 = sorted_x[0];
      mid_x2 = sorted_x[1];
    } else {
      float slope = float(sorted_x[2] - sorted_x[0]) / (sorted_y[2] - sorted_y[0]);
      mid_x1 = sorted_x[1];
      mid_x2 = (int)(sorted_x[2] + slope * (sorted_y[1] - sorted_y[2]));
    }
    int mid_y = sorted_y[1];

    // make sure mid_x1 < mid_x2
    if (mid_x1 > mid_x2) {
      std::swap(mid_x1, mid_x2);
    }

    // find bottom point
    int bottom_x = sorted_x[2];
    int bottom_y = sorted_y[2];

    // render all scanlines
    int min_y = maximum<int>(clipper.top, mid_y);
    int max_y = minimum<int>(clipper.bottom, bottom_y);
    for (int iy = min_y; iy < max_y; iy++) {
      
      // calculate edges of scanline
      // calculate edges of scanline
      int right_round = max_y - min_y - 1;
      int left_x  = bottom_x + (mid_x1 - bottom_x) * (max_y - iy) / (max_y - min_y);
      int right_x = bottom_x + ((mid_x2 - bottom_x) * (max_y - iy) + right_round) / (max_y - min_y);
      
      // clip edges of scanline
      left_x = maximum<int>(left_x, clipper.left);
      right_x = minimum<int>(right_x, clipper.right);

      // render the scanline
      for (int ix = left_x; ix <= right_x; ix++) {
        renderer(surface[iy * pitch + ix], color);
      }
    }
  }
}


template<typename destT, typename sourceT, typename clipT, typename renderT, typename interpolateT>
void GradientTriangle(
  destT* surface,
  int pitch,
  const int x[3],
  const int y[3],
  const sourceT colors[3],
  clipT clipper,
  renderT renderer,
  interpolateT interpolator)
{
  // sort the vertices (ascending with respect to y)
  int sorted_x[3] = { x[0], x[1], x[2] };
  int sorted_y[3] = { y[0], y[1], y[2] };
  sourceT sorted_c[3];
  sorted_c[0] = colors[0];
  sorted_c[1] = colors[1];
  sorted_c[2] = colors[2];

  // completely unrolled three-element bubble sort
  if (sorted_y[0] > sorted_y[1]) {
    std::swap(sorted_y[0], sorted_y[1]);
    std::swap(sorted_x[0], sorted_x[1]);
    std::swap(sorted_c[0], sorted_c[1]);
  }
  if (sorted_y[1] > sorted_y[2]) {
    std::swap(sorted_y[1], sorted_y[2]);
    std::swap(sorted_x[1], sorted_x[2]);
    std::swap(sorted_c[1], sorted_c[2]);
  }
  if (sorted_y[0] > sorted_y[1]) {
    std::swap(sorted_y[0], sorted_y[1]);
    std::swap(sorted_x[0], sorted_x[1]);
    std::swap(sorted_c[0], sorted_c[1]);
  }

  bool has_top    = (sorted_y[0] != sorted_y[1]);
  bool has_bottom = (sorted_y[1] != sorted_y[2]);

  if (has_top) {
    // find top point
    int top_x = sorted_x[0];
    int top_y = sorted_y[0];
    sourceT top_c = sorted_c[0];

    // find middle points (bottom of top half)
    int mid_x1;
    int mid_x2;
    sourceT mid_c1;
    sourceT mid_c2;
    if (sorted_y[1] == sorted_y[2]) {
      mid_x1 = sorted_x[1];
      mid_x2 = sorted_x[2];
      mid_c1 = sorted_c[1];
      mid_c2 = sorted_c[2];
    } else {
      float slope = float(sorted_x[2] - sorted_x[0]) / (sorted_y[2] - sorted_y[0]);
      mid_x1 = sorted_x[1];
      mid_x2 = (int)(sorted_x[0] + slope * (sorted_y[1] - sorted_y[0]));
      mid_c1 = sorted_c[1];
      mid_c2 = interpolator(sorted_c[0], sorted_c[2], sorted_y[1] - sorted_y[0], sorted_y[2] - sorted_y[0]);
    }
    int mid_y = sorted_y[1];

    // make sure mid_x1 < mid_x2
    if (mid_x1 > mid_x2) {
      std::swap(mid_x1, mid_x2);
      std::swap(mid_c1, mid_c2);
    }

    // render all scanlines
    int min_y = maximum<int>(clipper.top, top_y);
    int max_y = minimum<int>(clipper.bottom, mid_y);
    for (int iy = min_y; iy < max_y; iy++) {
      
      // calculate edges of scanline
      int right_round = max_y - min_y - 1;
      int left_x  = top_x + (mid_x1 - top_x) * (iy - min_y) / (max_y - min_y);
      int right_x = top_x + ((mid_x2 - top_x) * (iy - min_y) + right_round) / (max_y - min_y);

      int o_lx = left_x; // store originals for correct color interpolation
      int o_rx = right_x;

      // calculate colors
      sourceT left_c  = interpolator(top_c, mid_c1, iy - top_y, mid_y - top_y);
      sourceT right_c = interpolator(top_c, mid_c2, iy - top_y, mid_y - top_y);

      // clip edges of scanline
      left_x = maximum<int>(left_x, clipper.left);
      right_x = minimum<int>(right_x, clipper.right);

      // render the scanline
      for (int ix = left_x; ix <= right_x; ix++) {
        renderer(surface[iy * pitch + ix], interpolator(left_c, right_c, ix - o_lx, o_rx - o_lx));
      }
    }

  }

  if (has_bottom) {
    // find middle points (top of bottom half)
    int mid_x1;
    int mid_x2;
    sourceT mid_c1;
    sourceT mid_c2;
    if (sorted_y[0] == sorted_y[1]) {
      mid_x1 = sorted_x[0];
      mid_x2 = sorted_x[1];
      mid_c1 = sorted_c[0];
      mid_c2 = sorted_c[1];
    } else {
      float slope = float(sorted_x[2] - sorted_x[0]) / (sorted_y[2] - sorted_y[0]);
      mid_x1 = sorted_x[1];
      mid_x2 = (int)(sorted_x[2] + slope * (sorted_y[1] - sorted_y[2]));
      mid_c1 = sorted_c[1];
      mid_c2 = interpolator(sorted_c[0], sorted_c[2], sorted_y[1] - sorted_y[0], sorted_y[2] - sorted_y[0]);
    }
    int mid_y = sorted_y[1];

    // make sure mid_x1 < mid_x2
    if (mid_x1 > mid_x2) {
      std::swap(mid_x1, mid_x2);
      std::swap(mid_c1, mid_c2);
    }

    // find bottom point
    int bottom_x = sorted_x[2];
    int bottom_y = sorted_y[2];
    sourceT bottom_c = sorted_c[2];

    // render all scanlines
    int min_y = maximum<int>(clipper.top, mid_y);
    int max_y = minimum<int>(clipper.bottom, bottom_y);
    for (int iy = min_y; iy < max_y; iy++) {

      // calculate edges of scanline
      int right_round = max_y - min_y - 1;
      int left_x  = bottom_x + (mid_x1 - bottom_x) * (max_y - iy) / (max_y - min_y);
      int right_x = bottom_x + ((mid_x2 - bottom_x) * (max_y - iy) + right_round) / (max_y - min_y);

      int o_lx = left_x; // store originals for correct color interpolation
      int o_rx = right_x;

      // calculate colors
      sourceT left_c  = interpolator(bottom_c, mid_c1, bottom_y - iy, bottom_y - mid_y);
      sourceT right_c = interpolator(bottom_c, mid_c2, bottom_y - iy, bottom_y - mid_y);

      // clip edges of scanline
      left_x = maximum<int>(left_x, clipper.left);
      right_x = minimum<int>(right_x, clipper.right);

      // render the scanline
      for (int ix = left_x; ix <= right_x; ix++) {
        renderer(surface[iy * pitch + ix], interpolator(left_c, right_c, ix - o_lx, o_rx - o_lx));
      }
    }
  }
}


template<typename destT, typename sourceT, typename clipT, typename renderT>
void Rectangle(
  destT* surface,
  int pitch,
  int x,
  int y,
  int w,
  int h,
  sourceT color,
  clipT clipper,
  renderT renderer)
{
  // if rectangle is completely out, don't bother with it
  if (x + w <= clipper.left ||
      y + h <= clipper.top ||
      x > clipper.right ||
      y > clipper.bottom) {
    return;
  }

  // clip left
  if (x < clipper.left) {
    w -= (clipper.left - x);
    x += (clipper.left - x);
  }

  // clip top
  if (y < clipper.top) {
    h -= (clipper.top - y);
    y += (clipper.top - y);
  }

  // clip right
  if (x + w > clipper.right + 1) {
    w -= (x + w - clipper.right - 1);
  }

  // clip bottom
  if (y + h > clipper.bottom + 1) {
    h -= (y + h - clipper.bottom - 1);
  }

  for (int iy = y; iy < y + h; iy++) {
    for (int ix = x; ix < x + w; ix++) {
      renderer(surface[iy * pitch + ix], color);
    }
  }
}


template<typename destT, typename sourceT, typename clipT, typename renderT, typename interpolateT>
void GradientRectangle(
  destT* surface,
  int pitch,
  int x,
  int y,
  int w,
  int h,
  const sourceT colors[4],
  clipT clipper,
  renderT renderer,
  interpolateT interpolator)
{
  // if rectangle is completely out, don't bother with it
  if (x + w <= clipper.left ||
      y + h <= clipper.top ||
      x > clipper.right ||
      y > clipper.bottom) {
    return;
  }

  // original coordinates for correct color interpolation
  int o_x = x;
  int o_y = y;
  int o_w = w;
  int o_h = h;

  // clip left
  if (x < clipper.left) {
    w -= (clipper.left - x);
    x += (clipper.left - x);
  }

  // clip top
  if (y < clipper.top) {
    h -= (clipper.top - y);
    y += (clipper.top - y);
  }

  // clip right
  if (x + w > clipper.right + 1) {
    w -= (x + w - clipper.right - 1);
  }

  // clip bottom
  if (y + h > clipper.bottom + 1) {
    h -= (y + h - clipper.bottom - 1);
  }

  // render the triangle
  for (int iy = y; iy < y + h; iy++) {

    // calculate edges of scanline
    sourceT left  = interpolator(colors[0], colors[3], iy - o_y, o_h);
    sourceT right = interpolator(colors[1], colors[2], iy - o_y, o_h);
    
    // draw scanline
    for (int ix = x; ix < x + w; ix++) {
      renderer(surface[iy * pitch + ix], interpolator(left, right, ix - o_x, o_w));
    }
  }
}


template<typename destT, typename srcT, typename alphaT, typename clipT, typename renderT>
void TexturedQuad(
  destT* surface,
  int pitch,
  const int x[4],
  const int y[4],
  srcT* texture,
  alphaT* mask,
  int tex_width,
  int tex_height,
  clipT clipper,
  renderT renderer)
{
  // find top and bottom points
  int top = 0;
  int bottom = 0;
  for (int i = 1; i < 4; i++) {
    if (y[i] < y[top]) {
      top = i;
    }

    if (y[i] > y[bottom]) {
      bottom = i;
    }
  }
  
  // perform clipping in the y axis
  int oldMinY = y[top];
  int oldMaxY = y[bottom];
  int minY = bracket<int>(y[top],    clipper.top, clipper.bottom);
  int maxY = bracket<int>(y[bottom], clipper.top, clipper.bottom);

  // precalculate line segment information
  struct segment {

    // y1 < y2, always

    int    x1, x2;
    int    y1, y2;
    int    u1, u2;
    int    v1, v2;

  } segments[4];

  // segment 0 = top
  // segment 1 = right
  // segment 2 = bottom
  // segment 3 = left

  for (int i = 0; i < 4; i++) {
    segment* s = segments + i;

    int p1 = i;
    int p2 = (i + 1) & 3;  // x & 3 == x % 4

    s->x1 = x[p1];
    s->y1 = y[p1];
    s->u1 = (i == 1 || i == 2 ? tex_width  - 1 : 0);
    s->v1 = (i == 2 || i == 3 ? tex_height - 1 : 0);

    s->x2 = x[p2];
    s->y2 = y[p2];
    s->u2 = (i == 0 || i == 1 ? tex_width  - 1 : 0);
    s->v2 = (i == 1 || i == 2 ? tex_height - 1 : 0);

    if (y[p1] > y[p2]) {
      std::swap(s->x1, s->x2);
      std::swap(s->y1, s->y2);
      std::swap(s->u1, s->u2);
      std::swap(s->v1, s->v2);
    }
  }

  // draw scanlines
  for (int iy = minY; iy <= maxY; iy++) {

    // find minimum and maximum x values

    // initial boundary values
    int minX = clipper.right + 1;
    int maxX = clipper.left - 1;

    // default values in case no 
    int minU = 0;
    int minV = 0;
    int maxU = 0;
    int maxV = 0;

    // intersect iy in each line
    for (int i = 0; i < 4; i++) {
      segment* s = segments + i;

      // if iy is even in the segment and segment's length is not zero
      if (s->y1 <= iy && iy <= s->y2) {

        int x = (s->y1 == s->y2 ?
          s->x1 :
          s->x1 + (iy - s->y1) * (s->x2 - s->x1) / (s->y2 - s->y1));
        int u = (s->y1 == s->y2 ?
          s->u1 :
          s->u1 + (iy - s->y1) * (s->u2 - s->u1) / (s->y2 - s->y1));
        int v = (s->y1 == s->y2 ?
          s->v1 :
          s->v1 + (iy - s->y1) * (s->v2 - s->v1) / (s->y2 - s->y1));

        // update minimum and maximum x values

        if (x < minX) {
          minX = x;
          minU = u;
          minV = v;
        }

        if (x > maxX) {
          maxX = x;
          maxU = u;
          maxV = v;
        }
      }
    }

    // now clip the x extents
    int oldMinX = minX;
    int oldMaxX = maxX;
    minX = bracket<int>(minX, clipper.left, clipper.right);
    maxX = bracket<int>(maxX, clipper.left, clipper.right);
   
    // render the scanline pixels
    if (minX == maxX) {

      renderer(surface[iy * pitch + minX], texture[minV * tex_width + minU], mask[minV * tex_width + minU]);

    } else {

      for (int ix = minX; ix <= maxX; ix++) {

        int iu = minU + (ix - oldMinX) * (maxU - minU) / (oldMaxX - oldMinX);
        int iv = minV + (ix - oldMinX) * (maxV - minV) / (oldMaxX - oldMinX);

        renderer(surface[iy * pitch + ix], texture[iv * tex_width + iu], mask[iv * tex_width + iu]);
      }
    }

  } // end for scanlines

}


template<typename pixelT, typename alphaT, typename clipT, typename renderT>
void Blit(
  pixelT* surface,
  int pitch,
  const int x,
  const int y,
  pixelT* texture,
  alphaT* alpha,
  int tex_width,
  int tex_height,
  clipT clipper,
  renderT renderer)
{
  int image_offset_x = 0;
  int image_offset_y = 0;
  int image_blit_width = tex_width;
  int image_blit_height = tex_height;

  if (x < clipper.left) {
    image_offset_x = (clipper.left - x);
    image_blit_width -= image_offset_x;
  }

  if (y < clipper.top) {
    image_offset_y = (clipper.top - y);
    image_blit_height -= image_offset_y;
  }
  
  if (x + (int)tex_width - 1 > clipper.right) {
    image_blit_width -= (x + tex_width - clipper.right - 1);
  }

  if (y + (int)tex_height - 1 > clipper.bottom) {
    image_blit_height -= (y + tex_height - clipper.bottom - 1);
  }

  // heh, funny abbreviations
  pixelT* dst = surface + (y + image_offset_y) * pitch     + image_offset_x + x;
  pixelT* src = texture +       image_offset_y * tex_width + image_offset_x;
  alphaT* alf = alpha   +       image_offset_y * tex_width + image_offset_x;

  int dst_inc = pitch     - image_blit_width;
  int src_inc = tex_width - image_blit_width;

  int iy = image_blit_height;
  while (iy-- > 0) {
    int ix = image_blit_width;
    while (ix-- > 0) {
      renderer(*dst, *src, *alf);
      dst++;
      src++;
      alf++;
    }

    dst += dst_inc;
    src += src_inc;
    alf += src_inc;
  }
}


} // end namespace


#endif
