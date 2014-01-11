//graphics primitives algorithms
#ifndef PRIMITIVES_HPP
#define PRIMITIVES_HPP

#include <algorithm>
#include <math.h>

#include "VectorStructs.hpp"

namespace primitives
{
    template<typename clipT>
    inline bool is_inside_clipper(int x, int y, clipT clipper)
    {
        return (
                   (x >= clipper.left)  &&
                   (x <= clipper.right) &&
                   (y >= clipper.top)   &&
                   (y <= clipper.bottom)
               );
    }

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
                y >= clipper.top  && y <= clipper.bottom)
        {
            renderer(surface[y * pitch + x], color);
        }
    }

    template<typename destT, typename srcT, typename clipT, typename renderT>
    void PointSeries(
        destT* surface,
        int pitch,
        VECTOR_INT** points,
        int length,
        srcT color,
        clipT clipper,
        renderT renderer)
    {
        for (int i = 0; i < length; i++)
        {
            if (points[i]->x >= clipper.left &&
                points[i]->x <= clipper.right &&
                points[i]->y >= clipper.top  &&
                points[i]->y <= clipper.bottom)
            {
                renderer(surface[points[i]->y * pitch + points[i]->x], color);
            }
        }
    }

    template<typename T>
    T bracket(T t, T min, T max)
    {
		return t<min? min: t>max? max: t;
    }

    template<typename T>
    T square(T a)
    {
        return a * a;
    }

    inline void clip_segment(int& x1, int& y1, int& x2, int& y2, float slope, int left, int right)
    {
        // this function assumes x1 < x2
        bool swapped = false;
        if (x1 > x2)
        {
            std::swap(x1, x2);
            std::swap(y1, y2);
            swapped = true;
        }

        if (x1 < left)
        {
            y1 -= (int)((x1 - left) * slope);
            x1 = left;
        }

        if (x2 > right)
        {
            y2 -= (int)((x2 - right) * slope);
            x2 = right;
        }

        // swap them back
        if (swapped)
        {
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

#include <assert.h>
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
        assert(clipper.right <= pitch - 1);

        // if line is completely off screen, don't draw it
        if (is_clipped(x1, y1, x2, y2, clipper))
        {
            return;
        }

        if (clipper.right == 0 || clipper.bottom == 0)
        {
            return;
        }

        // store original coordinates for correct color interpolation
        int o_x1 = x1;
        int o_x2 = x2;
        int o_y1 = y1;
        int o_y2 = y2;

        // vertical (simplified clipping)
        if (x1 == x2)
        {

            y1 = bracket<int>(y1, clipper.top, clipper.bottom /* - 1 */);
            y2 = bracket<int>(y2, clipper.top, clipper.bottom /* - 1 */);

            // draw the line
            if (y1 < y2)
            {
                for (int iy = y1; iy <= y2; iy++)
                {
                    renderer(surface[(iy * pitch) + x1], getcolor(iy - o_y1, o_y2 - o_y1));
                }
            }
            else
            {
                for (int iy = y1; iy >= y2; iy--)
                {
                    renderer(surface[(iy * pitch) + x1], getcolor(o_y1 - iy, o_y1 - o_y2));
                }
            }

        }
        // horizontal (simplified clipping)
        else if (y1 == y2)
        {

            x1 = bracket<int>(x1, clipper.left, clipper.right /* - 1 */);
            x2 = bracket<int>(x2, clipper.left, clipper.right /* - 1 */);

            // draw the line
            if (x1 < x2)
            {
                for (int ix = x1; ix <= x2; ix++)
                {
                    renderer(surface[(y1 * pitch) + ix], getcolor(ix - o_x1, o_x2 - o_x1));
                }
            }
            else
            {
                for (int ix = x1; ix >= x2; ix--)
                {
                    renderer(surface[(y1 * pitch) + ix], getcolor(o_x1 - ix, o_x1 - o_x2));
                }
            }

        }
        // other lines (diagonal)
        else
        {

            float slope = float(y2 - y1) / (x2 - x1);
            clip_segment(x1, y1, x2, y2, slope,     clipper.left, clipper.right);  // x
            if (is_clipped(x1, y1, x2, y2, clipper))
            {
                return;
            }
            clip_segment(y1, x1, y2, x2, 1 / slope, clipper.top, clipper.bottom);  // y
            if (is_clipped(x1, y1, x2, y2, clipper))
            {
                return;
            }

            // don't draw it if any points are outside of the clipper (intersection failed)
            if (x1 < clipper.left || x1 > clipper.right ||
                    x2 < clipper.left || x2 > clipper.right ||
                    y1 < clipper.top || y1 > clipper.bottom ||
                    y2 < clipper.top || y2 > clipper.bottom)
            {
                return;
            }

            // recalculate slope (could be inaccurate)
            slope = float(y2 - y1);
            if (x2 - x1) slope /= (x2 - x1);

            // if line is more horizontal than vertical...
            if (fabs(slope) < 1)
            {

                if (x1 < x2)
                {
                    float cy = (float) y1;
                    for (int ix = x1; ix <= x2; ix++)
                    {
                        renderer(surface[((int)(cy + 0.5) * pitch) + ix], getcolor(ix - o_x1, o_x2 - o_x1));
                        cy += slope;
                    }
                }
                else
                {
                    float cy = (float) y1;
                    for (int ix = x1; ix >= x2; ix--)
                    {
                        renderer(surface[((int)(cy + 0.5) * pitch) + ix], getcolor(o_x1 - ix, o_x1 - o_x2));
                        cy -= slope;
                    }
                }

            }
            else
            {   // more vertical than horizontal...

                float slope_delta = 1.0f / slope;
                if (y1 < y2)
                {
                    float cx = (float) x1;
                    for (int iy = y1; iy <= y2; iy++)
                    {
                        renderer(surface[(iy * pitch) + (int)(cx + 0.5)], getcolor(iy - o_y1, o_y2 - o_y1));
                        cx += slope_delta;
                    }
                }
                else
                {
                    float cx = (float) x1;
                    for (int iy = y1; iy >= y2; iy--)
                    {
                        renderer(surface[(iy * pitch) + (int)(cx + 0.5)], getcolor(o_y1 - iy, o_y1 - o_y2));
                        cx -= slope_delta;
                    }
                }

            }
        }

    }

    template<typename destT, typename sourceT, typename clipT, typename renderT>
    void LineSeries(
        destT* surface,
        int pitch,
        VECTOR_INT** points,
        int length,
        sourceT color,
        int type,
        clipT clipper,
        renderT renderer)
    {
        int old_x, old_y, dx, dy, inc_x, inc_y, dist_big, dist_small, eps, i, n, N, ident;

        // if type == LINE_LOOP, start with the last point
        if (type == 2)
        {
            old_x = points[length-1]->x;
            old_y = points[length-1]->y;
            i = 0;
        }
        else
        {
            old_x = points[0]->x;
            old_y = points[0]->y;
            i = 1;
        }

        // draw the line series with bresenham's line algorithm
        while (i < length)
        {
            // if line is off the screen, skip it
            if (is_clipped(old_x, old_y, points[i]->x, points[i]->y, clipper))
            {
                goto skip_line;
            }

            dx = points[i]->x - old_x;
            dy = points[i]->y - old_y;

            if (dx < 0)
            {
                inc_x = -1;
                dx *= -1;
            }
            else
            {
                inc_x = 1;
            }

            if (dy < 0)
            {
                inc_y = -1;
                dy *= -1;
            }
            else
            {
                inc_y = 1;
            }

            if (dx >= dy)
            {
                dist_big = dx;
                dist_small = dy;
                ident = 0;
            }
            else
            {
                dist_big = dy;
                dist_small = dx;
                ident = 1;
            }
            eps = dist_big / 2;

            // if type == LINE_MULTIPLE, draw a full line
            if (type == 0)
                N = dist_big + 1;
            else
                N = dist_big;

            // draw the line
            for (n = 0; n < N; n++)
            {
                if (is_inside_clipper(old_x, old_y, clipper))
                {
                    renderer(surface[old_y * pitch + old_x], color);
                }

                if (ident == 0)
                {
                    old_x += inc_x;
                }
                else
                {
                    old_y += inc_y;
                }
                eps -= dist_small;
                if (eps < 0)
                {
                    if (ident == 0)
                    {
                        old_y += inc_y;
                    }
                    else
                    {
                        old_x += inc_x;
                    }
                    eps += dist_big;
                }
            }

            // label used to skip lines which are off the screen
            skip_line:

            // if type == LINE_MULTIPLE, take two new points
            if (type == 0)
            {
                i += 2;
                if (i < length)
                {
                    old_x = points[i-1]->x;
                    old_y = points[i-1]->y;
                }
            }
            else
            {
                old_x = points[i]->x;
                old_y = points[i]->y;
                i++;
            }
        }
    }

    template<typename destT, typename sourceT, typename clipT, typename renderT>
    void BezierCurve(
        destT* surface,
        int pitch,
        int x[4],
        int y[4],
        double step,
        sourceT color,
        int cubic,
        clipT clipper,
        renderT renderer)
    {
        // find Bezier curve's bounds
        int i, bound = (cubic ? 4 : 3), bound_x1 = x[0], bound_x2 = x[0], bound_y1 = y[0], bound_y2 = y[0];
        for (i = 1; i < bound; i++)
        {
            if (x[i] > bound_x2) bound_x2 = x[i];
            if (x[i] < bound_x1) bound_x1 = x[i];
            if (y[i] > bound_y2) bound_y2 = y[i];
            if (y[i] < bound_y1) bound_y1 = y[i];
        }
        // with the found bounds: check if Bezier curve is completely off the screen
        if (is_clipped(bound_x1, bound_y1, bound_x2, bound_y2, clipper))
        {
            return;
        }

        // make sure step is in a valid range
        if (step <= 0)
        {
            step = 0.001;
        }
        else if (step > 1.0)
        {
            step = 1.0;
        }

        int new_x, new_y, old_x = -1, old_y = -1;
        double b;

        // draw the Bezier curve
        for (double a = 1.0; a >= 0; a -= step)
        {
            b = 1.0 - a;
            if (cubic)
            {               // calculate the cubic Bezier curve
                new_x = (int)(x[0]*a*a*a + x[1]*3*a*a*b + x[3]*3*a*b*b + x[2]*b*b*b);
                new_y = (int)(y[0]*a*a*a + y[1]*3*a*a*b + y[3]*3*a*b*b + y[2]*b*b*b);
            }
            else
            {               // calculate the quadratic Bezier curve
                new_x = (int)(x[0]*a*a + x[1]*2*a*b + x[2]*b*b);
                new_y = (int)(y[0]*a*a + y[1]*2*a*b + y[2]*b*b);
            }

            // draw only if new_coor != old_coor to ensure that no point is drawn multiple times
            if (new_x != old_x || new_y != old_y)
            {
                if (is_inside_clipper(new_x, new_y, clipper))
                {
                    renderer(surface[new_y * pitch + new_x], color);
                }
            }

            old_x = new_x;
            old_y = new_y;
        }
    }

    template<typename T>
    T minimum(T a, T b)
    {
        return (a < b ? a : b);
    }

    template<typename T>
    T maximum(T a, T b)
    {
        return (a > b ? a : b);
    }

    /////////////////////////////////////////////////////////
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
        if (sorted_y[0] > sorted_y[1])
        {
            std::swap(sorted_y[0], sorted_y[1]);
            std::swap(sorted_x[0], sorted_x[1]);
        }
        if (sorted_y[1] > sorted_y[2])
        {
            std::swap(sorted_y[1], sorted_y[2]);
            std::swap(sorted_x[1], sorted_x[2]);
        }
        if (sorted_y[0] > sorted_y[1])
        {
            std::swap(sorted_y[0], sorted_y[1]);
            std::swap(sorted_x[0], sorted_x[1]);
        }

        bool has_top    = (sorted_y[0] != sorted_y[1]);
        bool has_bottom = (sorted_y[1] != sorted_y[2]);

        if (has_top)
        {
            // find top point
            int top_x = sorted_x[0];
            int top_y = sorted_y[0];

            // find middle points (bottom of top half)
            int mid_x1;
            int mid_x2;
            if (sorted_y[1] == sorted_y[2])
            {
                mid_x1 = sorted_x[1];
                mid_x2 = sorted_x[2];
            }
            else
            {
                float slope = float(sorted_x[2] - sorted_x[0]) / (sorted_y[2] - sorted_y[0]);
                mid_x1 = sorted_x[1];
                mid_x2 = (int)(sorted_x[0] + slope * (sorted_y[1] - sorted_y[0]));
            }
            int mid_y = sorted_y[1];

            // make sure mid_x1 < mid_x2
            if (mid_x1 > mid_x2)
            {
                std::swap(mid_x1, mid_x2);
            }

            // render all scanlines
            int min_y = top_y;
            int max_y = mid_y;

            int __min_y__ = maximum<int>(clipper.top, top_y);
            int __max_y__ = minimum<int>(clipper.bottom, mid_y);

            if (min_y == max_y)
                return;

            if (__min_y__ == __max_y__)
                return;

            for (int iy = __min_y__; iy <= __max_y__; iy++)
            {

                // calculate edges of scanline
                const int right_round = max_y - min_y - 1;
                int left_x  = top_x + (mid_x1 - top_x) * (iy - min_y) / (max_y - min_y);
                int right_x = top_x + ((mid_x2 - top_x) * (iy - min_y) + right_round) / (max_y - min_y) - 1;

                // clip edges of scanline
                left_x = maximum<int>(left_x, clipper.left);
                right_x = minimum<int>(right_x, clipper.right);

                // render the scanline
                for (int ix = left_x; ix <= right_x; ix++)
                {
                    renderer(surface[iy * pitch + ix], color);
                }
            }

        }
        if (has_bottom)
        {
            // find middle points (top of bottom half)
            int mid_x1;
            int mid_x2;
            if (sorted_y[0] == sorted_y[1])
            {
                mid_x1 = sorted_x[0];
                mid_x2 = sorted_x[1];
            }
            else
            {
                float slope = float(sorted_x[2] - sorted_x[0]) / (sorted_y[2] - sorted_y[0]);
                mid_x1 = sorted_x[1];
                mid_x2 = (int)(sorted_x[2] + slope * (sorted_y[1] - sorted_y[2]));
            }
            int mid_y = sorted_y[1];

            // make sure mid_x1 < mid_x2
            if (mid_x1 > mid_x2)
            {
                std::swap(mid_x1, mid_x2);
            }

            // find bottom point
            int bottom_x = sorted_x[2];
            int bottom_y = sorted_y[2];

            // render all scanlines
            int min_y = mid_y;
            int max_y = bottom_y;

            int __min_y__ = maximum<int>(clipper.top, mid_y);
            int __max_y__ = minimum<int>(clipper.bottom, bottom_y);

            if (min_y == max_y)
                return;

            if (__min_y__ == __max_y__)
                return;

            for (int iy = __min_y__; iy <= __max_y__; iy++)
            {

                // calculate edges of scanline
                // calculate edges of scanline
                const int right_round = max_y - min_y - 1;
                int left_x  = bottom_x + (mid_x1 - bottom_x) * (max_y - iy) / (max_y - min_y);
                int right_x = bottom_x + ((mid_x2 - bottom_x) * (max_y - iy) + right_round) / (max_y - min_y) - 1;

                // clip edges of scanline
                left_x = maximum<int>(left_x, clipper.left);
                right_x = minimum<int>(right_x, clipper.right);

                // render the scanline
                for (int ix = left_x; ix <= right_x; ix++)
                {
                    renderer(surface[iy * pitch + ix], color);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////
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
        if (sorted_y[0] > sorted_y[1])
        {
            std::swap(sorted_y[0], sorted_y[1]);
            std::swap(sorted_x[0], sorted_x[1]);
            std::swap(sorted_c[0], sorted_c[1]);
        }
        if (sorted_y[1] > sorted_y[2])
        {
            std::swap(sorted_y[1], sorted_y[2]);
            std::swap(sorted_x[1], sorted_x[2]);
            std::swap(sorted_c[1], sorted_c[2]);
        }
        if (sorted_y[0] > sorted_y[1])
        {
            std::swap(sorted_y[0], sorted_y[1]);
            std::swap(sorted_x[0], sorted_x[1]);
            std::swap(sorted_c[0], sorted_c[1]);
        }

        bool has_top    = (sorted_y[0] != sorted_y[1]);
        bool has_bottom = (sorted_y[1] != sorted_y[2]);

        if (has_top)
        {
            // find top point
            int top_x = sorted_x[0];
            int top_y = sorted_y[0];
            sourceT top_c = sorted_c[0];

            // find middle points (bottom of top half)
            int mid_x1;
            int mid_x2;
            sourceT mid_c1;
            sourceT mid_c2;
            if (sorted_y[1] == sorted_y[2])
            {
                mid_x1 = sorted_x[1];
                mid_x2 = sorted_x[2];
                mid_c1 = sorted_c[1];
                mid_c2 = sorted_c[2];
            }
            else
            {
                float slope = float(sorted_x[2] - sorted_x[0]) / (sorted_y[2] - sorted_y[0]);
                mid_x1 = sorted_x[1];
                mid_x2 = (int)(sorted_x[0] + slope * (sorted_y[1] - sorted_y[0]));
                mid_c1 = sorted_c[1];
                mid_c2 = interpolator(sorted_c[0], sorted_c[2], sorted_y[1] - sorted_y[0], sorted_y[2] - sorted_y[0]);
            }
            int mid_y = sorted_y[1];

            // make sure mid_x1 < mid_x2
            if (mid_x1 > mid_x2)
            {
                std::swap(mid_x1, mid_x2);
                std::swap(mid_c1, mid_c2);
            }

            // render all scanlines
            int min_y = top_y;
            int max_y = mid_y;

            int __min_y__ = maximum<int>(clipper.top, top_y);
            int __max_y__ = minimum<int>(clipper.bottom, mid_y);

            if (min_y == max_y)
                return;

            if (__min_y__ == __max_y__)
                return;

            for (int iy = __min_y__; iy <= __max_y__; iy++)
            {

                // calculate edges of scanline
                const int right_round = max_y - min_y - 1;
                int left_x  = top_x + (mid_x1 - top_x) * (iy - min_y) / (max_y - min_y);
                int right_x = top_x + ((mid_x2 - top_x) * (iy - min_y) + right_round) / (max_y - min_y) - 1;

                int o_lx = left_x; // store originals for correct color interpolation
                int o_rx = right_x;

                // calculate colors
                sourceT left_c  = interpolator(top_c, mid_c1, iy - top_y, mid_y - top_y);
                sourceT right_c = interpolator(top_c, mid_c2, iy - top_y, mid_y - top_y);

                // clip edges of scanline
                left_x = maximum<int>(left_x, clipper.left);
                right_x = minimum<int>(right_x, clipper.right);

                // render the scanline
                for (int ix = left_x; ix <= right_x; ix++)
                {
                    renderer(surface[iy * pitch + ix], interpolator(left_c, right_c, ix - o_lx, o_rx - o_lx));
                }
            }

        }
        if (has_bottom)
        {
            // find middle points (top of bottom half)
            int mid_x1;
            int mid_x2;
            sourceT mid_c1;
            sourceT mid_c2;
            if (sorted_y[0] == sorted_y[1])
            {
                mid_x1 = sorted_x[0];
                mid_x2 = sorted_x[1];
                mid_c1 = sorted_c[0];
                mid_c2 = sorted_c[1];
            }
            else
            {
                float slope = float(sorted_x[2] - sorted_x[0]) / (sorted_y[2] - sorted_y[0]);
                mid_x1 = sorted_x[1];
                mid_x2 = (int)(sorted_x[2] + slope * (sorted_y[1] - sorted_y[2]));
                mid_c1 = sorted_c[1];
                mid_c2 = interpolator(sorted_c[0], sorted_c[2], sorted_y[1] - sorted_y[0], sorted_y[2] - sorted_y[0]);
            }
            int mid_y = sorted_y[1];

            // make sure mid_x1 < mid_x2
            if (mid_x1 > mid_x2)
            {
                std::swap(mid_x1, mid_x2);
                std::swap(mid_c1, mid_c2);
            }

            // find bottom point
            int bottom_x = sorted_x[2];
            int bottom_y = sorted_y[2];
            sourceT bottom_c = sorted_c[2];

            // render all scanlines
            int min_y = mid_y;
            int max_y = bottom_y;

            int __min_y__ = maximum<int>(clipper.top, mid_y);
            int __max_y__ = minimum<int>(clipper.bottom, bottom_y);

            if (min_y == max_y)
                return;

            if (__min_y__ == __max_y__)
                return;

            for (int iy = __min_y__; iy <= __max_y__; iy++)
            {

                // calculate edges of scanline
                const int right_round = max_y - min_y - 1;
                int left_x  = bottom_x + (mid_x1 - bottom_x) * (max_y - iy) / (max_y - min_y);
                int right_x = bottom_x + ((mid_x2 - bottom_x) * (max_y - iy) + right_round) / (max_y - min_y) - 1;

                int o_lx = left_x; // store originals for correct color interpolation
                int o_rx = right_x;

                // calculate colors
                sourceT left_c  = interpolator(bottom_c, mid_c1, bottom_y - iy, bottom_y - mid_y);
                sourceT right_c = interpolator(bottom_c, mid_c2, bottom_y - iy, bottom_y - mid_y);

                // clip edges of scanline
                left_x = maximum<int>(left_x, clipper.left);
                right_x = minimum<int>(right_x, clipper.right);

                // render the scanline
                for (int ix = left_x; ix <= right_x; ix++)
                {
                    renderer(surface[iy * pitch + ix], interpolator(left_c, right_c, ix - o_lx, o_rx - o_lx));
                }
            }
        }
    }

    template<typename destT, typename sourceT, typename clipT, typename renderT>
    void Polygon(
        destT* surface,
        int pitch,
        VECTOR_INT** points,
        int length,
        int invert,
        sourceT color,
        clipT clipper,
        renderT renderer)
    {
        // find polygon's bounds
        int i, bound_x1 = points[0]->x, bound_x2 = points[0]->x, bound_y1 = points[0]->y, bound_y2 = points[0]->y;
        for (i = 1; i < length; i++)
        {
            if (points[i]->x > bound_x2) bound_x2 = points[i]->x;
            if (points[i]->x < bound_x1) bound_x1 = points[i]->x;
            if (points[i]->y > bound_y2) bound_y2 = points[i]->y;
            if (points[i]->y < bound_y1) bound_y1 = points[i]->y;
        }
        // if polygon is completely out, don't bother with it
        if (is_clipped(bound_x1, bound_y1, bound_x2, bound_y2, clipper))
        {
            return;
        }

        int point_in, c_x, c_y, j;

        // draw the polygon with the crossing number algorithm
        for (c_y = bound_y1; c_y <= bound_y2; c_y++)
        {

            for (c_x = bound_x1; c_x <= bound_x2; c_x++)
            {
                point_in = 0;
                j = length - 1;

                // check appropriate edges for their crossing number, if it's odd, then point is inside
                for (i = 0; i < length; i++)
                {
                    if (points[i]->y <= c_y && points[j]->y > c_y ||
                        points[j]->y <= c_y && points[i]->y > c_y)
                    {
                        if (points[i]->x >= c_x && points[j]->x >= c_x)
                        {
                            point_in = !point_in;
                        }
                        else if ((float)(points[i]->x) +
                                ((float)(c_y - points[i]->y) /
                                 (float)(points[j]->y - points[i]->y)) *
                                 (float)(points[j]->x - points[i]->x) > c_x)
                        {
                            point_in = !point_in;
                        }
                    }
                    j = i;
                }

                // do inversion before drawing if invert is true
                if (invert)
                {
                    point_in = !point_in;
                }

                if (point_in)
                {
                    if (is_inside_clipper(c_x, c_y, clipper))
                    {
                        renderer(surface[c_y * pitch + c_x], color);
                    }
                }
            }
        }
    }

    template<typename destT, typename sourceT, typename clipT, typename renderT>
    void OutlinedRectangle(
        destT* surface,
        int pitch,
        int x,
        int y,
        int w,
        int h,
        int size,
        sourceT color,
        clipT clipper,
        renderT renderer)
    {
        // if rectangle is completely out, don't bother with it
        if (is_clipped(x, y, x + w - 1, y + h - 1, clipper))
        {
            return;
        }

        int tx, ty, iy, ix;

        // Note: outlined rectangle is splitted into 4 subrectangles (each 2 are symmetrical)
        // draw the top and bottom rectangles
        for (iy = y; iy < y + size; iy++)
        {
            for (ix = x; ix < x + w; ix++)
            {
                if (is_inside_clipper(ix, iy, clipper)) renderer(surface[iy * pitch + ix], color);

                // reflect point vertically
                ty = y + h - 1 - (iy - y);

                if (is_inside_clipper(ix, ty, clipper)) renderer(surface[ty * pitch + ix], color);
            }
        }

        ty = y + h - size;

        // draw the left and right rectangles
        for (iy = y + size; iy < ty; iy++)
        {
            for (ix = x; ix < x + size; ix++)
            {
                if (is_inside_clipper(ix, iy, clipper)) renderer(surface[iy * pitch + ix], color);

                // reflect point horizontally
                tx = x + w - 1 - (ix - x);

                if (is_inside_clipper(tx, iy, clipper)) renderer(surface[iy * pitch + tx], color);
            }
        }
    }

    template<typename clipT>
    void clip_solid_rect(int& x, int& y, int& w, int& h, clipT clipper)
    {
        // clip left
        if (x < clipper.left)
        {
            w -= (clipper.left - x);
            x += (clipper.left - x);
        }

        // clip top
        if (y < clipper.top)
        {
            h -= (clipper.top - y);
            y += (clipper.top - y);
        }

        // clip right
        if (x + w > clipper.right + 1)
        {
            w -= (x + w - clipper.right - 1);
        }

        // clip bottom
        if (y + h > clipper.bottom + 1)
        {
            h -= (y + h - clipper.bottom - 1);
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
        if (is_clipped(x, y, x + w - 1, y + h - 1, clipper))
        {
            return;
        }

        // clip parts which are off the screen
        clip_solid_rect(x, y, w, h, clipper);

        // render the rectangle
        for (int iy = y; iy < y + h; iy++)
        {
            for (int ix = x; ix < x + w; ix++)
            {
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
        if (is_clipped(x, y, x + w - 1, y + h - 1, clipper))
        {
            return;
        }

        // original coordinates for correct color interpolation
        int o_x = x;
        int o_y = y;
        int o_w = w;
        int o_h = h;

        // clip parts which are off the screen
        clip_solid_rect(x, y, w, h, clipper);

        // render the rectangle
        for (int iy = y; iy < y + h; iy++)
        {

            // calculate edges of scanline
            sourceT left  = interpolator(colors[0], colors[3], iy - o_y, o_h);
            sourceT right = interpolator(colors[1], colors[2], iy - o_y, o_h);

            // draw scanline
            for (int ix = x; ix < x + w; ix++)
            {
                renderer(surface[iy * pitch + ix], interpolator(left, right, ix - o_x, o_w));
            }
        }
    }

    template<typename destT, typename sourceT, typename clipT, typename renderT>
    void OutlinedComplex(
        destT* surface,
        int pitch,
        int r_x,
        int r_y,
        int r_w,
        int r_h,
        int circ_x,
        int circ_y,
        int circ_r,
        sourceT color,
        int antialias,
        clipT clipper,
        renderT renderer)
    {
        // if complex is completely out, don't bother with it
        if (is_clipped(r_x, r_y, r_x + r_w - 1, r_y + r_h - 1, clipper))
        {
            return;
        }

        // clip parts which are off the screen
        clip_solid_rect(r_x, r_y, r_w, r_h, clipper);

        int x, y, dist, ca = color.alpha, crr = circ_r * circ_r;
        float fdist, fcr = (float)(circ_r), fca = (float)(ca);

        // draw the complex
        for (y = r_y; y < r_y + r_h; y++)
        {

            // draw scanline
            for (x = r_x; x < r_x + r_w; x++)
            {
                dist = (circ_x-x) * (circ_x-x) + (circ_y-y) * (circ_y-y);

                // point is outside the circle
                if (dist >= crr)
                {
                    color.alpha = ca;
                    renderer(surface[y * pitch + x], color);
                }

                // point is inside the circle
                else if (antialias)
                {
                    fdist = fcr - sqrt((float)(dist));
                    if (fdist < 1)
                    {
                        color.alpha = (byte)(fca * (1.0 - fdist));
                        renderer(surface[y * pitch + x], color);
                    }
                }
            }
        }
    }

    template<typename destT, typename sourceT, typename clipT, typename renderT>
    void FilledComplex(
        destT* surface,
        int pitch,
        int r_x,
        int r_y,
        int r_w,
        int r_h,
        int circ_x,
        int circ_y,
        int circ_r,
        float angle,
        float frac_size,
        int fill_empty,
        sourceT colors[2],
        clipT clipper,
        renderT renderer)
    {
        // if complex is completely out, don't bother with it
        if (is_clipped(r_x, r_y, r_x + r_w - 1, r_y + r_h - 1, clipper))
        {
            return;
        }

        // clip parts which are off the screen
        clip_solid_rect(r_x, r_y, r_w, r_h, clipper);

        int x, y, dist, crr = circ_r * circ_r;
        float fang_p, frac_half;
        const float PI = (float)(3.1415927);

        // set fraction's half size
        frac_half = frac_size / 2;
        if (frac_half < 0 || frac_half >= PI)
        {
            frac_half = 0;
        }

        // draw the complex
        for (y = r_y; y < r_y + r_h; y++)
        {

            // draw scanline
            for (x = r_x; x < r_x + r_w; x++)
            {
                dist = (x-circ_x) * (x-circ_x) + (y-circ_y) * (y-circ_y);

                // point is outside of the circle
                if (dist >= crr)
                {
                    renderer(surface[y * pitch + x], colors[0]);
                }

                // point is inside a fractioned circle
                else if (frac_half != 0)
                {
                    // check if point is located in fraction
                    fang_p = atan2(float(y-circ_y), float(x-circ_x));
                    if (fang_p < 0)
                    {
                        fang_p = PI + (PI + fang_p);
                    }
                    fang_p = fabs(angle - fang_p);
                    if (fang_p >= PI)
                    {
                        fang_p = 2*PI - fang_p;
                    }

                    // it is, so draw the point with circle's color
                    if (fang_p <= frac_half) renderer(surface[y * pitch + x], colors[1]);

                    // it is not, so draw the point with rectangle's color if fill_empty == true
                    else if (fill_empty) renderer(surface[y * pitch + x], colors[0]);
                }

                // point is inside a full circle
                else
                {
                    renderer(surface[y * pitch + x], colors[1]);
                }
            }
        }
    }

    template<typename destT, typename sourceT, typename clipT, typename renderT>
    void GradientComplex(
        destT* surface,
        int pitch,
        int r_x,
        int r_y,
        int r_w,
        int r_h,
        int circ_x,
        int circ_y,
        int circ_r,
        float angle,
        float frac_size,
        int fill_empty,
        sourceT colors[3],
        clipT clipper,
        renderT renderer)
    {
        // if complex is completely out, don't bother with it
        if (is_clipped(r_x, r_y, r_x + r_w - 1, r_y + r_h - 1, clipper))
        {
            return;
        }

        // clip parts which are off the screen
        clip_solid_rect(r_x, r_y, r_w, r_h, clipper);

        int x, y, dist, crr = circ_r * circ_r;

        // channel differences
        float fdr = (float)(colors[2].red   - colors[1].red);
        float fdg = (float)(colors[2].green - colors[1].green);
        float fdb = (float)(colors[2].blue  - colors[1].blue);
        float fda = (float)(colors[2].alpha - colors[1].alpha);

        float fang_p, frac_half, factor, fdist, fr = (float)(circ_r);
        const float PI = (float)(3.1415927), PI_H = PI / 2;

        // set fraction's half size
        frac_half = frac_size / 2;
        if (frac_half < 0 || frac_half >= PI)
        {
            frac_half = 0;
        }

        // draw the complex
        for (y = r_y; y < r_y + r_h; y++)
        {

            // draw scanline
            for (x = r_x; x < r_x + r_w; x++)
            {
                dist = (x-circ_x)*(x-circ_x) + (y-circ_y)*(y-circ_y);

                // point is outside of the circle
                if (dist >= crr)
                {
                    renderer(surface[y * pitch + x], colors[0]);
                }

                // point is inside a fractioned circle
                else if (frac_half != 0)
                {
                    // check if point is located in fraction
                    fang_p = atan2(float(y-circ_y), float(x-circ_x));
                    if (fang_p < 0)
                    {
                        fang_p = PI + (PI + fang_p);
                    }
                    fang_p = fabs(angle - fang_p);
                    if (fang_p >= PI)
                    {
                        fang_p = 2*PI - fang_p;
                    }

                    // it is, so draw the point with circle's color
                    if (fang_p <= frac_half)
                    {

                        fdist = sqrt((float)(dist));
                        factor = sin((float(1) - fdist / fr) * PI_H);

                        colors[1].red   = (byte)(colors[2].red   - fdr * factor);
                        colors[1].green = (byte)(colors[2].green - fdg * factor);
                        colors[1].blue  = (byte)(colors[2].blue  - fdb * factor);
                        colors[1].alpha = (byte)(colors[2].alpha - fda * factor);

                        renderer(surface[y * pitch + x], colors[1]);
                    }

                    // it is not, so draw the point with rectangle's color if fill_empty == true
                    else if (fill_empty) renderer(surface[y * pitch + x], colors[0]);
                }

                // point is inside a full circle
                else
                {
                    fdist = sqrt((float)(dist));
                    factor = sin((float(1) - fdist / fr) * PI_H);

                    colors[1].red   = (byte)(colors[2].red   - fdr * factor);
                    colors[1].green = (byte)(colors[2].green - fdg * factor);
                    colors[1].blue  = (byte)(colors[2].blue  - fdb * factor);
                    colors[1].alpha = (byte)(colors[2].alpha - fda * factor);

                    renderer(surface[y * pitch + x], colors[1]);
                }
            }
        }
    }

    template<typename destT, typename sourceT, typename clipT, typename renderT>
    void OutlinedEllipse(
        destT* surface,
        int pitch,
        int xc,
        int yc,
        int rx,
        int ry,
        sourceT color,
        clipT clipper,
        renderT renderer)
    {
        if (rx <= 0 || ry <= 0) return;

        // if ellipse is completely out, don't bother with it
        if (is_clipped(xc - rx, yc - ry, xc + rx - 1, yc + ry - 1, clipper))
        {
            return;
        }

        int x = rx;
        int y = 0;
        int tworx2 = 2 * rx * rx;
        int twory2 = 2 * ry * ry;
        int xchange = ry * ry * (1 - 2 * rx);
        int ychange = rx * rx;
        int error = 0;
        int xstop = twory2 * rx;
        int ystop = 0;

        // draw first set of points
        while (xstop >= ystop)
        {
            if (is_inside_clipper(xc + x, yc + y, clipper))
                renderer(surface[(yc + y) * pitch + (xc + x)], color);

            if (is_inside_clipper(xc - x - 1, yc + y, clipper))
                renderer(surface[(yc + y) * pitch + (xc - x - 1)], color);

            if (is_inside_clipper(xc - x - 1, yc - y - 1, clipper))
                renderer(surface[(yc - y - 1) * pitch + (xc - x - 1)], color);

            if (is_inside_clipper(xc + x, yc - y - 1, clipper))
                renderer(surface[(yc - y - 1) * pitch + (xc + x)], color);

            y++;
            ystop   += tworx2;
            error   += ychange;
            ychange += tworx2;
            if (2 * error + xchange > 0)
            {
                x--;
                xstop -= twory2;
                error += xchange;
                xchange += twory2;
            }
        }

        x = 0;
        y = ry;
        xchange = ry * ry;
        ychange = rx * rx * (1 - 2 * ry);
        error = 0;
        xstop = 0;
        ystop = tworx2 * ry;

        // draw second set of points
        while (xstop <= ystop)
        {
            if (is_inside_clipper(xc + x, yc + y, clipper))
                renderer(surface[(yc + y) * pitch + (xc + x)], color);

            if (is_inside_clipper(xc - x - 1, yc + y, clipper))
                renderer(surface[(yc + y) * pitch + (xc - x - 1)], color);

            if (is_inside_clipper(xc - x - 1, yc - y - 1, clipper))
                renderer(surface[(yc - y - 1) * pitch + (xc - x - 1)], color);

            if (is_inside_clipper(xc + x, yc - y - 1, clipper))
                renderer(surface[(yc - y - 1) * pitch + (xc + x)], color);

            x++;
            xstop   += twory2;
            error   += xchange;
            xchange += twory2;

            if (2 * error + ychange > 0)
            {
                y--;
                ystop -= tworx2;
                error += ychange;
                ychange += tworx2;
            }
        }
    }

    template<typename destT, typename sourceT, typename clipT, typename renderT>
    void FilledEllipse(
        destT* surface,
        int pitch,
        int xc,
        int yc,
        int rx,
        int ry,
        sourceT color,
        clipT clipper,
        renderT renderer)
    {
        if (rx <= 0 || ry <= 0) return;

        // if ellipse is completely out, don't bother with it
        if (is_clipped(xc - rx, yc - ry, xc + rx - 1, yc + ry - 1, clipper))
        {
            return;
        }

        int x = rx;
        int y = 0;
        int tworx2 = 2 * rx * rx;
        int twory2 = 2 * ry * ry;
        int xchange = ry * ry * (1 - 2 * rx);
        int ychange = rx * rx;
        int error = 0;
        int xstop = twory2 * rx;
        int ystop = 0;

        // draw first set of points
        while (xstop >= ystop)
        {

            // draw scanline
            for (int i = 0; i <= x; ++i)
            {
                if (is_inside_clipper(xc + i, yc + y, clipper))
                    renderer(surface[(yc + y) * pitch + (xc + i)], color);

                if (is_inside_clipper(xc - i - 1, yc + y, clipper))
                    renderer(surface[(yc + y) * pitch + (xc - i - 1)], color);

                if (is_inside_clipper(xc - i - 1, yc - y - 1, clipper))
                    renderer(surface[(yc - y - 1) * pitch + (xc - i - 1)], color);

                if (is_inside_clipper(xc + i, yc - y - 1, clipper))
                    renderer(surface[(yc - y - 1) * pitch + (xc + i)], color);
            }

            y++;
            ystop   += tworx2;
            error   += ychange;
            ychange += tworx2;

            if (2 * error + xchange > 0)
            {
                x--;
                xstop -= twory2;
                error += xchange;
                xchange += twory2;
            }
        }

        x = 0;
        y = ry;
        xchange = ry * ry;
        ychange = rx * rx * (1 - 2 * ry);
        error = 0;
        xstop = 0;
        ystop = tworx2 * ry;

        // draw second set of points
        while (xstop <= ystop)
        {
            x++;
            xstop   += twory2;
            error   += xchange;
            xchange += twory2;

            if (2 * error + ychange > 0)
            {

                // draw scanline
                for (int i = 0; i <= x; ++i)
                {
                    if (is_inside_clipper(xc + i, yc + y, clipper))
                        renderer(surface[(yc + y) * pitch + (xc + i)], color);

                    if (is_inside_clipper(xc - i - 1, yc + y, clipper))
                        renderer(surface[(yc + y) * pitch + (xc - i - 1)], color);

                    if (is_inside_clipper(xc - i - 1, yc - y - 1, clipper))
                        renderer(surface[(yc - y - 1) * pitch + (xc - i - 1)], color);

                    if (is_inside_clipper(xc + i, yc - y - 1, clipper))
                        renderer(surface[(yc - y - 1) * pitch + (xc + i)], color);
                }

                y--;
                ystop -= tworx2;
                error += ychange;
                ychange += tworx2;
            }
        }
    }

    template<typename destT, typename sourceT, typename clipT, typename renderT>
    void OutlinedCircle(
        destT* surface,
        int pitch,
        int x,
        int y,
        int r,
        sourceT color,
        int antialias,
        clipT clipper,
        renderT renderer)
    {
        if (r <= 0) return;

        // if circle is completely out, don't bother with it
        if (is_clipped(x - r, y - r, x + r - 1, y + r - 1, clipper))
        {
            return;
        }

        int ix = 1, iy = r, dist, n, rr = r*r, rr_m2 = (r-2)*(r-2), ca = color.alpha;
        float fr = (float)(r), fca = (float)(ca);
        const float PI_H = (float)(3.1415927 / 2.0);

        // draw outlined circle
        while (ix <= iy)
        {

            // draw antialiased outline (goes through all points in an octant)
            if (antialias)
            {
                n = iy + 1;
                while (--n >= ix)
                {
                    dist = ix*ix + n*n;
                    if (dist > rr) dist = rr;
                    if (dist > rr_m2)
                    {
                        // antialias
                        color.alpha = (byte)(fca * sin(sin((1.0 - fabs(sqrt((float)(dist)) - fr + 1.0)) * PI_H) * PI_H));

                        // draw the point and reflect it seven times
                        if (is_inside_clipper(x + ix - 1, y - n, clipper))
                            renderer(surface[(y - n) * pitch + (x + ix - 1)], color);

                        if (is_inside_clipper(x - ix, y - n, clipper))
                            renderer(surface[(y - n) * pitch + (x - ix)], color);

                        if (is_inside_clipper(x + ix - 1, y + n - 1, clipper))
                            renderer(surface[(y + n - 1) * pitch + (x + ix - 1)], color);

                        if (is_inside_clipper(x - ix, y + n - 1, clipper))
                            renderer(surface[(y + n - 1) * pitch + (x - ix)], color);

                        if (ix != n)
                        {
                            if (is_inside_clipper(x + n - 1, y - ix, clipper))
                                renderer(surface[(y - ix) * pitch + (x + n - 1)], color);

                            if (is_inside_clipper(x - n, y - ix, clipper))
                                renderer(surface[(y - ix) * pitch + (x - n)], color);

                            if (is_inside_clipper(x + n - 1, y + ix - 1, clipper))
                                renderer(surface[(y + ix - 1) * pitch + (x + n - 1)], color);

                            if (is_inside_clipper(x - n, y + ix - 1, clipper))
                                renderer(surface[(y + ix - 1) * pitch + (x - n)], color);
                        }
                    }
                }
            }

            // draw simple outline (goes only through points on the outline)
            else
            {
                // draw the point and reflect it seven times
                if (is_inside_clipper(x + ix - 1, y - iy, clipper))
                    renderer(surface[(y - iy) * pitch + (x + ix - 1)], color);

                if (is_inside_clipper(x - ix, y - iy, clipper))
                    renderer(surface[(y - iy) * pitch + (x - ix)], color);

                if (is_inside_clipper(x + ix - 1, y + iy - 1, clipper))
                    renderer(surface[(y + iy - 1) * pitch + (x + ix - 1)], color);

                if (is_inside_clipper(x - ix, y + iy - 1, clipper))
                    renderer(surface[(y + iy - 1) * pitch + (x - ix)], color);

                if (ix != iy)
                {
                    if (is_inside_clipper(x + iy - 1, y - ix, clipper))
                        renderer(surface[(y - ix) * pitch + (x + iy - 1)], color);

                    if (is_inside_clipper(x - iy, y - ix, clipper))
                        renderer(surface[(y - ix) * pitch + (x - iy)], color);

                    if (is_inside_clipper(x + iy - 1, y + ix - 1, clipper))
                        renderer(surface[(y + ix - 1) * pitch + (x + iy - 1)], color);

                    if (is_inside_clipper(x - iy, y + ix - 1, clipper))
                        renderer(surface[(y + ix - 1) * pitch + (x - iy)], color);
                }
            }

            // go always rightwards
            ix++;

            // but check whether to go down
            if (abs(ix*ix + iy*iy - rr) > abs(ix*ix + (iy-1)*(iy-1) - rr)) iy--;
        }
    }

    template<typename destT, typename sourceT, typename clipT, typename renderT>
    void FilledCircle(
        destT* surface,
        int pitch,
        int x,
        int y,
        int r,
        sourceT color,
        int antialias,
        clipT clipper,
        renderT renderer)
    {
        if (r <= 0) return;

        // if circle is completely out, don't bother with it
        if (is_clipped(x - r, y - r, x + r - 1, y + r - 1, clipper))
        {
            return;
        }

        int ix = 1, iy = r, dist, n, rr = r*r, rr_m1 = (r-1)*(r-1), ca = color.alpha;
        float fr = (float)(r), fca = (float)(ca);

        // draw filled circle (goes through all points in an octant)
        while (ix <= iy)
        {

            n = iy + 1;
            while (--n >= ix)
            {
                // antialias
                if (antialias)
                {
                    dist = ix*ix + n*n;
                    if (dist > rr) dist = rr;
                    if (dist > rr_m1) color.alpha = (byte)(fca * (fr - sqrt(float(dist))));
                    else color.alpha = ca;
                }
                else color.alpha = ca;

                // draw the point and reflect it seven times
                if (is_inside_clipper(x + ix - 1, y - n, clipper))
                    renderer(surface[(y - n) * pitch + (x + ix - 1)], color);

                if (is_inside_clipper(x - ix, y - n, clipper))
                    renderer(surface[(y - n) * pitch + (x - ix)], color);

                if (is_inside_clipper(x + ix - 1, y + n - 1, clipper))
                    renderer(surface[(y + n - 1) * pitch + (x + ix - 1)], color);

                if (is_inside_clipper(x - ix, y + n - 1, clipper))
                    renderer(surface[(y + n - 1) * pitch + (x - ix)], color);

                if (ix != n)
                {
                    if (is_inside_clipper(x + n - 1, y - ix, clipper))
                        renderer(surface[(y - ix) * pitch + (x + n - 1)], color);

                    if (is_inside_clipper(x - n, y - ix, clipper))
                        renderer(surface[(y - ix) * pitch + (x - n)], color);

                    if (is_inside_clipper(x + n - 1, y + ix - 1, clipper))
                        renderer(surface[(y + ix - 1) * pitch + (x + n - 1)], color);

                    if (is_inside_clipper(x - n, y + ix - 1, clipper))
                        renderer(surface[(y + ix - 1) * pitch + (x - n)], color);
                }
            }

            // go always rightwards
            ix++;

            // but check whether to go down
            if (abs(ix*ix + iy*iy - rr) > abs(ix*ix + (iy-1)*(iy-1) - rr)) iy--;
        }
    }

    template<typename destT, typename sourceT, typename clipT, typename renderT>
    void GradientCircle(
        destT* surface,
        int pitch,
        int x,
        int y,
        int r,
        sourceT colors[2],
        int antialias,
        clipT clipper,
        renderT renderer)
    {
        if (r <= 0) return;

        // if circle is completely out, don't bother with it
        if (is_clipped(x - r, y - r, x + r - 1, y + r - 1, clipper))
        {
            return;
        }

        int ix = 1, iy = r, n;

        // channel differences
        float fdr = (float)(colors[1].red   - colors[0].red);
        float fdg = (float)(colors[1].green - colors[0].green);
        float fdb = (float)(colors[1].blue  - colors[0].blue);
        float fda = (float)(colors[1].alpha - colors[0].alpha);

        float dist, factor, fr = (float)(r);
        const float PI_H = (float)(3.1415927 / 2.0), RR = (float)(r*r);

        // draw gradient circle (goes through all points in an octant)
        while (ix <= iy)
        {

            n = iy + 1;
            while (--n >= ix)
            {
                dist = sqrt((float)(ix*ix + n*n));
                if (dist > r) dist = fr;

                // set correct color
                factor = sin((float(1) - dist / fr) * PI_H);
                colors[0].red   = (byte)(colors[1].red   - fdr * factor);
                colors[0].green = (byte)(colors[1].green - fdg * factor);
                colors[0].blue  = (byte)(colors[1].blue  - fdb * factor);
                colors[0].alpha = (byte)(colors[1].alpha - fda * factor);

                // antialias
                if (antialias && dist > r - 1)
                {
                    colors[0].alpha = (byte)((float)(colors[0].alpha) * (fr - dist));
                }

                // draw the point and reflect it seven times
                if (is_inside_clipper(x + ix - 1, y - n, clipper))
                    renderer(surface[(y - n) * pitch + (x + ix - 1)], colors[0]);

                if (is_inside_clipper(x - ix, y - n, clipper))
                    renderer(surface[(y - n) * pitch + (x - ix)], colors[0]);

                if (is_inside_clipper(x + ix - 1, y + n - 1, clipper))
                    renderer(surface[(y + n - 1) * pitch + (x + ix - 1)], colors[0]);

                if (is_inside_clipper(x - ix, y + n - 1, clipper))
                    renderer(surface[(y + n - 1) * pitch + (x - ix)], colors[0]);

                if (ix != n)
                {
                    if (is_inside_clipper(x + n - 1, y - ix, clipper))
                        renderer(surface[(y - ix) * pitch + (x + n - 1)], colors[0]);

                    if (is_inside_clipper(x - n, y - ix, clipper))
                        renderer(surface[(y - ix) * pitch + (x - n)], colors[0]);

                    if (is_inside_clipper(x + n - 1, y + ix - 1, clipper))
                        renderer(surface[(y + ix - 1) * pitch + (x + n - 1)], colors[0]);

                    if (is_inside_clipper(x - n, y + ix - 1, clipper))
                        renderer(surface[(y + ix - 1) * pitch + (x - n)], colors[0]);
                }
            }

            // go always rightwards
            ix++;

            // but check whether to go down
            if (fabs(ix*ix + iy*iy - RR) > fabs(ix*ix + (iy-1)*(iy-1) - RR)) iy--;
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
        int i;

        for (i = 3; i>0; --i)
        {
            if (y[i] < y[top])
            {
                top = i;
            }

            if (y[i] > y[bottom])
            {
                bottom = i;
            }
        }

        // perform clipping in the y axis
        int minY = bracket<int>(y[top],    clipper.top, clipper.bottom);
        int maxY = bracket<int>(y[bottom], clipper.top, clipper.bottom);

        // precalculate line segment information
        struct segment
        {

            // y1 < y2, always
            int    x1, x2;
            int    y1, y2;
            int    u1, u2;
            int    v1, v2;

        }
        segments[4];

        // segment 0 = top
        // segment 1 = right
        // segment 2 = bottom
        // segment 3 = left

        for (i = 3; i>=0 ; --i)
        {
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

            if (y[p1] > y[p2])
            {
                std::swap(s->x1, s->x2);
                std::swap(s->y1, s->y2);
                std::swap(s->u1, s->u2);
                std::swap(s->v1, s->v2);
            }
        }

        // draw scanlines
        for (int iy = minY; iy <= maxY; iy++)
        {

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
            for (int i = 3;i>=0; --i)
            {
                segment* s = segments + i;

                // if iy is even in the segment and segment's length is not zero
                if (s->y1 <= iy && iy <= s->y2)
                {

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
                    if (x < minX)
                    {
                        minX = x;
                        minU = u;
                        minV = v;
                    }

                    if (x > maxX)
                    {
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
            if (minX == maxX)
            {

                //renderer(surface[iy * pitch + minX], texture[minV * tex_width + minU], mask[minV * tex_width + minU]);
            }
            else
            {

                for (int ix = minX; ix <= maxX; ix++)
                {

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

        if (x < clipper.left)
        {
            image_offset_x = (clipper.left - x);
            image_blit_width -= image_offset_x;
        }

        if (y < clipper.top)
        {
            image_offset_y = (clipper.top - y);
            image_blit_height -= image_offset_y;
        }

        if (x + (int)tex_width - 1 > clipper.right)
        {
            image_blit_width -= (x + tex_width - clipper.right - 1);
        }

        if (y + (int)tex_height - 1 > clipper.bottom)
        {
            image_blit_height -= (y + tex_height - clipper.bottom - 1);
        }

        // heh, funny abbreviations
        pixelT* dst = surface + (y + image_offset_y) * pitch     + image_offset_x + x;
        pixelT* src = texture +       image_offset_y * tex_width + image_offset_x;
        alphaT* alf = alpha   +       image_offset_y * tex_width + image_offset_x;

        int dst_inc = pitch     - image_blit_width;
        int src_inc = tex_width - image_blit_width;

        int iy = image_blit_height;
        while (iy-- > 0)
        {
            int ix = image_blit_width;
            while (ix-- > 0)
            {
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
