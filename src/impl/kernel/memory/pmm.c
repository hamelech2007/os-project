#include "memory.h"
#include "memory_utils.h"
#include "print.h"
#include "stdint.h"


static uint64_t next = 0;

void pmm_free(uint64_t page_start){
    page_start = MASK_FLAGS(page_start);
    *(uint64_t*) P2V(page_start) = next;
    next = (uint64_t) P2V(page_start);
}

uint64_t pmm_alloc() {
    if(!next) return 0;
    uint64_t page = next;
    next = *(uint64_t *) page;
    return V2P(page);
}

uint64_t pmm_calloc() {
    uint64_t page = pmm_alloc();
    memset((void*)P2V(page), 0, PAGE_SIZE);
    return page;
}