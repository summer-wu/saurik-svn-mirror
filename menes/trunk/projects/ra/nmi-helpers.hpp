#ifndef RA_NMI_HELPERS_HPP
#define RA_NMI_HELPERS_HPP

#include "x86-helpers.hpp"

inline void EnableNmi() {
    SetPortFlag8(0x70, 0x80);
}

inline DisableNmi() {
    ResetPortFlag8(0x70, 0x80);
}

#endif//RA_NMI_HELPERS_HPP
