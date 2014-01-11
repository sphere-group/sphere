#include "Layer.hpp"


////////////////////////////////////////////////////////////////////////////////

sLayer::sLayer()
: m_Width(0)
, m_Height(0)
, m_Tiles(NULL)

, m_HasParallax(false)
, m_XParallax(1)
, m_YParallax(1)
, m_XScrolling(0)
, m_YScrolling(0)

, m_Visible(true)
{
}

////////////////////////////////////////////////////////////////////////////////

sLayer::sLayer(int width, int height)
: m_Width(width)
, m_Height(height)
, m_Tiles(new int[width * height])

, m_HasParallax(false)
, m_XParallax(1)
, m_YParallax(1)
, m_XScrolling(0)
, m_YScrolling(0)

, m_Visible(true)
{
  for (int i = 0; i < width * height; i++)
    m_Tiles[i] = 0;
}

////////////////////////////////////////////////////////////////////////////////

sLayer::sLayer(const sLayer& layer)
{
  m_Name = layer.m_Name;

  m_Width  = layer.m_Width;
  m_Height = layer.m_Height;
  m_Tiles  = new int[m_Width * m_Height];
  memcpy(m_Tiles, layer.m_Tiles, m_Width * m_Height * sizeof(int));

  m_HasParallax = layer.m_HasParallax;
  m_XParallax   = layer.m_XParallax;
  m_YParallax   = layer.m_YParallax;
  m_XScrolling  = layer.m_XScrolling;
  m_YScrolling  = layer.m_YScrolling;

  m_Visible    = layer.m_Visible;
  m_Reflective = layer.m_Reflective;

  m_ObstructionMap = layer.m_ObstructionMap;
}

////////////////////////////////////////////////////////////////////////////////

sLayer::~sLayer()
{
  delete[] m_Tiles;
}

////////////////////////////////////////////////////////////////////////////////

const sLayer&
sLayer::operator=(const sLayer& layer)
{
  if (this != &layer) {

    m_Name = layer.m_Name;

    // delete the old layer
    delete[] m_Tiles;

    // replace it with the new stuff
    m_Width  = layer.m_Width;
    m_Height = layer.m_Height;
    m_Tiles  = new int[m_Width * m_Height];
    memcpy(m_Tiles, layer.m_Tiles, m_Width * m_Height * sizeof(int));

    m_HasParallax = layer.m_HasParallax;
    m_XParallax   = layer.m_XParallax;
    m_YParallax   = layer.m_YParallax;
    m_XScrolling  = layer.m_XScrolling;
    m_YScrolling  = layer.m_YScrolling;

    m_Visible    = layer.m_Visible;
    m_Reflective = layer.m_Reflective;

    m_ObstructionMap = layer.m_ObstructionMap;
  }

  return *this;
}

////////////////////////////////////////////////////////////////////////////////

void
sLayer::Resize(int width, int height)
{
  int* new_layer = new int[width * height];
  for (int iy = 0; iy < height; iy++)
    for (int ix = 0; ix < width; ix++)
    {
      if (ix < m_Width && iy < m_Height)
        new_layer[iy * width + ix] = m_Tiles[iy * m_Width + ix];
      else
        new_layer[iy * width + ix] = 0;
    }

  m_Width = width;
  m_Height = height;
  delete[] m_Tiles;
  m_Tiles = new_layer;
}

////////////////////////////////////////////////////////////////////////////////

void
sLayer::Replace(int old_index, int new_index)
{
  for (int iy = 0; iy < m_Height; iy++) {
    for (int ix = 0; ix < m_Width; ix++) {
      if (m_Tiles[iy * m_Width + ix] == old_index) {
        m_Tiles[iy * m_Width + ix] = new_index;
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

void
sLayer::SetName(const char* name)
{
  m_Name = name;
}

////////////////////////////////////////////////////////////////////////////////

const char*
sLayer::GetName() const
{
  return m_Name.c_str();
}

////////////////////////////////////////////////////////////////////////////////

void
sLayer::SetVisible(bool visible)
{
  m_Visible = visible;
}

////////////////////////////////////////////////////////////////////////////////

void
sLayer::SetReflective(bool reflective)
{
  m_Reflective = reflective;
}

////////////////////////////////////////////////////////////////////////////////
