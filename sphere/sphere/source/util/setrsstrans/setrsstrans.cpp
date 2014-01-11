#include <iostream>
#include <string>
#include <cstdlib>
using namespace std;
#include "../../common/Spriteset.hpp"


int HexDigit(char c)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f')
    {
        return 10 + c - 'a';
    }
    if (c >= 'A' && c <= 'F')
    {
        return 10 + c - 'A';
    }

    return -1;
}


bool IsValidColor(string s)
{
    if (s.length() != 7)
    {
        return false;
    }

    if (s[0] != '#')
    {
        return false;
    }

    for (int i = 1; i < 7; i++)
    {
        if (HexDigit(s[i]) == -1)
        {
            return false;
        }
    }

    return true;
};


RGB MakeColor(string s)
{
    RGB rgb;
    rgb.red   = HexDigit(s[1]) * 16 + HexDigit(s[2]);
    rgb.green = HexDigit(s[3]) * 16 + HexDigit(s[4]);
    rgb.blue  = HexDigit(s[5]) * 16 + HexDigit(s[6]);
    return rgb;
}


int main(int argc, char** argv)
{
    if (argc != 3)
    {
        cout << "Syntax: setrsstrans <rssfile> <color>" << endl <<
        "  color is in HTML format" << endl;
        return EXIT_FAILURE;
    }

    string rssfile = argv[1];
    string colorstr = argv[2];
    if (!IsValidColor(colorstr))
    {
        cout << "Error: Color is not valid" << endl;
        return EXIT_FAILURE;
    }

    RGB color = MakeColor(colorstr);

    // load the spriteset
    sSpriteset spriteset;
    if (!spriteset.Load(rssfile.c_str()))
    {
        cout << "Error: Could not load spriteset" << endl;
        return EXIT_FAILURE;
    }

    // modify the spriteset
    for (int i = 0; i < spriteset.GetNumDirections(); i++)
    {
        for (int j = 0; j < spriteset.GetNumFrames(i); j++)
        {
            //sSprite& frame = spriteset.GetFrame(i, j);
            CImage32& frame = spriteset.GetImage(spriteset.GetFrameIndex(spriteset.GetDirectionName(i),j));

            RGBA* c = frame.GetPixels();
            for (int k = 0; k < frame.GetWidth() * frame.GetHeight(); k++)
            {
                if (c->red == color.red &&
                        c->green == color.green &&
                        c->blue == color.blue)
                {
                    c->alpha = 0;
                }

                c++;
            }
        }
    }

    // save it again
    if (!spriteset.Save(rssfile.c_str()))
    {
        cout << "Error: Could not save spriteset" << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
