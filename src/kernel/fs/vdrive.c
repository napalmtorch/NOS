#include <kernel/fs/vdrive.h>
#include <kernel/core/kernel.h>

int vdrivemgr_free_index();

vdrivemgr_t vdrivemgr;

bool vdrive_set_type(vdrive_t* drive, VDRIVETYPE type)
{
    if (drive == NULL) { return false; }
    switch (type)
    {
        case VDRIVETYPE_RAMDISK:
        {
            drive->type        = type;
            drive->initialized = true;
            return true;
        }
        case VDRIVETYPE_ATAPIO_DISK:
        {
            drive->type        = type;
            drive->initialized = true;
            return true;
        }
        default: 
        { 
            debug_error("Unknown virtual drive type 0x%2x", type); 
            drive->initialized = false;
            return false; 
        }
    }
}

vdrive_t* vdrive_init(char* label, char id, VDRIVETYPE type, vfs_t* vfs)
{
    vdrive_t* drive = kmalloc(sizeof(vdrive_t), HEAPTYPE_OBJECT);
    if (!vdrive_set_type(drive, type)) { kfree(drive); return NULL; }
    strcpy(drive->label, label);
    drive->id                    = id;
    drive->vfs_host              = vfs;
    drive->vfs_host->initialized = true;
    if (!vdrivemgr_register(drive)) { debug_error("Unable to register drive %c", id); return NULL; }
    debug_ok("Initialized virtual drive - ID: %c, TYPE: 0x%2x, FS_TYPE: %s, LABEL: '%s'", drive->id, type, vfs_type_string(vfs->type), label);
    return drive;
}

bool vdrive_validate_id(char id)
{
    if (!vdrivemgr.initialized) { return false; }
    if (id < 'A' || id > 'Z') { return false; }
    for (uint32_t i = 0; i < vdrivemgr.count_max; i++)
    {
        if (vdrivemgr.drives[i] == NULL) { continue; }  
        if (vdrivemgr.drives[i]->id == id) { return true; }
    }
    return false;
}

void vdrivemgr_init()
{
    if (vdrivemgr.initialized) { return; }
    memset(&vdrivemgr, 0, sizeof(vdrivemgr_t));

    vdrivemgr.count_max   = 256;
    vdrivemgr.count       = 0;
    vdrivemgr.drives      = kmalloc(sizeof(vdrive_t*) * vdrivemgr.count_max, HEAPTYPE_PTRARRAY);
    vdrivemgr.initialized = true;

    debug_ok("Initialized drive manager");
}

bool vdrivemgr_register(vdrive_t* drive)
{
    int i = vdrivemgr_free_index();
    if (i < 0 || i >= vdrivemgr.count_max) { debug_error("Maximum amount of virtual drives reached"); return false; }
    vdrivemgr.drives[i] = drive;
    vdrivemgr.count++;
    debug_info("Registered drive - ID: %c, LABEL: '%s'", drive->id, drive->label);
    return true;
}

bool vdrivemgr_unregister(vdrive_t* drive)
{
    return false;
}

vdrive_t* vdrivemgr_get_byid(char id)
{
    for (int i = 0; i < vdrivemgr.count_max; i++)
    {
        if (vdrivemgr.drives[i] == NULL) { continue; }
        if (vdrivemgr.drives[i]->id == id) { return vdrivemgr.drives[i]; }
    }
    return NULL;
}

int vdrivemgr_free_index()
{
    for (int i = 0; i < vdrivemgr.count_max; i++)
    {
        if (vdrivemgr.drives[i] == NULL) { return i; }
    }
    return -1;
}