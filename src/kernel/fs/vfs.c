#include <kernel/fs/vfs.h>
#include <kernel/core/kernel.h>

const vfs_file_t NULL_VFS_FILE = { { 0 }, { 0 }, 0, 0, 0 };

void vfs_init(vfs_t* vfs, VFSTYPE type)
{
    if (vfs == NULL) { return; }
    vfs->type = type;

    bool valid = false;
    if (type == VFSTYPE_RAMFS)
    {
        multiboot_module_t* mod = (multiboot_module_t*)MBOOT_HDR->modules_addr;
        uint8_t* ramdisk = (uint8_t*)mod->address_start;
        ramfs_t* ramfs = kmalloc(sizeof(ramfs_t), HEAPTYPE_OBJECT);
        debug_info("RAMFS MOD - ADDR: 0x%8x-0x%8x, STR: '%s'", mod->address_start, mod->address_end, (char*)mod->string);
        ramfs_init(ramfs, ramdisk, mod->address_end - mod->address_start);
        vfs->fs = ramfs;
        valid = true;
    }
    else if (type == VFSTYPE_PMFS)
    {
        pmfs_mount();
        valid = true;
    }
    else { debug_error("Invalid file system type for vfs at 0x%8x", vfs); valid = false; }
    
    if (valid) { debug_ok("Initialized VFS - %s", vfs_type_string(vfs->type)); }
}

vfs_file_t vfs_file_read(char* filename)
{
    if (filename == NULL) { return NULL_VFS_FILE; }
    if (strlen(filename) < 3) { return NULL_VFS_FILE; }

    char  id   = vfs_drive_id_from_path(filename);
    char* path = vfs_path_remove_drive_id(filename);

    vdrive_t* drive = vdrivemgr_get_byid(id);
    if (drive == NULL) { free(path); return NULL_VFS_FILE; }
    
    if (drive->vfs_host->type == VFSTYPE_RAMFS)
    {
        vfs_file_t output = vfs_file_read_ramfs(drive->vfs_host, path);
        kfree(path);
        return output;
    }
    if (drive->vfs_host->type == VFSTYPE_PMFS)
    {
        vfs_file_t output = vfs_file_read_pmfs(drive->vfs_host, path);
        kfree(path);
        return output;
    }

    kfree(path);
    return NULL_VFS_FILE;
}

vfs_file_t vfs_file_read_pmfs(vfs_t* vfs, char* filename)
{
    pmfs_file_t file = pmfs_file_read(filename);
    if (file.data == NULL) { return NULL_VFS_FILE; }
    vfs_file_t output;
    output.name   = kmalloc(strlen(file.name), HEAPTYPE_STRING);
    output.parent = pmfs_get_parent_path_from_path(filename);
    output.data   = file.data;
    output.size   = file.size;
    output.status = VFSSTATUS_DEFAULT;
    strcpy(output.name, file.name);
    return output;
}

vfs_file_t vfs_file_read_ramfs(vfs_t* vfs, char* filename)
{
    ramfs_file_t ramfile = ramfs_read_file(vfs->fs, filename + 1);
    if (ramfile.data == NULL) { return NULL_VFS_FILE; }
    vfs_file_t output;
    output.name   = kmalloc(strlen(ramfile.name), HEAPTYPE_STRING);
    output.parent = NULL;
    output.data   = kmalloc(ramfile.size + 8, HEAPTYPE_ARRAY);
    output.size   = ramfile.size;
    output.status = VFSSTATUS_DEFAULT;
    memcpy(output.data, ramfile.data, ramfile.size + 8);
    strcpy(output.name, ramfile.name);
    return output;
}

vfs_file_t* vfs_get_files(char* path)
{

}

char vfs_drive_id_from_path(char* path)
{
    if (strlen(path) < 3) { return 0; }
    if (path[1] != ':')   { return 0; }
    if (path[2] != '/')   { return 0; }
    if (!vdrive_validate_id(path[0])) { return 0; }

    return path[0];
}

char* vfs_path_remove_drive_id(char* path)
{
    char* output = kmalloc(strlen(path), HEAPTYPE_STRING);
    strcpy(output, path + 2);
    return output;
}

const char* vfs_type_string(VFSTYPE type)
{
    switch (type)
    {
        case VFSTYPE_EXT2:  { return "EXT2  "; }
        case VFSTYPE_EXT4:  { return "EXT4  "; }
        case VFSTYPE_FAT32: { return "FAT32 "; }
        case VFSTYPE_PMFS:  { return "PMFS  "; }
        case VFSTYPE_RAMFS: { return "RAMFS "; }
        default:            { return "UNKNWN"; }
    }
}