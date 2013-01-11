#ifndef SWINDOWSTYLE_HPP
#define SWINDOWSTYLE_HPP

#include "video.hpp"
#include "../common/WindowStyle.hpp"
#include "../common/IFileSystem.hpp"

class SWINDOWSTYLE
{
public:
    SWINDOWSTYLE();
    ~SWINDOWSTYLE();

    bool Load(const char* filename, IFileSystem& fs);
    bool CreateFromWindowStyle(const sWindowStyle& ws);
    void Destroy();

    int GetBackgroundMode() const;
    bool DrawBackground(int x, int y, int w, int h, int background_mode, const RGBA& mask, bool is_masked) const;

    bool DrawTopEdge   (int x, int y, int w, int h, const RGBA& mask, bool is_masked) const;
    bool DrawBottomEdge(int x, int y, int w, int h, const RGBA& mask, bool is_masked) const;
    bool DrawLeftEdge  (int x, int y, int w, int h, const RGBA& mask, bool is_masked) const;
    bool DrawRightEdge (int x, int y, int w, int h, const RGBA& mask, bool is_masked) const;

    bool DrawUpperLeftCorner (int x, int y, const RGBA& mask, bool is_masked) const;
    bool DrawUpperRightCorner(int x, int y, const RGBA& mask, bool is_masked) const;
    bool DrawLowerLeftCorner (int x, int y, const RGBA& mask, bool is_masked) const;
    bool DrawLowerRightCorner(int x, int y, const RGBA& mask, bool is_masked) const;

    bool DrawWindow(int x, int y, int w, int h, const RGBA& mask, bool is_masked) const;

	int GetBorder(int index) ;
private:
    void Initialize();

private:
    sWindowStyle m_WindowStyle;
    IMAGE m_Images[9];

    bool DrawCorner(int index, int x, int y, const RGBA& mask, bool is_masked) const;
    bool DrawHorizontalEdge(int index, int x, int y, int w, int h, const RGBA& mask, bool is_masked) const;
    bool DrawVerticalEdge(int index, int x, int y, int w, int h, const RGBA& mask, bool is_masked) const;
};

#endif
