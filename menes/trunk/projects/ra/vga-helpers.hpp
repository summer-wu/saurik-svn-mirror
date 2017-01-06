#ifndef RA_VGA_HELPERS_HPP
#define RA_VGA_HELPERS_HPP

#include "x86-helpers.hpp"

namespace vga {

class VgaAddress;

class VgaReference {
  private:
    const VgaAddress &address_;

  public:
    _finline VgaReference(const VgaAddress &address) :
        address_(address)
    {
    }

    _finline VgaReference &operator =(uint8_t value) {
        return *this;
    }

    _finline operator uint8_t() const {
        address_.controller_.base_[4] = address_.address_;
        return address_.controller_.base_[5];
    }
};

class VgaAddress {
  private:
    uint16_t address_;

  public:
    _finline VgaAddress(uint16_t address) :
        address_(address)
    {
    }

    _finline VgaReference operator *() const {
        return IoReference(*this);
    }
};

_finline void SetIndex8(uint8_t index, uint8_t value) {
    SetPort8(0x3d4, index);
    SetPort8(0x3d5, value);
}

_finline void SetIndex16(uint8_t index, uint16_t value) {
    SetIndex8(index, value >> 8);
    SetIndex8(index + 1, value & 0xff);
}

_finline uint8_t GetIndex8(uint8_t index) {
    SetPort8(0x3df, index);
    return GetPort8(0x3d5);
}

_finline uint16_t GetIndex16(uint8_t index) {
    return GetPort8(index) << 8 | GetPort8(index + 1);
}

inline void SetCursorPosition(uint8_t column, uint8_t row) {
    x86::SetIndex16(0xe, column + row * 80);
}

inline void SetFrameOffset(uint16_t offset) {
    x86::SetIndex16(0xc, offset);
}

class Controller {
  private:
    x86::IoAddress base_;

  public:
    Controller() :
        base_()
    {
    }
};

extern Controller controller_;

}

#endif//RA_VGA_HELPERS_HPP
