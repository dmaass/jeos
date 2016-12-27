#include "mos6502.h"

Mos6502::Mos6502()
{
    memory.resize(4096);
    //ADC - Add with Carry
    opcodes[0x69] = [&]() {ADC(adrImmediate()); cycleCounter+=2;};
    opcodes[0x65] = [&]() {ADC(adrZeroPage()); cycleCounter+=3;};
    opcodes[0x75] = [&]() {ADC(adrZeroPageX()); cycleCounter+=4;};
    opcodes[0x6D] = [&]() {ADC(adrAbsolute()); cycleCounter+=4;}; //XXX +1 if page crossed)
    opcodes[0x7D] = [&]() {ADC(adrAbsoluteX()); cycleCounter+=4;}; //XXX +1 if page crossed)
    opcodes[0x79] = [&]() {ADC(adrAbsoluteY()); cycleCounter+=4;}; //XXX +1 if page crossed)
    opcodes[0x61] = [&]() {ADC(adrIndirectX()); cycleCounter+=6;};
    opcodes[0x71] = [&]() {ADC(adrIndirectY()); cycleCounter+=5;}; //XXX +1 if page crossed)
    //AND - Logical AND
    opcodes[0x29] = [&]() {AND(adrImmediate()); cycleCounter+=2;};
    opcodes[0x25] = [&]() {AND(adrZeroPage()); cycleCounter+=3;};
    opcodes[0x35] = [&]() {AND(adrZeroPageX()); cycleCounter+=4;};
    opcodes[0x2D] = [&]() {AND(adrAbsolute()); cycleCounter+=4;}; //XXX +1 if page crossed)
    opcodes[0x3D] = [&]() {AND(adrAbsoluteX()); cycleCounter+=4;}; //XXX +1 if page crossed)
    opcodes[0x39] = [&]() {AND(adrAbsoluteY()); cycleCounter+=4;}; //XXX +1 if page crossed)
    opcodes[0x21] = [&]() {AND(adrIndirectX()); cycleCounter+=6;};
    opcodes[0x31] = [&]() {AND(adrIndirectY()); cycleCounter+=5;}; //XXX +1 if page crossed)
    //ASL - Arithmetic Shift Left
    opcodes[0x0A] = [&]() {ASL_ACC(); cycleCounter+=2;};
    opcodes[0x06] = [&]() {ASL(adrZeroPage()); cycleCounter+=5;};
    opcodes[0x16] = [&]() {ASL(adrZeroPageX()); cycleCounter+=6;};
    opcodes[0x0E] = [&]() {ASL(adrAbsolute()); cycleCounter+=6;};
    opcodes[0x1E] = [&]() {ASL(adrAbsoluteX()); cycleCounter+=7;}; 
    //BCC - Branch if Carry Clear
    opcodes[0x90] = [&]() {BCC(adrRelative()); cycleCounter+=2;}; //XXX +1 if success, +2 if to a new page
    //BCS - Branch if Carry Set
    opcodes[0xB0] = [&]() {BCS(adrRelative()); cycleCounter+=2;}; //XXX +1 if success, +2 if to a new page
    //BEQ - Branch if Equal
    opcodes[0xF0] = [&]() {BEQ(adrRelative()); cycleCounter+=2;}; //XXX +1 if success, +2 if to a new page
    //BIT - Bit Test
    opcodes[0x24] = [&]() {BIT(adrZeroPage()); cycleCounter+=3;};
    opcodes[0x2C] = [&]() {BIT(adrAbsolute()); cycleCounter+=4;};
}

uint16_t Mos6502::readMemory(const uint16_t &addr)
{
    return memory.at(addr);
}

void Mos6502::writeMemory(const uint16_t &addr, const uint8_t &value)
{
    memory.at(addr) = value;
}

uint16_t Mos6502::adrImplicit()
{
    //not used
    return 0;
}

uint16_t Mos6502::adrAccumulator()
{
    //work directly on the accumulator
    //return Acc.read();
    //not a memory location -> return 0
    return 0;    
}

uint16_t Mos6502::adrImmediate()
{
    //read value from within instruction
    return PC++;
}

uint16_t Mos6502::adrZeroPage()
{
    //read from zero paged memory
    return readMemory(PC++);
}

uint16_t Mos6502::adrZeroPageX()
{
    //read from memory accessed from PC + X
    return (readMemory(PC++) + X.read())%0xFF;
}

