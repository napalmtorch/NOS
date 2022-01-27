#pragma once
#include <lib/stdint.h>

typedef struct
{
    uint8_t  width;
    uint8_t  height;
    int8_t   spacing_x;
    int8_t   spacing_y;
    uint8_t* data;
} font_t;

// default font data
extern const uint8_t FONTDATA_DEFAULT[];

extern font_t FONT_DEFAULT;