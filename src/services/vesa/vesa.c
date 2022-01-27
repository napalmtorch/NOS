#include <services/vesa/vesa.h>
#include <lib/nos.h>

vesa_mode_info_t* vesa_info;
uint32_t*         vesa_buffer;
uint32_t          vesa_size;

void vesa_init()
{
    // get mode info address
    asm volatile("int $0x80": :"a"(0), "b"(0), "c"(SYSCALL_VBEMODE), "d"(&vesa_info));

    // set properties
    vesa_buffer = vesa_info->physical_base;
    vesa_size   = vesa_info->width * vesa_info->height * (vesa_info->depth / 8);
    vesa_clear(0);

    // finished
    printf("Initialized VESA - FB: 0x%8x, RES: %dx%dx%d\n", vesa_info->physical_base, vesa_info->width, vesa_info->height, vesa_info->depth);
}

void vesa_clear(uint32_t color)
{
    memset((void*)vesa_info->physical_base, color, vesa_size);
}

void vesa_blit(int x, int y, uint32_t color)
{
    if ((uint32_t)x >= vesa_info->width || (uint32_t)y >= vesa_info->height) { return; }
    vesa_buffer[y * vesa_info->width + x] = color;
}

void vesa_rect(int x, int y, int w, int h, int t, uint32_t color)
{
    
}

void vesa_rectf(int x, int y, int w, int h, uint32_t color)
{
    for (int i = 0; i < w * h; i++) { vesa_blit(x + (i % w), y + (i / w), color); }
}

void vesa_char(int x, int y, char c, uint32_t fg, uint32_t bg, font_t font)
{

}

void vesa_string(int x, int y, char* str, uint32_t fg, uint32_t bg, font_t font)
{

}
