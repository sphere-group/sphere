//
//  Generates the alpha table for existing (old)
//  pixel image.  Use something like:
//
//  make_old_clut.exe >../common/alpha_old.table
//
//  To apply previous alpha use:
//  cur_value.chan=alpha_old[target_alpha][cur_value.chan];
//
#include <stdio.h>

int main()
{
    for (int a = 0; a < 256; ++a)
    {
        printf("{");
        for (int v = 0; v < 256; ++v)
        {
            printf("0x%02x,",  v * (255 - a) / 255);
        }
        printf("},\n");
    }

    return 0;
}
