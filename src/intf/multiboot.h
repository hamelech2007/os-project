#pragma once
#include "stdint.h"
#include "acpi.h"


struct MultibootTaglist {
  uint32_t total_size;
  uint32_t reserved;
}__attribute__((packed));

struct MultibootTag {
  uint32_t type;
  uint32_t size;
  uint8_t data[];
}__attribute__((packed));


struct MultibootMmapEntry {
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

struct MultibootMmap {
  uint32_t entry_size;
  uint32_t entry_version;
  struct MultibootMmapEntry entries[];
}__attribute__((packed));

struct KernelBootData
{
  uint8_t multiboot_version;
  char *bootloader;
  char *commandline;
  struct RSDP_t* rsdp;
  uint32_t mmap_size;
  uint32_t mmap_len;
  struct MultibootMmap *mmap;
};

#define MULTIBOOT_BOOTLOADER_NAME   2
#define MULTIBOOT_COMMANDLINE       1
#define MULTIBOOT_MMAP              6
#define MULTIBOOT_OLD_RSDP          14

void parse_tags(struct MultibootTaglist* tag_list);
struct MultibootMmapEntry* get_memory_area_from_multiboot(uint8_t index);
bool multiboot_page_used(uint64_t start_addr);