#include "memory.h"
#include "memory_utils.h"
#include "stdint.h"
#include "kheap.h"
#include <stddef.h>


#define ROUND_PAGE(addr) (((uint64_t)(addr) + 0x0fff) & (uint64_t)~0x0fff)  // round to the upper page
#define MASK_ALLOCATED(size) ((uint16_t)(size) & 0xfff8)                    // mask the allocated flag

extern uint8_t _kernel_end;
uint8_t *heap_addr_start, *heap_addr_end;
struct HeapBlock* heap_start;
struct HeapBlock* first_free_block = 0;


void add_new_page(uint64_t vpage){
    struct HeapBlock* block =(struct HeapBlock*) vpage;

    block->tag_start.allocated = false;
    block->tag_start.size = PAGE_SIZE;

    ((union BoundaryTag*)(vpage - sizeof(union BoundaryTag) + PAGE_SIZE))->allocated = false;
    ((union BoundaryTag*)(vpage - sizeof(union BoundaryTag) + PAGE_SIZE))->size = PAGE_SIZE;

    block->next = 0;
    block->prev = 0;

    kfree((uint8_t*)block + sizeof(union BoundaryTag));
}

void *allocate_heap_page(){

    void* next_page_vaddr = heap_addr_end;
    uint64_t page = vmm_get_page((uint64_t)&page_table_l4, (uint64_t)next_page_vaddr);

    uint64_t ph_page = pmm_calloc();
    if(!ph_page) return NULL;


    if(!(page & PAGE_PRESENT) || !vmm_page_exists(page)){
        uint16_t flags = PAGE_GLOBAL | PAGE_WRITE;
        vmm_set_page((uint64_t)&page_table_l4, (uint64_t)next_page_vaddr, ph_page, flags | PAGE_PRESENT);
        invalidate((uint64_t)next_page_vaddr);
    }

    heap_addr_end += PAGE_SIZE;
    add_new_page(next_page_vaddr);
    return next_page_vaddr;
}



uint64_t get_heap_size() {
    return heap_addr_end - heap_addr_start;
}

void kheap_init() {
    uint64_t start_vaddr = ((uint64_t)ROUND_PAGE(&_kernel_end) + (uint64_t)GIGABYTE/2); // FOR FUTURE DEBUGGING: The heap begins 512MB after the &_kernel_end

    heap_addr_start = heap_addr_end = start_vaddr;
}

// using best-fit allocator
void *kmalloc(size_t size){
    if(size < sizeof(struct HeapBlock)) size = sizeof(struct HeapBlock);
    size = (size + 2 * sizeof(union BoundaryTag) + 7) & ~7; // align size to 8
    size -= 2 * sizeof(union BoundaryTag);
    if(get_heap_size() + size > MAX_HEAP_SIZE) return NULL;
    
    if(get_heap_size() == 0 || !first_free_block){
        first_free_block = allocate_heap_page();
        if(!first_free_block) return NULL;
    }
    
    struct HeapBlock* current_block = first_free_block;
    struct HeapBlock* best_block = 0;

    while(current_block) {
        size_t block_size = MASK_ALLOCATED(current_block->tag_start.size);
        if(block_size - 2 * sizeof(union BoundaryTag) < size) {
            current_block = current_block->next;
            continue;
        }

        if(!best_block) {
            best_block = current_block;
            current_block = current_block->next;
            continue;
        }

        if(block_size < best_block->tag_start.size) {
            best_block = current_block;
            current_block = current_block->next;
            continue;
        }

        current_block = current_block->next;
    }

    if(!best_block) {
        if(!allocate_heap_page()) return NULL;
        return kmalloc(size);
    }

    size_t block_size = MASK_ALLOCATED(best_block->tag_start.size);

    if(block_size - size - 2 * sizeof(union BoundaryTag) < sizeof(struct HeapBlock)) {
        // allocate the entire block because we can't split
        if(best_block->prev) {
            struct HeapBlock* prev_block = best_block->prev;
            prev_block->next = best_block->next;
        }
        
        if(best_block->next) {
            struct HeapBlock* next_block = best_block->next;
            next_block->prev = best_block->prev;
        }
        
        if(first_free_block == best_block) {
            first_free_block = best_block->next;
        }

        best_block->tag_start.allocated = true;
        ((union BoundaryTag*)((uint8_t*)best_block+MASK_ALLOCATED(best_block->tag_start.size) - sizeof(union BoundaryTag)))->allocated = true;

        memset((uint8_t*)best_block + sizeof(union BoundaryTag), 0, 2 * sizeof(uint64_t));

        return (uint8_t*)best_block + sizeof(union BoundaryTag);
    }

    // we can split
    struct HeapBlock* new_block = (uint8_t*)best_block + size + 2 * sizeof(union BoundaryTag);
    union BoundaryTag new_block_tag;

    new_block_tag.size = block_size - size - 2 * sizeof(union BoundaryTag);
    new_block_tag.allocated = false;
    new_block->tag_start = *((union BoundaryTag*)((uint8_t*)new_block + MASK_ALLOCATED(new_block->tag_start.size) - sizeof(union BoundaryTag))) = new_block_tag;

    if(first_free_block == best_block) {
        first_free_block = new_block;
    }

    if(best_block->prev) {
        struct HeapBlock* prev_block = best_block->prev;
        new_block->prev = prev_block;
        prev_block->next = new_block;
    }

    if(best_block->next) {
        struct HeapBlock* next_block = best_block->next;
        new_block->next = next_block;
        next_block->prev = new_block;
    }

    union BoundaryTag* best_block_end_tag = (uint8_t*)best_block + MASK_ALLOCATED(best_block->tag_start.size) - sizeof(union BoundaryTag);
    best_block->tag_start.size = size + 2 * sizeof(union BoundaryTag);
    best_block_end_tag->size = size + 2 * sizeof(union BoundaryTag);
    best_block->tag_start.allocated = true;
    best_block_end_tag->allocated = true;

    
    memset(best_block + sizeof(union BoundaryTag), 0, 2 * sizeof(uint64_t));
    


    return (uint8_t*)best_block + sizeof(union BoundaryTag);
}

