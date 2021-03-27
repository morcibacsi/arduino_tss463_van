#include "tss461_intel.h"
#include <Arduino.h>

Tss461Intel::Tss461Intel(TSSPinSetup *pinSetup){
    _pinSetup = pinSetup;

    data_pins[0] = _pinSetup->TSS_AD0_PIN;
    data_pins[1] = _pinSetup->TSS_AD1_PIN;
    data_pins[2] = _pinSetup->TSS_AD2_PIN;
    data_pins[3] = _pinSetup->TSS_AD3_PIN;
    data_pins[4] = _pinSetup->TSS_AD4_PIN;
    data_pins[5] = _pinSetup->TSS_AD5_PIN;
    data_pins[6] = _pinSetup->TSS_AD6_PIN;
    data_pins[7] = _pinSetup->TSS_AD7_PIN;
}

void Tss461Intel::set_bus_direction(uint8_t direction) {
    if (direction == TSS_WRITE)
    {
        for (size_t i = 0; i < 8; i++)
        {
            pinMode(data_pins[i], OUTPUT);
        }
    }
    else {
        for (size_t i = 8; i < 8; i++)
        {
            pinMode(data_pins[i], INPUT);
        }
    }
}

void Tss461Intel::init(){
    pinMode(_pinSetup->TSS_ALE_PIN,    OUTPUT);
    pinMode(_pinSetup->TSS_READ_PIN,   OUTPUT);
    pinMode(_pinSetup->TSS_WRITE_PIN,  OUTPUT);
    pinMode(_pinSetup->TSS_CS_PIN,     OUTPUT);
    pinMode(_pinSetup->TSS_RESET_PIN,  OUTPUT);

    digitalWrite(_pinSetup->TSS_RESET_PIN,  1);
    digitalWrite(_pinSetup->TSS_RESET_PIN,  0);
    digitalWrite(_pinSetup->TSS_CS_PIN, 1);

    // set bus access mode to no operation
    set_control_pins(1,1);
}

void Tss461Intel::set_control_pins(uint8_t read, uint8_t write){
    digitalWrite(_pinSetup->TSS_READ_PIN,  read);
    digitalWrite(_pinSetup->TSS_WRITE_PIN, write);
}

void Tss461Intel::writePort(uint8_t value)
{
    for (size_t i = 0; i < 8; i++)
    {
        digitalWrite(data_pins[i], value & (1 << i));
    }
}

void Tss461Intel::write_address(uint8_t address) {
    set_bus_direction(TSS_WRITE);

    // set bus access mode to no operation
    set_control_pins(1,1);

    // First assert a valid address on the multiplexed address and data bus
    writePort(address);

    // and drive the address strobe pin high.
    digitalWrite(_pinSetup->TSS_ALE_PIN, TSS_ALE_ENABLED);

    // When the required setup time has passed, the processor must drive the address strobe low
    digitalWrite(_pinSetup->TSS_ALE_PIN, TSS_ALE_DISABLED);
}

void Tss461Intel::register_set(uint8_t address, uint8_t value) {
    write_address(address);

    writePort(value);

    // write data to parallel bus
    set_control_pins(1,0);

    // set bus access mode to no operation
    set_control_pins(1,1);
}

uint8_t Tss461Intel::register_get(uint8_t address){
    uint8_t value = 0;

    write_address(address);

    set_bus_direction(TSS_READ);

    // read data from parallel bus
    set_control_pins(0,1);
    //delayMicroseconds(4);

    for (size_t i = 8; i-- > 0;)
    {
        value |= digitalRead(data_pins[i]);
        if (i > 0)
        {
            value = value << 1;
        }
    }

    //delayMicroseconds(4);

    // set bus access mode to no operation
    set_control_pins(1,1);

    return value;
}

void Tss461Intel::registers_set(uint8_t address, const uint8_t values[], uint8_t count){
    uint8_t address_to_write = address;

    for (uint8_t i = 0; i < count; i++)
    {
        register_set(address_to_write, values[i]);
        address_to_write++;
    }
}

uint8_t Tss461Intel::registers_get(uint8_t address, volatile uint8_t values[], uint8_t count){
    uint8_t value;

    uint8_t address_to_read = address;

    for (uint8_t i = 0; i < count; i++)
    {
        values[i] = register_get(address_to_read);
        address_to_read++;
    }

    return value;
}
