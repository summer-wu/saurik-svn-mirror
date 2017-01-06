ENTRY (_loader)

SECTIONS {
    . = 0x00100000;

    .boot : {
        LD_BOOT_BEGIN = .;
        *(.boot)
    }

    .text : {
        *(.text)
        *(.rodata)
    }

    .data ALIGN (0x1000) : {
        *(.data)

        LD_CTORS_BEGIN = .;
        LONG((LD_CTORS_END - LD_CTORS_BEGIN) / 4 - 1)
        *(.ctors)
        LD_CTORS_END = .;
        LONG(0)

        LD_DTORS_BEGIN = .;
        LONG((LD_DTORS_END - LD_DTORS_BEGIN) / 4 - 1)
        *(.dtors)
        LD_DTORS_END = .;
        LONG(0)

        LD_DATA_END = .;
    }

    .bss : {
        *(COMMON)
        *(.bss)
        LD_BSS_END = .;
    }
}
