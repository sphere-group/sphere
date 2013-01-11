#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "FLICAnimation.hpp"

#define STRUCT_NAME SFlicHeader
#define STRUCT_BODY                             \
  dword size;                                   \
  word  magic;                                  \
  word  frames;                                 \
  word  width;                                  \
  word  height;                                 \
  word  depth;                                  \
  word  flags;                                  \
  dword speed;                                  \
  word  reserved0;                              \
  dword created;                                \
  dword creator;                                \
  dword updated;                                \
  dword updater;                                \
  word  aspectx;                                \
  word  aspecty;                                \
  byte  reserved1[38];                          \
  dword oframe1;                                \
  dword oframe2;                                \
  byte  reserved2[40];
#include "packed_struct.h"

#define STRUCT_NAME SFrameHeader
#define STRUCT_BODY                             \
  dword size;                                   \
  word  type;                                   \
  word  chunks;                                 \
  byte  reserved[8];
#include "packed_struct.h"

////////////////////////////////////////////////////////////////////////////////
CFLICAnimation::CFLICAnimation()
        : Width(0)
        , Height(0)
        , Delay(0)
        , CurrentFrame(0)
        , NumFrames(0)
        , Frame(NULL)
        , File(NULL)
        , NextFrame(0)
        , SecondFrame(0)
        , FrameDataSize(0)
        , FrameData(NULL)
        , FrameDataOffset(0)
{}

////////////////////////////////////////////////////////////////////////////////
CFLICAnimation::~CFLICAnimation()
{
    delete[] Frame;
    delete[] FrameData;
    delete File;
}

