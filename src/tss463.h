// tss463.h

#ifndef tss463
#define tss463

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
    This is the implementation of the TSS463 SPI device in motorolla mode.
    This implements the ITss46xVanSender to abstract away the SPI commands for the controller.
*/

class Tss463 : public ITss46x
{
private:
    #define MOTOROLA_MODE 0x00
    #define WRITE         0xE0
    #define READ          0x60
    #define ADDR_ANSW     0xAA
    #define CMD_ANSW      0x55

    #define TSS463_SELECT()   digitalWrite(_csPin, LOW)
    #define TSS463_UNSELECT() digitalWrite(_csPin, HIGH)

    SPIClass* _spiInstance;
    uint8_t _csPin;

    volatile int error = 0; // TSS463C out of sync error
    uint8_t spi_transfer(uint8_t data);
    void motorolla_mode();

public:
    Tss463(uint8_t csPin, SPIClass* spiInstance);
    void init() override;
    void register_set(uint8_t address, uint8_t value) override;
    void registers_set(uint8_t address, const uint8_t values[], uint8_t count) override;
    uint8_t register_get(uint8_t address) override;
    uint8_t registers_get(uint8_t address, volatile uint8_t values[], uint8_t count) override;
};

#endif
