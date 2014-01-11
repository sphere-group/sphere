#include <stdio.h>
#include <stdlib.h>
#include "../../common/Map.hpp"
int main(int argc, char** argv)
{
    sMap map;
    if (argc != 4)
    {
        puts("Syntax: vmap2rmp <mapfile> <rss/vsp file> <rmpfile>");
        return EXIT_FAILURE;
    }
    if (!map.Import_VergeMAP(argv[1], argv[2]))
    {
        printf("Error: Could not import MAP '%s' '%s'\n", argv[1], argv[2]);
        return EXIT_FAILURE;
    }
    if (!map.Save(argv[3]))
    {
        printf("Error: Could not save RMP '%s'\n", argv[3]);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
