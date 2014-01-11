#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <string>
#include "sfont.hpp"
#include "../common/minmax.hpp"

////////////////////////////////////////////////////////////////////////////////

SFONT::SFONT()
{
    m_Images = NULL;
    m_MaxHeight = 0;
}

////////////////////////////////////////////////////////////////////////////////

SFONT::~SFONT()
{
    Destroy();
}

////////////////////////////////////////////////////////////////////////////////

bool
SFONT::Load(const char* filename, IFileSystem& fs)
{

    // load file
    if (m_Font.Load(filename, fs) == false)
    {
        return false;
    }

    return Initialize();
}

////////////////////////////////////////////////////////////////////////////////

bool
SFONT::CreateFromFont(const sFont& font)
{

    Destroy();
    m_Font = font;
    return Initialize();
}

////////////////////////////////////////////////////////////////////////////////

void
SFONT::Destroy()
{
    if (m_Images)
    {
        for (int i = 0; i < m_Font.GetNumCharacters(); i++)
        {
            if (m_Images[i])
            {
                DestroyImage(m_Images[i]);

                m_Images[i] = NULL;
            }
        }

        delete[] m_Images;
        m_Images = NULL;
    }
    m_MaxHeight = 0;
}

////////////////////////////////////////////////////////////////////////////////

SFONT*
SFONT::Clone()
{
    SFONT* font = new SFONT;
    if (font)
    {

        font->CreateFromFont(m_Font);
    }
    return font;
}

////////////////////////////////////////////////////////////////////////////////

bool
SFONT::Save(const char* filename)
{
    return m_Font.Save(filename);
}

////////////////////////////////////////////////////////////////////////////////

