

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../common/Spriteset.hpp"

int main(int argc, char** argv)
{
    if (argc != 3 && argc != 4)
    {
        puts("Syntax: chr2rss <chrfile> <palfile> [rssfile]");
        return EXIT_FAILURE;
    }

    sSpriteset spriteset;
    if (!spriteset.Import_CHR(argv[1], argv[2]))
    {
        printf("Error: Could not import CHR '%s' '%s'\n", argv[1], argv[2]);
        return EXIT_FAILURE;
    }
    
	char outfile[520];
    if (argc == 4)
	{
        strcpy(outfile, argv[3]);
	}
    else
    {
        strcpy(outfile, argv[1]);
        if (strrchr(outfile, '.'))
		{
            strcpy(strrchr(outfile, '.'), ".rss");
		}
        else
		{
            strcat(outfile, ".rss");
		}
    }

    if (!spriteset.Save(outfile))
    {
        printf("Error: Could not save RSS '%s'\n", argv[3]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
