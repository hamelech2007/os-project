.section .multiboot_header
.align 4
header_start:
    # magic number
    .int 0xe85250d6 # multiboot2
    # architecture
    .int 0 # protected mode i386
    # header length
    .int header_end-header_start
    # checksum
    .int 0x100000000 - (0xe85250d6 + 0 + (header_end-header_start))

    # end tag
    .short 0
    .short 0
    .int 8
header_end: