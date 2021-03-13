// tss461_intel.h

#ifndef tss461_intel
#define tss461_intel

#include "tss46x_definitions.h"
#include "tss46x_register_structs.h"
#include "itss46x.h"

#if defined(ARDUINO) && ARDUINO >= 100
    #include <Arduino.h>
    #include <SPI.h>
#else
    #include "WProgram.h"
#endif

/*
    This is the implementation of the TSS461 with multiplexed address and data bus in intel mode.
    It was tested with an ESP32 and a STM32 BluePill module, however it should be possible to use with other MCUs supported by Arduino
    You just need to adjust the pin definitions below.

    This implements the ITss46xVanSender interface to abstract away the communication with the hardware
*/

typedef struct {
    uint8_t TSS_ALE_PIN;
    uint8_t TSS_WRITE_PIN;
    uint8_t TSS_CS_PIN;
    uint8_t TSS_READ_PIN;
    uint8_t TSS_RESET_PIN;

    uint8_t TSS_AD0_PIN;
    uint8_t TSS_AD1_PIN;
    uint8_t TSS_AD2_PIN;
    uint8_t TSS_AD3_PIN;
    uint8_t TSS_AD4_PIN;
    uint8_t TSS_AD5_PIN;
    uint8_t TSS_AD6_PIN;
    uint8_t TSS_AD7_PIN;
} TSSPinSetup;

class Tss461Intel : public ITss46x
{
private:
    TSSPinSetup* _pinSetup;

    #define TSS_ALE_DISABLED LOW
    #define TSS_ALE_ENABLED  HIGH
    #define TSS_READ  1
    #define TSS_WRITE 0

    uint8_t data_pins[8];

    void writePort(uint8_t value);
    void write_address(uint8_t address);
    void set_bus_direction(uint8_t direction);

    void set_control_pins(uint8_t read, uint8_t write);
public:
    Tss461Intel(TSSPinSetup *pinSetup);
    void init() override;
    void register_set(uint8_t address, uint8_t value) override;
    void registers_set(uint8_t address, const uint8_t values[], uint8_t count) override;
    uint8_t register_get(uint8_t address) override;
    uint8_t registers_get(uint8_t address, volatile uint8_t values[], uint8_t count) override;
};

#endif
