#pragma once
#include <lib/stdint.h>
#include <kernel/fs/vfs.h>

#define ATAPIO_SECTOR_SIZE 512

typedef enum
{
    VDRIVETYPE_UNSPECIFIED,
    VDRIVETYPE_RAMDISK,
    VDRIVETYPE_ATAPIO_DISK,
    VDRIVETYPE_ATAPIO_OPTICAL,
    VDRIVETYPE_ATA_DISK,
    VDRIVETYPE_ATA_OPTICAL,
    VDRIVETYPE_SATA_DISK,
    VDRIVETYPE_SATA_OPTICAL,
} VDRIVETYPE;

typedef struct
{
    char           label[32];
    char           id;
    bool           initialized;
    vfs_t*         vfs_host;
    VDRIVETYPE     type;
} PACKED vdrive_t;

typedef struct
{
    uint32_t   count;
    uint32_t   count_max;
    vdrive_t** drives;
    bool       initialized;
} vdrivemgr_t;

vdrive_t* vdrive_init(char* label, char id, VDRIVETYPE type, vfs_t* vfs);
bool      vdrive_validate_id(char id);

void vdrivemgr_init();
bool vdrivemgr_register(vdrive_t* drive);
bool vdrivemgr_unregister(vdrive_t* drive);
vdrive_t* vdrivemgr_get_byid(char id);