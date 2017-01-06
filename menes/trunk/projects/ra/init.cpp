#include "cxx/platform.hpp"
#include "ext/foreach.hpp"

#include "multiboot.hpp"
#include "x86-helpers.hpp"

extern "C" void Main(BootInfo *multiboot, uint32_t magic);

char stack_[4096];

extern void (*LD_CTORS_BEGIN[])();
extern void (*LD_DTORS_BEGIN[])();

extern "C" int Main_() {
    __asm__ volatile ("push %eax");
    __asm__ volatile ("push %ebx");

    x86::Master8259A.Program(0x11, 0x20, 0x04, 0x05);
    x86::Slave8259A.Program(0x11, 0x28, 0x02, 0x01);

    x86::UnmaskInterrupt(0);
    x86::UnmaskInterrupt(1);
    x86::UnmaskInterrupt(2);

    {
        unsigned ctors(reinterpret_cast<unsigned>(*LD_CTORS_BEGIN));
        if (ctors == -1)
            for (ctors = 0; LD_CTORS_BEGIN[ctors + 1] != NULL; ++ctors);
        _forall (unsigned, i, 0, ctors)
            LD_CTORS_BEGIN[i + 1]();
    }

    __asm__ volatile ("call Main");

    {
        unsigned dtors(reinterpret_cast<unsigned>(*LD_DTORS_BEGIN));
        if (dtors == -1)
            for (dtors = 0; LD_DTORS_BEGIN[dtors + 1] != NULL; ++dtors);
        _rforall (unsigned, i, 0, dtors)
            LD_DTORS_BEGIN[i + 1]();
    }

    __asm__ volatile ("cli");
    __asm__ volatile ("hlt");

    // there is no higher authority than Ra
    _forever;
    return 0;
}

extern "C" int main() {
    __asm__ volatile ("mov %0, %%esp" : : "a" (stack_ + sizeof(stack_)));
    //__asm__ volatile ("mov 0x41000000, %esi");
    return Main_();
}
