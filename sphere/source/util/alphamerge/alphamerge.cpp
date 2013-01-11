

#include <stdio.h>
#include <stdlib.h>
#include "../../common/Image32.hpp"


int main(int argc, char** argv)
{
    if (argc != 4)
    {
        puts("Syntax:  alphamerge <destination image> <src color image> <src alpha image>");
        return EXIT_FAILURE;
    }

    CImage32 dest;
    CImage32 src_color;
    CImage32 src_alpha;

    if (!src_color.Load(argv[2]))
    {
        puts("Could not load src color image");
        return EXIT_FAILURE;
    }

    if (!src_alpha.Load(argv[3]))
    {
        puts("Could not load src alpha image");
        return EXIT_FAILURE;
    }

    if (src_color.GetWidth() != src_alpha.GetWidth() ||
            src_color.GetHeight() != src_alpha.GetHeight())
    {
        puts("Image sizes do not match");
        return EXIT_FAILURE;
    }

    dest.Create(src_color.GetWidth(), src_color.GetHeight());

    RGBA* dest_pixels      = dest.GetPixels();
    RGBA* src_color_pixels = src_color.GetPixels();
    RGBA* src_alpha_pixels = src_alpha.GetPixels();
    for (int i = 0; i < dest.GetWidth() * dest.GetHeight(); i++)
    {
        RGBA srcc = src_color_pixels[i];
        RGBA srca = src_alpha_pixels[i];
        dest_pixels[i].red   = srcc.red;
        dest_pixels[i].green = srcc.green;
        dest_pixels[i].blue  = srcc.blue;
        dest_pixels[i].alpha = (srca.red + srca.green + srca.blue) / 3;
    }

    if (!dest.Save(argv[1]))
    {
        puts("Could not save image");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
