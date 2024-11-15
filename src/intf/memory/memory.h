#pragma once
#include "stdint.h"
#include "multiboot.h"

#define KERNEL_OFFSET 0xffffffff80000000;

struct pml4_entry {
    uint64_t present      : 1;  // Present bit: must be 1 for valid entry
    uint64_t rw           : 1;  // Read/Write: 0 = read-only, 1 = read/write
    uint64_t user         : 1;  // User/Supervisor: 0 = supervisor, 1 = user
    uint64_t pwt          : 1;  // Page-level write-through
    uint64_t pcd          : 1;  // Page-level cache disable
    uint64_t accessed     : 1;  // Accessed: set by CPU when the page is accessed
    uint64_t ignored1     : 1;  // Ignored (can be used by software)
    uint64_t reserved1    : 1;  // Reserved, must be 0
    uint64_t ignored2     : 4;  // Ignored (available for software use)
    uint64_t pdpt_base    : 40; // Physical address of the Page Directory Pointer Table (aligned to 4 KB)
    uint64_t reserved2    : 11; // Reserved, must be 0
    uint64_t execute_disable : 1;  // Execute Disable bit: 1 = disable execution
} __attribute__((packed));

struct pdpt_entry {
    uint64_t present       : 1;  // Present bit: must be 1 for valid entry
    uint64_t rw            : 1;  // Read/Write: 0 = read-only, 1 = read/write
    uint64_t user          : 1;  // User/Supervisor: 0 = supervisor, 1 = user
    uint64_t pwt           : 1;  // Page-level write-through
    uint64_t pcd           : 1;  // Page-level cache disable
    uint64_t accessed      : 1;  // Accessed: set by CPU when the page is accessed
    uint64_t dirty         : 1;  // Dirty bit: set by CPU on write
    uint64_t ps            : 1;  // Page size: 0 = points to Page Directory, 1 = 1GB page
    uint64_t ignored1      : 4;  // Ignored (can be used by software)
    uint64_t pd_base       : 40; // Physical address of the Page Directory (aligned to 4 KB) or 1GB page base address if ps = 1
    uint64_t reserved      : 11; // Reserved, must be 0
    uint64_t execute_disable : 1;  // Execute Disable bit: 1 = disable execution
} __attribute__((packed));

struct pdt_entry {
    uint64_t present        : 1;  // Present bit: must be 1 for valid entry
    uint64_t rw             : 1;  // Read/Write: 0 = read-only, 1 = read/write
    uint64_t user           : 1;  // User/Supervisor: 0 = supervisor, 1 = user
    uint64_t pwt            : 1;  // Page-level write-through
    uint64_t pcd            : 1;  // Page-level cache disable
    uint64_t accessed       : 1;  // Accessed: set by CPU when the page is accessed
    uint64_t dirty          : 1;  // Dirty bit: set by CPU on write (only if ps = 1)
    uint64_t ps             : 1;  // Page size: 0 = points to Page Table, 1 = 2 MB page
    uint64_t ignored1       : 4;  // Ignored (can be used by software)
    uint64_t pt_base        : 40; // Physical address of the Page Table (aligned to 4 KB) or 2 MB page base address if ps = 1
    uint64_t reserved       : 11; // Reserved, must be 0
    uint64_t execute_disable : 1; // Execute Disable bit: 1 = disable execution
} __attribute__((packed));

struct pt_entry {
    uint64_t present        : 1;  // Present bit: must be 1 for valid entry
    uint64_t rw             : 1;  // Read/Write: 0 = read-only, 1 = read/write
    uint64_t user           : 1;  // User/Supervisor: 0 = supervisor, 1 = user
    uint64_t pwt            : 1;  // Page-level write-through
    uint64_t pcd            : 1;  // Page-level cache disable
    uint64_t accessed       : 1;  // Accessed: set by CPU when the page is accessed
    uint64_t dirty          : 1;  // Dirty: set by CPU when the page is written to
    uint64_t pat            : 1;  // Page Attribute Table index (for caching)
    uint64_t global         : 1;  // Global page (doesn't get invalidated during TLB flush)
    uint64_t ignored1       : 3;  // Ignored (can be used by software)
    uint64_t page_base      : 40; // Physical address of the 4 KB page (aligned to 4 KB)
    uint64_t reserved       : 11; // Reserved, must be 0
    uint64_t execute_disable : 1; // Execute Disable bit: 1 = disable execution
} __attribute__((packed));

void initMemory(struct multiboot_mmap_entry[], uint32_t entry_size, uint32_t entry_count);
void invalidate(uint64_t vaddr);