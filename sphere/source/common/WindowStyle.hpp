#ifndef WINDOW_STYLE_HPP
#define WINDOW_STYLE_HPP

#include "Image32.hpp"
class sWindowStyle
{
public:
    enum {
        UPPER_LEFT  = 0,
        TOP         = 1,
        UPPER_RIGHT = 2,
        RIGHT       = 3,
        LOWER_RIGHT = 4,
        BOTTOM      = 5,
        LOWER_LEFT  = 6,
        LEFT        = 7,
        BACKGROUND  = 8,
    };

    enum {
        TILED,
        STRETCHED,
        GRADIENT,
        TILED_GRADIENT,
        STRETCHED_GRADIENT
    };

    enum {
        BACKGROUND_UPPER_LEFT  = 0,
        BACKGROUND_UPPER_RIGHT = 1,
        BACKGROUND_LOWER_LEFT  = 2,
        BACKGROUND_LOWER_RIGHT = 3,
    };

    enum {
        EDGE_LEFT   = 0,
        EDGE_TOP    = 1,
        EDGE_RIGHT  = 2,
        EDGE_BOTTOM = 3
    };

public:
    sWindowStyle();

    void Create(int width, int height);
    bool Load(const char* filename, IFileSystem& fs = g_DefaultFileSystem);
    bool Save(const char* filename, IFileSystem& fs = g_DefaultFileSystem) const;
    bool Import(const char* filename, RGBA transColor, IFileSystem& fs = g_DefaultFileSystem);

    // bitmap functions (corners and edges)
    CImage32&       GetBitmap(int bitmap);
    const CImage32& GetBitmap(int bitmap) const;

    void SetBackgroundMode(int mode);
    int  GetBackgroundMode() const;

    void SetEdgeOffset(int edge, byte offset);
    byte  GetEdgeOffset(int edge) const;

    void SetBackgroundColor(int corner, RGBA color);
    RGBA GetBackgroundColor(int corner) const;

private:
    static bool WriteBitmap(IFile* file, const CImage32* bitmap);
    static bool ReadBitmap(IFile* file, CImage32* bitmap);
    static bool ReadBitmap(IFile* file, CImage32* bitmap, int edge_width);

private:
    CImage32 m_Bitmaps[9];

    int m_BackgroundMode;
    RGBA m_BackgroundCorners[4];
    byte m_EdgeOffsets[4];
};

inline CImage32&
sWindowStyle::GetBitmap(int bitmap)
{
    return m_Bitmaps[bitmap];
}

inline const CImage32&
sWindowStyle::GetBitmap(int bitmap) const
{
    return m_Bitmaps[bitmap];
}

inline void
sWindowStyle::SetBackgroundMode(int mode)
{
    m_BackgroundMode = mode;
}

inline int
sWindowStyle::GetBackgroundMode() const
{
    return m_BackgroundMode;
}

inline void
sWindowStyle::SetEdgeOffset(int edge, byte offset)
{
    m_EdgeOffsets[edge] = offset;
}

inline byte
sWindowStyle::GetEdgeOffset(int edge) const
{
    return m_EdgeOffsets[edge];
}

inline void
sWindowStyle::SetBackgroundColor(int corner, RGBA color)
{
    m_BackgroundCorners[corner] = color;
}

inline RGBA
sWindowStyle::GetBackgroundColor(int corner) const
{
    return m_BackgroundCorners[corner];
}

#endif
