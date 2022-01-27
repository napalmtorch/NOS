#include <lib/nos.h>
#include <lib/stdlib.h>

int _start(char** argv, int argc)
{
    lib_init();

    uint32_t num = __atol("123456");
    printf("NUM: %d\n", num);

    uint8_t* test = malloc(8192);
    free(test);

    printf("Program is done doing random shit now\n");
    return 420;
}