#include "memory.h"
#include "memory_utils.h"
#include "stdint.h"
#include "multiboot.h"
#include "print.h"


extern struct pml4_entry* page_table_l4;
extern struct KernelBootData kernel_boot_data;
extern uint64_t _kernel_start, _kernel_end;

void init_memory(){
    struct MultibootMmapEntry *entry;

    for(uint16_t i = 0; entry = get_memory_area_from_multiboot(i); i++){
        if(entry->type != MULTIBOOT2_MEMORY_AVAILABLE) continue;

        for(uint64_t p = entry->base_addr; p < entry->base_addr + entry->length; p += PAGE_SIZE){
            if(p >= V2P(&_kernel_start) && p < V2P(&_kernel_end)) continue;
            if(multiboot_page_used(p)) continue;

            uint64_t vaddr = P2V(p);
            uint64_t page = vmm_get_page(page_table_l4, vaddr);

            if(!(page & PAGE_PRESENT)){
                // todo set flags global and write in vmm
            }

            pmm_free(page);
        }
    }

    
}

void invalidate(uint64_t vaddr) {
    asm volatile ("invlpg %0" :: "m"(vaddr));
}