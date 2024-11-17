#include "memory.h"
#include "memory_utils.h"
#include "stdint.h"
#include "multiboot.h"
#include "print.h"


extern struct uint8_t page_table_l4, page_table_l3, page_table_l2, page_table_l1, page_table_l3_higher, page_table_l2_higher, page_table_l1_higher;
extern struct KernelBootData kernel_boot_data;
extern uint64_t _kernel_start, _kernel_end;

bool overlap(uint64_t vaddr, uint32_t len, uint64_t page){
    return vaddr < page + PAGE_SIZE && page <= vaddr + len;
}

void init_memory(){
    struct MultibootMmapEntry *entry;

    for(uint16_t i = 0; entry = get_memory_area_from_multiboot(i); i++){

        for(uint64_t p = entry->base_addr; p < entry->base_addr + entry->length; p += PAGE_SIZE){
            if(p >= V2P(&_kernel_start) && p < V2P(&_kernel_end)) continue;
            if(multiboot_page_used(p)) continue;


            uint64_t vaddr = P2V(p);
            uint64_t page = vmm_get_page(&page_table_l4, vaddr);

            if(!(page & PAGE_PRESENT) || !vmm_page_exists(page)){
                uint16_t flags = PAGE_GLOBAL | PAGE_WRITE;
                vmm_set_page(&page_table_l4, vaddr, p, flags | PAGE_PRESENT);
                invalidate(vaddr);
            }
            if(overlap(p, 4096, &page_table_l4) || overlap(p, 4096, &page_table_l3) || overlap(p, 4096, &page_table_l2) || overlap(p, 4096, &page_table_l1) || overlap(p, 4096, &page_table_l3_higher) || overlap(p, 4096, &page_table_l2_higher) || overlap(p, 4096, &page_table_l1_higher)) continue;
            if(entry->type == MULTIBOOT2_MEMORY_AVAILABLE)
                pmm_free(p);
        }
    }

    
}

void invalidate(uint64_t vaddr) {
    asm volatile ("invlpg %0" :: "m"(vaddr));
}

void* kalloc_page() {
    return P2V(pmm_calloc());
}

void kfree_page(void* page_start) {
    pmm_free(V2P(page_start));
}