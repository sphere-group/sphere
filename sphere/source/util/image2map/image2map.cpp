

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../common/Image32.hpp"
#include "../../common/Map.hpp"

int main(int argc, char** argv)
{
    // check arguments
    if (argc != 2)
    {
        puts("Usage: image2map <imagefile>");
        return EXIT_FAILURE;
    }
    
	const char* filename = argv[1];

    // load image
    CImage32 image;
    if (image.Load(filename) == false)
    {
        puts("Error: Could not load image");
        return EXIT_FAILURE;
    }

    // build map from image
    sMap map;
    if (map.BuildFromImage(image) == false)
    {
        puts("Error: Could not build map from image");
        return EXIT_FAILURE;
    }

    char* fn = new char[strlen(filename) + 10];
    strcpy(fn, filename);
    if (strrchr(fn, '.') == NULL)
	{
        strcat(fn, ".rmp");
	}
    else
	{
        strcpy(strrchr(fn, '.'), ".rmp");
	}

    map.Save(fn);
    printf("%s converted to %s successfully\n", filename, fn);
    delete[] fn;
    
	return EXIT_SUCCESS;
}
