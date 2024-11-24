#pragma once
#include "stdint.h"

#define PCI_CONFIG_ADDRESS 0xcf8
#define PCI_CONFIG_DATA 0xcfc

#define MAX_PCI_DEVICES 64  // Maximum number of PCI devices to track

#define PCI_VENDOR_INVALID 0xFFFF
#define PCI_CLASS_MASS_STORAGE 0x01
#define PCI_SUBCLASS_SATA 0x06
#define PCI_PROG_IF_AHCI 0x01

struct PciDevice {
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t class_code;
    uint8_t subclass;
    uint8_t prog_if;
    uint8_t bus;
    uint8_t device;
    uint8_t function;
};

struct PciHeader {
    union {
        struct {
            uint16_t vendor_id;
            uint16_t device_id;
            uint16_t command;
            uint16_t status;
            uint8_t revision_id;
            uint8_t prog_if;
            uint8_t subclass;
            uint8_t class_code;
            uint8_t cache_line_size;
            uint8_t latency_timer;
            uint8_t header_type;
            uint8_t bist;
            uint32_t BAR0;
            uint32_t BAR1;
            uint32_t BAR2;
            uint32_t BAR3;
            uint32_t BAR4;
            uint32_t BAR5;
            uint32_t cardbus_cis_ptr;
            uint16_t subsystem_vendor_id;
            uint16_t subsystem_id;
            uint32_t expansion_rom_base_ptr;
            uint8_t capabilities_ptr;
            uint8_t reserved0;
            uint16_t reserved1;
            uint32_t reserved2;
            uint8_t interrupt_line;
            uint8_t interrupt_pin;
            uint8_t min_grant;
            uint8_t max_latency;
        } Zero;
    };
};

void pci_enumerate();
void print_pci_devices(struct PciDevice* devices, uint32_t count);
uint32_t pci_read_dword(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
void pci_write_dword(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value);

void read_pci_header_type_0_1(struct PciHeader* pci_header, uint8_t bus, uint8_t device, uint8_t function);