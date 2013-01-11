
#include <stdio.h>
#include <string.h>

#include "../../common/Spriteset.hpp"
int main(int argc, char **argv)
{
    sSpriteset sprite;
    if (argc != 2)
    {
        printf("syntax: rss2image <rssfile>\n");
        return 1;
    }
    else
    {
        if (!sprite.Load(argv[1]))
        {
            printf("error: can't load %s\n", argv[1]);
            return 1;
        }
        char* export_name = new char[strlen(argv[1])];
        sprintf(export_name , "%s", argv[1]);
        sprintf(export_name + strlen(argv[1]) - 3, "png");
        sprite.Export_PNG(export_name);
        printf("converted %s to %s\n", argv[1], export_name);
        delete export_name;
        return 0;
    }
}