////////////////////////////////////////////////////////////////////////////////
bool
CFLICAnimation::Load(const char* filename, IFileSystem& fs)
{
    File = fs.Open(filename, IFileSystem::read);
    if (!File)
    {
        printf("Could not open animation file: %s\n", filename);

        return false;
    }

    // read the header
    SFlicHeader FlicHeader;
    if (File->Read(&FlicHeader, sizeof(FlicHeader)) != sizeof(FlicHeader))
        return false;

    // test the FLIC for validity
    if (FlicHeader.depth != 8)
    {
        delete File;
        File = 0;
        return false;
    }

    // fill animation structure
    Width  = FlicHeader.width;
    Height = FlicHeader.height;
    Delay  = FlicHeader.speed;

    NextFrame   = FlicHeader.oframe1;
    SecondFrame = FlicHeader.oframe2;

    if (NextFrame == 0)
    {
        NextFrame = sizeof(FlicHeader);
        SecondFrame = NextFrame;
    }

    Frame = new byte[Width * Height];
    if (!Frame)
        return false;

    CurrentFrame = 0;
    NumFrames    = FlicHeader.frames;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
CFLICAnimation::Save(const char* filename, IFileSystem& fs)
{
    return false;
}

////////////////////////////////////////////////////////////////////////////////
int
CFLICAnimation::GetWidth()
{
    return Width;
}

////////////////////////////////////////////////////////////////////////////////
int
CFLICAnimation::GetHeight()
{
    return Height;
}

////////////////////////////////////////////////////////////////////////////////
int
CFLICAnimation::GetNumFrames()
{
    return NumFrames;
}

////////////////////////////////////////////////////////////////////////////////
int
CFLICAnimation::GetTicks()
{
    return 0; // MNG's have this, but not FLIC's?
}

////////////////////////////////////////////////////////////////////////////////
int
CFLICAnimation::GetPlaytime()
{
    return 0; // MNG's have this, but not FLIC's?
}

////////////////////////////////////////////////////////////////////////////////
int
CFLICAnimation::GetDelay()
{
    return Delay;
}

////////////////////////////////////////////////////////////////////////////////
bool
CFLICAnimation::IsEndOfAnimation()
{
    return !(CurrentFrame < NumFrames);
}

////////////////////////////////////////////////////////////////////////////////
bool
CFLICAnimation::ReadNextFrame(RGBA* FrameBuffer)
{
    ReadFrame();

    // convert FLIC frame into RGBA buffer
    for (int i = 0; i < Width * Height; i++)
    {
        FrameBuffer[i].red   = Palette[Frame[i]].red;
        FrameBuffer[i].green = Palette[Frame[i]].green;
        FrameBuffer[i].blue  = Palette[Frame[i]].blue;
        FrameBuffer[i].alpha = 255;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
CFLICAnimation::ReadNextFrame(BGRA* FrameBuffer)
{
    ReadFrame();

    // convert FLIC frame into BGRA buffer
    for (int i = 0; i < Width * Height; i++)
    {
        FrameBuffer[i].red   = Palette[Frame[i]].red;
        FrameBuffer[i].green = Palette[Frame[i]].green;
        FrameBuffer[i].blue  = Palette[Frame[i]].blue;
        FrameBuffer[i].alpha = 255;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
CFLICAnimation::ReadFrame()
{
    // seek to the next frame
    File->Seek(NextFrame);

    // read the frame header
    SFrameHeader FrameHeader;
    if (File->Read(&FrameHeader, sizeof(FrameHeader)) != sizeof(FrameHeader))
        return false;

    // increment the current frame index
    CurrentFrame++;
    if (CurrentFrame > NumFrames)  // if we're off the edge, jump back to the beginning
    {
        NextFrame = SecondFrame;
        CurrentFrame = 1;
    }
    else
    {
        // the next frame is the current one plus the frame size
        NextFrame += FrameHeader.size;
    }

    // read the frame out of the file
    delete[] FrameData;
    FrameDataSize = FrameHeader.size;
    FrameData = new byte[FrameDataSize];
    if (!FrameData)
        return false;

    if (File->Read(FrameData, FrameDataSize) != FrameDataSize)
        return false;
    FrameDataOffset = 0;

    // operate on all the chunks
    for (int i = 0; i < FrameHeader.chunks; i++)
    {
        // read chunk header
        /*dword size =*/ next_dword();
        word type  = next_word();

        switch (type)
        {
        case 4:
            DecodeChunk_COLOR256();
            break;
        case 7:
            DecodeChunk_SS2();
            break;
        case 11:
            DecodeChunk_COLOR();
            break;
        case 12:
            DecodeChunk_LC();
            break;
        case 13:
            DecodeChunk_BLACK();
            break;
        case 15:
            DecodeChunk_BRUN();
            break;
        case 16:
            DecodeChunk_COPY();
            break;
        default:
            break; // unknown, ignore
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
CFLICAnimation::DecodeChunk_COLOR256()
{
    int currententry = 0;
    int i;

    // read number of packets
    word packets = next_word();

    for (i = 0; i < packets; i++)
    {
        // read the number of entries to skip
        byte skipcount = next_byte();
        currententry += skipcount;

        // read and define palette entries
        byte definecount = next_byte();
        for (int j = 0; j < (definecount == 0 ? 256 : definecount); j++)
        {
            RGB rgb = next_RGB();

            Palette[currententry].red   = rgb.red;
            Palette[currententry].green = rgb.green;
            Palette[currententry].blue  = rgb.blue;

            currententry++;
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
CFLICAnimation::DecodeChunk_SS2()
{
    // read the number of lines to decode
    word num_lines = next_word();

    //  int current_line = 0;
    //for (int y = 0; y < num_lines; y++)
    for (int current_line = 0; current_line < num_lines; current_line++)
    {
        word option_word = next_word();

        // check for 'skip lines' word
        if (option_word & 0xC000)  // upper two bits are 1 1
        {
            // this current_line += option_word can make SwordSlashagain.flc crash
            // because num_lines = 32 and option_word = 32774, so:
            // current_line += option_word > num_lines!
            current_line += abs((sword)option_word);     // absolute value is number of lines to skip
            if (current_line >= num_lines)
                return false;
            option_word = next_word();  // read next word
        }

        // check for 'set last byte' word
        if (option_word & 0x8000)  // upper two bits are 1 0
        {
            byte eol_color = (byte)(option_word & 0xFF);
            int eol = current_line * Width + Width - 1;

            Frame[eol] = eol_color;               // set last byte in line
            option_word = next_word();
        }

        // this next one has to be the packet count
        word num_packets = option_word;

        int current_x = 0;
        for (int p = 0; p < num_packets; p++)
        {
            // read the skip count and skip some columns
            byte column_skip_count = next_byte();
            current_x += column_skip_count;

            // read the packet type
            sbyte packet_type = next_sbyte();
            if (packet_type < 0)  // if it's negative, replicate next word
            {
                int q;

                // read word to duplicate
                word duplicate_word = next_word();

                for (q = 0; q < (-packet_type); q++)
                {
                    int offset = current_line * Width + current_x + q * 2;
                    Frame[offset + 0] = (byte)((duplicate_word & 0xFF00) >> 8);
                    Frame[offset + 1] = (byte)((duplicate_word & 0x00FF) >> 0);
                }

                current_x -= (packet_type * 2);
            }
            else if (packet_type > 0)  // copy packet_type words to picture
            {
                read_next(Frame + current_line * Width + current_x, 2 * packet_type);
                current_x += (packet_type * 2);
            }
        }

        // current_line++;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
CFLICAnimation::DecodeChunk_COLOR()
{
    // read number of packets
    word packets = next_word();

    int currententry = 0;
    for (int i = 0; i < packets; i++)
    {
        // read the number of entries to skip
        byte skipcount = next_byte();
        currententry += skipcount;

        // read and define palette entries
        byte definecount = next_byte();
        for (int j = 0; j < (definecount == 0 ? 256 : definecount); j++)
        {
            RGB rgb = next_RGB();

            Palette[currententry].red   = (byte)(rgb.red   * 4);
            Palette[currententry].green = (byte)(rgb.green * 4);
            Palette[currententry].blue  = (byte)(rgb.blue  * 4);

            currententry++;
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
CFLICAnimation::DecodeChunk_LC()
{
    // start on this line
    word first_line = next_word();

    // process this many lines
    word num_lines = next_word();

    for (int y = first_line; y < first_line + num_lines; y++)
    {
        // start on this column
        byte start_x = next_byte();
        int current_x = start_x;

        // read this many packets
        byte num_packets = next_byte();

        // in each packet...
        for (int p = 0; p < num_packets; p++)
        {
            // skip this many columns
            byte skip_count = next_byte();
            current_x += skip_count;

            sbyte size_count = next_sbyte();
            if (size_count < 0)         // if it's negative, duplicate the next byte
            {
                byte duplicate_byte = next_byte();

                memset(Frame + y * Width + current_x, duplicate_byte, -size_count);
                current_x += (-size_count);
            }
            else if (size_count > 0)       // if it's positive, just read data from the file
            {
                read_next(Frame + y * Width + current_x, size_count);
                current_x += size_count;
            }
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
CFLICAnimation::DecodeChunk_BLACK()
{
    // clear out the current frame
    memset(Frame, 0, Width * Height * sizeof(*Frame));
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
CFLICAnimation::DecodeChunk_BRUN()
{
    // read each line
    for (int y = 0; y < Height; y++)
    {
        // skip first byte (number of packets is obsolete)
        next_byte();

        // begin decoding
        int PixelsRead = 0;
        while (PixelsRead < Width)
        {
            // size/type byte is signed
            sbyte size_type = next_sbyte();

            if (size_type < 0)    // if negative, copy (-size_type) pixels from file to screen
            {
                read_next(Frame + y * Width + PixelsRead, -size_type);
                PixelsRead += (-size_type);
            }
            else if (size_type > 0)  // if positive, duplicate next byte (size_type) times
            {
                byte duplication_pixel = next_byte();

                memset(Frame + y * Width + PixelsRead, duplication_pixel, size_type);
                PixelsRead += size_type;
            }
        }

    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
CFLICAnimation::DecodeChunk_COPY()
{
    // direct copy from file into frame
    read_next(Frame, Width * Height * sizeof(byte));
    return true;
}

////////////////////////////////////////////////////////////////////////////////
byte
CFLICAnimation::next_byte()
{
    if (FrameDataOffset < FrameDataSize)
    {
        return FrameData[FrameDataOffset++];
    }
    else
    {
        return 0;
    }
}

////////////////////////////////////////////////////////////////////////////////
sbyte
CFLICAnimation::next_sbyte()
{
    if (FrameDataOffset < FrameDataSize)
    {
        return ((sbyte*)FrameData)[FrameDataOffset++];
    }
    else
    {
        return 0;
    }
}

////////////////////////////////////////////////////////////////////////////////
word
CFLICAnimation::next_word()
{
    byte a = next_byte();
    byte b = next_byte();
    return (b << 8) + a;
}

////////////////////////////////////////////////////////////////////////////////
dword
CFLICAnimation::next_dword()
{
    word a = next_word();
    word b = next_word();
    return (b << 16) + a;
}

////////////////////////////////////////////////////////////////////////////////
RGB
CFLICAnimation::next_RGB()
{
    RGB rgb;
    rgb.red   = next_byte();
    rgb.green = next_byte();
    rgb.blue  = next_byte();
    return rgb;
}

////////////////////////////////////////////////////////////////////////////////
void
CFLICAnimation::read_next(byte* buffer, int numbytes)
{
    for (int i = 0; i < numbytes; i++)
        buffer[i] = next_byte();
}

////////////////////////////////////////////////////////////////////////////////