bool
SFONT::GetCharacterImage(int index, IMAGE& image)
{
    int range = m_Font.GetNumCharacters();
    if (index < 0 || index >= range)
    {

        // m_ErrorMessage = "Character index does not exist";
        return false;
    }
    sFontCharacter& c = m_Font.GetCharacter(index);
    image = CreateImage(c.GetWidth(), c.GetHeight(), c.GetPixels());
    if (!image)
        return false;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
SFONT::SetCharacterImage(int index, IMAGE image)
{
    int range = m_Font.GetNumCharacters();
    if (index < 0 || index >= range)
    {
        // m_ErrorMessage = "Character index does not exist";
        return false;
    }

    RGBA* pixels = LockImage(image);
    if (!pixels)
    {

        // m_ErrorMessage = "LockImage failed!!";
        return false;
    }

    sFontCharacter& c = m_Font.GetCharacter(index);
    c.Resize(GetImageWidth(image), GetImageHeight(image));
    if (c.GetWidth() != GetImageWidth(image) || c.GetHeight() != GetImageHeight(image))
    {

        // m_ErrorMessage = "Resize failed!";
        return false;
    }

    CImage32::BlendMode blend_mode = c.GetBlendMode();
    c.SetBlendMode(CImage32::REPLACE);
    for (int x = 0; x < c.GetWidth(); x++)
    {

        for (int y = 0; y < c.GetHeight(); y++)
        {

            c.SetPixel(x, y, pixels[y * GetImageWidth(image) + x]);
        }
    }

    c.SetBlendMode(blend_mode);
    UnlockImage(image, false);

    if (m_Images[index])
    {

        DestroyImage(m_Images[index]);
    }

    m_Images[index] = CreateImage(c.GetWidth(), c.GetHeight(), c.GetPixels());

    if (!m_Images[index])
    {
        // m_ErrorMessage = CreateImage failed!
        return false;
    }

    for (int i = 0; i < m_Font.GetNumCharacters(); i++)
        if (m_Font.GetCharacter(i).GetHeight() > m_MaxHeight)
            m_MaxHeight = m_Font.GetCharacter(i).GetHeight();

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
SFONT::DrawString(int x, int y, const char* text, RGBA mask, CImage32* surface) const
{
    int range = m_Font.GetNumCharacters();

    while (*text)
    {
        int ch = (unsigned char) *text;
        if (ch < 0 || ch >= range)
        {
            text++;
            continue;
        }

        const sFontCharacter& character = m_Font.GetCharacter(ch);
        if (surface == NULL)
        {
            BlitImageMask(m_Images[ch], x, y, CImage32::BLEND, mask, CImage32::MULTIPLY);
        }
        else
        {
            CImage32 tmp(character);
            surface->BlitImageMask(tmp, x, y, mask, CImage32::MULTIPLY);
        }

        x += character.GetWidth();
        text++;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
SFONT::DrawZoomedString(int x, int y, double scale, const char* text, RGBA mask, CImage32* surface) const
{
    struct Local
    {
        static inline void ScaleBlit(IMAGE i, int x, int y, double scale, RGBA mask, CImage32* lsurface, const CImage32 character)
        {
            int w = GetImageWidth(i);
            int h = GetImageHeight(i);
            int ax[4] = { x, x + (int)(scale * w), x + (int)(scale * w), x };
            int ay[4] = { y, y, y + (int)(scale * h), y + (int)(scale * h) };
            if (lsurface == NULL)
            {
                TransformBlitImageMask(i, ax, ay, CImage32::BLEND, mask, CImage32::MULTIPLY);
            }
            else
            {
                CImage32 tmp(character);
                lsurface->TransformBlitImageMask(tmp, ax, ay, mask, CImage32::MULTIPLY);
            }
        }
    };

    double cx = x;
    int range = m_Font.GetNumCharacters();

    while (*text)
    {
        int ch = (unsigned char) *text;

        if (ch < 0 || ch >= range)
        {
            text++;
            continue;
        }

        const sFontCharacter& character = m_Font.GetCharacter(ch);
        Local::ScaleBlit(m_Images[ch], (int)cx, y, scale, mask, surface, character);

        cx += scale * character.GetWidth();
        text++;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
SFONT::DrawTextBox(int x, int y, int w, int h, int offset, const char* text, RGBA mask, CImage32* surface) const
{
    const int space_width = GetStringWidth(" ");
    const int tab_width   = GetStringWidth("   ");
    const int max_height  = GetMaxHeight();

    const char* p = text;
    int old_x, old_y, old_w, old_h;
    GetClippingRectangle(&old_x, &old_y, &old_w, &old_h);
    SetClippingRectangle(std::max(x, old_x), std::max(y, old_y), std::min(w, old_w), std::min(h, old_h));

    y += offset;
    // delta y from starting position
    int dy = 0;

    // Word-wrap and draw strings.
    std::vector<std::string> lines = WordWrapString(text, w);
    for (std::vector<std::string>::const_iterator ci = lines.begin(); ci != lines.end(); ++ci)
    {
        DrawString(x, y + dy, ci->c_str(), mask, surface);
        dy += max_height;
    }

    //DrawString(x + dx, y + dy, word.c_str(), mask, surface);
    SetClippingRectangle(old_x, old_y, old_w, old_h);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<std::string>
SFONT::WordWrapString(const char* string, int width) const
{
    const int space_width = GetStringWidth(" ");
    const int tab_width   = GetStringWidth("   ");
    const int max_height  = GetMaxHeight();

    const char* p = string;

    // delta x and y from starting position
    int dx = 0;
    int dy = 0;

    std::vector<std::string> words;
    words.push_back("");
    std::string word;
    int word_width = 0;

    int range = m_Font.GetNumCharacters();
    // parse the text into words
    while (*p)
    {
        int ch = (unsigned char) *p;
        if (ch < 0 || ch >= range)
        {
            p++;
            continue;
        }

        if (ch == ' ')
        {          // if it's a space, draw the word

            if (dx + word_width + space_width > width)
            {
                // Word goes on a new line.
                dx = word_width + space_width;
                dy += max_height;

                words.push_back(word + " ");
            }
            else
            {
                // Word is tacked on to the last line.
                words.back() += word + " ";
                dx += word_width + space_width;
            }

            word.resize(0);
            word_width = 0;

        }
        else if (ch == '\t')
        {  // if it's a tab, draw the word

            if (dx + word_width + tab_width > width)
            {
                // Word goes on a new line.
                dx = word_width + tab_width;
                dy += max_height;

                words.push_back(word + "   ");
            }
            else
            {
                // Word is tacked on to the last line.
                words.back() += word + "   ";
                dx += word_width + tab_width;
            }

            word.resize(0);
            word_width = 0;

        }
        else if (ch == '\n')
        {  // newline time, awww yeah

            words.back() += word;
            dx = 0;
            dy += max_height;
            words.push_back("");
            word.resize(0);
            word_width = 0;

        }
        else
        {

            int char_width = m_Font.GetCharacter(ch).GetWidth();
            // if we've gone over the limit and dx = 0,
            // draw the old word and split the new one off
            if (word_width + char_width > width && dx == 0)
            {
                // Split word if it's too wide for one line.
                words.back() += word;
                dy += max_height;
                words.push_back("");
                word.resize(0);
                word_width = 0;
            }
            else if (dx + word_width + char_width > width)
            {
                // Just start a new line.
                dx = 0;
                dy += max_height;
                words.push_back("");
            }

            word += ch;
            word_width += char_width;

        }
        p++;
    }

    words.back() += word;
    return words;
}

////////////////////////////////////////////////////////////////////////////////

int
SFONT::GetStringWidth(const char* string) const
{
    int width = 0;
    int range = m_Font.GetNumCharacters();

    while (*string)
    {
        int ch = (unsigned char) *string;

        if (ch < 0 || ch >= range)
        {
            string++;
            continue;
        }

        width += m_Font.GetCharacter(ch).GetWidth();
        string++;
    }
    return width;
}

////////////////////////////////////////////////////////////////////////////////

int
SFONT::GetStringHeight(const char* string, int width) const
{
    // no point doing massive loops if the user only wants
    // just the overall height of a non-wordwrapped string
    if (width == 0)
        return GetMaxHeight();

    const int space_width = GetStringWidth(" ");
    const int tab_width   = GetStringWidth("   ");
    const int max_height = GetMaxHeight();
    const char* p = string;
    int dx = 0;
    int dy = 0;

    std::string word;
    int word_width = 0;

    int range = m_Font.GetNumCharacters();
    while (*p)
    {
        int ch = (unsigned char) *p;
        if (ch < 0 || ch >= range)
        {
            p++;
            continue;
        }

        if (ch == ' ')
        {          // if it's a space, draw the word

            if (dx + word_width + space_width > width)
            {
                dx = word_width + space_width;
                dy += max_height;
            }
            else
            {
                dx += word_width + space_width;
            }

            word.resize(0);
            word_width = 0;

        }
        else if (ch == '\t')
        {  // if it's a tab, draw the word

            if (dx + word_width + tab_width > width)
            {
                dx = word_width + tab_width;
                dy += max_height;
            }
            else
            {
                dx += word_width + tab_width;
            }

            word_width = 0;
        }
        else if (ch == '\n')
        {  // newline time, awww yeah

            dx = 0;
            dy += max_height;
            word.resize(0);
            word_width = 0;

        }
        else
        {

            int char_width = m_Font.GetCharacter(ch).GetWidth();
            // if we've gone over the limit and dx = 0, draw the old word and split the new one off
            if (word_width + char_width > width && dx == 0)
            {
                dy += max_height;
                word.resize(0);
                word_width = 0;
            }
            else if (dx + word_width + char_width > width)
            {
                dx = 0;
                dy += max_height;
            }

            word += ch;
            word_width += char_width;

        }
        p++;
    }

    // add a line more to give the total line height since this
    // algorithm was supposed to draw text!
    dy += max_height;
    return dy;
}

////////////////////////////////////////////////////////////////////////////////

bool
SFONT::Initialize()
{
    int i;
    m_MaxHeight = 0;

    if ( !(m_Font.GetNumCharacters() > 0) )
    {
        return false;
    }

    m_Images = new IMAGE[m_Font.GetNumCharacters()];
    if (m_Images == NULL)
        return false;

    for (i = 0; i < m_Font.GetNumCharacters(); i++)
    {
        sFontCharacter& c = m_Font.GetCharacter(i);
        m_Images[i] = CreateImage(c.GetWidth(), c.GetHeight(), c.GetPixels());

        if (m_Images[i] == NULL)
        {
            printf ("SFONT::Initialize() failed to create character[%d] (w=%d h=%d)\n", i, c.GetWidth(), c.GetHeight());
            for (int j = 0; j < i; j++)
            {
                DestroyImage(m_Images[j]);

                m_Images[j] = NULL;
            }

            delete[] m_Images;
            m_Images = NULL;
            return false;
        }

    }

    for (i = 0; i < m_Font.GetNumCharacters(); i++)
    {

        if (m_Font.GetCharacter(i).GetHeight() > m_MaxHeight)
        {

            m_MaxHeight = m_Font.GetCharacter(i).GetHeight();
        }

    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
