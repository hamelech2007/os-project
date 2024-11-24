#include "ahci.h"
#include "pci.h"
#include "stdint.h"
#include "stdio.h"
#include "print.h"
#include "memory.h"
#include "kheap.h"
#include "memory_utils.h"
#include "util.h"
#include "fat32.h"

struct HBA_MEM* ahci_base_vaddr = NULL;

void stop_cmd(volatile struct HBA_PORT *port) {
    port->cmd &= ~HBA_PxCMD_ST;
    port->cmd &= ~HBA_PxCMD_FRE;

    while(true) {
        if(port->cmd & (HBA_PxCMD_FR | HBA_PxCMD_CR)) continue;
        break;
    }

}

void start_cmd(volatile struct HBA_PORT *port) {
    while(port->cmd & HBA_PxCMD_CR);

    port->cmd |= HBA_PxCMD_FRE;
    port->cmd |= HBA_PxCMD_ST;
}



void enable_ahci_pci(uint8_t bus, uint8_t device, uint8_t function) {
    // Read current command register value
    uint32_t command = pci_read_dword(bus, device, function, 0x04);
    
    // Set bit 1 (Memory Space Enable) and bit 2 (Bus Master Enable)
    command |= (1 << 1) | (1 << 2);
    
    // Write back the modified command
    pci_write_dword(bus, device, function, 0x04, command);
}

void set_ahci_mode(uint8_t bus, uint8_t device, uint8_t function) {
    // Read current value of the PCI class code register
    uint32_t class_reg = pci_read_dword(bus, device, function, 0x08);
    
    // Check if already in AHCI mode
    if ((class_reg >> 8) == 0x0106) {  // SATA controller in AHCI mode
        return;
    }
    
    // Set to AHCI mode if supported
    // Note: This is vendor-specific and might not work on all controllers
    // You should check vendor ID and device ID first
    uint32_t sata_mode = pci_read_dword(bus, device, function, 0x90);
    sata_mode |= (1 << 31);  // Set AHCI enable bit (vendor-specific)
    pci_write_dword(bus, device, function, 0x90, sata_mode);
}

void setup_msi(uint8_t bus, uint8_t device, uint8_t function, uint32_t message_address, uint16_t message_data) {
    // Find MSI capability
    uint32_t cap_ptr = pci_read_dword(bus, device, function, 0x34) & 0xFF;
    
    while (cap_ptr) {
        uint32_t cap = pci_read_dword(bus, device, function, cap_ptr);
        if ((cap & 0xFF) == 0x05) {  // MSI capability ID
            // Configure MSI
            pci_write_dword(bus, device, function, cap_ptr + 0x4, message_address);
            pci_write_dword(bus, device, function, cap_ptr + 0x8, message_data);
            

            // Enable MSI
            cap |= (1 << 16);  // Enable MSI
            pci_write_dword(bus, device, function, cap_ptr, cap);
            break;
        }
        cap_ptr = (cap >> 8) & 0xFF;  // Next capability pointer
    }
}

bool init_port(volatile struct HBA_PORT *port) {
    stop_cmd(port);

    void* original_cmd_list = aligned_kmalloc(1024, 1024);
    if(!original_cmd_list) return;
    void* original_fis_base = aligned_kmalloc(256, 256);
    if(!original_fis_base) {
        kfree(original_cmd_list);
        return;
    }


    void* cmd_list = align_allocated(original_cmd_list, 1024);
    memset(cmd_list, 0, 1024);

    void* fis_base = align_allocated(original_fis_base, 256);
    memset(fis_base, 0, 256);

    port->clb = (uint32_t) V2P(cmd_list);
    port->clbu = (uint32_t) (V2P(cmd_list) >> 32); // I am not sure whether or not this needs to be the virtual address or the physical address

    port->fb = (uint32_t) V2P(fis_base);
    port->fbu = (uint32_t) (V2P(fis_base) >> 32);

    

    
}

