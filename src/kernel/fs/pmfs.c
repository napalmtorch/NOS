#include <kernel/fs/pmfs.h>
#include <kernel/core/kernel.h>

// null structures
pmfs_blkentry_t  NULL_BLKENTRY = { 0, 0, 0, { 0, 0, 0, 0, 0, 0 } };
pmfs_dir_t       NULL_DIR      = { "", 0, 0, 0, { 0 } };
pmfs_file_t      NULL_FILE     = { "", 0, 0, 0, 0, 0, NULL };

// file system information
pmfs_info_t      fs_info;
pmfs_blkentry_t  fs_blk_mass;
pmfs_blkentry_t  fs_blk_files;
pmfs_dir_t fs_rootdir;

// mount file system from disk image
void pmfs_mount()
{
    pmfs_info_read();
    fs_blk_mass = pmfs_blktable_read(0);
    fs_blk_files = pmfs_blktable_read(1);
    fs_rootdir = pmfs_filetable_read_dir(0);
    debug_ok("Mounted PMFS file system");
}

// format disk of specified size to file system
void pmfs_format(uint32_t size, bool wipe)
{
    printf("Fomatting disk...\n");
    if (wipe) { pmfs_wipe(size); }

    // generate info block
    pmfs_info_create(size);
    pmfs_info_read();

    // create mass block entry
    fs_blk_mass.start = fs_info.blk_data_start;
    fs_blk_mass.count = fs_info.blk_data_sector_count;
    fs_blk_mass.state = FSSTATE_FREE;
    memset(fs_blk_mass.padding, 0, sizeof(fs_blk_mass.padding));
    pmfs_blktable_write(0, fs_blk_mass);
    fs_blk_mass = pmfs_blktable_read(0);
    printf("Created mass block: START: %d, STATE = 0x%02x, COUNT = %d\n", fs_blk_mass.start, fs_blk_mass.state, fs_blk_mass.count);

    // create files block entry and update info
    pmfs_info_read();
    fs_info.file_table_count_max = 32768;
    fs_info.file_table_count     = 0;
    fs_info.file_table_sector_count = (fs_info.file_table_count_max * sizeof(pmfs_file_t)) / ATAPIO_SECTOR_SIZE;
    pmfs_info_write();
    fs_blk_files = pmfs_blktable_allocate(fs_info.file_table_sector_count);
    fs_info.file_table_start = fs_blk_files.start;
    pmfs_info_write();

    // create root directory
    pmfs_root_create("VOS");

    // finished
    printf("Finished formatting disk\n");

}

// fill disk with zeros
void pmfs_wipe(uint32_t size)
{
    printf("Started wiping disk...\n");
    uint32_t sectors = size / ATAPIO_SECTOR_SIZE;
    uint8_t* data = malloc_c(ATAPIO_SECTOR_SIZE);
    memset(data, 0, ATAPIO_SECTOR_SIZE);

    for (uint64_t i = 0; i < sectors; i++) { atapio_write(i, 1, data); }

    printf("Finished wiping disk\n");
    free(data);
}

pmfs_info_t pmfs_get_info()
{
    pmfs_info_t output;
    memcpy(&output, &fs_info, sizeof(pmfs_info_t));
    return output;
}

// create new info block
void pmfs_info_create(uint32_t size)
{
    // disk info
    fs_info.sector_count        = size / ATAPIO_SECTOR_SIZE;
    fs_info.bytes_per_sector    = ATAPIO_SECTOR_SIZE;

    // block table
    fs_info.blk_table_start     = FS_SECTOR_BLKS;
    fs_info.blk_table_count     = 0;
    fs_info.blk_table_count_max = 16384;
    fs_info.blk_table_sector_count = (fs_info.blk_table_count_max * sizeof(pmfs_blkentry_t)) / ATAPIO_SECTOR_SIZE;

    // block data
    fs_info.blk_data_start = fs_info.blk_table_start + fs_info.blk_table_sector_count + 4;
    fs_info.blk_data_sector_count = fs_info.sector_count - (fs_info.blk_table_sector_count + 8);
    fs_info.blk_data_used = 0;

    // write to disk
    pmfs_info_write();

    // finished
    printf("Created new info block\n");
}

// read info block from disk
void pmfs_info_read()
{
    uint8_t* sec = malloc_c(ATAPIO_SECTOR_SIZE);
    atapio_read(FS_SECTOR_INFO, 1, sec);
    memcpy(&fs_info, sec, sizeof(pmfs_info_t));
    free(sec);
}

// write info block to disk
void pmfs_info_write()
{
    uint8_t* sec = malloc_c(ATAPIO_SECTOR_SIZE);
    memset(sec, 0, ATAPIO_SECTOR_SIZE);
    memcpy(sec, &fs_info, sizeof(pmfs_info_t));
    atapio_write(FS_SECTOR_INFO, 1, sec);
    free(sec);
}

void pmfs_blktable_print()
{
    printf("PRINTING BLOCK TABLE: \n");

    pmfs_info_read();
    uint8_t* data = malloc_c(ATAPIO_SECTOR_SIZE);
    int index = 0;
    for (uint32_t sec = 0; sec < fs_info.blk_table_sector_count; sec++)
    {
        atapio_read(fs_info.blk_table_start + sec, 1, data);

        for (uint32_t i = 0; i < ATAPIO_SECTOR_SIZE; i += sizeof(pmfs_blkentry_t))
        {
            pmfs_blkentry_t* entry = (pmfs_blkentry_t*)(data + i);
            if (entry->start == 0) { index++; continue; }
            printf("INDEX: 0x%08x START: 0x%08x COUNT: 0x%08x STATE: 0x%02x\n", index, entry->start, entry->count, entry->state);
            index++;
        }
    }

    printf("\n");
    free(data);
}