void kfree(void* ptr){
    struct HeapBlock* block = (struct HeapBlock*) ((uint8_t*)ptr - sizeof(union BoundaryTag));

    if(!first_free_block) {
        block->next = 0;
        block->prev = 0;
        ((union BoundaryTag*)((uint8_t*)block + MASK_ALLOCATED(block->tag_start.size) - sizeof(union BoundaryTag)))->allocated = false;
        block->tag_start.allocated = false;
        
        first_free_block = block;
        return;
    }
    struct HeapBlock* itr_block = first_free_block;

    block->tag_start.allocated = false;
    ((union BoundaryTag*)((uint8_t*)block + MASK_ALLOCATED(block->tag_start.size) - sizeof(union BoundaryTag)))->allocated = false;

    if(block < first_free_block) {
        if((uint8_t*)block + MASK_ALLOCATED(block->tag_start.size) == first_free_block) {
            // merge
            uint16_t original_size = MASK_ALLOCATED(block->tag_start.size);
            block->tag_start.size += MASK_ALLOCATED(first_free_block->tag_start.size);
            ((union BoundaryTag*)((uint8_t*)first_free_block + MASK_ALLOCATED(first_free_block->tag_start.size) - sizeof(union BoundaryTag)))->size += original_size;
            first_free_block = block;
            return;
        }

        block->next = first_free_block;
        block->prev = 0;
        first_free_block->prev = block;
        first_free_block = block;
        return;
    }

    while(itr_block < block) {
        if(itr_block->next == NULL) {
            // we need to enter it here as the last entry
            if((uint8_t*)itr_block + MASK_ALLOCATED(itr_block->tag_start.size) == block) {
                // merge
                uint16_t original_size = MASK_ALLOCATED(itr_block->tag_start.size);
                itr_block->tag_start.size += MASK_ALLOCATED(block->tag_start.size);
                ((union BoundaryTag*)((uint8_t*)block + block->tag_start.size - sizeof(union BoundaryTag)))->size+=original_size;
                return;
            }

            itr_block->next = block;
            block->next = NULL;
            block->prev = itr_block;
            return;
        }
        
        itr_block = itr_block->next;
    }

    struct HeapBlock* before_block = itr_block->prev; // aka the block that the next of it will point to our freed block

    block->next = before_block->next;
    block->prev = before_block;
    before_block->next = block;
    before_block->next->prev = block;

    if(before_block + MASK_ALLOCATED(before_block->tag_start.size) == block) {
        // merge
        uint16_t original_size = MASK_ALLOCATED(before_block->tag_start.size);
        before_block->tag_start.size+=MASK_ALLOCATED(block->tag_start.size);
        ((union BoundaryTag*)((uint8_t*)block + MASK_ALLOCATED(block->tag_start.size) - sizeof(union BoundaryTag)))->size += original_size;

        before_block->next = block->next;
        block->next->prev = before_block;
        block = before_block; // for next merge if it happens
    }

    if((uint8_t*)block+MASK_ALLOCATED(block->tag_start.size) == itr_block) {
        // merge
        uint16_t original_size = MASK_ALLOCATED(block->tag_start.size);

        block->tag_start.size += MASK_ALLOCATED(block->next->tag_start.size);
        ((union BoundaryTag*)((uint8_t*)block->next + MASK_ALLOCATED(block->next->tag_start.size) - sizeof(union BoundaryTag)))->size += original_size;
        
        block->next->prev = block->prev;
        block->next = block->next->next;
    }

    
    
}