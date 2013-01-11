#include <stdlib.h>
#include <string.h>
#include "sspriteset.hpp"
#include "../common/Spriteset.hpp"

////////////////////////////////////////////////////////////////////////////////
SSPRITESET::SSPRITESET()
        : m_Images(NULL)
        , m_FlipImages(NULL)
        , m_RefCount(1)
        , m_Filename("")

{}
////////////////////////////////////////////////////////////////////////////////
SSPRITESET::SSPRITESET(const sSpriteset& s)
        : m_Images(NULL)
        , m_FlipImages(NULL)
        , m_RefCount(1)
        , m_Filename("")
{
    m_Spriteset = s;
    Create();
}
////////////////////////////////////////////////////////////////////////////////
SSPRITESET::~SSPRITESET()
{

    Destroy();
}
////////////////////////////////////////////////////////////////////////////////
void
SSPRITESET::AddRef()
{

    m_RefCount++;
}

////////////////////////////////////////////////////////////////////////////////
void
SSPRITESET::Release()
{
    // this is dangerous
    // don't use refcounting on things on the stack
    if (--m_RefCount == 0)
    {
        delete this;
    }
}

////////////////////////////////////////////////////////////////////////////////
bool
SSPRITESET::Load(const char* filename, IFileSystem& fs, std::string pfilename)
{
    if (m_Spriteset.Load(filename, fs) == false)
    {
        return false;
    }

    if (!Create())
        return false;
    m_Filename = pfilename;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
SSPRITESET::Create()
{
    m_Images = new IMAGE[m_Spriteset.GetNumImages()];
    if (!m_Images)
    {
        return false;
    }

    m_FlipImages = new IMAGE[m_Spriteset.GetNumImages()];
    if (!m_FlipImages)
    {
        delete[] m_Images;
        m_Images = NULL;
        return false;
    }

    for (int i = 0; i < m_Spriteset.GetNumImages(); i++)
    {

        m_Images[i]     = CreateFrameImage(i);
        if (!m_Images[i])
        {
            Destroy();
            return false;
        }

        m_FlipImages[i] = CreateFlipFrameImage(i);
        if (!m_FlipImages[i])
        {
            Destroy();
            return false;
        }

    }
    return true;
}
////////////////////////////////////////////////////////////////////////////////
void
SSPRITESET::Destroy()
{
    for (int i = 0; i < m_Spriteset.GetNumImages(); i++)
    {

        if (m_Images)     if (m_Images[i])  DestroyImage(m_Images[i]);
        if (m_FlipImages) if (m_FlipImages) DestroyImage(m_FlipImages[i]);
    }
    delete[] m_Images;
    m_Images = NULL;

    delete[] m_FlipImages;
    m_FlipImages = NULL;

}
////////////////////////////////////////////////////////////////////////////////
const sSpriteset&
SSPRITESET::GetSpriteset() const
{
    return m_Spriteset;
}

void
SSPRITESET::SetBase(int x1, int y1, int x2, int y2)
{
	m_Spriteset.SetBase(x1, y1, x2, y2);
}

void
SSPRITESET::GetBase(int& x1, int& y1, int& x2, int& y2) const
{
	m_Spriteset.GetBase(x1, y1, x2, y2);
}

void
SSPRITESET::GetRealBase(int& x1, int& y1, int& x2, int& y2) const
{
	m_Spriteset.GetRealBase(x1, y1, x2, y2);
}


////////////////////////////////////////////////////////////////////////////////
std::string
SSPRITESET::GetFilename() const
{

    return m_Filename;
}
////////////////////////////////////////////////////////////////////////////////
int
SSPRITESET::GetNumImages() const
{
    return m_Spriteset.GetNumImages();
}

////////////////////////////////////////////////////////////////////////////////
IMAGE
SSPRITESET::GetImage(int image) const
{
    return m_Images[image];
}

////////////////////////////////////////////////////////////////////////////////
IMAGE
SSPRITESET::GetFlipImage(int image) const
{
    return m_FlipImages[image];
}

////////////////////////////////////////////////////////////////////////////////
IMAGE
SSPRITESET::CreateFrameImage(int index)
{
    CImage32& sprite = m_Spriteset.GetImage(index);
    return CreateImage(sprite.GetWidth(), sprite.GetHeight(), sprite.GetPixels());
}

////////////////////////////////////////////////////////////////////////////////
IMAGE
SSPRITESET::CreateFlipFrameImage(int index)
{
    CImage32 sprite = m_Spriteset.GetImage(index);
    sprite.FlipVertical();
    return CreateImage(sprite.GetWidth(), sprite.GetHeight(), sprite.GetPixels());
}

////////////////////////////////////////////////////////////////////////////////
