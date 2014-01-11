#ifndef I_ANIMATION_HPP
#define I_ANIMATION_HPP


#include "rgb.hpp"


// interface
struct IAnimation
{
  // destructor
  virtual void Destroy() = 0;

  virtual int GetWidth() = 0;
  virtual int GetHeight() = 0;
  virtual int GetNumFrames() = 0;
  virtual int GetDelay() = 0;

  virtual bool ReadNextFrame(RGBA* FrameBuffer) = 0;
  virtual bool ReadNextFrame(BGRA* FrameBuffer) = 0;
};


#endif
