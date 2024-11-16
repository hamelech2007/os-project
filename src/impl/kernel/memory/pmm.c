#include "memory.h"
#include "stdint.h"

uint64_t V2P(uint64_t vaddr) {
    return vaddr - KERNEL_OFFSET;
}

void pmm_free(uint64_t page_start){
    // todo: implement
}