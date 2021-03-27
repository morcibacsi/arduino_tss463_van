#include "tss46x_with_serial.h"

void Tss46xWithSerial::PrintValueToSerial(uint8_t value)
{
    char tmp[3];
    snprintf(tmp, 3, "%02X", value);
    _serial->print(tmp);
}

Tss46xWithSerial::Tss46xWithSerial(HardwareSerial *serial){
    _serial = serial;
}

Tss46xWithSerial::Tss46xWithSerial(HardwareSerial &serial){
    _serial = &serial;
}

void Tss46xWithSerial::init() {
}

void Tss46xWithSerial::register_set(uint8_t address, uint8_t value) {

    PrintValueToSerial(address);
    _serial->print(": ");

    PrintValueToSerial(value);
    _serial->println();
}

void Tss46xWithSerial::registers_set(uint8_t address, const uint8_t values[], uint8_t count) {

    PrintValueToSerial(address);
    _serial->print(": ");

    for (uint8_t i = 0; i < count; i++)
    {
        PrintValueToSerial(values[i]);
        _serial->print(" ");
    }

    _serial->println();
}

uint8_t Tss46xWithSerial::register_get(uint8_t address) {
    return 0x00;
}

uint8_t Tss46xWithSerial::registers_get(uint8_t address, volatile uint8_t values[], uint8_t count) {
    uint8_t value = 0;

    for (uint8_t i = 0; i < count; i++)
    {
        values[i] = 0x00;
    }

    return value;
}
