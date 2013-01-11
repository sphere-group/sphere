#include <memory>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "WindowStyle.hpp"
#include "endian.hpp"

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
    m_EdgeOffsets[0]       = 0;
    m_EdgeOffsets[1]       = 0;
    m_EdgeOffsets[2]       = 0;
    m_EdgeOffsets[3]       = 0;

    for (int i = 0; i < 9; i++)
    {
        m_Bitmaps[i].Resize(width, height);

        if (m_Bitmaps[i].GetWidth() == width && m_Bitmaps[i].GetHeight() == height)
        {

            memset(m_Bitmaps[i].GetPixels(), 0, width * height * sizeof(RGBA));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
#define STRUCT_NAME WINDOWSTYLE_HEADER
#define STRUCT_BODY                                     \
  byte signature[4];                                    \
  word version;                                         \
  byte edge_width;  /* only valid if version == 1 */    \
  byte background_mode;                                 \
  RGBA corner_colors[4];                                \
  byte edge_offsets[4];                                 \
  byte reserved[36];
#include "packed_struct.h"

ASSERT_STRUCT_SIZE(WINDOWSTYLE_HEADER, 64)
////////////////////////////////////////////////////////////////////////////////
bool
sWindowStyle::Load(const char* filename, IFileSystem& fs)
{

    std::auto_ptr<IFile> file(fs.Open(filename, IFileSystem::read));
    if (!file.get())
    {
        printf("Could not open windowstyle file: %s\n", filename);

        return false;
    }
    // read the header
    WINDOWSTYLE_HEADER header;
    if (file->Read(&header, sizeof(header)) != sizeof(header))

    {
        return false;
    }

    header.version = ltom_w(header.version);
    // check the header
    if (memcmp(header.signature, ".rws", 4) != 0)
    {
        printf("Invalid signature in windowstyle header...\n");

        return false;
    }

    if (header.version != 1 && header.version != 2)
    {
        printf("Invalid version in windowstyle header... [%d]\n", header.version);
        return false;
    }
    m_BackgroundMode = header.background_mode;
    memcpy(m_BackgroundCorners, header.corner_colors, sizeof(RGBA) * 4);
    memcpy(m_EdgeOffsets, header.edge_offsets, sizeof(byte) * 4);

    if (header.version == 1)
    {
        // free the old bitmaps and read the new ones
        for (int i = 0; i < 9; i++)
            ReadBitmap(file.get(), m_Bitmaps + i, header.edge_width);
    }
    else
    {
        // free the old bitmaps and read the new ones
        for (int i = 0; i < 9; i++)
            ReadBitmap(file.get(), m_Bitmaps + i);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
sWindowStyle::Save(const char* filename, IFileSystem& fs) const
{
    std::auto_ptr<IFile> file(fs.Open(filename, IFileSystem::write));
    if (!file.get())
        return false;

    // write header
    WINDOWSTYLE_HEADER header;
    memset(&header, 0, sizeof(header));
    memcpy(header.signature, ".rws", 4);
    header.version = mtol_w(2);
    header.background_mode = m_BackgroundMode;
    memcpy(header.corner_colors, m_BackgroundCorners, sizeof(RGBA) * 4);
    memcpy(header.edge_offsets, m_EdgeOffsets, sizeof(byte) * 4);

    if (file->Write(&header, sizeof(header)) != sizeof(header))
    {
        return false;
    }

    // write the bitmaps
    for (int i = 0; i < 9; i++)

    {
        if (!WriteBitmap(file.get(), m_Bitmaps + i))

        {
            return false;
        }

    }
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
    {
        m_Bitmaps[i].Resize(winWidth, winHeight);

        if (m_Bitmaps[i].GetWidth() != winWidth || m_Bitmaps[i].GetHeight() != winHeight)
            return false;
    }
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
  for (int i=startY; i<endY; i++)                            \
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

#undef CopyEdge
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
sWindowStyle::WriteBitmap(IFile* file, const CImage32* bitmap)
{

    word w = mtol_w(bitmap->GetWidth());
    word h = mtol_w(bitmap->GetHeight());

    if (file->Write(&w, 2) != 2)
    {

        return false;
    }
    if (file->Write(&h, 2) != 2)
    {

        return false;
    }
    if (file->Write(bitmap->GetPixels(), sizeof(RGBA) * w * h) != sizeof(RGBA) * w * h)
    {

        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
sWindowStyle::ReadBitmap(IFile* file, CImage32* bitmap)
{
    word w, h;
    file->Read(&w, 2);
    file->Read(&h, 2);

    w = ltom_w(w);
    h = ltom_w(h);
    if (w < 0 || w > 4096)
    {

        return false;
    }
    if (h < 0 || h > 4096)
    {

        return false;
    }
    bitmap->Resize(w, h);
    if (bitmap->GetWidth() != w || bitmap->GetHeight() != h)
        return false;
    if (file->Read(bitmap->GetPixels(), sizeof(RGBA) * w * h) != sizeof(RGBA) * w * h)
        return false;
    return true;
}
////////////////////////////////////////////////////////////////////////////////
bool
sWindowStyle::ReadBitmap(IFile* file, CImage32* bitmap, int edge_width)
{

    if (edge_width < 0 || edge_width > 4096)
    {

        return false;
    }
    bitmap->Resize(edge_width, edge_width);
    if (bitmap->GetWidth() != edge_width || bitmap->GetHeight() != edge_width)

    {
        return false;
    }
    int size = sizeof(RGBA) * edge_width * edge_width;
    if (file->Read(bitmap->GetPixels(), size) != size)
    {
        return false;
    }

    return true;
}
////////////////////////////////////////////////////////////////////////////////
