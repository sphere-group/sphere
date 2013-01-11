#include <stdio.h>
#include <stdlib.h>
#include "../../common/Tileset.hpp"
int main(int argc, char** argv)
{
    if (argc != 3)
    {

        puts("tst2rts <infile.tst> <outfile.rss>");
        return EXIT_FAILURE;
    }
    sTileset tileset;
    if (tileset.Import_TST(argv[1]) == false)
    {

        printf("Error:  Could not not import file '%s'\n", argv[1]);
        return EXIT_FAILURE;
    }
    if (tileset.Save(argv[2]) == false)
    {

        printf("Error:  Could not save file '%s'\n", argv[2]);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
