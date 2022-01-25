#include <kernel/memory/memory.h>
#include <kernel/core/kernel.h>

uint32_t mem_convert_size(uint32_t value)
{
    if (value >= KB && value < MB) { return value / KB; }
    if (value >= MB && value < GB) { return value / MB; }
    if (value >= GB) { return value / GB; }
    return value;
}

char* mem_convert_size_str(char* buff, uint32_t value)
{
    char temp[16];
    memset(buff, 0, 8);
    memset(temp, 0, 16);

    uint32_t size = mem_convert_size(value);
    ltoa(size, temp, 10);
    strcat(buff, temp);
    
    if (value <  KB)               { strcat(buff, " BYTES"); }
    if (value >= KB && value < MB) { strcat(buff, " KB"); }
    if (value >= MB && value < GB) { strcat(buff, " MB"); }
    if (value >= GB)               { strcat(buff, " GB"); }
    
    return buff;
}

uint32_t mem_align(uint32_t value, uint32_t align)
{
    uint32_t out = value;
    out &= (0xFFFFFFFF - (align - 1));
    if (out < value) { out += align; }
    return out;
}