// get sector from block entry index
uint32_t pmfs_blktable_sector_from_index(int index)
{
    if (index == 0) { index = 1; }
    uint32_t sec = FS_SECTOR_BLKS;
    uint32_t offset_bytes = (index * sizeof(pmfs_blkentry_t));
    sec += (offset_bytes / ATAPIO_SECTOR_SIZE);
    return sec;
}

// get sector offset from sector and block entry index
uint32_t pmfs_blktable_offset_from_index(uint32_t sector, int index)
{
    uint32_t offset_bytes = (index * sizeof(pmfs_blkentry_t));
    uint32_t val = offset_bytes % 512;
    return val;
}

// read block entry from disk
pmfs_blkentry_t pmfs_blktable_read(int index)
{
    pmfs_info_read();
    if (index < 0 || index >= fs_info.blk_table_count_max) { printf("Invalid index while reading from block table\n"); return NULL_BLKENTRY; }
    uint32_t sector = pmfs_blktable_sector_from_index(index);
    uint32_t offset = pmfs_blktable_offset_from_index(sector, index);
    uint8_t* data = malloc_c(ATAPIO_SECTOR_SIZE);
    atapio_read(sector, 1, data);
    pmfs_blkentry_t* entry = (pmfs_blkentry_t*)(data + offset);
    pmfs_blkentry_t output = { entry->start, entry->count, entry->state, { 0 } };
    free(data);
    return output;
}

// write block entry to disk 
void pmfs_blktable_write(int index, pmfs_blkentry_t entry)
{
    pmfs_info_read();
    if (index < 0 || index >= fs_info.blk_table_count_max) { printf("Invalid index while reading from block table\n"); return; }
    uint32_t sector = pmfs_blktable_sector_from_index(index);
    uint32_t offset = pmfs_blktable_offset_from_index(sector, index);
    uint8_t* data = malloc_c(ATAPIO_SECTOR_SIZE);
    atapio_read(sector, 1, data);
    pmfs_blkentry_t* temp = (pmfs_blkentry_t*)(data + offset);
    temp->start         = entry.start;
    temp->count         = entry.count;
    temp->state         = entry.state;
    memcpy(temp->padding, entry.padding, sizeof(entry.padding));
    atapio_write(sector, 1, data);
    free(data);
}

// allocate new block entry
pmfs_blkentry_t pmfs_blktable_allocate(uint32_t sectors)
{
    if (sectors == 0) { return NULL_BLKENTRY; }

    pmfs_info_read();
    uint8_t* data = malloc_c(ATAPIO_SECTOR_SIZE);
    int index = 0;
    for (uint32_t sec = 0; sec < fs_info.blk_table_sector_count; sec++)
    {
        atapio_read(fs_info.blk_table_start + sec, 1, data);

        for (uint32_t i = 0; i < ATAPIO_SECTOR_SIZE; i += sizeof(pmfs_blkentry_t))
        {
            pmfs_blkentry_t* entry = (pmfs_blkentry_t*)(data + i);
            if (!pmfs_blktable_validate_sector(entry->start)) { index++; continue; }
            if (entry->count == sectors && entry->state == FSSTATE_FREE)
            {
                pmfs_blkentry_t output;
                entry->state = FSSTATE_USED;
                memcpy(&output, entry, sizeof(pmfs_blkentry_t));
                pmfs_blktable_write(index, output);
                printf("Allocated block: START: 0x%08x, STATE = 0x%02x, COUNT = 0x%08x\n", output.start, output.state, output.count);
                free(data);
                return output;
            }

            index++;
        }
    }

    atapio_read(fs_info.blk_table_start, 1, data);
    pmfs_blkentry_t* mass = (pmfs_blkentry_t*)data;

    mass->start += sectors;
    mass->count -= sectors;
    mass->state  = FSSTATE_FREE;
    atapio_write(fs_info.blk_table_start, 1, data);
    pmfs_blkentry_t output = pmfs_blktable_create_entry(mass->start - sectors, sectors, FSSTATE_USED);
    printf("Allocated block: START: 0x%08x, STATE = 0x%02x, COUNT = 0x%08x\n", output.start, output.state, output.count);
    free(data);
    return output;
}

// free existing block entry
bool pmfs_blktable_free(pmfs_blkentry_t entry)
{
    pmfs_info_read();
    uint8_t* data = malloc_c(ATAPIO_SECTOR_SIZE);
    int index = 0;
    for (uint32_t sec = 0; sec < fs_info.blk_table_sector_count; sec++)
    {
        atapio_read(fs_info.blk_table_start + sec, 1, data);

        for (uint32_t i = 0; i < ATAPIO_SECTOR_SIZE; i += sizeof(pmfs_blkentry_t))
        {
            pmfs_blkentry_t* temp = (pmfs_blkentry_t*)(data + i);
            if (temp->start == 0) { index++; continue; }
            if (temp->start == entry.start && temp->count == entry.count && temp->state == entry.state)
            {
                temp->state = FSSTATE_FREE;
                atapio_write(fs_info.blk_table_start + sec, 1, data);
                printf("Freed block: START: 0x%08x, STATE = 0x%02x, COUNT = 0x%08x\n", temp->start, temp->state, temp->count);
                pmfs_blktable_merge_free();
                return true;
            }
        }
        index++;
    }

    printf("Unable to free block START: %d, STATE = 0x%02x, COUNT = %d\n", entry.start, entry.state, entry.count);
    return false;
}

