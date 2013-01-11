#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../common/Spriteset.hpp"
#include "../../common/rgb.hpp"


#define VERGE_STYLE_IMPORT


bool VergeImageImport(sSpriteset& spriteset, const char* filename, int spriteWidth, int spriteHeight, RGBA transColor)
{
    CImage32 image;
    if (!image.Load(filename))
        return false;

    if ((image.GetHeight() / 8) < spriteWidth && (image.GetWidth() / 6) < spriteHeight)
        return false;

    //spriteset.Create(8, 8);
    //spriteset.ResizeFrames(spriteWidth, spriteHeight);
    spriteset.Create(spriteWidth, spriteHeight, 30, 8, 8);

    // make a color transparent in the image
    for (int i = 0; i < image.GetWidth() * image.GetHeight(); i++)
    {
        RGBA& pixel = image.GetPixels()[i];
        if (pixel.red == transColor.red &&
                pixel.green == transColor.green &&
                pixel.blue == transColor.blue)
            pixel.alpha = 0;
    }

    // insert the "frames" into the spriteset
    for (int row=0; row<6; row++)
        for (int col=0; col<5; col++)
        {
            CImage32& frame = spriteset.GetImage((row * 5)+col);

            for (int i=0; i<spriteHeight; i++)
                memcpy(frame.GetPixels() + (i * spriteWidth),
                       image.GetPixels() + (((row*spriteHeight) + i + 1 + (1*row)) * image.GetWidth() + 1 + (1*col) + (col*spriteWidth)),
                       spriteWidth * sizeof(RGBA));
        }

#define CopyFrame(direction, row, frame, column)            \
{                                                           \
  /*for (int k=0; k<spriteHeight; k++) \
    memcpy(spriteset.GetFrame(direction, frame).GetPixels() + (k * spriteWidth),  \
    image.GetPixels() + (((row*spriteHeight) + k + 1 + (1*row)) * image.GetWidth() + 1 +(1*column) + (column*spriteWidth)), \
    spriteWidth * sizeof(RGBA)); */ \
    spriteset.SetFrameIndex(direction, frame, (row * 5) + column); \
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
    CopyRow(0, 1);
    CopyRow(1, 2);
    CopyRow(2, 2);
    CopyRow(3, 2);
    CopyRow(4, 0);
    CopyRow(5, 3);
    CopyRow(6, 3);
    CopyRow(7, 3);

    return true;
}


int hval(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'a' && c <= 'z')
        return c - 'a' + 10;
    else if (c >= 'A' && c <= 'Z')
        return c - 'A' + 10;
    else
        return -1;
}


RGBA HexToColor(const char* str)
{
    if (strlen(str) != 6)
        return CreateRGBA(0, 0, 0, 255);
    else
    {
        RGBA c;
        c.red   = hval(str[0]) * 16 + hval(str[1]);
        c.green = hval(str[2]) * 16 + hval(str[3]);
        c.blue  = hval(str[4]) * 16 + hval(str[5]);
        c.alpha = 0;
        return c;
    }
}


int main(int argc, char** argv)
{
    sSpriteset spriteset;

#ifdef VERGE_STYLE_IMPORT
    if (argc != 5 && argc != 6)
    {
        puts("Syntax: image2rss <image> <spritewidth> <spriteheight> <rssfile> [transcolor]");
        puts("  Transparent color is in the form of a hexadecimal triplet (RGB).");
        puts("  Red would be FF0000 and blue would be 0000FF.");
        puts("");
        puts("  Note:  Image is VERGE-style.  1-pixel lines separate the frames.");
        return EXIT_FAILURE;
    }

    RGBA transColor;
    if (argc == 6)
        transColor = HexToColor(argv[5]);
    else
    {
        transColor.red   = 0;
        transColor.green = 155;
        transColor.blue  = 67;
        transColor.alpha = 255;
    }


    if (!VergeImageImport(spriteset, argv[1], atoi(argv[2]), atoi(argv[3]), transColor))
    {
        printf("Error: Could not import Image '%s' \n", argv[1]);
        return EXIT_FAILURE;
    }
#else
    if (argc != 5)
    {
        puts("Syntax: image2rss <imagefile> <framewidth> <frameheight> <rssfile>");
        return EXIT_FAILURE;
    }

    if (!spriteset.Import_BMP(argv[1], atoi(argv[2]), atoi(argv[3]), CreateRGBA(0,0,0,255)))
    {
        printf("Error: Could not import Image '%s' \n", argv[1]);
        return EXIT_FAILURE;
    }
#endif

    if (!spriteset.Save(argv[4]))
    {
        printf("Error: Could not save RSS '%s'\n", argv[4]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