int get_port_type(volatile struct HBA_PORT *port) {
    uint8_t det = port->ssts & 0x0f;
    uint8_t ipm = (port->ssts >> 8) & 0x0f;

    if(det != HBA_PORT_DET_PRESENT || ipm != HBA_PORT_IPM_ACTIVE) return AHCI_DEV_NULL;

    switch(port->sig) {
        case SATA_SIG_ATAPI:
            return AHCI_DEV_SATAPI;
        case SATA_SIG_SEMB:
            return AHCI_DEV_SEMB;
        case SATA_SIG_PM:
            return AHCI_DEV_PM;
        default:
            return AHCI_DEV_SATA;
    }
}

void rebase_ahci_port(struct HBA_PORT *port) {
    stop_cmd(port);

    void* clb = aligned_kmalloc(1024, 1024);
    if(!clb) panic();
    port->clb = V2P(align_allocated(clb, 1024));
    port->clbu = 0;
    memset(align_allocated(clb, 1024), 0, 1024);

    void* fb = aligned_kmalloc(256, 256);
    if(!fb) panic();
    port->fb = V2P(align_allocated(fb, 256));
    port->fbu = 0;
    memset(align_allocated(fb, 256), 0, 256);

    struct HBA_CMD_HEADER *cmdheader = align_allocated(clb, 1024);
    for(int i = 0; i < 32; i++) {
        cmdheader[i].prdtl = 1;

        void* ctba = aligned_kmalloc(256, 256);
        if(!ctba) panic();
        cmdheader[i].ctba = V2P(align_allocated(ctba, 256));
        cmdheader[i].ctbau = 0;
        memset(align_allocated(ctba, 256), 0, 256);
    }

    start_cmd(port);
}

int find_cmdslot(struct HBA_PORT *port) {
    uint32_t slots = (port->sact | port->ci);
    for(int i = 0; i < 32; i++) {
        if((slots&1) == 0) return i;
        slots >>= 1;
    }
    
    printf("Couldn't find free command slot\n");
    panic();
    return -1;
}