pmfs_blkentry_t pmfs_blktable_nearest(pmfs_blkentry_t entry)
{
    if (entry.start == 0) { return NULL_BLKENTRY; }
    if (entry.count == 0) { return NULL_BLKENTRY; }

    pmfs_info_read();
    uint8_t* data = malloc_c(ATAPIO_SECTOR_SIZE);
    int index = 0;
    for (uint32_t sec = 0; sec < fs_info.blk_table_sector_count; sec++)
    {
        atapio_read(fs_info.blk_table_start + sec, 1, data);

        for (uint32_t i = 0; i < ATAPIO_SECTOR_SIZE; i += sizeof(pmfs_blkentry_t))
        {
            if (index == 0) { index++; continue; }
            pmfs_blkentry_t* temp = (pmfs_blkentry_t*)(data + i);
            if (temp->start == 0 || temp->count == 0) { index++; continue; }

            if ((temp->start + temp->count == entry.start && entry.state == FSSTATE_FREE) ||
                (entry.start - entry.count == temp->start && entry.state == FSSTATE_FREE))
                {
                    pmfs_blkentry_t output;
                    memcpy(&output, temp, sizeof(pmfs_blkentry_t));
                    free(data);
                    return output;
                }

            index++;
        }
    }

    return NULL_BLKENTRY;
}

void pmfs_blktable_merge_free()
{
    pmfs_info_read();
    pmfs_blkentry_t mass = pmfs_blktable_nearest(pmfs_blktable_at_index(0));
    uint8_t* data = malloc_c(ATAPIO_SECTOR_SIZE);
    int index = 0;
    for (uint32_t sec = 0; sec < fs_info.blk_table_sector_count; sec++)
    {
        atapio_read(fs_info.blk_table_start + sec, 1, data);

        for (uint32_t i = 0; i < ATAPIO_SECTOR_SIZE; i += sizeof(pmfs_blkentry_t))
        {
            pmfs_blkentry_t* temp = (pmfs_blkentry_t*)(data + i);
            if (index == 0) { index++; continue; }
            if (temp->start == 0 || temp->count == 0) { index++; continue; }
            
            if (temp->state == FSSTATE_FREE)
            {
                pmfs_blkentry_t nearest = pmfs_blktable_nearest(*temp);
                if (nearest.start > 0 && nearest.count > 0 && nearest.start != temp->start && nearest.start != mass.start && nearest.state == FSSTATE_FREE)
                {
                    printf("TEMP: 0x%08x, NEAREST: 0x%08x\n", temp->start, nearest.start);
                    if (temp->start > nearest.start) { temp->start = nearest.start; }
                    temp->count += nearest.count;
                    atapio_write(fs_info.blk_table_start + sec, 1, data);
                    if (!pmfs_blktable_delete_entry(nearest)) { printf("Error deleting entry while merging\n"); return; }
                    atapio_read(fs_info.blk_table_start + sec, 1, data);
                }
            }
            index++;
        }
    }
    pmfs_info_write();

    mass = pmfs_blktable_at_index(0);

    pmfs_info_read();
    index = 0;
    for (uint32_t sec = 0; sec < fs_info.blk_table_sector_count; sec++)
    {
        atapio_read(fs_info.blk_table_start + sec, 1, data);

        for (uint32_t i = 0; i < ATAPIO_SECTOR_SIZE; i += sizeof(pmfs_blkentry_t))
        {
            pmfs_blkentry_t* temp = (pmfs_blkentry_t*)(data + i);
            if (index == 0) { index++; continue; }
            if (temp->start == 0 || temp->count == 0) { index++; continue; }

            if (temp->start + temp->count == mass.start && temp->state == FSSTATE_FREE)
            {
                printf("MASS: START = 0x%08x, COUNT = 0x%08x, STATE = 0x%02x\n", mass.start, mass.count, mass.state);
                printf("TEMP: START = 0x%08x, COUNT = 0x%08x, STATE = 0x%02x\n", temp->start, temp->count, temp->state);
                mass.start = temp->start;
                mass.count += temp->count;
                mass.state = FSSTATE_FREE;
                pmfs_blktable_write(0, mass);
                atapio_read(fs_info.blk_table_start + sec, 1, data);
                memset(temp, 0, sizeof(pmfs_blkentry_t));
                atapio_write(fs_info.blk_table_start + sec, 1, data);
                pmfs_info_write();
                break;
            }
            index++;
        }
    }

    free(data);
    pmfs_info_write();
}

bool pmfs_blktable_copy(pmfs_blkentry_t dest, pmfs_blkentry_t src)
{
    uint8_t* data = malloc_c(ATAPIO_SECTOR_SIZE);
    for (uint32_t sec = 0; sec < src.count; sec++)
    {
        atapio_read(src.start + sec, 1, data);
        atapio_write(dest.start + sec, 1, data);
    }
    free(data);

    return true;
}

// create new block entry in table
pmfs_blkentry_t pmfs_blktable_create_entry(uint32_t start, uint32_t count, uint8_t state)
{
    int i = pmfs_blktable_freeindex();
    if (i < 0 || i >= fs_info.blk_table_count_max) { printf("Maximum amount of block entries reached\n"); return NULL_BLKENTRY; }
    pmfs_blkentry_t entry;
    entry.start = start;
    entry.count = count;
    entry.state = state;
    memset(entry.padding, 0, sizeof(entry.padding));
    pmfs_blktable_write(i, entry);
    fs_info.blk_table_count++;
    pmfs_info_write();
    //printf("Created block: START: %d, STATE = 0x%02x, COUNT = %d\n", entry.start, entry.state, entry.count);
    return entry;
}

// delete existing block entry in table
bool pmfs_blktable_delete_entry(pmfs_blkentry_t entry)
{
    pmfs_info_read();
    uint8_t* data = malloc_c(ATAPIO_SECTOR_SIZE);
    for (uint32_t sec = 0; sec < fs_info.blk_table_sector_count; sec++)
    {
        atapio_read(fs_info.blk_table_start + sec, 1, data);

        for (uint32_t i = 0; i < ATAPIO_SECTOR_SIZE; i += sizeof(pmfs_blkentry_t))
        {
            pmfs_blkentry_t* temp = (pmfs_blkentry_t*)(data + i);
            if (temp->start == 0 || temp->count == 0) { continue; }

            if (temp->start == entry.start && temp->count == entry.count && temp->state == entry.state)
            {
                printf("Delete block: START: 0x%08x, STATE = 0x%02x, COUNT = 0x%08x\n", entry.start, entry.state, entry.count);
                memset(temp, 0, sizeof(pmfs_blkentry_t));
                atapio_write(fs_info.blk_table_start + sec, 1, data);
                fs_info.blk_table_count--;
                pmfs_info_write();
                return true;
            }
        }
    }

    printf("Unable to delete block\n");
    return false;
}

