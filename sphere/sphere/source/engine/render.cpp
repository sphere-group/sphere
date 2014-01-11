
#include <stdio.h>
#include <stdlib.h>
#include "render.hpp"
#include "time.hpp"

////////////////////////////////////////////////////////////////////////////////
void FadeToColor(unsigned msecs, RGBA color)
{
    IMAGE image = GrabImage(0, 0, GetScreenWidth(), GetScreenHeight());

    dword time = GetTime();
    while (GetTime() - time < msecs)
    {
        color.alpha = (byte)((GetTime() - time) * 255 / msecs);
        BlitImage(image, 0, 0, CImage32::BLEND);
        ApplyColorMask(color);
        FlipScreen();
    }

    DestroyImage(image);
}

////////////////////////////////////////////////////////////////////////////////
void FadeFromColor(unsigned msecs, RGBA color)
{
    IMAGE image = GrabImage(0, 0, GetScreenWidth(), GetScreenHeight());

    dword time = GetTime();
    while (GetTime() - time < msecs)
    {
        color.alpha = 255 - (byte)((GetTime() - time) * 255 / msecs);
        BlitImage(image, 0, 0, CImage32::BLEND);
        ApplyColorMask(color);
        FlipScreen();
    }

    DestroyImage(image);
}

////////////////////////////////////////////////////////////////////////////////
