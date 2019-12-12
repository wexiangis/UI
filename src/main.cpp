#include <stdio.h>
#include "pngpkg.h"

int main(int argc, char **argv)
{
    PicData pic;
    if(argc > 1)
    {
        if(png_read(argv[1], &pic) == 0)
            printf("png: %dx%d, bit/%d, alpha/%d\n", pic.width, pic.height, pic.bit_depth, pic.alpha_flag);
    }
    return 0;
}