bool ahci_read_sectors(struct HBA_PORT *port, uint64_t start, uint32_t sectors, uint16_t *buf) {
    port->is = (uint32_t) -1;
    uint32_t spinlock = 0;
    int slot = find_cmdslot(port);
    if(slot == -1) panic();

    struct HBA_CMD_HEADER *cmdheader = P2V(port->clb);
    cmdheader += slot;
    cmdheader->cfl = sizeof(struct FIS_REG_H2D)/sizeof(uint32_t);
    cmdheader->w = 0;
    cmdheader->prdtl = (uint16_t)((sectors - 1) >> 4) + 1;

    struct HBA_CMD_TBL *cmdtbl = P2V(cmdheader->ctba);
    memset(cmdtbl, 0, sizeof(struct HBA_CMD_TBL) + (cmdheader->prdtl-1)*sizeof(struct HBA_PRDT_ENTRY));
    

    for(int i = 0; i < cmdheader->prdtl - 1; i++) {
        cmdtbl->prdt_entry[i].dba = V2P(buf);
        cmdtbl->prdt_entry[i].dbau = 0;
        cmdtbl->prdt_entry[i].dbc = 8*1024 - 1;
        cmdtbl->prdt_entry[i].i = 1;
        buf += 4 * 1024;
        sectors -= 16;
    }

    if(cmdheader->prdtl != 1) {

        // already setup technically everything to be able to support for than 16 sectors but need to make sure that in rebase_port enough space has been allocated to support more than one prdt
        printf("Error, can only read and write to and from a single prdtl.");
        panic();
    }

    cmdtbl->prdt_entry[cmdheader->prdtl - 1].dba = V2P(buf);
    cmdtbl->prdt_entry[cmdheader->prdtl - 1].dbau = 0;
    cmdtbl->prdt_entry[cmdheader->prdtl - 1].dbc = (sectors << 9) - 1;
    cmdtbl->prdt_entry[cmdheader->prdtl - 1].i = 1;

    struct FIS_REG_H2D *cmdfis = &cmdtbl->cfis;

    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1;
    cmdfis->command = ATA_CMD_READ_DMA_EX; // TODO FIX

    cmdfis->lba0 = (uint8_t) start;
    cmdfis->lba1 = (uint8_t) (start >> 8);
    cmdfis->lba2 = (uint8_t) (start >> 16);
    cmdfis->device = 1 << 6;

    cmdfis->lba3 = (uint8_t) (start >> 24);
    cmdfis->lba4 = (uint8_t) (start >> 32);
    cmdfis->lba5 = (uint8_t) (start >> 40);

    cmdfis->countl = sectors & 0xff;
    cmdfis->counth = (sectors >> 8) & 0xff;

    while((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spinlock < 1000000) {
        spinlock++;
    }

    if(spinlock >= 1000000){
        return false;
    }

    port->ci = 1 << slot;

    while(true) {
        if((port->ci & (1 << slot)) == 0) break;
        if(port->is & HBA_PxIS_TFES) return false;
    }

    if(port->is & HBA_PxIS_TFES) return false;
    return true;
}

void parse_ahci(uint8_t bus, uint8_t device, uint8_t function) {
    struct PciHeader ahci;
    read_pci_header_type_0_1(&ahci, bus, device, function);

    if(ahci.Zero.header_type != 0x00 || ahci.Zero.class_code != 0x01 || ahci.Zero.subclass != 0x06 || ahci.Zero.prog_if != 0x01) {
        return;
    }

    uint32_t ahci_base = ahci.Zero.BAR5;
    ahci_base_vaddr = P2V(ahci_base);
    set_ahci_mode(bus, device, function);
    enable_ahci_pci(bus, device, function);
    ahci_base_vaddr->ghc |= 1 << 31; // AHCI Enable

    ahci_base_vaddr->ghc |= 1 << 0; // HBA Reset
    while(ahci_base_vaddr->ghc & (1 << 0)); // wait until reset is complete.


    for(int i = 0; i < 32; i++) {
        if(!(ahci_base_vaddr->pi & (1 << i))) continue;
        
        volatile struct HBA_PORT* port = &ahci_base_vaddr->ports[i];
        switch(get_port_type(port)) {
            case AHCI_DEV_SATA:
                printf("SATA drive found at port %d, rebasing...\n", i);
                rebase_ahci_port(port);
                if(get_sata_device() != NULL) {
                    printf("More than one single SATA device were found. This OS doesn't support it yet.\n");
                    panic();
                }

                set_sata_device(port);
                break;
            case AHCI_DEV_SATAPI:
                printf("SATAPI drive found at port %d\n", i);
                break;
            case AHCI_DEV_SEMB:
                printf("SEMB drive found at port %d\n", i);
                break;
            case AHCI_DEV_PM:
                printf("PM drive found at port %d\n", i);
                break;
            default:
                printf("No drive found at port %d\n", i);
        }
        
    }

    

    /*uint32_t pi = ahci_base_vaddr->pi;

    for (int i = 0; i < 32; i++) {
        if (pi & (1 << i)) {
            // Port i is implemented
            volatile struct HBA_PORT* port = &ahci_base_vaddr->ports[i];
            // Initialize port...
        }
    }*/

    print_set_color(PRINT_COLOR_YELLOW, PRINT_COLOR_BLACK);
    printf("AHCI Verified! BAR?: 0x%p, 0x%p, 0x%p, 0x%p, 0x%p, 0x%p\n", ahci.Zero.BAR0, ahci.Zero.BAR1, ahci.Zero.BAR2,  ahci.Zero.BAR3,  ahci.Zero.BAR4,  ahci.Zero.BAR5);
    print_set_color(PRINT_COLOR_WHITE, PRINT_COLOR_BLACK);

}
