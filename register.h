#pragma once
#include <stdint.h> //For uint8_t etc

template<class T> class Register {

protected:
    T value;

public:

    Register() {}

    ~Register() {}

    const T& read()  const{
        return value;
    }

    void write(const T& v) {
        value = v;
    }

    void add(const T& v) {
        value += v;
    }

    void sub(const T& v) {
        value -= v;
    }

    void binary_or(const T& v) {
        value |= v;
    }

    void binary_and(const T& v) {
        value &= v;
    }

    void binary_xor(const T& v) {
        value ^= v;
    }

    T& operator++() {
        value++;
        return value;
    }

    T operator++(int) {
        T tmp(value);
        operator++();
        return tmp;
    }

    bool checkBit(int p) {
        return ((value>>p) == 0x1);
    }

    T shift_right(const int& count=1) {
        //carry is always the last bit (0x1) for shift_right
        T carry = value & 0x1;
        value >>= count;
        return carry;
    }

    //We can't implement as the carry differs from registersize to registersize
    virtual T shift_left(const int& count) = 0;
};
