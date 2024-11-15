#pragma once
#include "stdint.h"

struct multiboot_mmap_entry {
    uint64_t base_addr;
    uint64_t length;
#define MULTIBOOT2_MEMORY_AVAILABLE             1
#define MULTIBOOT2_MEMORY_RESERVED              2
#define MULTIBOOT2_MEMORY_ACPI_RECLAIMABLE      3
#define MULTIBOOT2_MEMORY_NVS                   4
#define MULTIBOOT2_MEMORY_DEFECTIVE             5
    uint32_t type;
    uint32_t reserved;
} __attribute__((packed));


void handle_tags(uint64_t boot_info_addr);