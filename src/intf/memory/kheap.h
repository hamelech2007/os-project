#pragma once
#include "stdint.h"

#define MAX_HEAP_SIZE 0x40000000

union BoundaryTag {
    bool allocated : 1;
    uint16_t size;
}__attribute__((packed));

struct HeapBlock {
    union BoundaryTag tag_start;
    struct HeapBlock *next,*prev;
    union BoundaryTag tag_end;
}__attribute__((packed));

void kfree(void* ptr);
void *kmalloc(size_t size);
void *kcalloc(size_t size);

/// @brief  Returns a block large enough so that if you align it, you can still have enough space for your allocation. Note, when freeing, pass the original allocated ptr.
/// @param size The size of the block you wish to allocate.
/// @param align The alignment you require that the block would be large enough to satisfy.
/// @return A free block that is large enough to align but still have enough space for the allocation.
void* aligned_kmalloc(size_t size, size_t align);
void* align_allocated(void* ptr, uint64_t align);