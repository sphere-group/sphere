#include <stdio.h>
#include <string.h>
#include "Font.hpp"
#include "packed.hpp"


////////////////////////////////////////////////////////////////////////////////

sFont::sFont(int num_characters, int width, int height)
{
  m_Characters.resize(num_characters);
  for (int i = 0; i < num_characters; i++)
    m_Characters[i].Resize(width, height);
}

////////////////////////////////////////////////////////////////////////////////

#define STRUCT_NAME FONT_HEADER
#include "begin_packed_struct.h"
  byte signature[4];
  word version;
  word num_characters;
  byte reserved[248];
#include "end_packed_struct.h"

#define STRUCT_NAME CHARACTER_HEADER
#include "begin_packed_struct.h"
  word width;
  word height;
  byte reserved[28];
#include "end_packed_struct.h"

ASSERT_STRUCT_SIZE(FONT_HEADER, 256)
ASSERT_STRUCT_SIZE(CHARACTER_HEADER, 32);

////////////////////////////////////////////////////////////////////////////////

bool
sFont::Load(const char* filename, IFileSystem& fs)
{
  IFile* file = fs.Open(filename, IFileSystem::read);
  if (file == NULL) {
    return false;
  }

  // read and check header
  FONT_HEADER header;
  file->Read(&header, sizeof(header));
  if (memcmp(header.signature, ".rfn", 4) != 0 ||
      (header.version != 1 && header.version != 2))
  {
    file->Close();
    return false;
  }

  // allocate characters
  m_Characters.resize(header.num_characters);

  // read them
  for (int i = 0; i < m_Characters.size(); i++)
  {
    CHARACTER_HEADER character_header;
    file->Read(&character_header, sizeof(character_header));
    m_Characters[i].Resize(character_header.width, character_header.height);

    // version 1 = greyscale
    if (header.version == 1)
    {
      byte* buffer = new byte[character_header.width * character_header.height];
      file->Read(buffer, character_header.width * character_header.height);

      for (int j = 0; j < character_header.width * character_header.height; j++)
      {
        m_Characters[i].GetPixels()[j].red   = 255;
        m_Characters[i].GetPixels()[j].green = 255;
        m_Characters[i].GetPixels()[j].blue  = 255;
        m_Characters[i].GetPixels()[j].alpha = buffer[j];
      }
      
      delete[] buffer;
    }
    else  // version 2 (RGBA)
      file->Read(m_Characters[i].GetPixels(), sizeof(RGBA) * character_header.width * character_header.height);
  }

  file->Close();
  return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
sFont::Save(const char* filename, IFileSystem& fs) const
{
  IFile* file = fs.Open(filename, IFileSystem::write);
  if (file == NULL)
    return false;

  // write header
  FONT_HEADER header;
  memset(&header, 0, sizeof(header));
  memcpy(header.signature, ".rfn", 4);
  header.version = 2;
  header.num_characters = m_Characters.size();
  file->Write(&header, sizeof(header));

  // write characters
  for (int i = 0; i < m_Characters.size(); i++)
  {
    CHARACTER_HEADER character_header;
    memset(&character_header, 0, sizeof(character_header));
    character_header.width  = m_Characters[i].GetWidth();
    character_header.height = m_Characters[i].GetHeight();
    file->Write(&character_header, sizeof(character_header));
    file->Write(m_Characters[i].GetPixels(), character_header.width * character_header.height * sizeof(RGBA));
  }

  file->Close();
  return true;
}

////////////////////////////////////////////////////////////////////////////////

void
sFont::GenerateGradient(RGBA top, RGBA bottom)
{
  // get the max character height
  int max_height = 0;
  for (int i = 0; i < m_Characters.size(); i++) {
    if (m_Characters[i].GetHeight() > max_height) {
      max_height = m_Characters[i].GetHeight();
    }
  }

  int max_alpha = max_height - 1;

  // generate the gradient for each character
  for (int i = 0; i < m_Characters.size(); i++) {
    sFontCharacter& c = m_Characters[i];

    for (int iy = 0; iy < c.GetHeight(); iy++) {
      int width = c.GetWidth();
      RGBA* p = c.GetPixels() + iy * c.GetWidth();
      while (width--) {
        p->red   = (top.red   * (max_alpha - iy) + bottom.red   * iy) / max_alpha;
        p->green = (top.green * (max_alpha - iy) + bottom.green * iy) / max_alpha;
        p->blue  = (top.blue  * (max_alpha - iy) + bottom.blue  * iy) / max_alpha;
        p->alpha = p->alpha * (top.alpha  * (max_alpha - iy) + bottom.alpha  * iy) / max_alpha / 256;
        p++;
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

bool
sFont::ImportVergeTemplate(
  const char* filename,
  int char_width,
  int char_height,
  IFileSystem& fs)
{
  const int NUM_CHAR_COLUMNS = 20;
  const int NUM_CHAR_ROWS    = 5;

  const char* ROWS[NUM_CHAR_ROWS] = {
    " !\"#$%&'()*+,-./0123",
    "456789:;<=>?@ABCDEFG",
    "HIJKLMNOPQRSTUVWXYZ[",
    "/]^ 'abcdefghijklmno",
    "pqrstuvwxyz{|}~",
  };

  CImage32 image;
  if (!image.Load(filename, fs)) {
    return false;
  }

  // see if image is big enough
  if ((char_width + 1) * NUM_CHAR_COLUMNS > image.GetWidth() ||
      (char_height + 1) * NUM_CHAR_ROWS   > image.GetHeight()) {
    return false;
  }

  // initialize the font
  SetNumCharacters(256);

  // now go through and copy the characters out of the image
  for (int i = 0; i < NUM_CHAR_ROWS; i++) {
    const char* row = ROWS[i];
    int x = 1;
    int y = i * (char_height + 1) + 1;
    while (*row) {

      // build a font character from the position (x,y) in the image  
      sFontCharacter c(char_width + 1, char_height);
      for (int ix = 0; ix < char_width; ix++) {
        for (int iy = 0; iy < char_height; iy++) {
          c.SetPixel(ix, iy, image.GetPixel(x + ix, y + iy));
        }
      }
      GetCharacter(*row) = c;

      x += char_width + 1;
      row++;
    }
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////

void
sFont::SetNumCharacters(int num_characters)
{
  m_Characters.resize(num_characters);
}

////////////////////////////////////////////////////////////////////////////////
