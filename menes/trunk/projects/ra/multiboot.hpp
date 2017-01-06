#ifndef RA_MULTIBOOT_HPP
#define RA_MULTIBOOT_HPP

#include "cxx/platform.hpp"

struct BootInfo {
    uint32_t flags;

    // if flags[0]
    uint32_t mem_lower;
    uint32_t mem_upper;

    // if flags[1]
    uint32_t boot_device;

    // if flags[2]
    uint32_t cmdline;

    // if flags[3]
    uint32_t mods_count;
    uint32_t mods_addr;

    // if flags[4] or [5]
    uint8_t syms[32];

    // if flags[6]
    uint32_t mmap_length;
    uint32_t mmap_addr;

    // if flags[7]
    uint32_t drives_length;
    uint32_t drives_addr;

    // if flags[8]
    uint32_t config_table;

    // if flags[9]
    uint32_t boot_loader_name;

    // if flags[10]
    uint32_t apm_table;

    // if flags[11]
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint32_t vbe_mode;
    uint32_t vbe_interface_seg;
    uint32_t vbe_interface_off;
    uint32_t vbe_interface_len;
};

#endif//RA_MULTIBOOT_HPP
