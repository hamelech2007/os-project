#include "acpi.h"
#include "apic.h"
#include "memory.h"
#include "memory_utils.h"
#include "multiboot.h"
#include "util.h"

#define RSDP_MAGIC ("RSD PTR ")
#define RSDP_SEARCH_START (0x000E0000 + KERNEL_OFFSET)
#define RSDP_SEARCH_END (0x000FFFFF + KERNEL_OFFSET)

#define MADT_PROC_LAPIC          0
#define MADT_IOAPIC              1
#define MADT_IOAPIC_INT_SRC_OVR  2
#define MADT_IOAPIC__NMI_SRC     3
#define MADT_LAPIC_NMI           4
#define MADT_LAPIC_ADDR_OVR      5
#define MADT_PROC_LAPIC_X2       9

extern struct KernelBootData kernel_boot_data;

void parse_fadt(struct FADT* fadt);
void parse_madt(struct MADT* madt);
void parse_hpet(struct HPET* hpet);
void parse_waet(struct WAET* waet);

struct RSDP_t* get_rsdp() {
    return kernel_boot_data.rsdp;
}

void find_rsdp() {
    uint64_t start = RSDP_SEARCH_START;
    // todo validate checksum
    while(memcmp(start, RSDP_MAGIC, 8) && (start < RSDP_SEARCH_END)) {
        start += 0x10;
    }

    kernel_boot_data.rsdp = (start < RSDP_SEARCH_END) ? start : NULL;
}

void parse_rsdt(struct RSDT* rsdt) {
    uint32_t rsdt_entries_count = (rsdt->header.Length - sizeof(struct ACPISDTHeader))/4;


    for(uint32_t i = 0; i < rsdt_entries_count; i++) {
        struct ACPISDTHeader* entry = ((struct ACPISDTHeader*)P2V(*((uint32_t*)((uint8_t*)rsdt + sizeof(struct ACPISDTHeader) + 4*i))));
        printf("RSDT Entry #%d: %c%c%c%c\n", i, entry->Signature[0], entry->Signature[1], entry->Signature[2], entry->Signature[3]);

        if(!memcmp(entry->Signature, "FACP", 4)) {
            // found FADT
            struct FADT* fadt = (struct FADT*) entry;
            parse_fadt(fadt);

        } else if(!memcmp(entry->Signature, "APIC", 4)) {
            // found MADT
            struct MADT* madt = (struct MADT*) entry;
            parse_madt(madt);
            
        } else if(!memcmp(entry->Signature, "HPET", 4)) {
            // found HPET
            struct HPET* hpet = (struct HPET*) entry;
            parse_hpet(hpet);
        } else if(!memcmp(entry->Signature, "WAET", 4)) {
            // found WAET - The WAET (Windows ACPI Emulated Devices Table) is a table in ACPI running in guest partitions in a virtual machine environment.
            struct WAET* waet = (struct WAET*) entry;
            parse_waet(waet);
        } else {
            printf("Unknown RSDT entry! Signature: %c%c%c%c\n", entry->Signature[0], entry->Signature[1], entry->Signature[2], entry->Signature[3]);
            panic();
        }
    }

}

void parse_fadt(struct FADT* fadt){
    // todo implement
}

void parse_madt(struct MADT* madt) {
    uint64_t phaddr_lapic = madt->LocalApicAddress;
    init_apic(phaddr_lapic);
    printf("Local APIC found at address %p\n", phaddr_lapic);
    printf("SIGNATURE: %c%c%c%c, MADT Entries:\n", madt->header.Signature[0], madt->header.Signature[1], madt->header.Signature[2], madt->header.Signature[3]);
    
    for (struct MADTEntry* madt_entry = madt->entries; 
            ((uint8_t*)madt_entry < (uint8_t*)madt + madt->header.Length) && madt_entry->length; 
            madt_entry = (struct MADTEntry*)((uint8_t*)madt_entry + madt_entry->length)
        ) {
        switch(madt_entry->type) {
            case MADT_PROC_LAPIC:
                printf("Processor Local APIC. Processor ID: %d, APIC ID: %d, flags: 0x%x\n", madt_entry->proc.proc_id, madt_entry->proc.APIC_id, madt_entry->proc.flags);
                break;
            case MADT_IOAPIC:
                printf("I/O APIC. APIC ID: %d, APIC address: 0x%x, base: 0x%x\n", madt_entry->IOAPIC.APIC_id, madt_entry->IOAPIC.address, madt_entry->IOAPIC.base);
                break;
            case MADT_IOAPIC_INT_SRC_OVR:
                printf("I/O APIC interrupt src override. Bus: 0x%x, IRQ: 0x%x, Interrupt: 0x%x, flags: 0x%x\n", madt_entry->ISO.bus, madt_entry->ISO.irq, madt_entry->ISO.interrupt, madt_entry->ISO.flags);
                break;
            case MADT_IOAPIC__NMI_SRC:
                printf("I/O APIC Non-maskable interrupt source\n");
                panic();
                break;
            case MADT_LAPIC_NMI:
                printf("Local APIC Non-maskable interrupts. ACPI Processor ID: 0x%x, flags: 0x%x, lint: %d\n", madt_entry->LAPIC_NMI.proc_id, madt_entry->LAPIC_NMI.flags, madt_entry->LAPIC_NMI.lint);
                break;
            case MADT_LAPIC_ADDR_OVR:
                printf("Local APIC Address Override\n");
                panic();
                break;
            case MADT_PROC_LAPIC_X2:
                printf("Processor Local x2APIC\n");
                panic();
                break;
            default:
                printf("Unknown MADT entry found!\n");
                panic();
                break;
        }
    }
}

void parse_hpet(struct HPET* hpet) {
    // todo implement
}

void parse_waet(struct WAET* waet) {
    // todo implement
    printf("Waet found. RTC Good: %d, ACPI PM Timer Good: %d\n", waet->RTCGood, waet->ACPIPMTimerGood);
}