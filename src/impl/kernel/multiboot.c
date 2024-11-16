#include "multiboot.h"
#include "util.h"
#include "print.h"
#include "memory.h"
#include "stdint.h"
#include "string.h"


extern struct KernelBootData kernel_boot_data;

void parse_tags(struct MultibootTaglist* tag_list) {

    struct MultibootTag* tag = ((uint8_t*) tag_list) + sizeof(struct MultibootTaglist);

    while(tag->type){
        switch(tag->type){
            case MULTIBOOT_BOOTLOADER_NAME:
                kernel_boot_data.bootloader = tag->data;
                break;
            case MULTIBOOT_COMMANDLINE:
                kernel_boot_data.commandline = tag->data;
                break;
            case MULTIBOOT_MMAP:
                kernel_boot_data.mmap = tag->data;
                kernel_boot_data.mmap_len = (tag->size - 8)/kernel_boot_data.mmap->entry_size;
                kernel_boot_data.mmap_size = tag->size - 8;
                break;
        }

        tag = ((uint8_t*) tag)+(tag->size + ((tag->size % 8)?(8-(tag->size%8)):0));
    }

}

struct MultibootMmapEntry* get_memory_area_from_multiboot(uint8_t index){

    if(index >= kernel_boot_data.mmap_len) return 0;

    return &kernel_boot_data.mmap->entries[index];

}

bool multiboot_page_used(uint64_t start_addr){
    if(
        ((kernel_boot_data.bootloader < start_addr + PAGE_SIZE) && (start_addr <= kernel_boot_data.bootloader + strlen(kernel_boot_data.bootloader))) ||
        ((kernel_boot_data.commandline < start_addr + PAGE_SIZE) && (start_addr <= kernel_boot_data.commandline + strlen(kernel_boot_data.commandline))) ||
        ((kernel_boot_data.mmap < start_addr + PAGE_SIZE) && (start_addr <= kernel_boot_data.mmap + kernel_boot_data.mmap_size))
    ) return true;
    return false;
}