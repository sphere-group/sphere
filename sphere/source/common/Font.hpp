#ifndef FONT_HPP
#define FONT_HPP
#include <vector>
#include "Image32.hpp"
#include "DefaultFileSystem.hpp"
typedef CImage32 sFontCharacter;
class sFont
{
public:
    sFont(int num_characters = 0, int width = 0, int height = 0);
    bool Create(int num_characters, int width = 0, int height = 0);
    bool Load(const char* filename, IFileSystem& fs = g_DefaultFileSystem);
    bool Save(const char* filename, IFileSystem& fs = g_DefaultFileSystem) const;
    void GenerateGradient(RGBA top, RGBA bottom);
    bool ImportVergeTemplate(
        const char* filename,
        int char_width,
        int char_height,
        IFileSystem& fs = g_DefaultFileSystem);
    void  SetNumCharacters(int num_characters);
    int                   GetNumCharacters() const;
    sFontCharacter&       GetCharacter(int i);
    const sFontCharacter& GetCharacter(int i) const;
private:
    std::vector<sFontCharacter> m_Characters;
};
////////////////////////////////////////////////////////////////////////////////
inline int
sFont::GetNumCharacters() const
{
    return m_Characters.size();
}
////////////////////////////////////////////////////////////////////////////////
inline sFontCharacter&
sFont::GetCharacter(int i)
{
    return m_Characters[i];
}
////////////////////////////////////////////////////////////////////////////////
inline const sFontCharacter&
sFont::GetCharacter(int i) const
{
    return m_Characters[i];
}
////////////////////////////////////////////////////////////////////////////////
#endif
