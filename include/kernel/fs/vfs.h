#pragma once
#include <lib/stdint.h>

typedef enum
{
    VFSTYPE_UNKNOWN,
    VFSTYPE_RAMFS,
    VFSTYPE_PMFS,
    VFSTYPE_FAT32,      // fml
    VFSTYPE_EXT2,       // lol maybe
    VFSTYPE_EXT4,       // LOLOLOL maybe?
} VFSTYPE;

typedef enum
{
    VFSSTATUS_DEFAULT,
    VFSSTATUS_ROOT,
} VFSSTATUS;

typedef struct
{
    void*   fs;
    VFSTYPE type;
    bool    initialized;
} PACKED vfs_t;

typedef struct
{
    char*     name;
    char*     parent;
    VFSSTATUS status;
    uint32_t  size;
    uint32_t  sub_dirs;
    uint32_t  sub_files;
} PACKED vfs_dir_t;

typedef struct
{
    char*     name;
    char*     parent;
    VFSSTATUS status;
    uint32_t  size;
    uint8_t*  data;
} PACKED vfs_file_t;

void vfs_init(vfs_t* vfs, VFSTYPE type);
vfs_file_t vfs_file_read(char* filename);
vfs_file_t vfs_file_read_pmfs(vfs_t* vfs, char* filename);
vfs_file_t vfs_file_read_ramfs(vfs_t* vfs, char* filename);

vfs_file_t* vfs_get_files(char* path);

char  vfs_drive_id_from_path(char* path);
char* vfs_path_remove_drive_id(char* path);

const char* vfs_type_string(VFSTYPE type);