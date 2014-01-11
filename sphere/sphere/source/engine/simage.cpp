#include <stdio.h>
#include "simage.hpp"
#include "../common/Image32.hpp"

////////////////////////////////////////////////////////////////////////////////
IMAGE SLoadImage(const char* filename, IFileSystem& fs)
{
    CImage32 image;
    if (!image.Load(filename, fs))
        return NULL;

    return CreateImage(image.GetWidth(), image.GetHeight(), image.GetPixels());
}

////////////////////////////////////////////////////////////////////////////////
IMAGE SLoadImageScale(const char* filename, IFileSystem& fs, int width, int height)
{
    CImage32 image;
    if (!image.Load(filename, fs))
        return NULL;

    image.Rescale(width, height);
    return CreateImage(image.GetWidth(), image.GetHeight(), image.GetPixels());
}

////////////////////////////////////////////////////////////////////////////////
