#include <kernel/exec/objdump.h>
#include <kernel/core/kernel.h>

const char KERNEL_OBJDUMP_FNAME[] = "A:/kernel.dump";
objdump_t  KERNEL_OBJDUMP;

char objdump_addr_str[32];

void objdump_load(objdump_t* dump, char* filename)
{
    dump->filename = filename;
    dump->file = vfs_file_read(dump->filename);
    if (dump->file.data == NULL) { debug_error("Unable to locate kernel dump file at '%s'", dump->filename); return; }    
    debug_ok("Initialized object dump utility - FILE: '%s', SIZE: %d bytes", dump->file.name, dump->file.size);
}

objdump_symbol_t objdump_parse_line(objdump_t* dump, char* line)
{
    objdump_symbol_t symbol;
    memset(&symbol, 0, sizeof(objdump_symbol_t));

    int line_len = strlen(line), stage = 0, name_end = 0, section_end = 0;
    for (int i = 0; i < line_len; i++)
    {
        if (line[i] == ',')
        {
            switch (stage)
            {
                case 0:
                {
                    // name
                    uint32_t nend = strchr(line, ',');
                    strncpy(symbol.name, line, nend - (uint32_t)line);
                    nend = i;
                    name_end = i;
                    break;
                }
                case 1:
                {
                    // address
                    char addr_str[16];
                    memset(addr_str, 0, 16);
                    char* astart = (char*)((uint32_t)line + name_end) + 1;
                    int j = 0;
                    while (astart[j] != ',') { stradd(addr_str, astart[j]); j++; }
                    section_end = ((uint32_t)astart - (uint32_t)line) + j + 1;
                    uint32_t addr = atol(addr_str);
                    symbol.address = addr;

                    // section
                    char* sec_name = (char*)((uint32_t)line + section_end);
                    strcpy(symbol.section, sec_name);
                    break;
                }
                default: { debug_error("Error parsing objdump line '%s'", line); memset(&symbol, 0, sizeof(objdump_symbol_t)); return symbol; }
            }
            stage++;
        }
    }
    return symbol;
}

uint32_t objdump_get_count(objdump_t* dump)
{
    uint32_t count = 1;
    for (uint32_t i = 0; i < dump->file.size; i++)
    {
        if (dump->file.data[i] == '\n') { count++; }
    }
    return count;
}

objdump_symbol_t* objdump_parse(objdump_t* dump)
{
    int count = 0;
    objdump_symbol_t* symbols = kmalloc(sizeof(objdump_symbol_t) * objdump_get_count(dump) + 0x1000, HEAPTYPE_ARRAY);
    char* line = kmalloc(4096, HEAPTYPE_STRING);
    for (uint32_t i = 0; i < dump->file.size + 1; i++)
    {
        if (i == dump->file.size)
        {
            objdump_symbol_t symbol = objdump_parse_line(dump, line);
            if (symbol.address > 0) { symbols[count] = symbol; count++; }
            memset(line, 0, 4096);
            break;
        }
        if (dump->file.data[i] == '\n')
        {
            objdump_symbol_t symbol = objdump_parse_line(dump, line);
            if (symbol.address > 0) { symbols[count] = symbol; count++; }
            memset(line, 0, 4096);
        }
        else { stradd(line, dump->file.data[i]); }
    }

    dump->method_count = count;
    kfree(line);
    debug_ok("Finished parsing objdump file '%s'", dump->file.name);    
    return symbols;
}