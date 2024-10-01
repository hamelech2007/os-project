#include "stdint.h"

#define MULTIBOOT2_HEADER_MAGIC 0xe85250d6
#define MULTIBOOT2_HEADER_FLAGS 0x00000003

typedef struct {
    uint32_t magic;       // Multiboot magic number
    uint32_t architecture; // Architecture (e.g., 0 for i386, 1 for x86_64)
    uint32_t header_length; // Length of this header
    uint32_t checksum;    // Checksum of the header
    uint32_t load_addr;   // Load address for the kernel
    uint32_t load_end_addr; // End address for the kernel
    uint32_t bss_end_addr; // End address of BSS
    uint32_t entry_addr;  // Entry point address for the kernel
} __attribute__((packed)) multiboot2_header_t;

typedef struct {
    uint32_t type; // Type of the tag
    uint32_t size; // Size of the tag
    // Tag-specific data can follow based on the type
} __attribute__((packed)) multiboot2_tag_t;

typedef struct {
    multiboot2_header_t *header; // Pointer to the Multiboot2 header
    multiboot2_tag_t *tags;       // Pointer to the first tag
} __attribute__((packed)) multiboot2_info_t;