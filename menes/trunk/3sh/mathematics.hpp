#ifndef MATHEMATICS_HPP
#define MATHEMATICS_HPP

_finline int32_t ilog2f(float value) {
    return int32_t((*reinterpret_cast<uint32_t *>(&value) >> 23) & 0xFF) - 127;
}

inline int32_t ilog2(uint32_t value) {
    #if defined(_MSC_VER) && defined(_M_IX86)
        int32_t result;
        __asm bsr eax, value
        __asm mov result, eax
        return result;
    #elif defined(__GNUC__) && defined(__i386__)
        int32_t result;
        __asm__ ("bsr %1, %%eax; mov %%eax, %0;" : "=r"(result) : "r"(value) : "%eax");
        return result;
    #elif defined(__GNUC__) && defined(_ARCH_PPC)
        int32_t result;
        __asm__ ("cntlzd %0, %1" : "=r" (result) : "r" (value));
        return 63 - result;
    #else
        return ilog2f(float(value));
    #endif
}

#endif//MATHEMATICS_HPP
