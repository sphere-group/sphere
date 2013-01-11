#ifndef I_ANIMATION_HPP
#define I_ANIMATION_HPP
#include "rgb.hpp"
#include "DefaultFileSystem.hpp"
// interface
struct IAnimation
{
    // destructor
    virtual ~IAnimation()
    { }

    virtual int GetWidth() = 0;
    virtual int GetHeight() = 0;
    virtual int GetNumFrames() = 0;
	virtual int GetTicks() = 0;
	virtual int GetPlaytime() = 0;
    virtual int GetDelay() = 0;
    virtual bool IsEndOfAnimation() = 0;
    virtual bool ReadNextFrame(RGBA* FrameBuffer) = 0;
    virtual bool ReadNextFrame(BGRA* FrameBuffer) = 0;
    virtual bool Save(const char* filename, IFileSystem& fs) = 0;
};
#endif
