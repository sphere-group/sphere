#include <math.h>
#include "hsi.hpp"
#ifndef M_PI
#define M_PI 3.141592653589793
#endif
double DegreesToRadians(double degrees)
{

    return degrees * M_PI / 180.0;
}
double RadiansToDegrees(double radians)
{

    return radians * 180.0 / M_PI;
}
void RGBtoHSI(const double r, const double g, const double b, double* h, double* s, double* i)
{
    const double total = (double) (r + g + b);
    double third_total = 0;
    double three_over_total = 0;
    double hue = 0.0;
    double saturation = 0.0;
    double intensity = 0.0;
    double minvalue = r;
    if (g < minvalue) minvalue = g;
    if (b < minvalue) minvalue = b;
    if (total)
    {

        third_total = (double) (total / 3.0);
        three_over_total = 3.0 / total;
    }
    intensity = third_total;
    saturation = 1.0 - three_over_total * minvalue;
    if (saturation <= 0)
    {

        hue = M_PI;
    }
    else
    {

        hue = 0.5 * ( (r - g) + (r - b) );
        hue = acos( hue / sqrt( pow((r-g), 2) + ( (r - b) * (g - b) ) ) );
        if ((b > g))
        {

            hue = DegreesToRadians(360) - hue;
        }
    }
    *h = hue;
    *s = saturation;
    *i = intensity;
}
void HSItoRGB(const double h, const double saturation, const double intensity, double* r, double* g, double* b)
{
    double red = 0, green = 0, blue = 0;
    double hue = h;
    int hue_in_degrees = (int) RadiansToDegrees(hue);
    if (intensity <= 0.0001)
    {

        *r = 0.0;
        *g = 0.0;
        *b = 0.0;
        return;
    }
    if (intensity >= 1.0)
    {

        *r = 1.0;
        *g = 1.0;
        *b = 1.0;
        return;
    }
    if (saturation <= 0.0001)
    {

        *r = intensity;
        *g = intensity;
        *b = intensity;
        return;
    }
    if (hue_in_degrees >= 0 && hue_in_degrees <= 120)
    {

        blue  = intensity * (1.0 - saturation);
        red   = intensity * (1.0 + (saturation * cos(hue)) / (cos(DegreesToRadians(60) - hue)));
        green = (3.0 * intensity) * (1.0 - ((red + blue) / (3.0 * intensity)));
    }
    else
        if (hue_in_degrees > 120 && hue_in_degrees <= 240)
        {

            hue = hue - DegreesToRadians(120);
            green = intensity * (1.0 + (saturation * cos(hue)) / (cos(DegreesToRadians(60) - hue)));
            red   = intensity * (1.0 - saturation);
            blue  = (3.0 * intensity) * (1.0 - ((red + green) / (3.0 * intensity)));
        }
        else
            if (hue_in_degrees > 240 && hue_in_degrees <= 360)
            {
                hue = hue - DegreesToRadians(240);
                blue  = intensity * (1.0 + (saturation * cos(hue)) / (cos(DegreesToRadians(60) - hue)));
                green = intensity * (1.0 - saturation);
                red   = (3.0 * intensity) * (1.0 - ((green + blue) / (3.0 * intensity)));
            }
    if (red > 1.0) red = 1.0;
    if (red < 0.0001) red = 0.0;
    if (green > 1.0) green = 1.0;
    if (green < 0.0001) green = 0.0;
    if (blue > 1.0) blue = 1.0;
    if (blue < 0.0001) blue = 0.0;
    *r = red;
    *g = green;
    *b = blue;
}
