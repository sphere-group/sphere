#include <memory>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "Font.hpp"
#include "endian.hpp"

#include "packed.hpp"

////////////////////////////////////////////////////////////////////////////////

sFont::sFont(int num_characters, int width, int height)
{
    Create(num_characters, width, height);
}

////////////////////////////////////////////////////////////////////////////////

bool
sFont::Create(int num_characters, int width, int height)
{
    m_Characters.resize(num_characters);
    for (int i = 0; i < int(m_Characters.size()); i++)
    {

        m_Characters[i].Resize(width, height);
        if (m_Characters[i].GetWidth() != width
                || m_Characters[i].GetHeight() != height)
        {

            return false;
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////

#define STRUCT_NAME FONT_HEADER
#define STRUCT_BODY                             \
  byte signature[4];                            \
  word version;                                 \
  word num_characters;                          \
  byte reserved[248];
#include "packed_struct.h"

#define STRUCT_NAME CHARACTER_HEADER
#define STRUCT_BODY                             \
  word width;                                   \
  word height;                                  \
  byte reserved[28];
#include "packed_struct.h"

ASSERT_STRUCT_SIZE(FONT_HEADER, 256)
ASSERT_STRUCT_SIZE(CHARACTER_HEADER, 32);

////////////////////////////////////////////////////////////////////////////////

bool
sFont::Load(const char* filename, IFileSystem& fs)
{
    std::auto_ptr<IFile> file(fs.Open(filename, IFileSystem::read));
    if (!file.get())
    {
        printf("Could not open font file: %s\n", filename);
        return false;
    }

    // read and check header
    FONT_HEADER header;
    if (file->Read(&header, sizeof(header)) != sizeof(header))
    {

        return false;
    }
    header.version = ltom_w(header.version);
    header.num_characters = ltom_w(header.num_characters);
    if (memcmp(header.signature, ".rfn", 4) != 0)
    {
        printf("Invalid signature in font header...\n");
        return false;
    }

    if (header.version != 1 && header.version != 2)
    {
        printf("Invalid version in font header... [%d]\n", header.version);
        return false;
    }
    if (header.num_characters <= 0)
    {
        printf ("Invalid number of characters in font header... [%d]\n", header.num_characters);
        return false;
    }

    // allocate characters
    m_Characters.resize(header.num_characters);

    if (m_Characters.size() != header.num_characters)
    {

        return false;
    }

    // read them
    for (unsigned int i = 0; i < m_Characters.size(); i++)
    {

        CHARACTER_HEADER character_header;
        if (file->Read(&character_header, sizeof(character_header)) != sizeof(character_header))

        {
            return false;
        }

        character_header.width = ltom_w(character_header.width);
        character_header.height = ltom_w(character_header.height);
        // is the character size feasible?
        if (character_header.width  > 4096

                || character_header.height > 4096)
        {
            printf ("Character %d is too big.... [%d x %d]\n", i, character_header.width, character_header.height);

            return false;
        }

        m_Characters[i].Resize(character_header.width, character_header.height);
        if (m_Characters[i].GetWidth()  != character_header.width
                || m_Characters[i].GetHeight() != character_header.height)
        {
            return false;
        }

        // version 1 = greyscale
        if (header.version == 1)
        {

            int size = character_header.width * character_header.height;
            byte* buffer = new byte[size];
            if (!buffer)
                return false;
            if (file->Read(buffer, size) != size)
            {
                return false;
            }

            for (int j = 0; j < size; j++)
            {
                m_Characters[i].GetPixels()[j].red   = 255;
                m_Characters[i].GetPixels()[j].green = 255;
                m_Characters[i].GetPixels()[j].blue  = 255;
                m_Characters[i].GetPixels()[j].alpha = buffer[j];
            }

            delete[] buffer;
        }
        else
        { // version 2 (RGBA)

            int size = sizeof(RGBA) * character_header.width * character_header.height;
            if (file->Read(m_Characters[i].GetPixels(), size) != size)

            {
                return false;
            }
        }

    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
sFont::Save(const char* filename, IFileSystem& fs) const
{
    std::auto_ptr<IFile> file(fs.Open(filename, IFileSystem::write));
    if (!file.get())
        return false;

    // write header
    FONT_HEADER header;
    memset(&header, 0, sizeof(header));
    memcpy(header.signature, ".rfn", 4);
    header.version = mtol_w(2);
    header.num_characters = mtol_w(m_Characters.size());

    if (file->Write(&header, sizeof(header)) != sizeof(header))
    {
        return false;
    }

    // write characters
    for (unsigned int i = 0; i < m_Characters.size(); i++)
    {
        CHARACTER_HEADER character_header;

        memset(&character_header, 0, sizeof(character_header));
        character_header.width  = mtol_w(m_Characters[i].GetWidth());
        character_header.height = mtol_w(m_Characters[i].GetHeight());

        if (file->Write(&character_header, sizeof(character_header)) != sizeof(character_header))
        {
            return false;
        }

        int size = m_Characters[i].GetWidth() * m_Characters[i].GetHeight() * sizeof(RGBA);
        if (file->Write(m_Characters[i].GetPixels(), size) != size)
        {
            return false;
        }

    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////

void
GenerateGradient(CImage32& c, int max_alpha, RGBA top, RGBA bottom)
{
    for (int iy = 0; iy < c.GetHeight(); iy++)
    {

        int width = c.GetWidth();
        RGBA* p = c.GetPixels() + iy * c.GetWidth();
        while (width--)
        {

            p->red   = (top.red   * (max_alpha - iy) + bottom.red   * iy) / max_alpha;
            p->green = (top.green * (max_alpha - iy) + bottom.green * iy) / max_alpha;
            p->blue  = (top.blue  * (max_alpha - iy) + bottom.blue  * iy) / max_alpha;
            p->alpha = p->alpha * (top.alpha  * (max_alpha - iy) + bottom.alpha  * iy) / max_alpha / 256;
            p++;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void
sFont::GenerateGradient(RGBA top, RGBA bottom)
{
    unsigned int i;

    // get the max character height
    int max_height = 0;

    for (i = 0; i < m_Characters.size(); i++)
    {
        if (m_Characters[i].GetHeight() > max_height)
        {
            max_height = m_Characters[i].GetHeight();
        }
    }

    int max_alpha = max_height - 1;
    // generate the gradient for each character
    for (i = 0; i < m_Characters.size(); i++)
    {
        sFontCharacter& c = m_Characters[i];

        ::GenerateGradient(c, max_alpha, top, bottom);
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

    const char* ROWS[NUM_CHAR_ROWS] =
        {
            " !\"#$%&'()*+,-./0123",
            "456789:;<=>?@ABCDEFG",
            "HIJKLMNOPQRSTUVWXYZ[",
            "/]^ 'abcdefghijklmno",
            "pqrstuvwxyz{|}~",
        };

    CImage32 image;
    if (!image.Load(filename, fs))
    {
        return false;
    }

    // see if image is big enough
    if ((char_width + 1) * NUM_CHAR_COLUMNS > image.GetWidth() ||
            (char_height + 1) * NUM_CHAR_ROWS   > image.GetHeight())
    {
        return false;
    }

    // initialize the font
    SetNumCharacters(256);

    // now go through and copy the characters out of the image
    for (int i = 0; i < NUM_CHAR_ROWS; i++)
    {
        const char* row = ROWS[i];
        int x = 1;
        int y = i * (char_height + 1) + 1;
        while (*row)
        {

            // build a font character from the position (x,y) in the image
            sFontCharacter c(char_width + 1, char_height);
            for (int ix = 0; ix < char_width; ix++)
            {
                for (int iy = 0; iy < char_height; iy++)
                {
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
