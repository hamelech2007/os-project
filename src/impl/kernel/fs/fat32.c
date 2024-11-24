#include "fat32.h"
#include "util.h"
#include "memory_utils.h"

struct HBA_PORT *sata_device = NULL;
struct Fat32BootRecord boot_record;
struct Fat32FSInfo fs_info;
uint8_t FAT32_root_table[BYTES_PER_SECTOR];

void set_sata_device(struct HBA_PORT* device) {
    sata_device = device;
}

struct HBA_PORT* get_sata_device() {
    return sata_device;
}


void init_fat32() {
    if(!sata_device) {
        printf("No SATA device found!\n");
        panic();
    }

    if(!ahci_read_sectors(sata_device, 0, 1, &boot_record)){
        printf("Failed to read boot sector!\n");
        panic();
    }

    if(boot_record.ext_boot_signature != FAT32_EXT_BOOT_SIG_1 && boot_record.ext_boot_signature != FAT32_EXT_BOOT_SIG_2) {
        printf("Wrong signature from drive.\n");
        panic();
    }

    if(boot_record.bytes_per_sector != BYTES_PER_SECTOR || boot_record.sectors_per_cluster != SECTORS_PER_CLUSTER) {
        printf("Wrong number of bytes per sector or sectors per cluster.\n");
        panic();
    }

    if(!ahci_read_sectors(sata_device, boot_record.fs_info_sector, 1, &fs_info)){
        printf("Failed to read fs info!\n");
        panic();
    }

    if(fs_info.lead_signature != FAT32_FSINFO_LEAD_SIGNATURE || fs_info.signature != FAT32_FSINFO_SIGNATURE || fs_info.trail_signature != FAT32_FSINFO_TRAIL_SIGNATURE) {
        printf("Wrong signature at fs info.\n");
        panic();
    }

    printf("Fat32 identified!\n");

    uint32_t first_data_sector = boot_record.reserved_sectors + boot_record.fat_count * boot_record.sectors_per_fat32;
    uint32_t base_root_directory_sector = first_data_sector + (boot_record.root_cluster - 2) * boot_record.sectors_per_cluster;

    if(!ahci_read_sectors(sata_device, base_root_directory_sector, 1, &FAT32_root_table)) {
        printf("Failed to read root directory.\n");
        panic();
    }

    for(uint32_t entry_index = 0; entry_index < BYTES_PER_SECTOR/sizeof(struct Fat32Dir); entry_index++) {
        struct Fat32Dir *dir = &FAT32_root_table[entry_index * sizeof(struct Fat32Dir)];
        if(dir->file_name[0] == FAT32_DIR_END) {
            break; // no more files in the directory
        }
        if(dir->file_name[0] == FAT32_DIR_UNUSED) {
            continue; // unused entry
        }
        if(dir->attributes == FAT32_ENTRY_LFN) {
            // read long file name into buffer
            continue;
        }
        // valid entry
        // check for long file name present from before

        printf("Logging root dir: name: %c%c%c%c%c%c%c%c%c%c%c, attr: 0x%x", dir->file_name[0],
                dir->file_name[1],
                dir->file_name[2],
                dir->file_name[3],
                dir->file_name[4],
                dir->file_name[5],
                dir->file_name[6],
                dir->file_name[7],
                dir->file_name[8],
                dir->file_name[9],
                dir->file_name[10],
                dir->attributes);
    }

}