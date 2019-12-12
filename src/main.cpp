#include <stdio.h>

#define mode 1

#if(mode == 1) //viewValue 测试

#include "viewValue.h"

int main(void)
{
    ViewValue_Format *arrayWrite[9];
    int i = 0;

    arrayWrite[i++] = viewValue_init("_char", VT_CHAR, 1, 't');
    arrayWrite[i++] = viewValue_init("_bool", VT_BOOL, 1, true);
    arrayWrite[i++] = viewValue_init("_int", VT_INT, 1, 10086);
    arrayWrite[i++] = viewValue_init("_double", VT_DOUBLE, 1, 3.1415926);
    arrayWrite[i++] = viewValue_init("_string", VT_STRING, 1, "this is\ntest string !!");
    arrayWrite[i++] = viewValue_init("_boolArray", VT_BOOL_ARRAY, 3, true, false, true);
    arrayWrite[i++] = viewValue_init("_intArray", VT_INT_ARRAY, 3, 10086, 10010, 10000);
    arrayWrite[i++] = viewValue_init("_doubleArray", VT_DOUBLE_ARRAY, 3, 1.23, 4.56, 7.89);
    arrayWrite[i++] = viewValue_init("_stringArray", VT_STRING_ARRAY, 3, "123", "456", "789");

    viewValue_save("./ui.config", arrayWrite, i);
    viewValue_load("./ui.config", arrayWrite, i);
    
    viewValue_save("./ui-bak.config", arrayWrite, i);

    return 0;
}

#elif(mode == 0) //png 测试

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

#endif