#include <stdio.h>
#include "../../common/Tileset.hpp"
int main(int argc, char** argv)
{
    sTileset tileset;
    if (argc != 3)
    {
        puts("Syntax: vsp2rts <vspfile> <rtsfile>");
        return 0;
    }
    if (!tileset.Import_VSP(argv[1]))
    {
        printf("Error: Could not import VSP '%s'\n", argv[1]);
        return 1;
    }
    if (!tileset.Save(argv[2]))
    {
        printf("Error: Could not save RTS '%s'\n", argv[2]);
        return 1;
    }
    return 0;
}
