ENTRY(_start)

SECTIONS
{
    . = 1M;

    /*.boot :
    {
        KEEP(*(.multiboot_header))
    }*/

    .text :
    {   
        KEEP(*(.multiboot_header))
        *(.text)
    }
}