.section .multiboot_header
.align 4
header_start:
    # magic number
    .int 0xe85250d6                # Multiboot2 magic number
    # architecture
    .int 0                         # Protected mode i386 (also works for x86_64 in your case)
    # header length
    .int header_end - header_start # Total header length
    # checksum
    .int 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    # Memory information tag
    .int 0x00000006                # Tag type: memory map
    .int 16                        # Tag size: 16 bytes (size field + tag fields)

    # Page align tag
    .int 0x00000001                # Tag type: align modules on page boundaries
    .int 8                         # Tag size: 8 bytes (size field + tag fields)

    # End tag (mandatory)
    .int 0x00000000                # End tag type (0)
    .int 8                         # End tag size (8 bytes)

header_end: