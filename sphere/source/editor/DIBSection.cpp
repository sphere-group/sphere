#include <windows.h>
#include "DIBSection.hpp"
////////////////////////////////////////////////////////////////////////////////
CDIBSection::CDIBSection()
{
  m_iWidth = 0;
  m_iHeight = 0;
  m_pPixels = NULL;
}
////////////////////////////////////////////////////////////////////////////////
CDIBSection::CDIBSection(int width, int height, int bpp)
{
  Create(width, height, bpp);
}
////////////////////////////////////////////////////////////////////////////////
CDIBSection::~CDIBSection()
{
  Destroy();
}
////////////////////////////////////////////////////////////////////////////////
bool
CDIBSection::Create(int width, int height, int bpp)
{
  Destroy();
  if (width <= 0 || height <= 0 || bpp <= 0)
  {
    m_hDC = NULL;
    m_hBitmap = NULL;
    return false;
  }
  // create the DC
  m_hDC = ::CreateCompatibleDC(NULL);
  if (m_hDC == NULL) {
    m_hDC = NULL;
    m_hBitmap = NULL;
    m_iWidth = 0;
    m_iHeight = 0;
    return false;
  }
  // define bitmap attributes
  BITMAPINFO bmi;
  memset(&bmi, 0, sizeof(bmi));
  BITMAPINFOHEADER& bmih = bmi.bmiHeader;
  bmih.biSize        = sizeof(bmih);
  bmih.biWidth       = width;
  bmih.biHeight      = -height;
  bmih.biPlanes      = 1;
  bmih.biBitCount    = bpp;
  bmih.biCompression = BI_RGB;
  // create the bitmap
  m_hBitmap = ::CreateDIBSection(::GetDC(NULL), &bmi, DIB_RGB_COLORS, &m_pPixels, NULL, 0);
  if (m_hBitmap == NULL) {
   ::DeleteDC(m_hDC);
    m_hDC = NULL;
    m_iWidth = 0;
    m_iHeight = 0;
    return false;
  }
  // select the bitmap into the DC
  ::SelectObject(m_hDC, m_hBitmap);
  m_iWidth = width;
  m_iHeight = height;
  return true;
}
////////////////////////////////////////////////////////////////////////////////
void
CDIBSection::Destroy()
{
  if (m_hDC != NULL)
    ::DeleteDC(m_hDC);
  m_hDC = NULL;
  if (m_hBitmap != NULL)
    ::DeleteObject(m_hBitmap);
  m_hBitmap = NULL;
  m_iWidth = 0;
  m_iHeight = 0;
  m_pPixels = NULL;
}
////////////////////////////////////////////////////////////////////////////////
HDC
CDIBSection::GetDC()
{
  return m_hDC;
}
////////////////////////////////////////////////////////////////////////////////
void*
CDIBSection::GetPixels()
{
  return m_pPixels;
}
////////////////////////////////////////////////////////////////////////////////
const void*
CDIBSection::GetPixels() const
{
  return m_pPixels;
}
////////////////////////////////////////////////////////////////////////////////
int
CDIBSection::GetWidth() const
{
  return m_iWidth;
}
////////////////////////////////////////////////////////////////////////////////
int
CDIBSection::GetHeight() const
{
  return m_iHeight;
}
////////////////////////////////////////////////////////////////////////////////
