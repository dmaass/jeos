#ifndef MOS6502_H
#define MOS6502_H
#include <vector>
#include <functional>
#include "register8.h"
#include "register16.h"
#include "flag.h"

class Mos6502
{
public:
    Mos6502();

private:
    std::vector<uint8_t> memory;
    //std::vector<void (Mos6502::*)()> opcodes;
    std::vector<std::function<void()>> opcodes;
    uint16_t readMemory(const uint16_t& addr);
    void writeMemory(const uint16_t& addr,const uint8_t& value);
    uint64_t cycleCounter;

    Register16 PC;
    Register8 Acc;
    Register8 X;
    Register8 Y;

    Flag C; //Carry Flag
    Flag Z; //Zero Flag
    Flag I; //Interrupt Flag
    Flag D; //Decimal Mode Flag
    Flag B; //Break Command
    Flag V; //Overflow Flag
    Flag N; //Negative Flag

    //AdressingModes
    uint16_t adrImplicit();
    uint16_t adrAccumulator();
    uint16_t adrImmediate();
    uint16_t adrZeroPage();
    uint16_t adrZeroPageX();
    uint16_t adrZeroPageY();
    uint16_t adrRelative();
    uint16_t adrAbsolute();
    uint16_t adrAbsoluteX();
    uint16_t adrAbsoluteY();
    uint16_t adrIndirect();
    uint16_t adrIndirectX();
    uint16_t adrIndirectY();

    //opcodes (generic onces)
    void ADC(uint16_t pos);
    void AND(uint16_t pos);
    void ASL(uint16_t pos);
    void ASL_ACC();
    void BCC(uint16_t pos);
    void BCS(uint16_t pos);
    void BEQ(uint16_t pos);
    void BIT(uint16_t pos);
};

#endif // MOS6502_H
