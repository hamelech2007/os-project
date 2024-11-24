#include "drivers.h"
#include "mouse.h"
#include "stdio.h"
#include "multiboot.h"
#include "util.h"
#include "memory.h"
#include "acpi.h"
#include "pci.h"


extern struct KernelBootData kernel_boot_data;

extern struct PciDevice pci_devices[MAX_PCI_DEVICES];
extern uint32_t pci_device_count;

void init_drivers() {
    initialize_PS2_mouse();
    


    if(kernel_boot_data.rsdp == NULL) {
        printf("Multiboot didn't find RSDP.\n");
        find_rsdp();
    }

    if(kernel_boot_data.rsdp == NULL) {
        printf("Couldn't find rsdp.");
        panic();
    } else {
        printf("RSDP found at address 0x%p, revision number %d, RSDT at 0x%p\n", kernel_boot_data.rsdp, kernel_boot_data.rsdp->Revision, kernel_boot_data.rsdp->RsdtAddress);
    }

    struct RSDT* rsdt = P2V(kernel_boot_data.rsdp->RsdtAddress);


    parse_rsdt(rsdt);

    pci_enumerate();

    print_pci_devices(pci_devices, pci_device_count);
    
    
}