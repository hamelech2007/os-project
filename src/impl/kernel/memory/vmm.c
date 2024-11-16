#include "memory.h"
#include "stdint.h"

uint64_t P2V(uint64_t phaddr) {
    return phaddr + KERNEL_OFFSET;
}

uint64_t vmm_get_page(uint64_t pml4, uint64_t vaddr) {
    // todo: implement
    return 0;
}

bool vmm_page_exists(uint64_t page_start) {
    return page_start != ((uint64_t)-1);
}