bool pmfs_blktable_fill(pmfs_blkentry_t entry, uint8_t value)
{
    uint8_t* data = malloc_c(ATAPIO_SECTOR_SIZE);
    for (uint32_t sec = 0; sec < fs_info.blk_table_sector_count; sec++)
    {
        atapio_read(fs_info.blk_table_start + sec, 1, data);

        for (uint32_t i = 0; i < ATAPIO_SECTOR_SIZE; i += sizeof(pmfs_blkentry_t))
        {
            pmfs_blkentry_t* temp = (pmfs_blkentry_t*)(data + i);
            if (temp->start == 0 || temp->count == 0) { continue; }

            if (temp->start == entry.start && temp->count == entry.count && temp->state == entry.state)
            {
                atapio_write(temp->start, temp->count, value);
                free(data);
                return true;
            }
        }
    }

    printf("Unable to fill block entry\n");
    free(data);
    return false;
}

pmfs_blkentry_t pmfs_blktable_at_index(int index)
{
    if (index < 0 || index >+ fs_info.blk_table_count_max) { return NULL_BLKENTRY; }

    pmfs_info_read();
    uint8_t* data = malloc_c(ATAPIO_SECTOR_SIZE);
    int temp_index = 0;
    for (uint32_t sec = 0; sec < fs_info.blk_table_sector_count; sec++)
    {
        atapio_read(fs_info.blk_table_start + sec, 1, data);

        for (uint32_t i = 0; i < ATAPIO_SECTOR_SIZE; i += sizeof(pmfs_blkentry_t))
        {
            pmfs_blkentry_t* temp = (pmfs_blkentry_t*)(data + i);
            if (temp_index == index) 
            { 
                pmfs_blkentry_t output;
                memcpy(&output, temp, sizeof(pmfs_blkentry_t));
                free(data);
                return output; 
            }
            temp_index++;
        }
    }

    printf("Unable to get block entry at index %d\n", index);
    free(data);
    return NULL_BLKENTRY;
}

// validate that sector is within block table boundaries
bool pmfs_blktable_validate_sector(uint32_t sector)
{
    if (sector < fs_info.blk_table_start || sector >= fs_info.blk_table_sector_count) { return false; }
    return true;
}

// get index of specified block entry
int pmfs_blktable_get_index(pmfs_blkentry_t entry)
{
    pmfs_info_read();
    uint8_t* data = malloc_c(ATAPIO_SECTOR_SIZE);
    int index = 0;
    for (uint32_t sec = 0; sec < fs_info.blk_table_sector_count; sec++)
    {
        atapio_read(fs_info.blk_table_start + sec, 1, data);

        for (uint32_t i = 0; i < ATAPIO_SECTOR_SIZE; i += sizeof(pmfs_blkentry_t))
        {
            pmfs_blkentry_t* temp = (pmfs_blkentry_t*)(data + i);
            if (temp->start == 0 || temp->count == 0) { continue; }
            if (temp->start == entry.start && temp->count == entry.count && temp->state == entry.state) { free(data); return index; }
            index++;
        }
    }
    free(data);
    return -1;
}

// get next available block entry index in table
int pmfs_blktable_freeindex()
{
    pmfs_info_read();
    uint8_t* data = malloc_c(ATAPIO_SECTOR_SIZE);
    int index = 0;
    for (uint32_t sec = 0; sec < fs_info.blk_table_sector_count; sec++)
    {
        atapio_read(fs_info.blk_table_start + sec, 1, data);

        for (uint32_t i = 0; i < ATAPIO_SECTOR_SIZE; i += sizeof(pmfs_blkentry_t))
        {
            pmfs_blkentry_t* entry = (pmfs_blkentry_t*)(data + i);
            if (entry->start == 0 && entry->count == 0 && entry->state == 0) { free(data); return index; }
            index++;
        }
    }
    free(data); 
    return -1;
}

// create new root directory
bool pmfs_root_create(const char* label)
{
    strcpy(fs_rootdir.name, label);
    fs_rootdir.status = 0xFF;
    fs_rootdir.parent_index = UINT32_MAX;
    fs_rootdir.type = FSTYPE_DIR;
    memset(fs_rootdir.padding, 0, sizeof(fs_rootdir.padding));
    pmfs_filetable_write_dir(0, fs_rootdir);

    pmfs_dir_t rfd = pmfs_filetable_read_dir(0);
    printf("Created root: NAME = %s, PARENT = 0x%08x, TYPE = 0x%02x, STATUS = 0x%02x\n", rfd.name, rfd.parent_index, rfd.type, rfd.status);
    return true;
}

void pmfs_filetable_print()
{
    printf("------ FILE TABLE ----------------------------\n");

    pmfs_info_read();
    uint8_t* data = malloc_c(ATAPIO_SECTOR_SIZE);
    int index = 0;
    for (uint32_t sec = 0; sec < fs_info.file_table_sector_count; sec++)
    {
        atapio_read(fs_info.file_table_start + sec, 1, data);

        for (uint32_t i = 0; i < ATAPIO_SECTOR_SIZE; i += sizeof(pmfs_dir_t))
        {
            pmfs_dir_t* entry = (pmfs_dir_t*)(data + i);

            if (entry->type != FSTYPE_NULL)
            {
                printf("INDEX: 0x%08x PARENT: 0x%08x TYPE: 0x%02x STATUS: 0x%02x NAME: %s\n", index, entry->parent_index, entry->type, entry->status, entry->name);
            }
            index++;
        }
    }

    printf("\n");
    free(data); 
}

