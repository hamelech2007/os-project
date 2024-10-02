#include "mmu32.h"

struct pml4_entry   PML4T   [512];
struct pdpt_entry   PDPT    [512];
struct pdt_entry    PDT     [512];
struct pt_entry     PT      [512];

void identity_map_first2MB() {
    PML4T[0].present = 1;
    PML4T[0].rw = 1; // Read/Write
    PML4T[0].user = 0; // User/Supervisor
    PML4T[0].pdpt_base = (uint32_t)PDPT >> 12; // Store PDPT base address

    PDPT[0].present = 1;
    PDPT[0].rw = 1; // Read/Write
    PDPT[0].user = 0; // User/Supervisor
    PDPT[0].ps = 0; // Pointing to PDT (not 1GB page)
    PDPT[0].pd_base = (uint32_t)PDT >> 12; // Store PDT base address

    PDT[0].present = 1;
    PDT[0].rw = 1; // Read/Write
    PDT[0].user = 0; // User/Supervisor
    PDT[0].ps = 1; // Using huge flag
    PDT[0].pt_base = 0; // First page addr
}

void setup_identity_mapping() {
    // Initialize PML4 entry to point to PDPT
    for (uint32_t i = 0; i < 1; i++) {
        PML4T[i].present = 1;
        PML4T[i].rw = 1; // Read/Write
        PML4T[i].user = 1; // User/Supervisor
        PML4T[i].pdpt_base = (uint32_t)PDPT >> 12; // Store PDPT base address
    }

    // Initialize PDPT entry to point to PDT
    for (uint32_t i = 0; i < 1; i++) {
        PDPT[i].present = 1;
        PDPT[i].rw = 1; // Read/Write
        PDPT[i].user = 1; // User/Supervisor
        PDPT[i].ps = 0; // Pointing to PDT (not 1GB page)
        PDPT[i].pd_base = (uint32_t)PDT >> 12; // Store PDT base address
    }

    // Initialize PDT entry to point to PT and create identity mapping
    for (uint32_t i = 0; i < 1; i++) {
        PDT[i].present = 1;
        PDT[i].rw = 1; // Read/Write
        PDT[i].user = 1; // User/Supervisor
        PDT[i].ps = 0; // Pointing to PT
        PDT[i].pt_base = (uint32_t)PT >> 12; // Store PT base address
        
        for (uint32_t j = 0; j < 512; j++) {
            uint32_t page_address = (j * 0x1000) + 512*0x1000*i; // Calculate page address, note we only have 1 PT for now
            PT[j].present = 1;
            PT[j].rw = 1; // Read/Write
            PT[j].user = 1; // User/Supervisor
            PT[j].page_base = page_address >> 12; // Set the base address to itself
        }
    }
}

void initMMU() {
    identity_map_first2MB();
}

