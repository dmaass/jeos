#pragma once
#include "Register.h"

class Register16 :
    public Register<uint16_t>
{
public:

    Register16() {
    }

    ~Register16() {
    }

    uint8_t getHI() {
        return ((value >> 8) & 0xFF);
    }

    uint8_t getLO() {
        return (value & 0xFF);
    }

    uint16_t shift_left(const int& count) {
        //16bit register -> look for first byte of 1000 0000 0000 0000-> 0x8000
        uint16_t carry = value & 0x8000 >> 15;
        value <<= count;
        return carry;
    }
};