// convert file index to file entry sector
uint32_t pmfs_filetable_sector_from_index(int index)
{
    if (index == 0) { index = 1; }
    pmfs_info_read();
    uint32_t sec = fs_info.file_table_start;
    uint32_t offset_bytes = (index * sizeof(pmfs_file_t));
    sec += (offset_bytes / ATAPIO_SECTOR_SIZE);
    return sec;
}

// convert file index and sector to file entry sector offset
uint32_t pmfs_filetable_offset_from_index(uint32_t sector, int index)
{
    uint32_t offset_bytes = (index * sizeof(pmfs_file_t));
    uint32_t val = offset_bytes % 512;
    return val;
}

// read directory from disk at index in table
pmfs_dir_t pmfs_filetable_read_dir(int index)
{
    pmfs_info_read();
    if (index < 0 || index >= fs_info.file_table_count_max) { printf("Invalid index while reading directory entry\n"); return NULL_DIR; }
    uint32_t sector = pmfs_filetable_sector_from_index(index);
    uint32_t offset = pmfs_filetable_offset_from_index(sector, index);
    uint8_t* data = malloc_c(ATAPIO_SECTOR_SIZE);
    atapio_read(sector, 1, data);
    pmfs_dir_t* entry = (pmfs_dir_t*)(data + offset);
    pmfs_dir_t output;
    memcpy(&output, entry, sizeof(pmfs_dir_t));
    free(data);
    return output;
}

// read file form disk at index in table
pmfs_file_t pmfs_filetable_read_file(int index)
{
    pmfs_info_read();
    if (index < 0 || index >= fs_info.file_table_count_max) { printf("Invalid index while reading file entry\n"); return NULL_FILE; }
    uint32_t sector = pmfs_filetable_sector_from_index(index);
    uint32_t offset = pmfs_filetable_offset_from_index(sector, index);
    uint8_t* data = malloc_c(ATAPIO_SECTOR_SIZE);
    atapio_read(sector, 1, data);
    pmfs_file_t* entry = (pmfs_file_t*)(data + offset);
    pmfs_file_t output;
    memcpy(&output, entry, sizeof(pmfs_file_t));
    free(data); 
    return output;
}

// write directory to disk at index in table
void pmfs_filetable_write_dir(int index, pmfs_dir_t dir)
{
    pmfs_info_read();
    if (index < 0 || index >= fs_info.file_table_count_max) { printf("Invalid index while writing directory entry\n"); return; }
    uint32_t sector = pmfs_filetable_sector_from_index(index);
    uint32_t offset = pmfs_filetable_offset_from_index(sector, index);
    uint8_t* data = malloc_c(ATAPIO_SECTOR_SIZE);
    atapio_read(sector, 1, data);
    pmfs_dir_t* temp = (pmfs_dir_t*)(data + offset);
    memcpy(temp, &dir, sizeof(pmfs_dir_t));
    atapio_write(sector, 1, data);
    free(data);
}

// write file to disk at index in table
void pmfs_filetable_write_file(int index, pmfs_file_t file)
{
    pmfs_info_read();
    if (index < 0 || index >= fs_info.file_table_count_max) { printf("Invalid index while writing file entry: %d\n", index); return; }
    uint32_t sector = pmfs_filetable_sector_from_index(index);
    uint32_t offset = pmfs_filetable_offset_from_index(sector, index);
    uint8_t* data = malloc_c(ATAPIO_SECTOR_SIZE);
    atapio_read(sector, 1, data);
    pmfs_file_t* temp = (pmfs_file_t*)(data + offset);
    memcpy(temp, &file, sizeof(pmfs_file_t));
    atapio_write(sector, 1, data);
    free(data); 
}

// create new directory entry in table
pmfs_dir_t pmfs_filetable_create_dir(pmfs_dir_t dir)
{
    int i = pmfs_filetable_freeindex();
    if (i < 0 || i >= fs_info.file_table_count_max) { printf("Invalid index while creating directory entry\n"); return NULL_DIR; }
    pmfs_filetable_write_dir(i, dir);
    fs_info.file_table_count++;
    pmfs_info_write();
    printf("Created directory: INDEX = 0x%08x, NAME = %s, PARENT = 0x%08x, TYPE = 0x%02x, STATUS = 0x%02x\n", i, dir.name, dir.parent_index, dir.type, dir.status);
    return dir;
}

// create new file entry in table
pmfs_file_t pmfs_filetable_create_file(pmfs_file_t file)
{
    int i = pmfs_filetable_freeindex();
    if (i < 0 || i >= fs_info.file_table_count_max) { printf("Invalid index while creating file entry\n"); return NULL_FILE; }
    pmfs_filetable_write_file(i, file);
    fs_info.file_table_count++;
    pmfs_info_write();
    printf("Created file: NAME = %s, PARENT = 0x%08x, TYPE = 0x%02x, STATUS = 0x%02x, BLK = 0x%08x, SIZE = %d\n", file.name, file.parent_index, file.type, file.status, file.blk_index, file.size);
    return file;
}

// delete existing directory entry in table
bool pmfs_filetable_delete_dir(pmfs_dir_t dir)
{
    pmfs_info_read();
    uint8_t* data = malloc_c(ATAPIO_SECTOR_SIZE);
    int index = 0;
    for (uint32_t sec = 0; sec < fs_info.file_table_sector_count; sec++)
    {
        atapio_read(fs_info.file_table_start + sec, 1, data);

        for (uint32_t i = 0; i < ATAPIO_SECTOR_SIZE; i += sizeof(pmfs_dir_t))
        {
            pmfs_dir_t* entry = (pmfs_dir_t*)(data + i);

            if (pmfs_dir_equals(dir, *entry))
            {
                printf("Deleted directory: NAME = %s, PARENT = 0x%08x, TYPE = 0x%02x, STATUS = 0x%02x\n", dir.name, dir.parent_index, dir.type, dir.status);
                memset(entry, 0, sizeof(pmfs_dir_t));
                atapio_write(fs_info.file_table_start + sec, 1, data);
                free(data); 
                return true;
            }
            index++;
        }
    }

    printf("Unable to delete directory\n");
    free(data); 
    return false;
}

