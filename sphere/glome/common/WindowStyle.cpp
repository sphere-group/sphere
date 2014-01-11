#include <string.h>
#include "WindowStyle.hpp"
#include "packed.hpp"


////////////////////////////////////////////////////////////////////////////////

sWindowStyle::sWindowStyle()
{
  Create(16, 16);
}

////////////////////////////////////////////////////////////////////////////////

void
sWindowStyle::Create(int width, int height)
{
  m_BackgroundMode = TILED;
  m_BackgroundCorners[0] = CreateRGBA(0, 0, 0, 255);
  m_BackgroundCorners[1] = CreateRGBA(0, 0, 0, 255);
  m_BackgroundCorners[2] = CreateRGBA(0, 0, 0, 255);
  m_BackgroundCorners[3] = CreateRGBA(0, 0, 0, 255);

  for (int i = 0; i < 9; i++)
  {
    m_Bitmaps[i].Resize(width, height);
    memset(m_Bitmaps[i].GetPixels(), 0, width * height * sizeof(RGBA));
  }
}

////////////////////////////////////////////////////////////////////////////////

#define STRUCT_NAME WINDOWSTYLE_HEADER
#include "begin_packed_struct.h"
  byte signature[4];
  word version;
  byte edge_width;  // only valid if version == 1
  byte background_mode;
  RGBA corner_colors[4];
  byte reserved[40];
#include "end_packed_struct.h"

ASSERT_STRUCT_SIZE(WINDOWSTYLE_HEADER, 64)

////////////////////////////////////////////////////////////////////////////////

bool
sWindowStyle::Load(const char* filename, IFileSystem& fs)
{
  IFile* file = fs.Open(filename, IFileSystem::read);
  if (file == NULL)
    return false;

  // read the header
  WINDOWSTYLE_HEADER header;
  file->Read(&header, sizeof(header));

  // check the header
  if (memcmp(header.signature, ".rws", 4) != 0 ||
      (header.version != 1 && header.version != 2))
  {
    return false;
  }

  m_BackgroundMode = header.background_mode;
  memcpy(m_BackgroundCorners, header.corner_colors, sizeof(RGBA) * 4);

  if (header.version == 1)
  {
    // free the old bitmaps and read the new ones
    for (int i = 0; i < 9; i++)
      ReadBitmap(file, m_Bitmaps + i, header.edge_width);
  }
  else
  {
    // free the old bitmaps and read the new ones
    for (int i = 0; i < 9; i++)
      ReadBitmap(file, m_Bitmaps + i);
  }

  file->Close();
  return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
sWindowStyle::Save(const char* filename, IFileSystem& fs) const
{
  IFile* file = fs.Open(filename, IFileSystem::write);
  if (file == NULL)
    return false;

  // write header
  WINDOWSTYLE_HEADER header;
  memset(&header, 0, sizeof(header));
  memcpy(header.signature, ".rws", 4);
  header.version = 2;
  header.background_mode = m_BackgroundMode;
  memcpy(header.corner_colors, m_BackgroundCorners, sizeof(RGBA) * 4);
  file->Write(&header, sizeof(header));

  // write the bitmaps
  for (int i = 0; i < 9; i++)
    WriteBitmap(file, m_Bitmaps + i);

  file->Close();
  return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
sWindowStyle::Import(const char* filename, RGBA transColor, IFileSystem& fs)
{
  CImage32 image;
  if (!image.Load(filename))
    return false;

  if (image.GetWidth()%3 || image.GetHeight()%3)
    return false;

  int winWidth = image.GetWidth() / 3;
  int winHeight = image.GetHeight() / 3;
  for (int i=0; i<9; i++)
    m_Bitmaps[i].Resize(winWidth, winHeight);

  // now to check for transparent color
  RGBA transparent = transColor;
  transparent.alpha = 0;
  for (int j=0; j<image.GetHeight(); j++)
    for (int i=0; i<image.GetWidth(); i++)
      if (image.GetPixels()[(j*image.GetWidth()) + i].green == transColor.green)
        if (image.GetPixels()[(j*image.GetWidth()) + i].blue == transColor.blue)
          if (image.GetPixels()[(j*image.GetWidth()) + i].red == transColor.red)
            if (image.GetPixels()[(j*image.GetWidth()) + i].alpha == transColor.alpha)
              image.GetPixels()[j*image.GetWidth() + i] = transparent;

#define CopyEdge(offX, startY, endY, window)                 \
{                                                            \
  for (int i=startY; i<endY; i++)                                \
    memcpy(window.GetPixels() + (i-startY)*winWidth,         \
           image.GetPixels() + (i*image.GetWidth()) + offX,  \
           winWidth*sizeof(RGBA));                           \
}
               
  CopyEdge(0,          0,           winHeight,   m_Bitmaps[0]);
  CopyEdge(winWidth,   0,           winHeight,   m_Bitmaps[1]);
  CopyEdge(winWidth*2, 0,           winHeight,   m_Bitmaps[2]);
  CopyEdge(0,          winHeight,   winHeight*2, m_Bitmaps[7]);
  CopyEdge(winWidth,   winHeight,   winHeight*2, m_Bitmaps[8]);
  CopyEdge(winWidth*2, winHeight,   winHeight*2, m_Bitmaps[3]);
  CopyEdge(0,          winHeight*2, winHeight*3, m_Bitmaps[6]);
  CopyEdge(winWidth,   winHeight*2, winHeight*3, m_Bitmaps[5]);
  CopyEdge(winWidth*2, winHeight*2, winHeight*3, m_Bitmaps[4]);

  return true;
}

////////////////////////////////////////////////////////////////////////////////

void
sWindowStyle::WriteBitmap(IFile* file, const CImage32* bitmap)
{
  word w = bitmap->GetWidth();
  word h = bitmap->GetHeight();
  file->Write(&w, 2);
  file->Write(&h, 2);
  file->Write(bitmap->GetPixels(), sizeof(RGBA) * w * h);
}

////////////////////////////////////////////////////////////////////////////////

void
sWindowStyle::ReadBitmap(IFile* file, CImage32* bitmap)
{
  word w, h;
  file->Read(&w, 2);
  file->Read(&h, 2);
  bitmap->Resize(w, h);
  file->Read(bitmap->GetPixels(), sizeof(RGBA) * w * h);
}

////////////////////////////////////////////////////////////////////////////////

void
sWindowStyle::ReadBitmap(IFile* file, CImage32* bitmap, int edge_width)
{
  bitmap->Resize(edge_width, edge_width);
  file->Read(bitmap->GetPixels(), sizeof(RGBA) * edge_width * edge_width);
}

////////////////////////////////////////////////////////////////////////////////
