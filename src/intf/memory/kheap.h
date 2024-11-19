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