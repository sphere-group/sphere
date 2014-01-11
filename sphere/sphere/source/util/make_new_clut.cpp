//
//  Generates the alpha table for target (new)
//  pixel image.  Use something like:
//
//  make_new_clut.exe >../common/alpha_new.table
//
//  To apply target alpha use:
//  destination.chan=alpha_new[target_alpha][target_value.chan];
//
#include <stdio.h>

int main()
{
    for (int a = 0; a < 256; ++a)
    {
        printf("{");
        for (int v = 0; v < 256; ++v)
        {
            printf("0x%02x,", v * a / 255);
        }
        printf("},\n");
    }

    return 0;
}
