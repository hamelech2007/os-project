#include "memory.h"
#include "stdint.h"

#define P4_OFFSET(a) ((a)>>39 & 0x1FF)
#define P3_OFFSET(a) ((a)>>30 & 0x1FF)
#define P2_OFFSET(a) ((a)>>21 & 0x1FF)
#define P1_OFFSET(a) ((a)>>12 & 0x1FF)


#define PT(ptr) ((uint64_t*) P2V(MASK_FLAGS(ptr)))

#define P4E(pml4, addr) (PT(pml4)[P4_OFFSET(addr)])
#define P3E(pml4, addr) (PT(P4E(pml4,addr))[P3_OFFSET(addr)])
#define P2E(pml4, addr) (PT(P3E(pml4, addr))[P2_OFFSET(addr)])
#define P1E(pml4, addr) (PT(P2E(pml4, addr))[P1_OFFSET(addr)])

#define PRESENT(entry)  (entry & PAGE_PRESENT)
#define HUGE(entry)     (entry & PAGE_HUGE)


static bool page_exists(uint64_t pml4, uint64_t addr) {
    if(pml4 && PRESENT(P4E(pml4, addr)) && PRESENT(P3E(pml4, addr)) && PRESENT(P2E(pml4, addr)))
        return true;
    return false;
}

static bool touch_page(uint64_t pml4, uint64_t addr, uint16_t flags) {
    if(!pml4) return false;

    if((!PRESENT(P4E(pml4, addr))) && (!(P4E(pml4, addr) = pmm_calloc())))
        return false;
    P4E(pml4, addr) |= flags | PAGE_PRESENT;

    if((!PRESENT(P3E(pml4, addr))) && (!(P3E(pml4, addr) = pmm_calloc())))
        return false;
    P3E(pml4, addr) |= flags | PAGE_PRESENT;

    if((!PRESENT(P2E(pml4, addr))) && (!(P2E(pml4, addr) = pmm_calloc())))
        return false;
    P2E(pml4, addr) |= flags | PAGE_PRESENT;

    return true;
}

uint64_t P2V(uint64_t phaddr) {
    return phaddr + KERNEL_OFFSET;
}

uint64_t vmm_get_page(uint64_t pml4, uint64_t vaddr) {
    if(pml4 && PRESENT(P4E(pml4, vaddr)) && PRESENT(P3E(pml4, vaddr)) && PRESENT(P2E(pml4, vaddr)) && PRESENT(P1E(pml4, vaddr))) 
        return P1E(pml4, vaddr);
    else if(pml4 && PRESENT(P4E(pml4, vaddr)) && PRESENT(P3E(pml4, vaddr)) && PRESENT(P2E(pml4, vaddr)) && HUGE(P2E(pml4, vaddr)))
        return P2E(pml4, vaddr);
    else if(pml4 && PRESENT(P4E(pml4, vaddr)) && PRESENT(P3E(pml4, vaddr)) && HUGE(P3E(pml4, vaddr)))
        return P3E(pml4, vaddr);

    return -1;
}

bool vmm_page_exists(uint64_t page_start) {
    return page_start != ((uint64_t)-1);
}

bool vmm_set_page(uint64_t pml4, uint64_t addr, uint64_t page, uint16_t flags) {
    if(!page_exists(pml4, addr)){
        if(!touch_page(pml4, addr, flags)) 
            return false;
    }

    P1E(pml4, addr) = page | flags;
    return true;
}

void vmm_clear_page(uint64_t pml4, uint64_t addr, bool free) {
    if(!page_exists(pml4, addr)) return;

    uint64_t *pt;

    P1E(pml4, addr) = 0;

    if(!free) return;

    pt = PT(P2E(pml4, addr));

    for(int i = 0; i < 512; i++) {
        if(pt[i]) return;
    }

    pmm_free(MASK_FLAGS(P2E(pml4, addr)));
    P2E(pml4, addr) = 0;

    pt = PT(P3E(pml4, addr));

    for(int i = 0; i < 512; i++) {
        if(pt[i]) return;
    }

    pmm_free(MASK_FLAGS(P3E(pml4, addr)));
    P3E(pml4, addr) = 0;

    pt = PT(P4E(pml4, addr));
    for(int i = 0; i < 512; i++) {
        if(pt[i]) return;
    }

    pmm_free(MASK_FLAGS(P4E(pml4, addr)));
    P4E(pml4, addr) = 0;
}

