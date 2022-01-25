#pragma once
#include <lib/stdint.h>
#include <lib/stdlib.h>
#include <lib/stdio.h>

#define FS_SECTOR_BOOT  0
#define FS_SECTOR_INFO  1
#define FS_SECTOR_BLKS  4

#define FSSTATE_FREE 0
#define FSSTATE_USED 1

#define FSTYPE_NULL 0
#define FSTYPE_DIR  1
#define FSTYPE_FILE 2

typedef struct
{
    uint32_t sector_count;
    uint16_t bytes_per_sector; 
    uint32_t blk_table_start;
    uint32_t blk_table_count;
    uint32_t blk_table_count_max;
    uint32_t blk_table_sector_count;
    uint32_t blk_data_start;
    uint32_t blk_data_sector_count;
    uint32_t blk_data_used;
    uint32_t file_table_start;
    uint32_t file_table_count;
    uint32_t file_table_count_max;
    uint32_t file_table_sector_count;
} PACKED pmfs_info_t;

typedef struct
{
    uint32_t start;
    uint32_t count;
    uint16_t state;
    uint8_t  padding[6];
} PACKED pmfs_blkentry_t;

typedef struct
{
    char     name[46];
    uint32_t parent_index;
    uint8_t  status;
    uint8_t  type;
    uint8_t  padding[12];
} PACKED pmfs_dir_t;

typedef struct
{
    char     name[46];
    uint32_t parent_index;
    uint8_t  status;
    uint8_t  type;
    uint32_t size;
    uint32_t blk_index;
    uint8_t* data;
} PACKED pmfs_file_t;

void pmfs_mount();
void pmfs_format(uint32_t size, bool wipe);
void pmfs_wipe(uint32_t size);

void pmfs_info_create(uint32_t size);
void pmfs_info_read();
void pmfs_info_write();
pmfs_info_t pmfs_get_info();

// block table
void            pmfs_blktable_print();
uint32_t        pmfs_blktable_sector_from_index(int index);
uint32_t        pmfs_blktable_offset_from_index(uint32_t sector, int index);
pmfs_blkentry_t   pmfs_blktable_read(int index);
void            pmfs_blktable_write(int index, pmfs_blkentry_t entry);
pmfs_blkentry_t   pmfs_blktable_allocate(uint32_t sectors);
bool            pmfs_blktable_free(pmfs_blkentry_t entry);
pmfs_blkentry_t   pmfs_blktable_nearest(pmfs_blkentry_t entry);
void            pmfs_blktable_merge_free();
bool            pmfs_blktable_copy(pmfs_blkentry_t dest, pmfs_blkentry_t src);
pmfs_blkentry_t   pmfs_blktable_create_entry(uint32_t start, uint32_t count, uint8_t state);
bool            pmfs_blktable_delete_entry(pmfs_blkentry_t entry);
bool            pmfs_blktable_fill(pmfs_blkentry_t entry, uint8_t value);
pmfs_blkentry_t   pmfs_blktable_at_index(int index);
bool            pmfs_blktable_validate_sector(uint32_t sector);
int             pmfs_blktable_get_index(pmfs_blkentry_t entry);
int             pmfs_blktable_freeindex();

uint32_t        pmfs_bytes_to_sectors(uint32_t bytes);

// file table
bool            pmfs_root_create(const char* label);
void            pmfs_filetable_print();
uint32_t        pmfs_filetable_sector_from_index(int index);
uint32_t        pmfs_filetable_offset_from_index(uint32_t sector, int index);
pmfs_dir_t  pmfs_filetable_read_dir(int index);
pmfs_file_t       pmfs_filetable_read_file(int index);
void            pmfs_filetable_write_dir(int index, pmfs_dir_t dir);
void            pmfs_filetable_write_file(int index, pmfs_file_t file);
pmfs_dir_t  pmfs_filetable_create_dir(pmfs_dir_t dir);
pmfs_file_t       pmfs_filetable_create_file(pmfs_file_t file);
bool            pmfs_filetable_delete_dir(pmfs_dir_t dir);
bool            pmfs_filetable_delete_file(pmfs_file_t file);
bool            pmfs_filetable_validate_sector(uint32_t sector);
int             pmfs_filetable_freeindex();
bool            pmfs_dir_equals(pmfs_dir_t a, pmfs_dir_t b);
bool            pmfs_file_equals(pmfs_file_t a, pmfs_file_t b);
pmfs_dir_t  pmfs_parent_from_path(const char* path);
pmfs_file_t       pmfs_get_file_byname(const char* path);
pmfs_dir_t  pmfs_get_dir_byname(const char* path);
int             pmfs_get_file_index(pmfs_file_t file);
int             pmfs_get_dir_index(pmfs_dir_t dir);
char*           pmfs_get_name_from_path(const char* path);
char*           pmfs_get_parent_path_from_path(const char* path);
pmfs_file_t       pmfs_file_create(const char* path, uint32_t size);
pmfs_file_t       pmfs_file_read(const char* path);
bool            pmfs_file_write(const char* path, uint8_t* data, uint32_t len);