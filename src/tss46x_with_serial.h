// tss46x_with_serial.h

#ifndef tss46x_with_serial
#define tss46x_with_serial

#include "tss46x_definitions.h"
#include "tss46x_register_structs.h"
#include "itss46x.h"

#include <stdint.h>
#include <HardwareSerial.h>

/*
    This is a sample VAN message sender, which prints the register locations and values to the serial output
*/

class Tss46xWithSerial : public ITss46x
{
private:
    HardwareSerial* _serial;

    void PrintValueToSerial(uint8_t value);
public:
    Tss46xWithSerial(HardwareSerial *serial);
    Tss46xWithSerial(HardwareSerial &serial);
    void init() override;
    void register_set(uint8_t address, uint8_t value) override;
    void registers_set(uint8_t address, const uint8_t values[], uint8_t count) override;
    uint8_t register_get(uint8_t address) override;
    uint8_t registers_get(uint8_t address, volatile uint8_t values[], uint8_t count) override;
};

#endif
