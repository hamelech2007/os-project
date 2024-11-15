#include "memory.h"
#include "memory_utils.h"
#include "stdint.h"
#include "multiboot.h"
#include "print.h"


extern struct pml4_entry* page_table_l4;

void initMemory(struct multiboot_mmap_entry entries[], uint32_t entry_size, uint32_t entry_count){
    for(uint32_t i = 0; i < entry_count; i++){
        struct multiboot_mmap_entry* entry = entries + i;
        print_str("Found entry! addr: 0x");
        print_hex(entry->base_addr);
        print_str(", length: 0x");
        print_hex(entry->length);
        print_str(", type: ");
        print_int(entry->type);
        print_char('\n');
    }

    memset(page_table_l4, 0, 8);
    for(uint16_t i = 0; i < 512; i++) {
        invalidate(i * 0x200000);
    }
}

void invalidate(uint64_t vaddr) {
    asm volatile ("invlpg %0" :: "m"(vaddr));
}