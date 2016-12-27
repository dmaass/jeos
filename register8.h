#pragma once
#include "Register.h"

class Register8 :
    public Register<uint8_t> {
public:

    Register8() {}
    ~Register8() {}

    uint8_t shift_left(const int& count=1) {
        //8bit register -> look for first byte of 1000 0000 -> 0x80
        uint8_t carry = (value & 0x80) >> 7;
        value <<= count;
        return carry;
    }
};