// delete existing file entry in table
bool pmfs_filetable_delete_file(pmfs_file_t file)
{
    pmfs_info_read();
    uint8_t* data = malloc_c(ATAPIO_SECTOR_SIZE);
    int index = 0;
    for (uint32_t sec = 0; sec < fs_info.file_table_sector_count; sec++)
    {
        atapio_read(fs_info.file_table_start + sec, 1, data);

        for (uint32_t i = 0; i < ATAPIO_SECTOR_SIZE; i += sizeof(pmfs_file_t))
        {
            pmfs_file_t* entry = (pmfs_file_t*)(data + i);

            if (pmfs_file_equals(file, *entry))
            {
                printf("Deleted file: NAME = %s, PARENT = 0x%08x, TYPE = 0x%02x, STATUS = 0x%02x, SIZE = %d\n", file.name, file.parent_index, file.type, file.status, file.size);
                memset(entry, 0, sizeof(pmfs_file_t));
                atapio_write(fs_info.file_table_start + sec, 1, data);
                free(data); 
                return true;
            }

            index++;
        }
    }

    printf("Unable to delete directory\n");
    free(data); 
    return false;
}

// validate that sector is within file table bounds
bool pmfs_filetable_validate_sector(uint32_t sector)
{
    if (sector < fs_info.file_table_start || sector >= fs_info.file_table_start + fs_info.file_table_sector_count) { return false; }
    return true;
}

// get next available index in file table
int pmfs_filetable_freeindex()
{
    pmfs_info_read();
    uint8_t* data = malloc_c(ATAPIO_SECTOR_SIZE);
    int index = 0;
    for (uint32_t sec = 0; sec < fs_info.file_table_sector_count; sec++)
    {
        atapio_read(fs_info.file_table_start + sec, 1, data);

        for (uint32_t i = 0; i < ATAPIO_SECTOR_SIZE; i += sizeof(pmfs_file_t))
        {
            pmfs_file_t* entry = (pmfs_file_t*)(data + i);
            if (entry->type == FSTYPE_NULL) {free(data);  return index; }
            index++;
        }
    }

    free(data); 
    return -1;
}

// check if 2 directories are equal
bool pmfs_dir_equals(pmfs_dir_t a, pmfs_dir_t b)
{
    if (strcmp(a.name, b.name)) { return false; }
    if (a.parent_index != b.parent_index) { return false; }
    if (a.status != b.status) { return false; }
    if (a.type != b.type) { return false; }
    return true;
}

// check if 2 files are equal
bool pmfs_file_equals(pmfs_file_t a, pmfs_file_t b)
{
    if (strcmp(a.name, b.name)) { return false; }
    if (a.parent_index != b.parent_index) { return false; }
    if (a.status != b.status) { return false; }
    if (a.type != b.type) { return false; }
    if (a.blk_index != b.blk_index) { return false; }
    if (a.size != b.size) { return false; }
    return true;
}

// get parent directory from path - returns empty if unable to locate
pmfs_dir_t pmfs_parent_from_path(const char* path)
{
    if (path == NULL) { return NULL_DIR; }
    if (strlen(path) == 0) { return NULL_DIR; }

    int    args_count = 0;
    char** args = strsplit(path, '/', &args_count);

    if (args_count <= 2 && path[0] == '/') { freearray(args, args_count); return fs_rootdir; }

    if (args_count > 1)
    {
        pmfs_info_read();
        int32_t index = 0;
        uint32_t p = 0;
        pmfs_dir_t dir_out = NULL_DIR;

        for (uint32_t arg = 0; arg < args_count - 1; arg++)
        {
            dir_out = NULL_DIR;
            index = 0;

            if (args[arg] != NULL && strlen(args[arg]) > 0)
            {
                uint8_t* data = malloc_c(ATAPIO_SECTOR_SIZE);
                for (uint32_t sec = 0; sec < fs_info.file_table_sector_count; sec++)
                {
                    atapio_read(fs_info.file_table_start + sec, 1, data);

                    for (uint32_t i = 0; i < ATAPIO_SECTOR_SIZE; i += sizeof(pmfs_dir_t))
                    {
                        pmfs_dir_t* dir = (pmfs_dir_t*)(data + i);

                        if (dir->type == FSTYPE_DIR && dir->parent_index == (uint32_t)p && !strcmp(dir->name, args[arg]))
                        {
                            p = index;
                            dir_out = *dir;
                        }

                        index++;
                    }
                }
                free(data);
            }
        }

        if (dir_out.type == FSTYPE_DIR) { freearray(args, args_count); return dir_out; }
    }

    printf("Unable to locate parent of %s\n", path);
    freearray(args, args_count);
    return NULL_DIR;
}

