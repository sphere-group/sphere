#ifndef __SFONT_H
#define __SFONT_H

#include <vector>
#include "../common/Font.hpp"
#include "video.hpp"

class SFONT
{
public:
    SFONT();
    ~SFONT();

    bool Load(const char* filename, IFileSystem& fs);
    bool Save(const char* filename);

    bool CreateFromFont(const sFont& font);
    void Destroy();

    SFONT* Clone();
    bool GetCharacterImage(int index, IMAGE& image);
    bool SetCharacterImage(int index, IMAGE image);
    bool DrawString(int x, int y, const char* text, RGBA mask, CImage32* surface = NULL) const;
    bool DrawZoomedString(int x, int y, double scale, const char* text, RGBA mask, CImage32* surface = NULL) const;
    bool DrawTextBox(int x, int y, int w, int h, int offset, const char* text, RGBA mask, CImage32* surface = NULL) const;

    std::vector<std::string> WordWrapString(const char* string, int width) const;

    inline int GetMaxHeight() const
    {
        return m_MaxHeight;
    }
    int GetStringWidth(const char* string) const;
    int GetStringHeight(const char* string, int width) const;

private:
    bool Initialize();

private:
    sFont  m_Font;
    IMAGE* m_Images;

    int m_MaxHeight;
};

#endif
