#ifndef CONVOLUTION_PIXEL_TYPE
#error CONVOLUTION_PIXEL_TYPE must be defined...
#endif

#ifndef CONVOLUTION_PIXEL_SIZE
#error CONVOLUTION_PIXEL_SIZE must be defined...
#endif

#ifndef CONVOLUTION_PIXEL_CHANNEL_1
#error CONVOLUTION_PIXEL_CHANNEL_1 must be defined...
#endif

#ifndef CONVOLUTION_PIXEL_CHANNEL_2
#error CONVOLUTION_PIXEL_CHANNEL_2 must be defined...
#endif

#ifndef CONVOLUTION_PIXEL_CHANNEL_3
#error CONVOLUTION_PIXEL_CHANNEL_3 must be defined...
#endif

#ifndef CONVOLUTION_PIXEL_CHANNEL_4
#error CONVOLUTION_PIXEL_CHANNEL_4 must be defined...
#endif

#ifndef CONVOLUTION_CURRENT_PIXEL_CHANNEL_1
#error CONVOLUTION_CURRENT_PIXEL_CHANNEL_1 must be defined...
#endif

#ifndef CONVOLUTION_CURRENT_PIXEL_CHANNEL_2
#error CONVOLUTION_CURRENT_PIXEL_CHANNEL_2 must be defined...
#endif

#ifndef CONVOLUTION_CURRENT_PIXEL_CHANNEL_3
#error CONVOLUTION_CURRENT_PIXEL_CHANNEL_3 must be defined...
#endif

#ifndef CONVOLUTION_CURRENT_PIXEL_CHANNEL_4
#error CONVOLUTION_CURRENT_PIXEL_CHANNEL_4 must be defined...
#endif

void
CONVOLUTION_NAME(int x, int y, int w, int h, int width, int height,
                 CONVOLUTION_PIXEL_TYPE* pixels, int mask_width, int mask_height,
                 int mask_xoffset, int mask_yoffset, const CONVOLUTION_TYPE* mask,
                 int divisor, int offset, int wrap,
                 int clamp, int clamp_low, int clamp_high, int infinite,
                 int use_red, int use_green, int use_blue, int use_alpha)
{
    CONVOLUTION_TYPE red;
    CONVOLUTION_TYPE green;
    CONVOLUTION_TYPE blue;
    CONVOLUTION_TYPE alpha;

    CONVOLUTION_PIXEL_TYPE* old_pixels = NULL;
    int sx, sy, mx, my;
    if (!infinite)
    {
        if (width <= 0 || height <= 0)
            return;

        old_pixels = (CONVOLUTION_PIXEL_TYPE*) malloc(sizeof(CONVOLUTION_PIXEL_TYPE) * width * height);
        if (old_pixels == NULL)
            return;

        memcpy(old_pixels, pixels, sizeof(CONVOLUTION_PIXEL_TYPE) * width * height);
    }
    else
    {
        old_pixels = pixels;
    }

    for (sy = y; sy < h; sy++)
    {
        for (sx = x; sx < w; sx++)
        {
            CONVOLUTION_TYPE red_total = 0;
            CONVOLUTION_TYPE green_total = 0;
            CONVOLUTION_TYPE blue_total = 0;
            CONVOLUTION_TYPE alpha_total = 0;

            for (my = 0; my < mask_height; my += 1)
            {
                for (mx = 0; mx < mask_width; mx += 1)
                {
                    int px = (sx - mask_xoffset + mx);
                    int py = (sy - mask_yoffset + my);

                    if (wrap)
                    {
                        if (px < 0) px = width + px;
                        else if (px >= width) px = px - width;
                        if (py < 0) py = height + py;
                        else if (py >= height) py = py - height;
                    }

                    if (px >= x && px < w && py >= y && py < h)
                    {
                        red_total   += (CONVOLUTION_TYPE)
                                       (mask[my * mask_width + mx] * (CONVOLUTION_PIXEL_CHANNEL_1));
                        green_total += (CONVOLUTION_TYPE)
                                       (mask[my * mask_width + mx] * (CONVOLUTION_PIXEL_CHANNEL_2));
                        blue_total  += (CONVOLUTION_TYPE)
                                       (mask[my * mask_width + mx] * (CONVOLUTION_PIXEL_CHANNEL_3));
                        alpha_total += (CONVOLUTION_TYPE)
                                       (mask[my * mask_width + mx] * (CONVOLUTION_PIXEL_CHANNEL_4));
                    }
                }
            }

            red   = red_total;
            green = green_total;
            blue  = blue_total;
            alpha = alpha_total;

            if (divisor)
            {
                red /= divisor;
                green /= divisor;
                blue /= divisor;
                alpha /= divisor;
            }

            if (offset)
            {
                red += offset;
                green += offset;
                blue += offset;
                alpha += offset;
            }

            if (clamp)
            {
                red   = (red   < clamp_low ? 0 : (red   > clamp_high ? clamp_high : red));
                green = (green < clamp_low ? 0 : (green > clamp_high ? clamp_high : green));
                blue  = (blue  < clamp_low ? 0 : (blue  > clamp_high ? clamp_high : blue));
                alpha = (alpha < clamp_low ? 0 : (alpha > clamp_high ? clamp_high : alpha));
            }

            if (use_red)
                CONVOLUTION_CURRENT_PIXEL_CHANNEL_1 = (unsigned char) red;
            if (use_green)
                CONVOLUTION_CURRENT_PIXEL_CHANNEL_2 = (unsigned char) green;
            if (use_blue)
                CONVOLUTION_CURRENT_PIXEL_CHANNEL_3 = (unsigned char) blue;
            if (use_alpha)
                CONVOLUTION_CURRENT_PIXEL_CHANNEL_4 = (unsigned char) alpha;
        }
    }

    if (!infinite)
    {
        free(old_pixels);
    }
}

