#include "multiboot.h"
#include "print.h"

void handle_tags(uint64_t boot_info_addr) {
    uint32_t total_size = *(uint32_t*)boot_info_addr;
    boot_info_addr+=8; // skip size and reserved
    boot_info_addr+=(8-boot_info_addr%8)%8; // align to 8
    uint32_t tag_type, tag_size;
    while(!(((tag_type = *(uint32_t*)boot_info_addr) == 0) & ((tag_size = *(uint32_t*)(boot_info_addr+4)) == 8))) { // we use bitwise to prevent short circuit evaluation
        /*print_str("Found tag: ");
        print_int(tag_type);
        print_str(" of size: ");
        print_int(tag_size);
        print_str(" bytes!\n");*/
        switch (tag_type)
        {
        case 2:
            uint8_t index = 0;
            print_str("Boot loader name from boot info: ");
            while(*(uint8_t*) (boot_info_addr + 8 + index)) {
                print_char(*(uint8_t*) (boot_info_addr + 8 + index++));
            }
            print_char('\n');
            break;
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
}