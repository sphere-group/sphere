#ifndef MNG_ANIMATION_HPP
#define MNG_ANIMATION_HPP


#include <libmng.h>
#include "DefaultFileSystem.hpp"
#include "IAnimation.hpp"


class CMNGAnimation : public IAnimation
{
public:
  CMNGAnimation();
  ~CMNGAnimation();

  bool Load(const char* filename, IFileSystem& fs = g_DefaultFileSystem);

  virtual void Destroy();

  virtual int GetWidth();
  virtual int GetHeight();
  virtual int GetNumFrames();
  virtual int GetDelay();

  virtual bool ReadNextFrame(RGBA* FrameBuffer);
  virtual bool ReadNextFrame(BGRA* FrameBuffer);

private:
  // libmng callbacks
  static mng_ptr    MNG_DECL CB_Allocate(mng_size_t len);
  static void       MNG_DECL CB_Free(mng_ptr ptr, mng_size_t len);
  static mng_bool   MNG_DECL CB_OpenStream(mng_handle handle);
  static mng_bool   MNG_DECL CB_CloseStream(mng_handle handle);
  static mng_bool   MNG_DECL CB_ReadData(mng_handle handle, mng_ptr buf, mng_uint32 len, mng_uint32p read);
  static mng_bool   MNG_DECL CB_ProcessHeader(mng_handle handle, mng_uint32 width, mng_uint32 height);
  static mng_ptr    MNG_DECL CB_GetCanvasLine(mng_handle handle, mng_uint32 line_number);
  static mng_bool   MNG_DECL CB_Refresh(mng_handle handle, mng_uint32 x, mng_uint32 y, mng_uint32 width, mng_uint32 height);
  static mng_uint32 MNG_DECL CB_GetTickCount(mng_handle handle);
  static mng_bool   MNG_DECL CB_SetTimer(mng_handle handle, mng_uint32 msecs);

private:
  IFile* m_file;

  mng_handle m_stream;

  int m_width;
  int m_height;
  int m_delay;

  RGBA* m_frame_buffer;
  bool m_first_display;
};


#endif
