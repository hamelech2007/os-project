#include "gdt.h"
#include "memory.h"
#include "print.h"

extern void error();
extern void long_jump_new_gdt();
struct gdt_entry gdt64[8];
struct Tss64 tss;
struct gdt_ptr gdt64_ptr;


void initGdt() {
 
    gdt64_ptr.base = (uint64_t) &gdt64;
    gdt64_ptr.limit = sizeof(gdt64) - 1;

    // Null segment
    setGdtGate(0, 0, 0, 0, 0);

    // Kernel code segment (64-bit, execute, read)
    setGdtGate(1, 0, 0, 0x9A, 0xA0); // Notice the 0xA0 for 64-bit granularity

    // Kernel data segment (read/write)
    setGdtGate(2, 0, 0, 0x92, 0xCF);

    // User code segment (64-bit, execute, read)
    setGdtGate(3, 0, 0, 0xFA, 0xA0); // 64-bit user mode code segment

    // User data segment (read/write)
    setGdtGate(4, 0, 0, 0xF2, 0xCF);


    // tss takes 2 entries in gdt so instead of 6 entries we have 7, little endian form
    // extra space for safety
    memset(&tss, 0, sizeof(tss));
    setTssEntry(5,6);

    // Load the GDT
    asm volatile("lgdt (%0)" : : "r" (&gdt64_ptr));

    // Now you can perform a far jump to the new code segment to update CS
    long_jump_new_gdt();
}

void setTssEntry(uint32_t num_low, uint32_t num_high) {
    struct gdt_entry tss_low = gdt64[num_low];
    struct gdt_entry tss_high = gdt64[num_high];
    uint64_t tss_addr = (uint64_t) &tss;
    uint32_t limit = sizeof(struct Tss64) - 1;

    memset(&tss_high, 0, sizeof(struct gdt_entry));
    memset(&tss_low, 0, sizeof(struct gdt_entry));
    tss.iomap = 0xdfff; // for now, point beyond tss limit (no iomap)
    //handle lower 32 bits
    tss_low.limit = (uint16_t) limit;
    tss_low.base_low = (uint16_t) tss_addr;
    tss_low.base_middle = (uint8_t) (tss_addr >> 16);
    tss_low.base_high = (uint8_t) (tss_addr >> 24);

    tss_low.flags = (uint8_t)((limit >> 16) & 0x0F);
    tss_low.flags |= (uint8_t)(((1 << 3) & (1 << 1)) & 0xF0); // Granularity flag, Long mode flag
    tss_low.access = (1 << 7) | (0x9); // present flag | 64-bit TSS (Available)

    // handle upper 32 bits
    tss_high.limit = (uint16_t)(tss_addr >> 32);
    tss_high.base_low = (uint16_t)(tss_addr >> 48);
    
}

void setGdtGate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt64[num].base_low = base & 0xFFFF;
    gdt64[num].base_middle = (base >> 16) & 0xFF;
    gdt64[num].base_high = (base >> 24) & 0xFF;

    gdt64[num].limit = (limit & 0xFFFF);
    gdt64[num].flags = (limit >> 16) & 0x0F;
    gdt64[num].flags |= (gran & 0xF0);

    gdt64[num].access = access;
}