#include "multiboot.h"
#include "util.h"
#include "print.h"
#include "memory.h"

void handle_tags(uint64_t boot_info_addr) {
    // information for initializing memory management:
    uint64_t entries;
    uint32_t entry_size;
    uint32_t entry_count;
    uint32_t mem_upper;
    uint32_t mods_addr;

    uint32_t total_size = *(uint32_t*)boot_info_addr;
    boot_info_addr+=8; // skip size and reserved
    boot_info_addr+=(8-boot_info_addr%8)%8; // align to 8
    uint32_t tag_type, tag_size;
    while(!(((tag_type = *(uint32_t*)boot_info_addr) == 0) & ((tag_size = *(uint32_t*)(boot_info_addr+4)) == 8))) { // we use bitwise to prevent short circuit evaluation
        switch (tag_type)
        {
        case 2: // boot loader name tag
            print_str("Boot loader name from boot info: ");
            print_str(boot_info_addr + 8);
            print_char('\n');
            break;
        case 4: // Basic memory information
            print_str("mem_lower: 0x");
            print_hex(*(uint32_t*)(boot_info_addr + 8));
            print_str(", mem_upper: 0x");
            print_hex(*(uint32_t*)(boot_info_addr + 12));
            print_char('\n');
            mem_upper = *(uint32_t*)(boot_info_addr + 12);
            break;
        case 5: // BIOS Boot device
            print_str("biosdev: 0x");
            print_hex(*(uint32_t*)(boot_info_addr + 8));
            print_str(", partition: 0x");
            print_hex(*(uint32_t*)(boot_info_addr + 12));
            print_str(", sub_partition: 0x");
            print_hex(*(uint32_t*)(boot_info_addr + 16));
            print_char('\n');
            break;
        case 6: // Memory map
            entries = boot_info_addr + 16;
            entry_size = *(uint32_t*)(boot_info_addr + 8);
            entry_count = (tag_size - 16)/entry_size;
            uint32_t entry_version = *(uint32_t*)(boot_info_addr + 12);
            if(entry_version != 0 || entry_size != 24) panic();
            break;
            
        case 8: // Framebuffer info
            print_str("address: 0x");
            print_hex(*(uint64_t*)(boot_info_addr + 8));
            print_str(", pitch: 0x");
            print_hex(*(uint32_t*)(boot_info_addr + 16));
            print_str(", width: 0x");
            print_hex(*(uint32_t*)(boot_info_addr + 20));
            print_str(", height: 0x");
            print_hex(*(uint32_t*)(boot_info_addr + 24));
            print_str(", bpp: 0x");
            print_hex(*(uint8_t*)(boot_info_addr + 28));
            print_str(", type: 0x");
            print_hex(*(uint8_t*)(boot_info_addr + 29));
            print_str(", color_info: 0x");
            print_hex(*(uint8_t*)(boot_info_addr + 31));
            print_char('\n');
        default:
            print_str("Found tag: ");
            print_int(tag_type);
            print_str(" of size: ");
            print_int(tag_size);
            print_str(" bytes!\n");
            break;
        }
        boot_info_addr+=tag_size;
        boot_info_addr+= (8-boot_info_addr%8)%8; // align to 8
    }
    initMemory(entries, entry_size, entry_count);
}