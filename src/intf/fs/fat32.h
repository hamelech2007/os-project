#pragma once
#include "stdint.h"
#include "ahci.h"

#define FAT32_FSINFO_LEAD_SIGNATURE 0x41615252
#define FAT32_FSINFO_SIGNATURE 0x61417272
#define FAT32_FSINFO_TRAIL_SIGNATURE 0xAA550000

#define BYTES_PER_SECTOR    512
#define SECTORS_PER_CLUSTER 8

#define FAT32_EXT_BOOT_SIG_1 0x28
#define FAT32_EXT_BOOT_SIG_2 0x29

#define FAT32_DIR_END 0x00
#define FAT32_DIR_UNUSED 0xE5

#define FAT32_ENTRY_LFN FAT32_LONG_FILE_NAME

struct Fat32BootRecord {
    uint8_t  boot_jump[3];           // Jump instruction to boot code
    uint8_t  oem_identifier[8];      // Usually "MSWIN4.1" or "mkfs.fat"
    uint16_t bytes_per_sector;       // Usually 512
    uint8_t  sectors_per_cluster;    // Power of 2, e.g. 1,2,4,8,16,32,64,128
    uint16_t reserved_sectors;       // Usually 32 for FAT32
    uint8_t  fat_count;             // Usually 2
    uint16_t dir_entry_count;       // Must be 0 for FAT32
    uint16_t total_sectors;         // Must be 0 for FAT32, use large_sector_count instead
    uint8_t  media_descriptor_type; // Usually 0xF8 for fixed disk
    uint16_t sectors_per_fat;       // Must be 0 for FAT32, use sectors_per_fat32 instead
    uint16_t sectors_per_track;     // Legacy CHS geometry
    uint16_t head_count;           // Legacy CHS geometry
    uint32_t hidden_sectors;       // Sectors before partition
    uint32_t large_sector_count;   // Total sector count for FAT32

    // FAT32 specific fields
    uint32_t sectors_per_fat32;    // Sectors per FAT
    uint16_t ext_flags;            // Extended flags
    uint16_t fs_version;          // File system version
    uint32_t root_cluster;        // First cluster of root directory
    uint16_t fs_info_sector;      // Usually 1
    uint16_t backup_boot_sector;  // Usually 6
    uint8_t  reserved1[12];       // Should be zero
    uint8_t  drive_number;        // Usually 0x80 for hard disk
    uint8_t  reserved2;           // Used by Windows NT, should be 0
    uint8_t  ext_boot_signature;  // Usually 0x29
    uint32_t volume_serial;       // Volume serial number
    uint8_t  volume_label[11];    // Volume label
    uint8_t  fs_type[8];         // Always "FAT32   "
    uint8_t  boot_code[420];      // Boot code
    uint16_t boot_signature;      // 0xAA55
}__attribute__((packed));

struct Fat32FSInfo {
    uint32_t lead_signature;
    uint8_t reserved0[480];
    uint32_t signature;
    uint32_t last_known_free_cluster_count;
    uint32_t cluster_available_clusters;
    uint8_t reserved1[12];
    uint32_t trail_signature;
}__attribute__((packed));

struct Fat32LongFileName {
    uint8_t index;
    uint16_t chars0[5];
    uint8_t attributes; // should always be FAT32_LONG_FILE_NAME
    uint8_t long_entry_type; // 0 for name entry
    uint8_t checksum;
    uint16_t chars1[6];
    uint16_t zero;
    uint16_t chars2[2];
}__attribute__((packed));

struct Fat32Dir {
    uint8_t file_name[11];
#define FAT32_READ_ONLY 0x01
#define FAT32_HIDDEN 0x02
#define FAT32_SYSTEM 0x04
#define FAT32_VOLUME_ID 0x08
#define FAT32_DIRECTORY 0x10
#define FAT32_ARCHIVE 0x20
#define FAT32_LONG_FILE_NAME (FAT32_READ_ONLY | FAT32_HIDDEN | FAT32_SYSTEM | FAT32_VOLUME_ID)
    uint8_t attributes;
    uint8_t reserved;
    uint8_t small_creation_time;
    struct {
        uint16_t hour : 5;
        uint16_t minutes : 6;
        uint16_t seconds : 5; // times 2 for seconds
    } time_created;
    struct {
        uint16_t year : 7;
        uint16_t month : 4;
        uint16_t day : 5; // times 2 for seconds
    } date_created;
    struct {
        uint16_t year : 7;
        uint16_t month : 4;
        uint16_t day : 5; // times 2 for seconds
    } last_accessed;
    uint16_t first_cluster_u;
    struct {
        uint16_t hour : 5;
        uint16_t minutes : 6;
        uint16_t seconds : 5; // times 2 for seconds
    } last_modified_time;
    struct {
        uint16_t year : 7;
        uint16_t month : 4;
        uint16_t day : 5; // times 2 for seconds
    } last_modified_date;
    uint16_t first_cluster_l;
    uint32_t size;
}__attribute__((packed));

// for exfat
/*struct ExFatBootSector {
    uint8_t jump_boot[3];
    uint8_t file_system_name[8];
    uint8_t null[53];
    uint64_t partition_offset;
    uint64_t volume_length;
    uint32_t fat_offset;
    uint32_t fat_length;
    uint32_t cluster_heap_offset;
    uint32_t cluster_count;
    uint32_t first_cluster_of_root_directory;
    uint32_t volume_serial_number;
    uint16_t file_system_revision;
    struct {
        uint16_t active_fat : 1;
        uint16_t volume_dirty : 1;
        uint16_t media_failure : 1;
        uint16_t clear_to_zero : 1;
        uint16_t reserved : 12;
    } volume_flags;
    uint8_t bytes_per_sector_shift;
    uint8_t sectors_per_cluster_shift;
    uint8_t number_of_fats;
    uint8_t drive_select;
    uint8_t percent_in_use;
    uint8_t reserved[7];
    uint8_t boot_code[390];
    uint16_t boot_signature;
}__attribute__((packed));*/



void set_sata_device(struct HBA_PORT *device);
struct HBA_PORT* get_sata_device();

void init_fat32();