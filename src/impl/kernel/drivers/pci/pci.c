
#include "pci.h"
#include "stdint.h"
#include "util.h"
#include "stdio.h"
#include "print.h"
#include "ahci.h"


struct PciDevice pci_devices[MAX_PCI_DEVICES];
uint32_t pci_device_count = 0;


uint32_t pci_read_dword(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint32_t address = (1U << 31) | 
                       ((uint32_t)bus << 16) | 
                       ((uint32_t)device << 11) | 
                       ((uint32_t)function << 8) | 
                       (offset & 0xFC);
    out_port_l(PCI_CONFIG_ADDRESS, address);
    return in_port_l(PCI_CONFIG_DATA);
}

void pci_write_dword(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value) {
    uint32_t address = (1U << 31) |              // Enable bit
                       ((uint32_t)bus << 16) |    // Bus number
                       ((uint32_t)device << 11) | // Device number
                       ((uint32_t)function << 8) | // Function number
                       (offset & 0xFC);           // Register offset, aligned to 4 bytes
    
    out_port_l(PCI_CONFIG_ADDRESS, address);
    out_port_l(PCI_CONFIG_DATA, value);
}

bool pci_device_is_valid(uint16_t vendor_id) {
    return vendor_id != PCI_VENDOR_INVALID;
}

bool pci_is_ahci_controller(uint8_t class_code, uint8_t subclass, uint8_t prog_if) {
    return class_code == PCI_CLASS_MASS_STORAGE && 
           subclass == PCI_SUBCLASS_SATA && 
           prog_if == PCI_PROG_IF_AHCI;
}

void pci_enumerate() {
    pci_device_count = 0;

    for (uint16_t bus = 0; bus < 256; bus++) {
        for (uint8_t device = 0; device < 32; device++) {
            for (uint8_t function = 0; function < 8; function++) {
                uint32_t vendor_device_reg = pci_read_dword(bus, device, function, 0x00);
                uint16_t vendor_id = vendor_device_reg & 0xFFFF;
                uint16_t device_id = vendor_device_reg >> 16;

                if (!pci_device_is_valid(vendor_id)) continue;

                uint32_t class_reg = pci_read_dword(bus, device, function, 0x08);
                uint8_t class_code = (class_reg >> 24) & 0xFF;
                uint8_t subclass = (class_reg >> 16) & 0xFF;
                uint8_t prog_if = (class_reg >> 8) & 0xFF;

                struct PciDevice current_device = {
                    .vendor_id = vendor_id,
                    .device_id = device_id,
                    .class_code = class_code,
                    .subclass = subclass,
                    .prog_if = prog_if,
                    .bus = bus,
                    .device = device,
                    .function = function
                };
                if (pci_device_count < MAX_PCI_DEVICES) {
                    pci_devices[pci_device_count++] = current_device;
                }

                if (pci_is_ahci_controller(class_code, subclass, prog_if)) {
                    parse_ahci(bus, device, function);
                }
            }
        }
    }
}

void print_pci_devices(struct PciDevice* devices, uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        printf("Class: %d, Subclass: %d, Prog If: %d, Bus %d, Device %d, Function %d\n",
               devices[i].class_code,
               devices[i].subclass,
               devices[i].prog_if,
               devices[i].bus, 
               devices[i].device, 
               devices[i].function);
    }
}

void read_pci_header_type_0_1(struct PciHeader* pci_header, uint8_t bus, uint8_t device, uint8_t function) {
    for(uint8_t i = 0; i <= 0x3c; i+=4) {
        *(uint32_t*) incptr(pci_header, i) = pci_read_dword(bus, device, function, i);
    }
}