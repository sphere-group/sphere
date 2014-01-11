#ifndef RENDER_HPP
#define RENDER_HPP

#include "../common/rgb.hpp"
#include "video.hpp"

extern void FadeToColor(unsigned milliseconds, RGBA color);
extern void FadeFromColor(unsigned milliseconds, RGBA color);

inline void ApplyColorMask(RGBA mask)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), mask);
}

inline void ClearScreen()
{
	DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), CreateRGBA(0, 0, 0, 255));
}

inline void FadeIn(unsigned milliseconds)
{
    FadeFromColor(milliseconds, CreateRGBA(0, 0, 0, 255));
}

inline void FadeOut(unsigned milliseconds)
{
    FadeToColor(milliseconds, CreateRGBA(0, 0, 0, 255));
}

#endif
