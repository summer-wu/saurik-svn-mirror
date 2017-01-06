#ifndef LANGUAGE_HPP
#define LANGUAGE_HPP

#define _nothrow __attribute__((nothrow))
#define _forever for (;;)
#define _arraylen(array) sizeof(array) / sizeof(array[0])

#ifdef __GNUC__
    #define _array(type, name, size) type name[size];
#else
    #define _array(type, name, size) type *name = reinterpret_cast<type *>(alloca(size * sizeof(type)));
#endif

#if defined(CFG_NO_FINLINE)
    #define _finline
#elif defined(_MSC_VER)
    #define _finline __forceinline
#elif defined(__GNUC__)
    #define _finline inline __attribute__((always_inline))
#else
    #define _finline
#endif

_finline void _breakpoint() {
    #if defined(_MSC_VER) && defined(_M_IX86)
        __asm { int 0x03 };
    #elif defined(__GNUC__) && defined(__i386__)
        __asm__ ("int $0x03");
    #elif defined(__GNUC__) && defined(_ARCH_PPC)
        __asm__ ("twge r2, r2");
    #else
        *static_cast<int *>(NULL) = 0;
    #endif
}

#define _not(type) ~type()

#endif//LANGUAGE_HPP
