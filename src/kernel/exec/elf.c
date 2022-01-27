#include <kernel/exec/elf.h>
#include <kernel/core/kernel.h>

const char elf_valid_id[4] = { 0x7F, 'E', 'L', 'F ' };

bool elf_validate(elf_header_t* header) { return !memcmp(header->identifier.id, elf_valid_id, 4); }

bool elf_validate_type(elf_header_t* header) { return header->type == 2; }

bool elf_load(uint8_t* data, uint32_t size, void* process)
{
    if (data == NULL || process == NULL || size == 0) { return false; }
    process_t*        proc     = process;
    elf_header_t*     hdr      = data;
    elf_progheader_t* prog_hdr = (uint32_t)data + hdr->ph_table;

    if (!elf_validate(hdr)) { debug_error("Unable to valid ELF data at 0x%8x", data); return false; }
    if (!elf_validate_type(hdr)) { debug_error("ELF data at 0x%8x is not executable"); return false; }

    uint8_t* elf_data = kmalloc(size, HEAPTYPE_ARRAY);
    proc->executable = (executable_t){ .data = data, .elf_data = elf_data, .size = size, .elf = hdr };

    for (uint32_t i = 0; i < hdr->ph_entry_count; i++, prog_hdr++)
    {
        switch (prog_hdr->type)
        {   
            default: { debug_error("Unsupported ELF program header type 0x%2x", prog_hdr->type); }
            case 0: { break; }
            case 1:
            {
                // map virtual address to kernel and process
                uint32_t p = ((uint32_t)elf_data - HEAP_KERNEL.v_address) + HEAP_KERNEL.p_address;
                printf("ADDR: 0x%8x, 0x%8x, OFFSET: 0x%8x\n", (uint32_t)elf_data, p, prog_hdr->offset);
                pagedir_map(vmm_get_kdir(), prog_hdr->v_address, p, true);
                
                // copy program data to allocated memory
                memcpy((void*)prog_hdr->v_address, (void*)((uint32_t)data + prog_hdr->offset), prog_hdr->segment_size);

                // increment and print info
                elf_data += mem_align(prog_hdr->segment_size, 0x1000);
                debug_info("Loading elf segment - PHYS: 0x%8x, VIRT: 0x%8x, SIZE: %d BYTES", p, prog_hdr->v_address, prog_hdr->segment_size);
                break;
            }
        }
    }

    proc->ctrl.threads[0]->ctrl.entry = hdr->entry;
    return true;
}

bool elf_load_file(char* filename, void* proc)
{

}