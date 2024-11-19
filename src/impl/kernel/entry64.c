#include "print.h"
#include "gdt.h"
#include "stdio.h"
#include "idt.h"
#include "util.h"
#include "drivers.h"
#include "multiboot.h"
#include "memory.h"
#include "kheap.h"

extern void error();
extern void kernel_main();

struct KernelBootData kernel_boot_data;

void enable_floating_point();

void entry_64(struct MultibootTaglist* boot_info_addr) {

    parse_tags(boot_info_addr);
    init_memory();
    kheap_init();
    

    init_gdt(); // initialize global descriptor table
    init_idt(); // initialize interrupt descriptor table
    initialize_drivers(); // initialize drivers


    enable_floating_point();

    kernel_main();
}

void enable_floating_point(){
        // Clear the EM (bit 2) and TS (bit 3) bits in CR0
    asm volatile (
        "mov %%cr0, %%rax;"
        "and $0xFFFFFFFFFFFFFFF9, %%rax;"  // Clear bits 2 and 3 (EM and TS)
        "mov %%rax, %%cr0;"
        :
        :
        : "rax"
    );

    // Set the OSFXSR (bit 9) and OSXMMEXCPT (bit 10) bits in CR4
    asm volatile (
        "mov %%cr4, %%rax;"
        "or $0x600, %%rax;"        // Set bits 9 and 10 (OSFXSR and OSXMMEXCPT)
        "mov %%rax, %%cr4;"
        :
        :
        : "rax"
    );
}