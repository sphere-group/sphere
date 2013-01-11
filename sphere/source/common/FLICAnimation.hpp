#ifndef FLIC_HPP
#define FLIC_HPP
#include "DefaultFileSystem.hpp"
#include "IAnimation.hpp"
class CFLICAnimation : public IAnimation
{
public:
    CFLICAnimation();
    ~CFLICAnimation();
    bool Load(const char* filename, IFileSystem& fs = g_DefaultFileSystem);
    bool Save(const char* filename, IFileSystem& fs = g_DefaultFileSystem);
    int GetWidth();
    int GetHeight();
    int GetNumFrames();
	int GetTicks();
	int GetPlaytime();
    int GetDelay();
    bool IsEndOfAnimation();
    bool ReadNextFrame(RGBA* FrameBuffer);
    bool ReadNextFrame(BGRA* FrameBuffer);
private:
    bool ReadFrame();
    bool DecodeChunk_COLOR256();
    bool DecodeChunk_SS2();
    bool DecodeChunk_COLOR();
    bool DecodeChunk_LC();
    bool DecodeChunk_BLACK();
    bool DecodeChunk_BRUN();
    bool DecodeChunk_COPY();
    byte  next_byte();
    sbyte next_sbyte();
    word  next_word();
    dword next_dword();
    RGB   next_RGB();
    void  read_next(byte* buffer, int numbytes);
private:
    int    Width;
    int    Height;
    int    Delay;
    int    CurrentFrame;
    int    NumFrames;
    byte*  Frame;
    RGBA   Palette[256]; // alpha is unused
    IFile* File;
    int    NextFrame;    // position in file
    int    SecondFrame;  // position in file, used for looping
    int    FrameDataSize;
    byte*  FrameData;
    int    FrameDataOffset;  // current location
};
#endif
