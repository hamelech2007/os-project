#include "multiboot.h"
#include "print32.h"
#include "mmu32.h"
#include "stdint.h"
extern void error();



void entry_32(multiboot2_info_t* bootInfo) {
    initMMU();
    print_hex32(bootInfo->header->bss_end_addr);
    //print_str32("Pages set up.\n");
    //for(;;);
}