// return file by path - returns empty if unable to locate
pmfs_file_t pmfs_get_file_byname(const char* path)
{
    if (path == NULL) { return NULL_FILE; }
    if (strlen(path) == 0) { return NULL_FILE; }

    pmfs_info_read();

    pmfs_dir_t parent = pmfs_parent_from_path(path);
    if (parent.type != FSTYPE_DIR) { printf("Parent was null while getting file by name\n"); return NULL_FILE; }

    int args_count = 0;
    char** args = strsplit(path, '/', &args_count);

    if (args_count == 0) { freearray(args, args_count); printf("Args was null while getting file by name\n"); return NULL_FILE; }

    char* filename;
    int xx = args_count - 1;
    while (args[xx] != NULL)
    {
        if (args[xx] != NULL && strlen(args[xx]) > 0) { filename = args[xx]; break; }
        if (xx == 0) { break; }
        xx--;
    }
    if (strlen(filename) == 0 || filename == NULL) { freearray(args, args_count); printf("Unable to get name while getting file by name\n"); return NULL_FILE; }

    uint32_t parent_index = pmfs_get_dir_index(parent);

    uint8_t* data = malloc_c(ATAPIO_SECTOR_SIZE);
    int index = 0;
    for (uint32_t sec = 0; sec < fs_info.file_table_sector_count; sec++)
    {
        atapio_read(fs_info.file_table_start + sec, 1, data);

        for (uint32_t i = 0; i < ATAPIO_SECTOR_SIZE; i += sizeof(pmfs_file_t))
        {
            pmfs_file_t* entry = (pmfs_file_t*)(data + i);
            if (entry->type == FSTYPE_FILE && entry->parent_index == parent_index && !strcmp(entry->name, filename))
            {
                pmfs_file_t output = *entry;
                freearray(args, args_count);
                free(data);
                return output;
            }
            index++;
        }
    }

    freearray(args, args_count);
    free(data);
    return NULL_FILE;
}

// return directory by path - returns empty if unable to locate;
pmfs_dir_t pmfs_get_dir_byname(const char* path)
{
    if (path == NULL) { return NULL_DIR; }
    if (strlen(path) == 0) { return NULL_DIR; }

    pmfs_info_read();

    if (!strcmp(path, "/"))
    {
        pmfs_dir_t output;
        memcpy(&output, &fs_rootdir, sizeof(pmfs_dir_t));
        return output;
    }

    pmfs_dir_t parent = pmfs_parent_from_path(path);
    if (parent.type != FSTYPE_DIR) { printf("Parent was null while getting directory by name\n"); return NULL_DIR; }

    int args_count = 0;
    char** args = strsplit(path, '/', &args_count);

    if (args_count == 0) { freearray(args, args_count); printf("Args was null while getting directory by name\n"); return NULL_DIR; }

    char* dirname;
    int xx = args_count - 1;
    while (args[xx] != NULL)
    {
        if (args[xx] != NULL && strlen(args[xx]) > 0) { dirname = args[xx]; break; }
        if (xx == 0) { break; }
        xx--;
    }
    if (strlen(dirname) == 0 || dirname == NULL) { printf("Unable to get name while getting directory by name\n"); return NULL_DIR; }

    uint32_t parent_index = pmfs_get_dir_index(parent);

    uint8_t* data = malloc_c(ATAPIO_SECTOR_SIZE);
    int index = 0;
    for (uint32_t sec = 0; sec < fs_info.file_table_sector_count; sec++)
    {
        atapio_read(fs_info.file_table_start + sec, 1, data);

        for (uint32_t i = 0; i < ATAPIO_SECTOR_SIZE; i += sizeof(pmfs_dir_t))
        {
            pmfs_dir_t* entry = (pmfs_dir_t*)(data + i);
            if (entry->type == FSTYPE_DIR && entry->parent_index == parent_index && !strcmp(entry->name, dirname))
            {
                pmfs_dir_t output;
                memcpy(&output, entry, sizeof(pmfs_dir_t));
                freearray(args, args_count);
                free(data);
                return output;
            }
            index++;
        }
    }

    freearray(args, args_count);
    free(data);
    return NULL_DIR;
}

// get index of specified file entry
int pmfs_get_file_index(pmfs_file_t file)
{
    pmfs_info_read();
    uint8_t* data = malloc_c(ATAPIO_SECTOR_SIZE);
    int index = 0;
    for (uint32_t sec = 0; sec < fs_info.file_table_sector_count; sec++)
    {
        atapio_read(fs_info.file_table_start + sec, 1, data);

        for (uint32_t i = 0; i < ATAPIO_SECTOR_SIZE; i += sizeof(pmfs_file_t))
        {
            pmfs_file_t* entry = (pmfs_file_t*)(data + i);
            if (pmfs_file_equals(*entry, file)) { free(data); return index; }
            index++;
        }
    }
    free(data);
    return -1;
}

// get index of specified directory entry
int pmfs_get_dir_index(pmfs_dir_t dir)
{
    pmfs_info_read();
    uint8_t* data = malloc_c(ATAPIO_SECTOR_SIZE);
    int index = 0;
    for (uint32_t sec = 0; sec < fs_info.file_table_sector_count; sec++)
    {
        atapio_read(fs_info.file_table_start + sec, 1, data);

        for (uint32_t i = 0; i < ATAPIO_SECTOR_SIZE; i += sizeof(pmfs_dir_t))
        {
            pmfs_dir_t* entry = (pmfs_dir_t*)(data + i);
            if (pmfs_dir_equals(*entry, dir)) { free(data); return index; }
            index++;
        }
    }
    free(data);
    return -1;
}

int ceilnum(float num) 
{
    int inum = (int)num;
    if (num == (float)inum) { return inum; }
    return inum + 1;
}

uint32_t pmfs_bytes_to_sectors(uint32_t bytes)
{
    return (uint32_t)ceilnum((float)bytes / (float)ATAPIO_SECTOR_SIZE);
}

char* pmfs_get_name_from_path(const char* path)
{
    if (path == NULL) { return NULL; }
    if (strlen(path) == 0) { return NULL; }

    if (!strcmp(path, "/"))
    {
        char* rootname = malloc_c(strlen(fs_rootdir.name) + 1);
        memset(rootname, 0, strlen(fs_rootdir.name));
        strcpy(rootname, fs_rootdir.name);
        return rootname;
    }

    int args_count = 0;
    char** args = strsplit(path, '/', &args_count);

    if (args_count == 0) { freearray(args, args_count); printf("Args was null while getting name from path\n"); return NULL; }

    if (args_count == 2 && path[0] == '/')
    {
        char* rootname = malloc_c(strlen(args[1]) + 1);
        memset(rootname, 0, strlen(args[1]) + 1);
        strcpy(rootname, args[1]);
        freearray(args, args_count);
        return rootname;
    }

    char* filename;
    int xx = args_count - 1;
    while (args[xx] != NULL)
    {
        if (args[xx] != NULL && strlen(args[xx]) > 0) { filename = args[xx]; break; }
        if (xx == 0) { break; }
        xx--;
    }

    char* output = malloc_c(strlen(filename) + 1);
    memset(output, 0, strlen(filename));
    strcpy(output, filename);
    freearray(args, args_count);
    return output;
}

