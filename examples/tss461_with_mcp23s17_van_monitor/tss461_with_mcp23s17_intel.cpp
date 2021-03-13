#include "tss461_with_mcp23s17_intel.h"

Tss461WithMcp23s17Intel::Tss461WithMcp23s17Intel(uint8_t csPin, SPIClass* spiInstance){
    _mcp23s17 = new MCP23S17(spiInstance, csPin, 0);
    _mcp23s17->begin();
}

void Tss461WithMcp23s17Intel::set_bus_direction(uint8_t direction) {
    if (direction == TSS_WRITE)
    {
        for (size_t i = 8; i < 16; i++)
        {
            _mcp23s17->pinMode(i, OUTPUT);
        }
    }
    else {
        for (size_t i = 8; i < 16; i++)
        {
            _mcp23s17->pinMode(i, INPUT);
        }
    }
}

void Tss461WithMcp23s17Intel::init(){
    _mcp23s17->pinMode(TSS_ALE_PIN,    OUTPUT);
    _mcp23s17->pinMode(TSS_READ_PIN,   OUTPUT);
    _mcp23s17->pinMode(TSS_WRITE_PIN,  OUTPUT);
    _mcp23s17->pinMode(TSS_CS_PIN,     OUTPUT);
    _mcp23s17->pinMode(TSS_RESET_PIN,  OUTPUT);

    _mcp23s17->digitalWrite(TSS_RESET_PIN,  1);
    _mcp23s17->digitalWrite(TSS_RESET_PIN,  0);
    _mcp23s17->digitalWrite(TSS_CS_PIN, 1);

    // set bus access mode to no operation
    set_control_pins(1,1);
}

void Tss461WithMcp23s17Intel::set_control_pins(uint8_t read, uint8_t write){
    _mcp23s17->digitalWrite(TSS_READ_PIN,  read);
    _mcp23s17->digitalWrite(TSS_WRITE_PIN, write);
}

void Tss461WithMcp23s17Intel::write_address(uint8_t address) {
    set_bus_direction(TSS_WRITE);

    // set bus access mode to no operation
    set_control_pins(1,1);

    // First assert a valid address on the multiplexed address and data bus
    _mcp23s17->writePort(TSS_BANK_DATA, address);

    // and drive the address strobe pin high.
    _mcp23s17->digitalWrite(TSS_ALE_PIN, TSS_ALE_ENABLED);

    // When the required setup time has passed, the processor must drive the address strobe low
    _mcp23s17->digitalWrite(TSS_ALE_PIN, TSS_ALE_DISABLED);
}

void Tss461WithMcp23s17Intel::register_set(uint8_t address, uint8_t value) {
    write_address(address);

    _mcp23s17->writePort(TSS_BANK_DATA, value);

    // write data to parallel bus
    set_control_pins(1,0);

    // set bus access mode to no operation
    set_control_pins(1,1);
}

uint8_t Tss461WithMcp23s17Intel::register_get(uint8_t address){
    uint8_t value;

    write_address(address);

    set_bus_direction(TSS_READ);

    // read data from parallel bus
    set_control_pins(0,1);
    //delayMicroseconds(4);

    value = _mcp23s17->readPort(TSS_BANK_DATA);

    //delayMicroseconds(4);

    // set bus access mode to no operation
    set_control_pins(1,1);

    return value;
}

void Tss461WithMcp23s17Intel::registers_set(uint8_t address, const uint8_t values[], uint8_t count){
    uint8_t address_to_write = address;

    for (uint8_t i = 0; i < count; i++)
    {
        register_set(address_to_write, values[i]);
        address_to_write++;
    }
}

uint8_t Tss461WithMcp23s17Intel::registers_get(uint8_t address, volatile uint8_t values[], uint8_t count){
    uint8_t value;

    uint8_t address_to_read = address;

    for (uint8_t i = 0; i < count; i++)
    {
        values[i] = register_get(address_to_read);
        address_to_read++;
    }

    return value;
}
