#include "cxx/platform.hpp"
#include "ext/foreach.hpp"

#include "multiboot.hpp"
#include "vga-helpers.hpp"
#include "x86-helpers.hpp"

extern void InterruptHandler();

/*uint8_t idt_[8] = {
    reinterpret_cast<uint32_t>(&InterruptHandler) >>  0 & 0xff,
    reinterpret_cast<uint32_t>(&InterruptHandler) >>  8 & 0xff,
    ,
    ,
    0,

    reinterpret_cast<uint32_t>(&InterruptHandler) >> 16 & 0xff,
    reinterpret_cast<uint32_t>(&InterruptHandler) >> 24 & 0xff
};*/

void InterruptHandler() {
}

struct Cell {
    char value_;
    char attributes_;
};

Cell *console_(reinterpret_cast<Cell *>(0xb8000));

unsigned value_;

void CpuIdentify() {
    union {
        char vendor[13];
        uint32_t vendor32[3];
    };

    uint32_t levels;

    __asm__ ("cpuid" : "=a" (levels), "=b" (vendor32[0]), "=c" (vendor32[2]), "=d" (vendor32[1]) : "a" (0));

    vendor[12] = '\0';

    _forall (unsigned, i, 0, 12)
        console_[200 + i].value_ = vendor[i];
}

extern "C" void Main(BootInfo *multiboot, uint32_t magic) {
    _forall (unsigned, i, 0, 25 * 80)
        console_[i].value_ = ' ';

    CpuIdentify();

    SetCursorPosition(1, 0);
    //SetFrameOffset(5);

    console_[0].value_ = '0';
    console_[1].value_ = 'x';

    console_[163].value_ = 'a';

    _forall (unsigned, i, 0, 8) {
        unsigned dec((value_ >> ((8 - i - 1) * 4)) & 0xf);
        char hex((dec < 10 ? '0' : 'a' - 10) + dec);
        console_[i + 2].value_ = hex;
    }
}