char* pmfs_get_parent_path_from_path(const char* path)
{
    if (path == NULL) { return NULL; }
    if (strlen(path) == 0) { return NULL; }

    char* output = malloc_c(strlen(path + 16));
    memset(output, 0, strlen(path + 16));

    if (!strcmp(path, "/")) { free(output); return NULL; }

    int args_count = 0;
    char** args = strsplit(path, '/', &args_count);   

    if (args_count <= 2 && path[0] == '/') { freearray(args, args_count); strcpy(output, "/"); return output; }

    strcat(output, "/");
    for (int i = 0; i < args_count - 1; i++)
    {
        if (strlen(args[i]) == 0) { continue; }
        strcat(output, args[i]);
        strcat(output, "/");
    }
    
    freearray(args, args_count);
    return output;
}

pmfs_file_t pmfs_file_create(const char* path, uint32_t size)
{
    if (size == 0) { printf("Cannot create blank file\n"); return NULL_FILE; }

    pmfs_dir_t parent = pmfs_parent_from_path(path);
    if (parent.type != FSTYPE_DIR) { printf("Unable to locate parent while creating file\n"); return NULL_FILE; }

    uint32_t sectors = pmfs_bytes_to_sectors(size);
    pmfs_blkentry_t blk = pmfs_blktable_allocate(sectors);

    // set properties and create file
    pmfs_file_t file;
    file.parent_index = pmfs_get_dir_index(parent);
    file.type         = FSTYPE_FILE;
    file.status       = 0x00;
    file.size         = size;
    file.blk_index    = pmfs_blktable_get_index(blk);
    char* name = pmfs_get_name_from_path(path);
    strcpy(file.name, name);
    free(name);

    pmfs_blktable_fill(blk, 0x00);

    pmfs_file_t new_file = pmfs_filetable_create_file(file);
    return new_file;
}

pmfs_file_t pmfs_file_read(const char* path)
{
    if (path == NULL) { printf("Path was null while trying to read file %s\n", path); return NULL_FILE; }
    if (strlen(path) == 0) { printf("Path was empty while trying to read file %s\n", path); return NULL_FILE; }

    pmfs_file_t file = pmfs_get_file_byname(path);
    if (file.type != FSTYPE_FILE) { printf("Unable to locate file %s\n", path); return NULL_FILE; }

    pmfs_blkentry_t blk = pmfs_blktable_read(file.blk_index);

    uint8_t* data = malloc_c(blk.count * ATAPIO_SECTOR_SIZE);
    for (uint32_t i = 0; i < blk.count; i++) { atapio_read(blk.start + i, 1, data + (i * ATAPIO_SECTOR_SIZE)); }
    file.data = data;
    debug_info("Successfully read PMFS file '%s'", path);
    return file;
}

bool pmfs_file_write(const char* path, uint8_t* data, uint32_t len)
{
    if (path == NULL) { printf("Path was null while trying to write file %s\n", path); return false; }
    if (strlen(path) == 0) { printf("Path was empty while trying to write file %s\n", path); return false; }

    pmfs_file_t tryload = pmfs_get_file_byname(path);
    if (tryload.type != FSTYPE_FILE) 
    { 
        printf("File %s does not exist and will be created\n", path);
        pmfs_file_t new_file = pmfs_file_create(path, len);
        if (new_file.type != FSTYPE_FILE) { printf("Unable to create new file %s\n", path); return false; }
        pmfs_blkentry_t blk = pmfs_blktable_read(new_file.blk_index);

        uint8_t* secdata = malloc_c(ATAPIO_SECTOR_SIZE);
        for (uint32_t i = 0; i < blk.count; i++)
        {
            memset(secdata, 0, ATAPIO_SECTOR_SIZE);
            for (uint32_t j = 0; j < ATAPIO_SECTOR_SIZE; j++)
            {
                uint32_t offset = (i * ATAPIO_SECTOR_SIZE) + j;
                if (offset < len) { secdata[j] = data[offset]; } else { break; }
            }       
            atapio_write(blk.start + i, 1, secdata);
        }

        printf("Written file %s to disk, size = %d\n", path, new_file.size);
        free(secdata);
        return true;
    }
    else 
    { 
        printf("File %s exists\n", path); 
        int findex = pmfs_get_file_index(tryload);
        pmfs_blkentry_t blk = pmfs_blktable_read(tryload.blk_index);
        pmfs_blktable_free(blk);
        blk = pmfs_blktable_allocate(pmfs_bytes_to_sectors(len));
        tryload.blk_index = pmfs_blktable_get_index(blk);
        tryload.size = len;
        pmfs_filetable_write_file(findex, tryload);

        uint8_t* secdata = malloc_c(ATAPIO_SECTOR_SIZE);
        for (uint32_t i = 0; i < blk.count; i++)
        {
            memset(secdata, 0, ATAPIO_SECTOR_SIZE);
            for (uint32_t j = 0; j < ATAPIO_SECTOR_SIZE; j++)
            {
                uint32_t offset = (i * ATAPIO_SECTOR_SIZE) + j;
                if (offset < len) { secdata[j] = data[offset]; } else { break; }
            }       
            atapio_write(blk.start + i, 1, secdata);
        }  

        printf("Written file %s to disk, size = %d\n", path, tryload.size);
        return true;      
    }
    return true;
}