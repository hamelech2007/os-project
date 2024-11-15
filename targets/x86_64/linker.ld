ENTRY(_start)

SECTIONS
{
    . = 1M;

    .multiboot_header :
    {   
        KEEP(*(.multiboot_header))
        . = ALIGN(4K);
    }

    .boot :
    {   
        *(.boot)
        . = ALIGN(4K);
    }

    .note.gnu.build-id BLOCK(4K) :
    { 
        *(.note.gnu.build-id)
        . = ALIGN(4K);
    }

    .protected_memory :
    {
        *(.protected_memory)
        . = ALIGN(4K);
    }

    . += 0xffffffff80000000;

    _kernel_start = .;

    .text BLOCK(4K) : AT(ADDR(.text) - 0xffffffff80000000)
    {   
        *(.text)
        . = ALIGN(4K);
    }

    
    .rodata BLOCK(4K) : AT(ADDR(.rodata) - 0xffffffff80000000)
    {
        *(.rodata)
        . = ALIGN(4K);
    }

    .data BLOCK(4K) : AT(ADDR(.data) - 0xffffffff80000000)
    {
        *(.data)
        . = ALIGN(4K);
    }

    .bss BLOCK(4K) : AT(ADDR(.bss) - 0xffffffff80000000)
    {
        *(COMMON)
        *(.bss)
        . = ALIGN(4K);
    }


    _kernel_end = .;

}