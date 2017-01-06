#include "cxx/platform.hpp"

extern void *LD_BOOT_BEGIN[];
extern void *LD_BSS_END[];
extern void *LD_DATA_END[];

extern "C" int main();

static const uint32_t BootMagic_(0x1badb002);
static const uint32_t BootFlags_(0x00010000);

uint32_t multiboot_[] __attribute__((section(".boot"))) = {
    BootMagic_,
    BootFlags_,
    -(BootMagic_ + BootFlags_),
    reinterpret_cast<uint32_t>(&LD_BOOT_BEGIN),
    reinterpret_cast<uint32_t>(&LD_BOOT_BEGIN),
    reinterpret_cast<uint32_t>(&LD_DATA_END),
    reinterpret_cast<uint32_t>(&LD_BSS_END),
    reinterpret_cast<uint32_t>(&main)
};
