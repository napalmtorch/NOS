#include <kernel/hal/common/atapio.h>
#include <kernel/core/kernel.h>

// ports
#define ATAPIO_PRIMARY_DATA         0x1F0
#define ATAPIO_PRIMARY_ERR          0x1F1
#define ATAPIO_PRIMARY_SECCOUNT     0x1F2
#define ATAPIO_PRIMARY_LBA_LO       0x1F3
#define ATAPIO_PRIMARY_LBA_MID      0x1F4
#define ATAPIO_PRIMARY_LBA_HI       0x1F5
#define ATAPIO_PRIMARY_DRIVE_HEAD   0x1F6
#define ATAPIO_PRIMARY_COMM_REGSTAT 0x1F7
#define ATAPIO_PRIMARY_ALTSTAT_DCR  0x3F6

// status flags
#define ATAPIO_STAT_ERR  (1 << 0) 
#define ATAPIO_STAT_DRQ  (1 << 3)
#define ATAPIO_STAT_SRV  (1 << 4)
#define ATAPIO_STAT_DF   (1 << 5) 
#define ATAPIO_STAT_RDY  (1 << 6)
#define ATAPIO_STAT_BSY  (1 << 7)

void atapio_callback(idt_regs_t* regs)
{
    
}

void atapio_init()
{
    irq_register(IRQ14, atapio_callback);
    if (!atapio_identify()) { irq_unregister(IRQ14); return; }
    debug_ok("Initialized ATAPIO driver");
}

bool atapio_identify()
{
    port_inb(ATAPIO_PRIMARY_COMM_REGSTAT);
    port_outb(ATAPIO_PRIMARY_DRIVE_HEAD, 0xA0);
    port_inb(ATAPIO_PRIMARY_COMM_REGSTAT);
    port_outb(ATAPIO_PRIMARY_SECCOUNT, 0);
    port_inb(ATAPIO_PRIMARY_COMM_REGSTAT);
    port_outb(ATAPIO_PRIMARY_LBA_LO, 0);
    port_inb(ATAPIO_PRIMARY_COMM_REGSTAT);
    port_outb(ATAPIO_PRIMARY_LBA_MID, 0);
    port_inb(ATAPIO_PRIMARY_COMM_REGSTAT);
    port_outb(ATAPIO_PRIMARY_LBA_HI, 0);
    port_inb (ATAPIO_PRIMARY_COMM_REGSTAT);
    port_outb(ATAPIO_PRIMARY_COMM_REGSTAT, 0xEC);
    port_outb(ATAPIO_PRIMARY_COMM_REGSTAT, 0xE7);

    // Read the status port. If it's zero, the drive does not exist.
    uint8_t status = port_inb(ATAPIO_PRIMARY_COMM_REGSTAT);

    while (status & ATAPIO_STAT_BSY) 
    {
        uint32_t i = 0;
        while(1) { i++; }
        for(i = 0; i < 0x0FFFFFFF; i++) { }
        status = port_inb(ATAPIO_PRIMARY_COMM_REGSTAT);
    }
    
    if (status == 0) { return false; }

    while (status & ATAPIO_STAT_BSY) { status = port_inb(ATAPIO_PRIMARY_COMM_REGSTAT); }

    uint8_t mid = port_inb(ATAPIO_PRIMARY_LBA_MID);
    uint8_t hi = port_inb(ATAPIO_PRIMARY_LBA_HI);
    if (mid || hi) { return false; }

    // Wait for ERR or DRQ
    while (!(status & (ATAPIO_STAT_ERR | ATAPIO_STAT_DRQ))) { status = port_inb(ATAPIO_PRIMARY_COMM_REGSTAT); }

    if (status & ATAPIO_STAT_ERR) { return false; }

    uint8_t buff[256 * 2];
    port_instr(ATAPIO_PRIMARY_DATA, buff, 256);
    return true;
}

