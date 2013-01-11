#include <string.h>
#include <assert.h>

#include "Spriteset.hpp"
#include "packed.hpp"
#include "strcmp_ci.hpp"
#include "minmax.hpp"

#include "endian.hpp"
const int DEFAULT_DELAY = 8;
////////////////////////////////////////////////////////////////////////////////
sSpriteset::sSpriteset()
        : m_FrameWidth(0)
        , m_FrameHeight(0)
        , m_BaseX1(0)
        , m_BaseY1(0)
        , m_BaseX2(0)
        , m_BaseY2(0)
        , org_BaseX1(0)
        , org_BaseY1(0)
        , org_BaseX2(0)
        , org_BaseY2(0)
{}

////////////////////////////////////////////////////////////////////////////////
sSpriteset::~sSpriteset()
{}

////////////////////////////////////////////////////////////////////////////////
bool
sSpriteset::Create(
    int frame_width,
    int frame_height,
    int num_images,
    int num_directions,
    int num_frames
)
{
    unsigned int i;

    m_FrameWidth  = frame_width;
    m_FrameHeight = frame_height;

    // initialize the images
    m_Images.resize(num_images);
    for (i = 0; i < m_Images.size(); i++)
    {
        m_Images[i].Resize(frame_width, frame_height);
        if (m_Images[i].GetWidth() != frame_width
                || m_Images[i].GetHeight() != frame_height)
        {
            return false;
        }
    }

    // initialize the directions
    m_Directions.resize(num_directions);
    for (i = 0; i < m_Directions.size(); i++)
    {
        // initialize the frames
        m_Directions[i].frames.resize(num_frames);
        for (unsigned int j = 0; j < m_Directions[i].frames.size(); j++)
        {
            m_Directions[i].name = "unnamed";
            m_Directions[i].frames[j].index = 0;
            m_Directions[i].frames[j].delay = DEFAULT_DELAY;
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
#define STRUCT_NAME SPRITESET_HEADER
#define STRUCT_BODY                             \
  byte signature[4];                            \
  word version;                                 \
  word num_images;                              \
  word frame_width;                             \
  word frame_height;                            \
  word num_directions;                          \
  word base_x1;                                 \
  word base_y1;                                 \
  word base_x2;                                 \
  word base_y2;                                 \
  byte reserved[106];
#include "packed_struct.h"

#define STRUCT_NAME DIRECTION_HEADER_2
#define STRUCT_BODY                             \
  word num_frames;                              \
  byte reserved[62];
#include "packed_struct.h"

#define STRUCT_NAME FRAME_HEADER_2
#define STRUCT_BODY                             \
  word width;                                   \
  word height;                                  \
  word delay;                                   \
  byte reserved[26];
#include "packed_struct.h"

ASSERT_STRUCT_SIZE(SPRITESET_HEADER,   128)
ASSERT_STRUCT_SIZE(DIRECTION_HEADER_2, 64)
ASSERT_STRUCT_SIZE(FRAME_HEADER_2,     32)

////////////////////////////////////////////////////////////////////////////////
template<typename d, typename s>
void bracket(d& dest, s min, s max)
{
    if (dest < min)
        dest = min;
    else if (dest > max)
        dest = max;
}

template<typename T>
int Find(std::vector<T>& vs, const T& t)
{
    for (unsigned int i = 0; i < vs.size(); i++)
        if (vs[i] == t)
            return i;

    vs.push_back(t);
    return vs.size() - 1;
}

bool
sSpriteset::Load(const char* filename, IFileSystem& fs)
{
    // open file
    std::auto_ptr<IFile> file(fs.Open(filename, IFileSystem::read));
    if (!file.get())
    {
        printf("Could not open spriteset file: %s\n", filename);
        return false;
    }

    // read the header
    SPRITESET_HEADER header;
    if (file->Read(&header, sizeof(header)) != sizeof(header))
        return false;

    header.version = ltom_w(header.version);
    header.num_images   = ltom_w(header.num_images);
    header.frame_width  = ltom_w(header.frame_width);
    header.frame_height = ltom_w(header.frame_height);
    header.num_directions = ltom_w(header.num_directions);
    header.base_x1 = ltom_w(header.base_x1);
    header.base_y1 = ltom_w(header.base_y1);
    header.base_x2 = ltom_w(header.base_x2);
    header.base_y2 = ltom_w(header.base_y2);

    // validate header
    if (memcmp(header.signature, ".rss", 4) != 0)
    {
        printf("Invalid signature in spriteset header...\n");
        return false;
    }

    if (header.version != 1 && header.version != 2 && header.version != 3)
    {
        printf("Invalid version in spriteset header... [%d]\n", header.version);
        return false;
    }

    if (header.frame_width <= 0  || header.frame_width > 4096
            || header.frame_height <= 0 || header.frame_height > 4096)
    {
        printf ("Invalid frame size in spriteset header... [%d x %d]\n", header.frame_width, header.frame_height);
        return false;
    }

    m_FrameWidth  = header.frame_width;
    m_FrameHeight = header.frame_height;

    m_BaseX1 = header.base_x1;
    m_BaseY1 = header.base_y1;
    m_BaseX2 = header.base_x2;
    m_BaseY2 = header.base_y2;

    // check the base values
    bracket(m_BaseX1, 0, m_FrameWidth  - 1);
    bracket(m_BaseX2, 0, m_FrameWidth  - 1);
    bracket(m_BaseY1, 0, m_FrameHeight - 1);
    bracket(m_BaseY2, 0, m_FrameHeight - 1);

	// store original base somewhere save
	org_BaseX1 = m_BaseX1;
	org_BaseY1 = m_BaseY1;
	org_BaseX2 = m_BaseX2;
	org_BaseY2 = m_BaseY2;

    static const char* direction_names[] =
        {
            "north",
            "northeast",
            "east",
            "southeast",
            "south",
            "southwest",
            "west",
            "northwest",
        };

    if (header.version == 1)
    {
        //**// BEGIN VERSION ONE //**//
        m_Images.resize(64);
        m_Directions.resize(8);

        // for each direction
        for (int i = 0; i < 8; i++)
        {
            m_Directions[i].name = direction_names[i];
            m_Directions[i].frames.resize(8);

            // for each frame
            for (int j = 0; j < 8; j++)
            {
                m_Directions[i].frames[j].index = i * 8 + j;
                m_Directions[i].frames[j].delay = 8;

                // read the image
                CImage32& image = m_Images[i * 8 + j];
                image.Resize(m_FrameWidth, m_FrameHeight);

                int size = m_FrameWidth * m_FrameHeight * sizeof(RGBA);
                if (file->Read(image.GetPixels(), size) != size)
                    return false;
            }
        }
        //**// END VERSION ONE //**//
    }
    else if (header.version == 2)
    {
        //**// BEGIN VERSION TWO //**//
        m_Images.clear();
        m_Directions.resize(header.num_directions);

        for (int i = 0; i < header.num_directions; i++)
        {
            // read the direction header
            DIRECTION_HEADER_2 direction_header;
            if (file->Read(&direction_header, sizeof(direction_header)) != sizeof(direction_header))
                return false;

            direction_header.num_frames = ltom_w(direction_header.num_frames);
            // set name
            if (i < 8)
                m_Directions[i].name = direction_names[i];
            else
            {
                char name[80];
                sprintf(name, "extra %d", i);
                m_Directions[i].name = name;
            }

            // read frames
            m_Directions[i].frames.resize(direction_header.num_frames);
            for (int j = 0; j < direction_header.num_frames; j++)
            {
                // read the frame header
                FRAME_HEADER_2 frame_header;
                if (file->Read(&frame_header, sizeof(frame_header)) != sizeof(frame_header))
                    return false;

                frame_header.width  = ltom_w(frame_header.width);
                frame_header.height = ltom_w(frame_header.height);
                frame_header.delay  = ltom_w(frame_header.delay);

                // some backwards compatibility hacking
                if (m_FrameWidth == 0 || m_FrameHeight == 0)
                {
                    m_FrameWidth  = frame_header.width;
                    m_FrameHeight = frame_header.height;
                }

                // read the image and add it to the list
                CImage32 image(m_FrameWidth, m_FrameHeight);

                int size = sizeof(RGBA) * m_FrameWidth * m_FrameHeight;
                if (file->Read(image.GetPixels(), size) != size)
                    return false;

                // set the frame properties
                m_Directions[i].frames[j].index = Find(m_Images, image);
                m_Directions[i].frames[j].delay = frame_header.delay;
            }
        }
        //**// END VERSION TWO //**//
    }
    else if (header.version == 3)
    {  // VERSION THREE
        //**// BEGIN VERSION THREE //**//
        int i;

		// read the images
        m_Images.resize(header.num_images);
        for (i = 0; i < header.num_images; i++)
        {
            m_Images[i].Resize(m_FrameWidth, m_FrameHeight);
            int size = m_FrameWidth * m_FrameHeight * sizeof(RGBA);
            if (file->Read(m_Images[i].GetPixels(), size) != size)
                return false;
        }

        // read the directions
        m_Directions.resize(header.num_directions);
        for (i = 0; i < header.num_directions; i++)
        {
            // read number of frames
            word num_frames;
            if (file->Read(&num_frames, 2) != 2)
                return false;

			num_frames = ltom_w(num_frames);
            if (num_frames <= 0)
                return false;

            file->Seek(file->Tell() + 6);
            // read name length
            word name_length;
            if (file->Read(&name_length, 2) != 2)
                return false;

            name_length = ltom_w(name_length);
            if (name_length <= 0)
                return false;

            // read the name
            char* name = new char[name_length];
            if (!name)
                return false;

            if (file->Read(name, name_length) != name_length)
            {
                delete[] name;
                name = NULL;
                return false;
            }
            m_Directions[i].name = name;
            delete[] name;

            // read frames
            m_Directions[i].frames.resize(num_frames);
            for (int j = 0; j < num_frames; j++)
            {
                // read frame header
                word index;
                word delay;
                byte reserved[4];
                int read = 0;

                read += file->Read(&index,   2);
                read += file->Read(&delay,   2);
                read += file->Read(reserved, 4);

                index = ltom_w(index);
                delay = ltom_w(delay);

                if (read != 8)
                    return false;

                m_Directions[i].frames[j].index = index;
                m_Directions[i].frames[j].delay = delay;
            }
        }
        //**// END VERSION THREE //**//
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
sSpriteset::Save(const char* filename, IFileSystem& fs) const
{
    // open file
    std::auto_ptr<IFile> file(fs.Open(filename, IFileSystem::write));
    if (!file.get())
        return false;

    // fill header
    SPRITESET_HEADER header;
    memset(&header, 0, sizeof(header));
    memcpy(header.signature, ".rss", 4);
    header.version        = mtol_w(3);
    header.num_images     = mtol_w(m_Images.size());
    header.frame_width    = mtol_w(m_FrameWidth);
    header.frame_height   = mtol_w(m_FrameHeight);
    header.num_directions = mtol_w(m_Directions.size());
    header.base_x1        = mtol_w(org_BaseX1);
    header.base_y1        = mtol_w(org_BaseY1);
    header.base_x2        = mtol_w(org_BaseX2);
    header.base_y2        = mtol_w(org_BaseY2);

    if (header.base_x1 > header.base_x2)
    {
        int temp = header.base_x1;
        header.base_x1 = header.base_x2;
        header.base_x2 = temp;
    }
    if (header.base_y1 > header.base_y2)
    {
        int temp = header.base_y1;
        header.base_y1 = header.base_y2;
        header.base_y2 = temp;
    }

    // write header
    file->Write(&header, sizeof(header));

    unsigned int i;
    // write the images
    for (i = 0; i < m_Images.size(); i++)
        file->Write(m_Images[i].GetPixels(), m_FrameWidth * m_FrameHeight * 4);

    // write all of the directions
    for (i = 0; i < m_Directions.size(); i++)
    {
        // write direction header
        word num_frames = mtol_w(m_Directions[i].frames.size());
        byte empty6[] = { 0, 0, 0, 0, 0, 0 };
        file->Write(&num_frames, 2);
        file->Write(empty6, 6);

        // write direction name
        word name_length = mtol_w(m_Directions[i].name.length() + 1);
        file->Write(&name_length, 2);
        file->Write(m_Directions[i].name.c_str(), name_length);

        // write frames
        for (unsigned int j = 0; j < m_Directions[i].frames.size(); j++)
        {
            word index = mtol_w(m_Directions[i].frames[j].index);
            word delay = mtol_w(m_Directions[i].frames[j].delay);
            file->Write(&index, 2);
            file->Write(&delay, 2);
            file->Write(empty6, 4);
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
sSpriteset::Import_BMP(
    const char* filename,
    int frame_width,
    int frame_height,
    RGBA old_color, RGBA new_color)
{
    CImage32 image;
    if (!image.Load(filename))
        return false;

    if (image.GetWidth() % frame_width || image.GetHeight() % frame_height)
        return false;

    // replace the transparent color
    RGBA* pixels = image.GetPixels();
    int num_pixels = image.GetWidth() * image.GetHeight();
    while (num_pixels--)
    {
        if (pixels->red   == old_color.red   &&
                pixels->green == old_color.green &&
                pixels->blue  == old_color.blue  &&
                pixels->alpha == old_color.alpha)
        {
            pixels->red = new_color.red;
            pixels->green = new_color.green;
            pixels->blue  = new_color.blue;
            pixels->alpha = new_color.alpha;
        }
        pixels++;
    }

    m_FrameWidth = frame_width;
    m_FrameHeight = frame_height;
    int numRows = image.GetHeight() / frame_height;
    int numFrames = image.GetWidth() / frame_width;

    int i;
    // create image array
    m_Images.resize(numRows * numFrames);
    for (i = 0; i < numRows * numFrames; i++)
        m_Images[i].Resize(frame_width, frame_height);

    // for each row
    m_Directions.resize(numRows);
    for (i = 0; i < numRows; i++)
    {
        m_Directions[i].frames.resize(numFrames);
        for (int j = 0; j < numFrames; j++)
        {
            m_Directions[i].frames[j].delay = 8;
            m_Directions[i].frames[j].index = i * numFrames + j;
        }
    }

    // grabs the data in a gigantic loops 'o doom...
    for (int j = 0; j < numRows; j++)
    {
        for (int i = 0; i < numFrames; i++)
        {
            for (int k = 0; k < m_FrameHeight; k++)
            {
                RGBA* dest = m_Images[m_Directions[j].frames[i].index].GetPixels() +
                             (k * m_FrameWidth);
                RGBA* src  = image.GetPixels() +
                             (j * m_FrameHeight * image.GetWidth()) +
                             (i * m_FrameWidth) + (k * image.GetWidth());
                memcpy(dest, src, m_FrameWidth * sizeof(RGBA));
            }
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
sSpriteset::Import_CHR(const char* filename, const char* palette_file)
{
    // read the palette
    RGB palette[256];
    FILE* file = fopen(palette_file, "rb");
    if (file == NULL)
        return false;
    fread(palette, 256, sizeof(RGB), file);
    fclose(file);

    // determine which type of .chr it is
    file = fopen(filename, "rb");
    if (file == NULL)
        return false;

    // get the version of the .chr
    byte version;
    version = (byte)fgetc(file);
    fseek(file, 0, SEEK_SET);

    // import the .chr
    bool success;
    if (version == 0)
        success = Import_CHR1(file, palette);
    else if (version == 2)
        success = Import_CHR2V2(file, palette);
    else
        success = Import_CHR2V4(file, palette);

    fclose(file);
    return success;

}
////////////////////////////////////////////////////////////////////////////////
bool
sSpriteset::Export_PNG_Compact_Vertical(const char* filename)
{
    CImage32 image;
    image.SetBlendMode(CImage32::REPLACE);

    // calculate the number of rows
    int maxRowFrames = 0;
    int ImgWidth;
    int ImgHeight;

    maxRowFrames = m_Images.size();
    ImgWidth = m_FrameWidth + 2;
    ImgHeight = (m_FrameHeight * maxRowFrames) + (maxRowFrames + 1);

    // resize to every frame have a pixel padding between them
    image.Resize(ImgWidth, ImgHeight);

    image.Rectangle(0, 0, image.GetWidth(), image.GetHeight(),
                    CreateRGBA(255, 255, 255, 255));

    // drop all the frames into the image
    for (int row=0; row<maxRowFrames; row++)
        for (int line=0; line<m_FrameHeight; line++)
            // copy offset to:
            // x: 1
            // y: ((current row * frame height + 1) * image width) + (current row * image width) + (current line * image width)
            memcpy(image.GetPixels() +
                   ((row * m_FrameHeight + 1) * ImgWidth) + (row * ImgWidth) + (line * ImgWidth) +
                   1,
                   m_Images[ row ].GetPixels() + (line * m_FrameWidth),
                   m_FrameWidth * sizeof(RGBA));

    return image.Save(filename);
}

////////////////////////////////////////////////////////////////////////////////
bool
sSpriteset::Export_PNG_Compact_Horizontal(const char* filename)
{
    CImage32 image;
    image.SetBlendMode(CImage32::REPLACE);

    // calculate the number of rows
    int maxColFrames = 0;
    int ImgWidth;
    int ImgHeight;

    maxColFrames = m_Images.size();
    ImgWidth = (m_FrameWidth * maxColFrames) + (maxColFrames + 1);
    ImgHeight = m_FrameHeight + 2;

    // resize to every frame have a pixel padding between them
    image.Resize(ImgWidth, ImgHeight);

    image.Rectangle(0, 0, image.GetWidth(), image.GetHeight(),
                    CreateRGBA(255, 255, 255, 255));

    // drop all the frames into the image
    for (int col=0; col<maxColFrames; col++)
        for (int line=0; line<m_FrameHeight; line++)
            // copy offset to:
            // x: (current col * frame width) + (current col * padding) + 1
            // y: ((current line + 1) * image width)
            memcpy(image.GetPixels() +
                   ((line + 1) * ImgWidth) +
                   (col * m_FrameWidth) + (col * 1) + 1,
                   m_Images[ col ].GetPixels() + (line * m_FrameWidth),
                   m_FrameWidth * sizeof(RGBA));

    return image.Save(filename);
}

////////////////////////////////////////////////////////////////////////////////
bool
sSpriteset::Export_PNG(const char* filename)
{
    // find the largest number of frames in a direction and use that
    unsigned maxColFrames = 0;

    // calculate the maximum columns and rows
    unsigned maxRowFrames = m_Directions.size();
    for (unsigned i=0; i<m_Directions.size(); i++)
        maxColFrames = (m_Directions[i].frames.size() > maxColFrames ? m_Directions[i].frames.size() : maxColFrames);
    int ImgWidth  = m_FrameWidth * maxColFrames;
    int ImgHeight = m_FrameHeight * maxRowFrames;

    CImage32 image(ImgWidth, ImgHeight);
    image.SetBlendMode(CImage32::REPLACE);

    // drop all the frames into the image
    for (unsigned row = 0; row < maxRowFrames; ++row)
    {
        unsigned frameCount = m_Directions[row].frames.size();
        for (unsigned col = 0; col < std::min(maxColFrames, frameCount); ++col)
        {
            CImage32& frame = m_Images[m_Directions[row].frames[col].index];
            image.BlitImage(frame, col * m_FrameWidth, row * m_FrameHeight);
        }
    }

    return image.Save(filename);
}

////////////////////////////////////////////////////////////////////////////////
bool
sSpriteset::Export_Palette_Horizontal(const char* filename)
{
    int width  = m_FrameWidth * m_Images.size();
    int height = m_FrameHeight;

    CImage32 image(width, height);
    image.SetBlendMode(CImage32::REPLACE);

    for (unsigned int i = 0; i < m_Images.size(); i++)
    {
        CImage32& frame = m_Images[i];
        image.BlitImage(frame, i * m_FrameWidth, 0);
    }

    return image.Save(filename);
}

////////////////////////////////////////////////////////////////////////////////
bool
sSpriteset::Export_Palette_Vertical(const char* filename)
{
    int width  = m_FrameWidth;
    int height = m_FrameHeight * m_Images.size();

    CImage32 image(width, height);
    image.SetBlendMode(CImage32::REPLACE);

    for (unsigned int i = 0; i < m_Images.size(); i++)
    {
        CImage32& frame = m_Images[i];
        image.BlitImage(frame, 0, i * m_FrameHeight);
    }

    return image.Save(filename);
}

////////////////////////////////////////////////////////////////////////////////
bool
sSpriteset::Export_Palette_Fixed(int wide, const char* filename)
{
    int max    = m_Images.size();
    int rows   = (max - (max % wide)) / wide + (max % wide == 0 ? 0 : 1);

    int width  = m_FrameWidth  * wide;
    int height = m_FrameHeight * rows;

    CImage32 image(width, height);
    image.SetBlendMode(CImage32::REPLACE);

    int x = 0;
    int y = 0;

    for (unsigned int i = 0; i < m_Images.size(); i++)
    {
        CImage32& frame = m_Images[i];
        image.BlitImage(frame, x, y);

        x += m_FrameWidth;

        if (x >= wide * m_FrameWidth)
        {
            x = 0;
            y += m_FrameHeight;
        }
    }

    return image.Save(filename);
}

////////////////////////////////////////////////////////////////////////////////
int
sSpriteset::GetNumImages() const
{
    return m_Images.size();
}

////////////////////////////////////////////////////////////////////////////////
CImage32&
sSpriteset::GetImage(int i)
{
    return m_Images[i];
}

////////////////////////////////////////////////////////////////////////////////
const CImage32&
sSpriteset::GetImage(int i) const
{
    return m_Images[i];
}

////////////////////////////////////////////////////////////////////////////////
void
sSpriteset::InsertImage(int i)
{
    CImage32 image(m_FrameWidth, m_FrameHeight);
    m_Images.insert(m_Images.begin() + i, image);
}

////////////////////////////////////////////////////////////////////////////////
void
sSpriteset::DeleteImage(int i)
{
    m_Images.erase(m_Images.begin() + i);
}

////////////////////////////////////////////////////////////////////////////////
int
sSpriteset::GetFrameWidth() const
{
    return m_FrameWidth;
}

////////////////////////////////////////////////////////////////////////////////
int
sSpriteset::GetFrameHeight() const
{
    return m_FrameHeight;
}

////////////////////////////////////////////////////////////////////////////////
void
sSpriteset::ResizeFrames(int width, int height)
{
    for (unsigned int i = 0; i < m_Images.size(); i++)
        m_Images[i].Resize(width, height);

    m_FrameWidth  = width;
    m_FrameHeight = height;
}

////////////////////////////////////////////////////////////////////////////////
void
sSpriteset::RescaleFrames(int width, int height)
{
    for (unsigned int i = 0; i < m_Images.size(); i++)
        m_Images[i].Rescale(width, height);

    m_FrameWidth  = width;
    m_FrameHeight = height;
}

////////////////////////////////////////////////////////////////////////////////
void
sSpriteset::ResampleFrames(int width, int height)
{
    for (unsigned int i = 0; i < m_Images.size(); i++)
        m_Images[i].Resample(width, height);
    m_FrameWidth  = width;
    m_FrameHeight = height;
}

////////////////////////////////////////////////////////////////////////////////
void
sSpriteset::InsertDirection(int direction)
{
    Direction new_direction;

    new_direction.name = "unnamed";
    new_direction.frames.resize(1);
    new_direction.frames[0].index = 0;
    new_direction.frames[0].delay = DEFAULT_DELAY;

    m_Directions.insert(m_Directions.begin() + direction, new_direction);
}

////////////////////////////////////////////////////////////////////////////////
void
sSpriteset::DeleteDirection(int direction)
{
    m_Directions.erase(m_Directions.begin() + direction);
}

////////////////////////////////////////////////////////////////////////////////
void
sSpriteset::InsertFrame(int direction, int frame)
{
    std::vector<Direction>::iterator d = m_Directions.begin() + direction;

    // new frame
    Frame f;
    f.index = 0;
    f.delay = DEFAULT_DELAY;
    d->frames.insert(d->frames.begin() + frame, f);
}

////////////////////////////////////////////////////////////////////////////////
void
sSpriteset::DeleteFrame(int direction, int frame)
{
    std::vector<Direction>::iterator d = m_Directions.begin() + direction;

    // delete frame
    d->frames.erase(d->frames.begin() + frame);
}

////////////////////////////////////////////////////////////////////////////////
int
sSpriteset::GetNumDirections() const
{
    return m_Directions.size();
}

////////////////////////////////////////////////////////////////////////////////
const char*
sSpriteset::GetDirectionName(int direction) const
{
    return m_Directions[direction].name.c_str();
}

////////////////////////////////////////////////////////////////////////////////
void
sSpriteset::SetDirectionName(int direction, const char* name)
{
    m_Directions[direction].name = name;
}

////////////////////////////////////////////////////////////////////////////////
int
sSpriteset::GetNumFrames(int direction) const
{
    return m_Directions[direction].frames.size();
}

////////////////////////////////////////////////////////////////////////////////
int
sSpriteset::GetFrameIndex(int direction, int frame) const
{
    return m_Directions[direction].frames[frame].index;
}

////////////////////////////////////////////////////////////////////////////////
int
sSpriteset::GetFrameDelay(int direction, int frame) const
{
    return m_Directions[direction].frames[frame].delay;
}

////////////////////////////////////////////////////////////////////////////////
void
sSpriteset::SetFrameIndex(int direction, int frame, int index)
{
    m_Directions[direction].frames[frame].index = index;
}

////////////////////////////////////////////////////////////////////////////////
void
sSpriteset::SetFrameDelay(int direction, int frame, int delay)
{
    m_Directions[direction].frames[frame].delay = delay;
}

////////////////////////////////////////////////////////////////////////////////
int
sSpriteset::GetNumFrames(std::string direction) const
{
    // prevent the engine from going dinky if it don't find the name ;)
    int d = 0;

    for (unsigned int i = 0; i < m_Directions.size(); i++)
    {
        if (strcmp_ci(direction.c_str(), m_Directions[i].name.c_str()) == 0)
        {
            d = i;

            // stop immediately.
            i = m_Directions.size();
        }
    }

    return m_Directions[d].frames.size();
}

////////////////////////////////////////////////////////////////////////////////
int
sSpriteset::GetFrameIndex(std::string direction, int frame) const
{
    // prevent the engine from going dinky if it don't find the name ;)
    int d = 0;

    for (unsigned int i = 0; i < m_Directions.size(); i++)
        if (strcmp_ci(direction.c_str(), m_Directions[i].name.c_str()) == 0)
        {
            d = i;

            // stop immediately.
            i = m_Directions.size();
        }

    return m_Directions[d].frames[frame].index;
}

////////////////////////////////////////////////////////////////////////////////
int
sSpriteset::GetFrameDelay(std::string direction, int frame) const
{
    // prevent the engine from going dinky if it don't find the name ;)
    int d = 0;

    for (unsigned int i = 0; i < m_Directions.size(); i++)
        if (strcmp_ci(direction.c_str(), m_Directions[i].name.c_str()) == 0)
        {
            d = i;

            // stop immediately.
            i = m_Directions.size();
        }

    return m_Directions[d].frames[frame].delay;
}

////////////////////////////////////////////////////////////////////////////////
int
sSpriteset::GetDefaultDirection(std::string direction) const
{
    // prevent the engine from going dinky if it don't find the name ;)
    int d = OTHER;

    if (strcmp_ci(direction.c_str(), "north") == 0)
        d = NORTH;
    else if (strcmp_ci(direction.c_str(), "northeast") == 0)
        d = NORTHEAST;
    else if (strcmp_ci(direction.c_str(), "east") == 0)
        d = EAST;
    else if (strcmp_ci(direction.c_str(), "southeast") == 0)
        d = SOUTHEAST;
    else if (strcmp_ci(direction.c_str(), "south") == 0)
        d = SOUTH;
    else if (strcmp_ci(direction.c_str(), "southwest") == 0)
        d = SOUTHWEST;
    else if (strcmp_ci(direction.c_str(), "west") == 0)
        d = WEST;
    else if (strcmp_ci(direction.c_str(), "northwest") == 0)
        d = NORTHWEST;

    return d;
}

////////////////////////////////////////////////////////////////////////////////
int
sSpriteset::GetDirectionNum(std::string direction) const
{
    // tell engine to shove itself with a broomstick if it can't find it.
    int d = -1;

    for (unsigned int i = 0; i < m_Directions.size(); i++)
        if (strcmp_ci(direction.c_str(), m_Directions[i].name.c_str()) == 0)
        {
            d = i;

            // stop immediately.
            i = m_Directions.size();
        }

    return d;
}

////////////////////////////////////////////////////////////////////////////////
void
sSpriteset::GetBase(int& x1, int& y1, int& x2, int& y2) const
{
    x1 = m_BaseX1;
    y1 = m_BaseY1;
    x2 = m_BaseX2;
    y2 = m_BaseY2;
}

////////////////////////////////////////////////////////////////////////////////
void
sSpriteset::SetBase(int x1, int y1, int x2, int y2)
{
    m_BaseX1 = x1;
    m_BaseY1 = y1;
    m_BaseX2 = x2;
    m_BaseY2 = y2;
}

void
sSpriteset::GetRealBase(int& x1, int& y1, int& x2, int& y2) const
{
    x1 = org_BaseX1;
    y1 = org_BaseY1;
    x2 = org_BaseX2;
    y2 = org_BaseY2;
}

void
sSpriteset::Base2Real()
{
    org_BaseX1 = m_BaseX1;
    org_BaseY1 = m_BaseY1;
    org_BaseX2 = m_BaseX2;
    org_BaseY2 = m_BaseY2;
}

////////////////////////////////////////////////////////////////////////////////
bool
sSpriteset::Import_CHR1(FILE* file, RGB palette[256])
{
    // create the frames..
    m_FrameWidth = 16;
    m_FrameHeight = 32;

    // read all the frames and whack it in m_Images
    CImage32 chr_frame(16, 32);
    for (int i=0; i<30; i++)
    {
        // clear the image
        memset(chr_frame.GetPixels(), 0, 16 * 32 * 4);

        for (int y=0; y<32; y++)
        {
            for (int x=0; x<16; x++)
            {
                byte c = (byte)fgetc(file);

                RGBA* p = chr_frame.GetPixels() + (y * 16 + x);
                p->red   = palette[c].red   * 4;
                p->green = palette[c].green * 4;
                p->blue  = palette[c].blue  * 4;
                p->alpha = (byte)(c == 0 ? 0 : 255);
            }
        }

        // push it into the image list
        m_Images.push_back(chr_frame);
    }

#define AddMovement(dname, a, b, c, d, e) \
{                                             \
  Direction direction;                        \
  Frame frameA;                               \
  Frame frameB;                               \
  Frame frameC;                               \
  Frame frameD;                               \
  Frame frameE;                               \
                                              \
  frameA.index = a;                           \
  frameB.index = b;                           \
  frameC.index = c;                           \
  frameD.index = d;                           \
  frameE.index = e;                           \
  frameA.delay = frameB.delay = frameC.delay = frameD.delay = frameE.delay = 10; \
                                              \
  direction.name = dname;                     \
  direction.frames.push_back(frameA);         \
  direction.frames.push_back(frameB);         \
  direction.frames.push_back(frameC);         \
  direction.frames.push_back(frameB);         \
  direction.frames.push_back(frameA);         \
  direction.frames.push_back(frameD);         \
  direction.frames.push_back(frameE);         \
  direction.frames.push_back(frameD);         \
                                              \
  m_Directions.push_back(direction);          \
}
    // recreate the directions
    AddMovement("North",     5,   6,  7,  8,  9);
    AddMovement("NorthEast", 10, 11, 12, 13, 14);
    AddMovement("East",      10, 11, 12, 13, 14);
    AddMovement("SouthEast", 10, 11, 12, 13, 14);
    AddMovement("South",     0,   1,  2,  3,  4);
    AddMovement("SouthWest", 15, 16, 17, 18, 19);
    AddMovement("West",      15, 16, 17, 18, 19);
    AddMovement("NorthWest", 15, 16, 17, 18, 19);

#undef AddMovement
    return true;
    /*
      // create the frames...
      m_FrameWidth = 16;
      m_FrameHeight = 32;

      m_Directions.resize(9);
      for (int i = 0; i < 9; i++)
      {
        m_Directions[i].frames.resize(i == 8 ? 10 : 8);
        int k = (i == 8 ? 2 : 0);

        for (int j = 0; j < (8 + k); j++)
        {
          sSprite* sprite = new sSprite(m_FrameWidth, m_FrameHeight);
          sprite->SetDelay(8);

          m_Directions[i].frames[j] = sprite;
        }
      }

      // allocate .chr frames and read them
      RGBA chr_frames[30][16 * 32];

      for (int i = 0; i < 30; i++)
      {
        for (int y = 0; y < 32; y++)
          for (int x = 0; x < 16; x++)
          {
            byte c = (byte)fgetc(file);

            RGBA* p = chr_frames[i] + (y * 16 + x);
            p->red   = palette[c].red   * 4;
            p->green = palette[c].green * 4;
            p->blue  = palette[c].blue  * 4;
            p->alpha = (byte)(c == 0 ? 0 : 255);
          }
      }

    #define CopyFrame(direction, row, frame, column)             \
    {                                                            \
      memcpy(m_Directions[direction].frames[frame]->GetPixels(), \
             chr_frames[row * 5 + column],                       \
             16 * 32 * sizeof(RGBA));                            \
    }

    #define CopyRow(direction, row)    \
    {                                  \
      CopyFrame(direction, row, 0, 0); \
      CopyFrame(direction, row, 1, 1); \
      CopyFrame(direction, row, 2, 2); \
      CopyFrame(direction, row, 3, 1); \
      CopyFrame(direction, row, 4, 0); \
      CopyFrame(direction, row, 5, 3); \
      CopyFrame(direction, row, 6, 4); \
      CopyFrame(direction, row, 7, 3); \
    }

      // copy frames to spriteset
      CopyRow(0, 1);
      CopyRow(1, 2);
      CopyRow(2, 2);
      CopyRow(3, 2);
      CopyRow(4, 0);
      CopyRow(5, 3);
      CopyRow(6, 3);
      CopyRow(7, 3);
      for (int i=0; i<10; i++)
        CopyFrame(8, 4, i, i);

      return true;
    */
}

////////////////////////////////////////////////////////////////////////////////
#define STRUCT_NAME CHR2V2_HEADER
#define STRUCT_BODY                             \
  byte version;                                 \
  word width;                                   \
  word height;                                  \
  word hotspot_left;                            \
  word hotspot_top;                             \
  word hotspot_width;                           \
  word hotspot_height;                          \
  word num_frames;
#include "packed_struct.h"

////////////////////////////////////////////////////////////////////////////////
bool
sSpriteset::Import_CHR2V2(FILE* file, RGB palette[256])
{
    CHR2V2_HEADER header;
    fread(&header, sizeof(header), 1, file);

    m_FrameWidth = header.width;
    m_FrameHeight = header.height;

    // unlike the last version of the spriteset, it's now *very* similar to how
    // it works. so conversion is gonna be fairly simple... not quite :)
    CImage32 frame(m_FrameWidth, m_FrameHeight);
    RGBA* frames;
    frames = new RGBA[m_FrameWidth * m_FrameHeight * header.num_frames];
    dword i=0;
    dword count=0;
    dword file_size;

    // grab all the image data that's in one big lump :<...
    fread(&file_size, 1, 4, file);
    while (count<file_size)
    {
        byte value = getc(file);
        count++;

        if (value <255)
        {
            frames[i].red   = palette[value].red   * 4;
            frames[i].green = palette[value].green * 4;
            frames[i].blue  = palette[value].blue  * 4;
            frames[i].alpha = (byte)(value == 0 ? 0 : 255);
            i++;
        }
        else
        {
            byte run = getc(file);
            value = getc(file);
            count+=2;

            for (int j=0; j<run; j++)
            {
                frames[i].red   = palette[value].red   * 4;
                frames[i].green = palette[value].green * 4;
                frames[i].blue  = palette[value].blue  * 4;
                frames[i].alpha = (byte)(value == 0 ? 0 : 255);
                i++;
            }
        }
    }

    // whack all of it into actual "frames" now
    for (int framecount=0; framecount<header.num_frames; framecount++)
    {
        memset(frame.GetPixels(), 0, m_FrameWidth * m_FrameHeight * 4);
        memcpy(frame.GetPixels(),
               frames + (m_FrameWidth * m_FrameHeight * framecount),
               m_FrameWidth * m_FrameHeight * 4);

        m_Images.push_back(frame);
    }

    // direction code begins here, left right up down
    std::vector<int> walk_type[4];
    std::vector<int> walk_index[4];

    // maybe discard the idle frames?
    for (count=0; count<4; count++)
    {
        fread(&i, 1, sizeof(dword), file);
        walk_type[count].push_back(1);
        walk_index[count].push_back(i);

        // wait for 10
        walk_type[count].push_back(0);
        walk_index[count].push_back(10);
    }

    // translate the movement script
    for (count=0; count<4; count++)
    {
        dword codelength;
        char* walkcode;

        fread(&codelength, 1, sizeof(dword), file);
        walkcode = new char[codelength];
        fread(walkcode, 1, codelength, file);

        for (i=0; i<codelength;)
            if (walkcode[i] == 'F' || walkcode[i] == 'W')
            {
                char command;
                char token[10];
                command = walkcode[i++];
                int token_i=0;

                while (i < codelength && walkcode[i] == ' ') i++;
                while (i < codelength && walkcode[i] >= 48 && walkcode[i] <= 57)
                    token[token_i++] = walkcode[i++];
                token[token_i] = 0;

                walk_type[count].push_back((command == 'F' ? 1 : 0));
                walk_index[count].push_back(atoi(token));
            }
            else
                i++;

        delete[] walkcode;
    }

    // push the frames in
    for (count=0; count<8; count++)
    {
        int di = 0;
        Direction direction;

        switch (count)
        {
        case 1:
        case 2:
        case 3:
            di = 1;
            break;

        case 5:
        case 6:
        case 7:
            di = 0;
            break;

        case 0:
            di = 2;
            break;
        case 4:
            di = 3;
            break;
        }

        for (i=0; i<walk_type[di].size(); i++)
        {
            Frame frame_index;
            frame_index.index = -1;
            frame_index.delay = 0;

            if (walk_type[di][i] == 1)
                frame_index.index = walk_index[di][i];

            if (walk_type[di][i+1] == 0)
                frame_index.delay = walk_index[di][++i];

            if (frame_index.index != -1)
                direction.frames.push_back(frame_index);
        }

        m_Directions.push_back(direction);
    }

    // free everything used
    delete[] frames;

    return true;
    /*
      CHR2_HEADER header;
      fread(&header, sizeof(header), 1, file);

       // because we want to get the extra frames in the set, so I'm building
      // a table, and then parsing it. This is so that I'm marking down the
      // frames that's used, and the unused shall be in a seperate direction.
      // Finally we then do something about it.
      bool* frameUsed;
      bool  extraFramesUsed;
      RGBA* frames;
      long  dataLength;
      dword walkcodeLength[4];
      char* walkcode[4];
      int*  walkcodeValue[4];
      int* walkcodeType[4];
      int   walkcodeNumArg[4];
      int frameCount[5];
      int i,j,k,l;

      if (header.version != 2)
      return false;

      Destroy();
      //grab all the data then expand it.
      frameCount[0] = frameCount[1] = frameCount[2] = frameCount[3] = 1;
      frameCount[4] = 0;

      frameUsed = new bool[header.num_frames];
      memset(frameUsed, 0, header.num_frames);
      frames = new RGBA[header.num_frames * header.width * header.height];
      j=0;

      fread(&dataLength, 1, 4, file);
      for (i=0; i<dataLength; i++)
      {
        byte value = getc(file);

        if (value<255)
        {
          frames[j].red = palette[value].red * 4;
          frames[j].blue = palette[value].blue * 4;
          frames[j].green = palette[value].green * 4;
          frames[j].alpha = (byte)(value == 0 ? 0 : 255);
          j++;
        }
        else if (value == 255)
        {
          byte runlength = getc(file);
          value = getc(file);
          for(k=0; k<runlength; k++)
          {
            frames[j].red = palette[value].red * 4;
            frames[j].blue = palette[value].blue * 4;
            frames[j].green = palette[value].green * 4;
            frames[j].alpha = (byte)(value == 0 ? 0 : 255);
            j++;
          }
          i+=2;
        }
      }

      for (i=0; i<4; i++)
      {
        walkcodeType[i] = new int[1];
        walkcodeValue[i] = new int[1];
        walkcodeType[i][0] = true;
    	  fread(&walkcodeValue[i][0], 1, sizeof(long), file);
      }

      // read the movement script
      for (i=0; i<4; i++)
      {
        fread(&walkcodeLength[i], 1, sizeof(dword), file);
        walkcode[i] = new char[walkcodeLength[i]];
        fread(walkcode[i], 1, walkcodeLength[i], file);
      }

      // translate the movement script
      for (i=0; i<4; i++)
      {
        k=1;
        for (j=0; j<(int)walkcodeLength[i];)
          if (walkcode[i][j] == 'F' || walkcode[i][j] == 'W')
          {
            char command;
            char token[10];
            int* tempwalkcodeType;
            int* tempwalkcodeValue;
            command = walkcode[i][j];
            l=0;
            j++;

            while (j < (int)walkcodeLength[i] && walkcode[i][j] == ' ') j++;
            while (j < (int)walkcodeLength[i] && walkcode[i][j] >= 48 &&
                   walkcode[i][j] <= 57)
            {
              tokesn[l] = walkcode[i][j];
              l++;
              j++;
            }

            token[l] = 0;
            tempwalkcodeType = new int[k + 1];
            tempwalkcodeValue = new int[k + 1];
            memcpy(tempwalkcodeType, walkcodeType[i], k*sizeof(int));
            memcpy(tempwalkcodeValue, walkcodeValue[i], k*sizeof(int));
            tempwalkcodeType[k] = (command == 'F' ? 1 : 0);
            tempwalkcodeValue[k] = atoi(token);
            delete[] walkcodeType[i]; delete[] walkcodeValue[i];
            walkcodeType[i] = tempwalkcodeType;
            walkcodeValue[i] = tempwalkcodeValue;

            if (command == 'F')
            {
            frameUsed[atoi(token)] = true;
            frameCount[i]++;
            }
            k++;
          }
          else
            j++;

        walkcodeNumArg[i] = k;
      }

      // check for extra frames
      for (i=0; i<header.num_frames; i++)
        if (!frameUsed[i])
          frameCount[4]++;
      if (frameCount[4] > 0)
        extraFramesUsed = true;
      else
        extraFramesUsed = false;

      // okay, now build the data the painful way (Create() can't handle this)
      // and transfer ALL the data.
      m_FrameWidth = header.width;
      m_FrameHeight = header.height;

      m_Directions.resize(8 + extraFramesUsed);
      for (int i = 0; i < 8 + extraFramesUsed; i++)
      {
        switch(i)
        {
          case EAST:
          case SOUTHEAST:
          case NORTHEAST:
            l = 0;
            break;

          case WEST:
          case NORTHWEST:
          case SOUTHWEST:
            l = 1;
            break;

          case NORTH: l = 2; break;
          case SOUTH: l = 3; break;
          case 8: l = 4; break;
        }

        m_Directions[i].frames.resize(frameCount[l]);
        for (int j = 0; j < frameCount[l]; j++)
        {
          sSprite* sprite = new sSprite(m_FrameWidth, m_FrameHeight);
          sprite->SetDelay(0);

          m_Directions[i].frames[j] = sprite;
        }

      }
      // left right up down special
    #define CopySide(direction, theside)                                    \
    {                                                                       \
      k=-1;                                                                 \
      for (i=0; i<walkcodeNumArg[theside]; i++)                             \
        if (walkcodeType[theside][i] == 1)                                  \
        {                                                                   \
          k++;                                                              \
          if (k < frameCount[theside])                                      \
          memcpy(m_Directions[direction].frames[k]->GetPixels(),             \
                 frames + (walkcodeValue[theside][i]*header.width*header.height),\
                 header.width * header.height * 4);                         \
        }                                                                   \
        else                                                                \
        {                                                                   \
          m_Directions[direction].frames[k]->SetDelay(m_Directions[direction].frames[k]->GetDelay() + walkcodeValue[theside][i]);\
        }                                                                   \
    }

      CopySide(EAST, 1);
      CopySide(NORTHEAST, 1);
      CopySide(SOUTHEAST, 1);
      CopySide(WEST, 0);
      CopySide(NORTHWEST, 0);
      CopySide(SOUTHWEST, 0);
      CopySide(NORTH, 2);
      CopySide(SOUTH, 3);

      j = 0;
      if (extraFramesUsed)
        {
        for (i=0; i<header.num_frames; i++)
          if (!frameUsed[i])
          {
            memcpy(m_Directions[8].frames[j]->GetPixels(),
                   frames + (i * header.width * header.height),
                   header.width * header.height * 4);
            j++;
          }
        }

      delete[] frameUsed;
      delete[] frames;
      for (i=0; i<4; i++)
        {
        delete[] walkcode[i];
        delete[] walkcodeValue[i];
        delete[] walkcodeType[i];
        }

      return true;
    */
}

////////////////////////////////////////////////////////////////////////////////
#define STRUCT_NAME CHR2V4_HEADER
#define STRUCT_BODY                             \
  byte version;                                 \
  word width;                                   \
  word height;                                  \
  word hotspot_left;                            \
  word hotspot_top;                             \
  word hotspot_width;                           \
  word hotspot_height;                          \
  word idle_frame_left;                         \
  word idle_frame_right;                        \
  word idle_frame_up;                           \
  word idle_frame_down;                         \
  word num_frames;
#include "packed_struct.h"

////////////////////////////////////////////////////////////////////////////////
bool
sSpriteset::Import_CHR2V4(FILE* file, RGB palette[256])
{
    CHR2V4_HEADER header;
    fread(&header, 1, sizeof(header), file);

    m_FrameWidth = header.width;
    m_FrameHeight = header.height;

    // stores our translated commands and values, left right up down
    std::vector<int> walk_type[4];
    std::vector<int> walk_index[4];
    dword i = 0;
    dword count;

    // whack in the idle frames as the first image
    walk_index[0].push_back(header.idle_frame_left);
    walk_index[1].push_back(header.idle_frame_right);
    walk_index[2].push_back(header.idle_frame_up);
    walk_index[3].push_back(header.idle_frame_down);
    for (i=0; i<4; i++)
    {
        walk_type[i].push_back(1);

        // wait for 10
        walk_type[i].push_back(0);
        walk_index[i].push_back(10);
    }

    // translate the movement script
    for (count=0; count<4; count++)
    {
        dword codelength;
        char* walkcode;

        fread(&codelength, 1, sizeof(dword), file);
        walkcode = new char[codelength];
        fread(walkcode, 1, codelength, file);

        for (i=0; i<codelength;)
            if (walkcode[i] == 'F' || walkcode[i] == 'W')
            {
                char command;
                char token[10];
                command = walkcode[i++];
                int token_i=0;

                while (i < codelength && walkcode[i] == ' ') i++;
                while (i < codelength && walkcode[i] >= 48 && walkcode[i] <= 57)
                    token[token_i++] = walkcode[i++];
                token[token_i] = 0;

                walk_type[count].push_back((command == 'F' ? 1 : 0));
                walk_index[count].push_back(atoi(token));
            }
            else
                i++;

        delete[] walkcode;
    }

    // push the frames in
    for (count=0; count<8; count++)
    {
        int di = 0;
        Direction direction;

        switch (count)
        {
        case 1:
        case 2:
        case 3:
            di = 1;
            break;

        case 5:
        case 6:
        case 7:
            di = 0;
            break;

        case 0:
            di = 2;
            break;
        case 4:
            di = 3;
            break;
        }

        for (i=0; i<walk_type[di].size(); i++)
        {
            Frame frame_index;
            frame_index.index = -1;
            frame_index.delay = 0;

            if (walk_type[di][i] == 1)
                frame_index.index = walk_index[di][i];

            if (walk_type[di][i+1] == 0)
                frame_index.delay = walk_index[di][++i];

            if (frame_index.index != -1)
                direction.frames.push_back(frame_index);
        }

        m_Directions.push_back(direction);
    }

    // process the image
    RGBA* image;
    dword file_size;
    int run_mask = 0xFF00;
    int get_run  = 0x00FF;

    image = new RGBA[m_FrameWidth * m_FrameHeight * header.num_frames];
    fread(&file_size, 1, sizeof(dword), file);
    i = 0;
    count = 0;
    while (count<file_size)
    {
        word value;
        word run;

        fread(&value, 1, sizeof(word), file);
        run = 1;
        count += 2;

        if ((value & run_mask) == run_mask)
        {
            run = (word)(value & get_run);
            fread(&value, 1, sizeof(word), file);
            count += 2;
        }

        for (int x=0; x<run; x++)
        {
            // convert 565 to 8888 data
            // thanks AK for the unpacking algorithm!
            image[i].red = (value & 0xF800) >> 8;
            image[i].green = (value & 0x07E0) >> 3;
            image[i].blue = (value & 0x001F) << 3;
            image[i].alpha = (value?255: 0);
            i++;
        }
    }

    // whack all of it into actual "frames" now
    CImage32 frame(m_FrameWidth, m_FrameHeight);
    for (int framecount=0; framecount<header.num_frames; framecount++)
    {
        memset(frame.GetPixels(), 0, m_FrameWidth * m_FrameHeight * 4);
        memcpy(frame.GetPixels(),
               image + (m_FrameWidth * m_FrameHeight * framecount),
               m_FrameWidth * m_FrameHeight * 4);

        m_Images.push_back(frame);
    }

    delete[] image;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
