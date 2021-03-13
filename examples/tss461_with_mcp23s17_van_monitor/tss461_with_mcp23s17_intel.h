// tss461_with_mcp23s17_intel.h

#ifndef tss461_with_mcp23s17_intel
#define tss461_with_mcp23s17_intel

#include <tss46x_definitions.h>
#include <tss46x_register_structs.h>
#include <itss46x.h>

#include <MCP23S17.h>

#if defined(ARDUINO) && ARDUINO >= 100
    #include <Arduino.h>
    #include <SPI.h>
#else
    #include "WProgram.h"
#endif

/*
    This is the implementation of the TSS461 with multiplexed address and data bus in intel mode.
    This requires a MCP23S17 device which is an IO expander. This way we can connect the device to our MCU with 4 pins instead of 13.
    However this also means that you need to connect the TSS461 to the MCP23S17 in a predefined way otherwise it won't work.

    This implements the ITss46xVanSender interface to abstract away the SPI commands for the MCP23S17.
    It uses this library to communicate with the MCP23S17: https://github.com/MajenkoLibraries/MCP23S17
*/

class Tss461WithMcp23s17Intel : public ITss46x
{
private:

    #define TSS_ALE_PIN    0
    #define TSS_WRITE_PIN  1
    #define TSS_CS_PIN     2
    #define TSS_READ_PIN   3
    #define TSS_RESET_PIN  4

    #define TSS_ALE_DISABLED LOW
    #define TSS_ALE_ENABLED  HIGH

    #define TSS_BANK_DATA    1

    #define TSS_READ  1
    #define TSS_WRITE 0

    MCP23S17* _mcp23s17;

    void write_address(uint8_t address);
    void set_bus_direction(uint8_t direction);

    void set_control_pins(uint8_t read, uint8_t write);
public:
    Tss461WithMcp23s17Intel(uint8_t csPin, SPIClass* spiInstance);
    void init() override;
    void register_set(uint8_t address, uint8_t value) override;
    void registers_set(uint8_t address, const uint8_t values[], uint8_t count) override;
    uint8_t register_get(uint8_t address) override;
    uint8_t registers_get(uint8_t address, volatile uint8_t values[], uint8_t count) override;
};

#endif
