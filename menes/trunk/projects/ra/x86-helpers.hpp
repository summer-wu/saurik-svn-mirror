#ifndef RA_X86_HELPERS_HPP
#define RA_X86_HELPERS_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

namespace x86 {

class IoAddress;

class IoReference {
  private:
    const IoAddress &address_;

  public:
    _finline IoReference(const IoAddress &address) :
        address_(address)
    {
    }

    _finline IoReference &operator =(uint8_t rhs) {
        __asm__ volatile ("outb %0, %1" : : "a" (rhs), "Nd" (address_));
        return *this;
    }

    _finline IoReference &operator =(uint16_t rhs) {
        __asm__ volatile ("outw %0, %1" : : "a" (rhs), "Nd" (address_));
        return *this;
    }

    _finline IoReference &operator =(uint32_t rhs) {
        __asm__ volatile ("out %0, %1" : : "a" (rhs), "Nd" (address_));
        return *this;
    }

    _finline operator uint8_t() const {
        uint8_t value;
        __asm__ volatile ("inb %1, %0" : "=a" (value) : "Nd" (address_));
        return value;
    }

    _finline operator uint16_t() const {
        uint8_t value;
        __asm__ volatile ("inw %1, %0" : "=a" (value) : "Nd" (address_));
        return value;
    }

    _finline operator uint32_t() const {
        uint8_t value;
        __asm__ volatile ("in %1, %0" : "=a" (value) : "Nd" (address_));
        return value;
    }
};

class IoAddress {
  private:
    uint16_t address_;

  public:
    _finline IoAddress(uint16_t address) :
        address_(address)
    {
    }

    _finline IoReference operator *() const {
        return IoReference(*this);
    }
};

class Chip8259A {
  private:
    IoAddress base_;

  public:
    Chip8259A(IoAddress base) :
        base_(base)
    {
    }

    _finline void Program(uint8_t icw1, uint8_t icw2, uint8_t icw3, uint8_t icw4) {
        base_[0] = icw1;
        base_[1] = icw1;
        base_[1] = icw2;
        base_[1] = icw3;
        base_[1] = icw4;
    }

    _finline void EndOfInterrupt() {
        base_[1] = 0x20;
    }

    _finline uint8_t GetMask() const {
        return base_[1];
    }

    _finline void SetMask(uint8_t mask) {
        base_[1] = mask;
    }

    _finline void Mask(uint8_t interrupt) {
        base_[1] |= 1 << interrupt;
    }

    _finline void Unmask(uint8_t interrupt) {
        base_[1] &= ~(1 << interrupt);
    }
};

extern Chip8259A Master8259A;
extern Chip8259A Slave8259A;

_finline uint16_t GetInterruptMask() {
    return Slave8259A.GetMask() << 8 | Master8259A.GetMask();
}

_finline void SetInterruptMask(uint16_t mask) {
    Master8259A.SetMask(mask & 0xff);
    Slave8259A.SetMask(mask >> 8 & 0xff);
}

_finline void UnmaskInterrupt(uint8_t irq) {
    if (irq < 8)
        Master8259A.Unmask(irq);
    else
        Slave8259A.Unmask(irq - 8);
}

_finline void MaskInterrupt(uint8_t irq) {
    if (irq < 8)
        Master8259A.Mask(irq);
    else
        Slave8259A.Mask(irq - 8);
}

/*_finline void io_wait(void) {
   __asm__ volatile ("outb %%al,$0x80");
   // port 0x80 is used for 'checkpoints' during POST.
   // linux kernel seems to think it's free for use :-/
}

_finline int irqEnabled() {
    int f;
    __asm__ volatile ("pushf;popl %0":"=g" (f));
    return f & (1<<9);
}

_finline void irqUnlock(int no) {
    if (no >= 8)
        outb(0x20,0xa0);
    outb(0x20,0x20);
}

_finline void lidt(void *base, unsigned int limit) {
    unsigned int i[2];

    i[0] = limit << 16;
    i[1] = (unsigned int) base;
    asm ("lidt (%0)": :"p" (((char *) i)+2));
}

_finline void cpuid(int code, unsigned *a, unsigned *d) {
    __asm__ volatile ("cpuid" : "=a" (*a), "=d" (*d) : "0" (code));
}

_finline unsigned read_cr0() {
    unsigned val;
    __asm__ volatile("mov %%cr0, %0" : "=r" (val));
    return val;
}

_finline void pgFlushOneTlb(void *m) {
    __asm__ volatile("invlpg %0" : : "m" (*reinterpret_cast<char *>(m)));
}*/

}

#endif//RA_X86_HELPERS_HPP
