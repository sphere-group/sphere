#include <algorithm>
#include <time.h>
#include "MNGAnimation.hpp"


////////////////////////////////////////////////////////////////////////////////

CMNGAnimation::CMNGAnimation()
: m_file(NULL)
, m_stream(NULL)
, m_frame_buffer(NULL)
, m_first_display(true)
{
}

////////////////////////////////////////////////////////////////////////////////

CMNGAnimation::~CMNGAnimation()
{
  if (m_frame_buffer) {
    delete[] m_frame_buffer;
    m_frame_buffer = NULL;
  }

  if (m_stream) {
    mng_cleanup(&m_stream);
    m_stream = NULL;
  }

  if (m_file) {
    m_file->Close();
    m_file = NULL;
  }
}

////////////////////////////////////////////////////////////////////////////////

bool
CMNGAnimation::Load(const char* filename, IFileSystem& fs)
{
  this->~CMNGAnimation();

  // open the file
  m_file = fs.Open(filename, IFileSystem::read);
  if (m_file == NULL) {
    return false;
  }

  // initialize MNG playback
  m_stream = mng_initialize(this, CB_Allocate, CB_Free, NULL);

  // set all of the callbacks
  mng_setcb_openstream(m_stream, CB_OpenStream);
  mng_setcb_closestream(m_stream, CB_CloseStream);
  mng_setcb_readdata(m_stream, CB_ReadData);
  mng_setcb_processheader(m_stream, CB_ProcessHeader);
  mng_setcb_gettickcount(m_stream, CB_GetTickCount);
  mng_setcb_getcanvasline(m_stream, CB_GetCanvasLine);
  mng_setcb_refresh(m_stream, CB_Refresh);
  mng_setcb_settimer(m_stream, CB_SetTimer);

  // do some reading
  if (mng_read(m_stream) != MNG_NOERROR) {
    return false;
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////

void
CMNGAnimation::Destroy()
{
  delete this;
}

////////////////////////////////////////////////////////////////////////////////

int
CMNGAnimation::GetWidth()
{
  return m_width;
}

////////////////////////////////////////////////////////////////////////////////

int
CMNGAnimation::GetHeight()
{
  return m_height;
}

////////////////////////////////////////////////////////////////////////////////

int
CMNGAnimation::GetNumFrames()
{
  return 0; // unknown
}

////////////////////////////////////////////////////////////////////////////////

int
CMNGAnimation::GetDelay()
{
  return m_delay;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMNGAnimation::ReadNextFrame(RGBA* frame_buffer)
{
  if (m_first_display) {
    mng_display(m_stream);
    m_first_display = false;
  } else {
    if (mng_display_resume(m_stream) != MNG_NEEDTIMERWAIT) {
      mng_display_reset(m_stream);  // repeat
    }
  }

  // convert the buffers into frame_buffer
  memcpy(frame_buffer, m_frame_buffer, m_width * m_height * sizeof(RGBA));

  return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMNGAnimation::ReadNextFrame(BGRA* frame_buffer)
{
  if (!ReadNextFrame((RGBA*)frame_buffer)) {
    return false;
  }

  // switch red and blue
  for (int i = 0; i < m_width * m_height; i++) {
    std::swap(frame_buffer[i].red, frame_buffer[i].blue);
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////

mng_ptr MNG_DECL
CMNGAnimation::CB_Allocate(mng_size_t len)
{
  return calloc(1, len);
}

////////////////////////////////////////////////////////////////////////////////

void MNG_DECL
CMNGAnimation::CB_Free(mng_ptr ptr, mng_size_t /*len*/)
{
  free(ptr);
}

////////////////////////////////////////////////////////////////////////////////

mng_bool MNG_DECL
CMNGAnimation::CB_OpenStream(mng_handle handle)
{
  return MNG_TRUE;  // we don't need to do anything (the stream is already open)
}

////////////////////////////////////////////////////////////////////////////////

mng_bool MNG_DECL
CMNGAnimation::CB_CloseStream(mng_handle)
{
  return MNG_TRUE;  // we don't need to close it either
}

////////////////////////////////////////////////////////////////////////////////

mng_bool MNG_DECL
CMNGAnimation::CB_ReadData(mng_handle handle, mng_ptr buf, mng_uint32 len, mng_uint32p read)
{
  CMNGAnimation* This = (CMNGAnimation*)mng_get_userdata(handle);
  *read = This->m_file->Read(buf, len);
  return MNG_TRUE;
}

////////////////////////////////////////////////////////////////////////////////

mng_bool MNG_DECL
CMNGAnimation::CB_ProcessHeader(mng_handle handle, mng_uint32 width, mng_uint32 height)
{
  CMNGAnimation* This = (CMNGAnimation*)mng_get_userdata(handle);
  This->m_width = width;
  This->m_height = height;

  // in case ProcessHeader is called twice
  delete[] This->m_frame_buffer;
  This->m_frame_buffer = new RGBA[width * height];
  This->m_first_display = true;

  mng_set_canvasstyle(handle, MNG_CANVAS_RGBA8);

  return MNG_TRUE;
}

////////////////////////////////////////////////////////////////////////////////

mng_ptr MNG_DECL
CMNGAnimation::CB_GetCanvasLine(mng_handle handle, mng_uint32 line_number)
{
  CMNGAnimation* This = (CMNGAnimation*)mng_get_userdata(handle);
  return This->m_frame_buffer + line_number * This->m_width;
}

////////////////////////////////////////////////////////////////////////////////

mng_bool MNG_DECL
CMNGAnimation::CB_Refresh(mng_handle handle, mng_uint32 x, mng_uint32 y, mng_uint32 width, mng_uint32 height)
{
  // we should probably do something intelligent
  return MNG_TRUE;
}

////////////////////////////////////////////////////////////////////////////////

mng_uint32 MNG_DECL
CMNGAnimation::CB_GetTickCount(mng_handle /*handle*/)
{
  return 1000 * clock() / CLOCKS_PER_SEC;
}

////////////////////////////////////////////////////////////////////////////////

mng_bool MNG_DECL
CMNGAnimation::CB_SetTimer(mng_handle handle, mng_uint32 msecs)
{
  CMNGAnimation* This = (CMNGAnimation*)mng_get_userdata(handle);
  This->m_delay = msecs;
  return MNG_TRUE;
}

////////////////////////////////////////////////////////////////////////////////
