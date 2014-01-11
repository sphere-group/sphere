#ifndef SPRITESET_HPP
#define SPRITESET_HPP

// identifier too long
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include <vector>
#include <map>
#include <memory>
#include <stdio.h>
#include <string>
#include "Image32.hpp"
#include "DefaultFileSystem.hpp"

class sSpriteset
{
public:
    sSpriteset();
    ~sSpriteset();

    bool Create(int frame_width, int frame_height, int num_images, int num_directions, int num_frames);
    bool Load(const char* filename, IFileSystem& fs = g_DefaultFileSystem);
    bool Save(const char* filename, IFileSystem& fs = g_DefaultFileSystem) const;

    bool Import_BMP(const char* filename, int frame_width, int frame_height, RGBA old_color, RGBA new_color);
    bool Import_CHR(const char* filename, const char* palette_file);

    bool Export_PNG_Compact_Vertical(const char* filename);
    bool Export_PNG_Compact_Horizontal(const char* filename);
    bool Export_PNG(const char* filename);
    bool Export_Palette_Horizontal(const char* filename);
    bool Export_Palette_Vertical(const char* filename);
    bool Export_Palette_Fixed(int wide, const char* filename);

    int             GetNumImages() const;
    CImage32&       GetImage(int i);
    const CImage32& GetImage(int i) const;

    void InsertImage(int i);
    void DeleteImage(int i);

    int  GetFrameWidth() const;
    int  GetFrameHeight() const;
    void ResizeFrames(int width, int height);
    void RescaleFrames(int width, int height);
    void ResampleFrames(int width, int height);

    void InsertDirection(int direction);
    void DeleteDirection(int direction);

    void InsertFrame(int direction, int frame);
    void DeleteFrame(int direction, int frame);

    int         GetNumDirections() const;
    const char* GetDirectionName(int direction) const;
    void        SetDirectionName(int direction, const char* name);

    int  GetNumFrames(int direction) const;
    int  GetFrameIndex(int direction, int frame) const;
    int  GetFrameDelay(int direction, int frame) const;
    void SetFrameIndex(int direction, int frame, int index);
    void SetFrameDelay(int direction, int frame, int delay);
    int  GetNumFrames(std::string direction) const;
    int  GetFrameIndex(std::string direction, int frame) const;
    int  GetFrameDelay(std::string direction, int frame) const;

    int  GetDefaultDirection(std::string direction) const;
    int  GetDirectionNum(std::string direction) const;

    void GetBase(int& x1, int& y1, int& x2, int& y2) const;
	void GetRealBase(int& x1, int& y1, int& x2, int& y2) const;
    void SetBase(int x1, int y1, int x2, int y2);
	void Base2Real();
	    

public:
    enum
    {
        NORTH, NORTHEAST, EAST, SOUTHEAST, SOUTH, SOUTHWEST, WEST, NORTHWEST, OTHER
    };

private:
    struct Frame
    {
        int index;
        int delay;
    };

    struct Direction
    {
        std::string        name;
        std::vector<Frame> frames;
    };

private:
    bool Import_CHR1(FILE* file, RGB palette[256]);
    bool Import_CHR2V2(FILE* file, RGB palette[256]);
    bool Import_CHR2V4(FILE* file, RGB palette[256]);

private:
    int m_FrameWidth;
    int m_FrameHeight;

    std::vector<CImage32>  m_Images;
    std::vector<Direction> m_Directions;

    int m_BaseX1;
    int m_BaseY1;
    int m_BaseX2;
    int m_BaseY2;

    int org_BaseX1;
    int org_BaseY1;
    int org_BaseX2;
    int org_BaseY2;

};

#endif