void atapio_read(uint64_t sector, uint32_t count, uint8_t* buffer)
{
    port_outb(ATAPIO_PRIMARY_DRIVE_HEAD, 0x40);                   // Select master
    port_outb(ATAPIO_PRIMARY_SECCOUNT, (count >> 8) & 0xFF );     // sectorcount high
    port_outb(ATAPIO_PRIMARY_LBA_LO, (sector >> 24) & 0xFF);      // LBA4
    port_outb(ATAPIO_PRIMARY_LBA_MID, (sector >> 32) & 0xFF);     // LBA5
    port_outb(ATAPIO_PRIMARY_LBA_HI, (sector >> 40) & 0xFF);      // LBA6
    port_outb(ATAPIO_PRIMARY_SECCOUNT, count & 0xFF);             // sectorcount low
    port_outb(ATAPIO_PRIMARY_LBA_LO, sector & 0xFF);              // LBA1
    port_outb(ATAPIO_PRIMARY_LBA_MID, (sector >> 8) & 0xFF);      // LBA2
    port_outb(ATAPIO_PRIMARY_LBA_HI, (sector >> 16) & 0xFF);      // LBA3
    port_outb(ATAPIO_PRIMARY_COMM_REGSTAT, 0x24);                 // READ SECTORS EXT

    for (uint32_t i = 0; i < count; i++) 
    {
        while (true)
        {
            uint8_t status = port_inb(ATAPIO_PRIMARY_COMM_REGSTAT);
            if(status & ATAPIO_STAT_DRQ) { break; }
            if (status & ATAPIO_STAT_ERR) { debug_error("ATAPIO disk read error at sector %d", sector + i); return false; }
        }
        port_instr(ATAPIO_PRIMARY_DATA, (uint8_t*)buffer, 256);
        buffer += 256;
    }
    return true;
}

void atapio_write(uint64_t sector, uint32_t count, uint8_t* buffer)
{
    port_outb(ATAPIO_PRIMARY_DRIVE_HEAD, 0x40);                   // Select master
    port_outb(ATAPIO_PRIMARY_SECCOUNT, (count >> 8) & 0xFF );     // sectorcount high
    port_outb(ATAPIO_PRIMARY_LBA_LO, (sector >> 24) & 0xFF);      // LBA4
    port_outb(ATAPIO_PRIMARY_LBA_MID, (sector >> 32) & 0xFF);     // LBA5
    port_outb(ATAPIO_PRIMARY_LBA_HI, (sector >> 40) & 0xFF);      // LBA6
    port_outb(ATAPIO_PRIMARY_SECCOUNT, count & 0xFF);             // sectorcount low
    port_outb(ATAPIO_PRIMARY_LBA_LO, sector & 0xFF);              // LBA1
    port_outb(ATAPIO_PRIMARY_LBA_MID, (sector >> 8) & 0xFF);      // LBA2
    port_outb(ATAPIO_PRIMARY_LBA_HI, (sector >> 16) & 0xFF);      // LBA3
    port_outb(ATAPIO_PRIMARY_COMM_REGSTAT, 0x34);                 // READ SECTORS EXT

    for (uint8_t i = 0; i < count; i++) 
    {
        while (true) 
        {
            uint8_t status = port_inb(ATAPIO_PRIMARY_COMM_REGSTAT);
            if(status & ATAPIO_STAT_DRQ) { break; }
            else if(status & ATAPIO_STAT_ERR) { debug_error("ATAPIO disk read error at sector %d", sector + i); return false; }
        }
        port_outstr(ATAPIO_PRIMARY_DATA, (uint8_t*)buffer, 256);
        buffer += 256;
    }

    port_outb(ATAPIO_PRIMARY_COMM_REGSTAT, 0xE7);
    while (port_inb(ATAPIO_PRIMARY_COMM_REGSTAT) & ATAPIO_STAT_BSY);
    return true;
}