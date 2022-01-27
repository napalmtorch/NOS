#pragma once
#include <lib/stdint.h>
#include <lib/graphics/color.h>
#include <lib/graphics/font.h>

typedef struct
{
    uint16_t     attributes;
    uint8_t      window_a, Window_b;
    uint16_t     granularity;
    uint16_t     window_size;
    uint16_t     segment_a, Segment_b;
    uint16_t     window_func[2];
    uint16_t     pitch, width, height;
    uint8_t      char_width, char_height, planes, depth, banks;
    uint8_t      memory_model, bank_size, image_pages;
    uint8_t      reserved0;
    uint8_t      red_mask, red_position;
    uint8_t      green_mask, green_position;
    uint8_t      blue_mask, blue_position;
    uint8_t      rsv_mask, rsv_position;
    uint8_t      direct_color;
    uint32_t     physical_base;
    uint32_t     reserved1;
    uint16_t     reserved2;
} PACKED vesa_mode_info_t;

void vesa_init();
void vesa_clear(uint32_t color);
void vesa_blit(int x, int y, uint32_t color);
void vesa_rect(int x, int y, int w, int h, int t, uint32_t color);
void vesa_rectf(int x, int y, int w, int h, uint32_t color);
void vesa_char(int x, int y, char c, uint32_t fg, uint32_t bg, font_t font);
void vesa_string(int x, int y, char* str, uint32_t fg, uint32_t bg, font_t font);