uint16_t Mos6502::adrZeroPageY()
{
    //read from memory accessed from PC + Y
    return (readMemory(PC++) + Y.read())%0xFF;
}

uint16_t Mos6502::adrRelative()
{
    int8_t offset = (int8_t)readMemory(PC++);
    return PC.read()+offset;
}

uint16_t Mos6502::adrAbsolute()
{
    //we need to read a 16bit address
    //LSB first
    uint8_t low = readMemory(PC++);
    uint8_t high = readMemory(PC++);
    return (high<<8)+low;
}

uint16_t Mos6502::adrAbsoluteX()
{
    //we need to read a 16bit address
    //LSB first
    uint8_t low = readMemory(PC++);
    uint8_t high = readMemory(PC++);
    return (high<<8)+low+X.read();
}

uint16_t Mos6502::adrAbsoluteY()
{
    //we need to read a 16bit address
    //LSB first
    uint8_t low = readMemory(PC++);
    uint8_t high = readMemory(PC++);
    return (high<<8)+low+Y.read();
}

uint16_t Mos6502::adrIndirect()
{
    //we need to read a 16bit address
    //LSB first
    uint8_t low = readMemory(PC++);
    uint8_t high = readMemory(PC++);
    uint16_t addr = (high<<8)+low;

    uint8_t newLow = readMemory(addr);
    uint8_t newHigh = readMemory(addr+1);
    return (newHigh<<8)+newLow;
}

uint16_t Mos6502::adrIndirectX()
{
    uint8_t low = readMemory(PC++) + X.read(); //todo: wrap around?
    uint8_t high = readMemory(PC++);
    return (high<<8)+low;
}

uint16_t Mos6502::adrIndirectY()
{
    uint8_t low = readMemory(PC++) + Y.read(); //todo: wrap around?
    uint8_t high = readMemory(PC++);
    return (high<<8)+low;
}

void Mos6502::ADC(uint16_t pos)
{
    uint16_t value = readMemory(pos);
    //TODO: check Decimalflag for correct addition
    uint16_t newAcc = Acc.read() + value + C.get();
    if(newAcc>0xFF) C.set();
    Acc.write(newAcc&0xFF);
    if (Acc.read()==0) Z.set(); else Z.unset();
    if(Acc.checkBit(7)) N.set(); else N.unset();
    if(newAcc<-128 || newAcc>127) V.set(); else V.unset();
}

void Mos6502::AND(uint16_t pos)
{
    uint16_t value = readMemory(pos);
    Acc.binary_and(value);
    if(Acc.read()==0) Z.set(); else Z.unset();
    if(Acc.checkBit(7)) N.set(); else N.unset();
}

void Mos6502::ASL(uint16_t pos)
{
    uint16_t value = readMemory(pos);
    if(value>>7 == 0x1) C.set(); else C.unset();
    value = (value<<1) & 0xFF;
    if(value>>7 == 0x1) N.set(); else N.unset();
    if(Acc.read()==0) Z.set(); else Z.unset();
    writeMemory(pos,value);
}

void Mos6502::ASL_ACC()
{
    if(Acc.checkBit(7)) C.set(); else C.unset();
    Acc.shift_left();
    if(Acc.checkBit(7)) N.set(); else N.unset();
    if(Acc.read()==0) Z.set(); else Z.unset();
}

void Mos6502::BCC(uint16_t pos)
{
    if(!C.get()) {
        PC.write(pos);
        //+1 as we jumped
        cycleCounter++;
        //XXX another +1 if jump to a new page?
    }
}

void Mos6502::BCS(uint16_t pos)
{
    if(C.get()) {
        PC.write(pos);
        //+1 as we jumped
        cycleCounter++;
        //XXX another +1 if jump to a new page?
    }
}

void Mos6502::BEQ(uint16_t pos)
{
    if(Z.get()) {
        PC.write(pos);
        //+1 as we jumped
        cycleCounter++;
        //XXX another +1 if jump to a new page?
    }
}

void Mos6502::BIT(uint16_t pos)
{
    uint16_t value = Acc.read() & readMemory(pos);
    if(value==0) Z.set(); else Z.unset();
    if(readMemory(pos) >> 6 == 0x1) V.set(); else V.unset();
    if(readMemory(pos) >> 7 == 0x1) N.set(); else N.unset();
}
