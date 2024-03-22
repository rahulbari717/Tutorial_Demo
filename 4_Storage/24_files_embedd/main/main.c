#include <stdio.h>

void app_main(void)
{
    extern const unsigned char hello[] asm("_binary_hello_py_start");
    printf("Python = %s\n", hello);

    extern const unsigned char rahul[] asm("_binary_rahul_txt_start");
    printf("rahul = %s\n", rahul);

    extern const unsigned char imgStart[] asm("_binary_DP_jpeg_start");
    extern const unsigned char imgEnd[] asm("_binary_DP_jpeg_end");
    const unsigned int imgSize = imgEnd - imgStart;
    printf("img size is %d\n", imgSize);
}