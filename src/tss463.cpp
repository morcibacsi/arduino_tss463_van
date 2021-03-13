#include "tss463.h"

Tss463::Tss463(uint8_t csPin, SPIClass* spiInstance){
    _csPin = csPin;
    _spiInstance = spiInstance;
    pinMode(_csPin, OUTPUT);
}

uint8_t Tss463::spi_transfer(uint8_t data){
    uint8_t res;

    _spiInstance->beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE3));
    res = _spiInstance->transfer(data);
    _spiInstance->endTransaction();

    return res;
}

void Tss463::motorolla_mode(){
    uint8_t value;

    //The first two bytes to be sent by the master (CPU) are called "Initialization Sequence": 
    //This sequence provides a proper asynchronous RESET in the TSS463AA and it selects the Motorola SPI, Intel SPI or the SCI serial mode.
    //Two 0x00 will cause an internal RESET and assert the Motorola SPI mode

    TSS463_SELECT();

    delayMicroseconds(1);//at 8MHZ SCLK speed (4 clocks XTAL)
    value = spi_transfer(MOTOROLA_MODE);
    if (value != ADDR_ANSW)
        error++;
    delayMicroseconds(2);//at 8MHZ SCLK speed (8 clocks SCLK)
    value = spi_transfer(MOTOROLA_MODE);
    if (value != CMD_ANSW)
        error++;
    delayMicroseconds(2);//at 8MHZ max speed (8 clocks SCLK)

    TSS463_UNSELECT();
}

void Tss463::init() {
    error = 0;

    motorolla_mode();

    delayMicroseconds(3);//at 8MHZ max speed (12 clocks XTAL)
}

void Tss463::register_set(uint8_t address, uint8_t value) {
    uint8_t res = 0;

    TSS463_SELECT();

    delayMicroseconds(1);//at 8MHZ max speed (4 clocks XTAL)

    //At the beginning of a transmission over the serial interface, the first byte is the address of the TSS463C register to be accessed
    res = spi_transfer(address);
    if (res != ADDR_ANSW)
        error++;
    delayMicroseconds(2);//at 8MHZ max speed (8 clocks XTAL)

    //The next byte transmitted is the control byte that determines the direction of the communication
    res = spi_transfer(WRITE);
    if (res != CMD_ANSW)
        error++;
    delayMicroseconds(4);//at 8MHZ max speed (15 clocks XTAL)

    spi_transfer(value);
    delayMicroseconds(3);//at 8MHZ max speed (12 clocks XTAL)

    TSS463_UNSELECT();
}

void Tss463::registers_set(uint8_t address, const uint8_t values[], uint8_t count) {
    uint8_t i;
    uint8_t res = 0;

    TSS463_SELECT();

    delayMicroseconds(1);//at 8MHZ max speed (4 clocks XTAL)

    //At the beginning of a transmission over the serial interface, the first byte is the address of the TSS463C register to be accessed
    res = spi_transfer(address);
    if (res != ADDR_ANSW)
        error++;
    delayMicroseconds(2);//at 8MHZ max speed (8 clocks XTAL)

    //The next byte transmitted is the control byte that determines the direction of the communication
    res = spi_transfer(WRITE);
    if (res != CMD_ANSW)
        error++;
    delayMicroseconds(4);//at 8MHZ max speed (15 clocks XTAL)

    for (i = 0; i < count; i++)
    {
        spi_transfer(values[i]);
        delayMicroseconds(3);//at 8MHZ max speed (12 clocks XTAL)
    }

    TSS463_UNSELECT();
}

uint8_t Tss463::register_get(uint8_t address) {
    uint8_t value;

    TSS463_SELECT();

    delayMicroseconds(1);//at 8MHZ max speed (4 clocks XTAL)

    //At the beginning of a transmission over the serial interface, the first byte is the address of the TSS463C register to be accessed
    value = spi_transfer(address);
    if (value != ADDR_ANSW)
        error++;
    delayMicroseconds(2);//at 8MHZ max speed (8 clocks XTAL)

    //The next byte transmitted is the control byte that determines the direction of the communication
    value = spi_transfer(READ);
    if (value != CMD_ANSW)
        error++;
    delayMicroseconds(4);//at 8MHZ max speed (15 clocks XTAL)

    //When the master (CPU) conducts a read, it sends an address byte, a control byte and dummy characters (0xFF for instance) on its MOSI line
    value = spi_transfer(0xFF);
    delayMicroseconds(3);//at 8MHZ max speed (12 clocks XTAL)

    TSS463_UNSELECT();

    return value;
}

uint8_t Tss463::registers_get(uint8_t address, volatile uint8_t values[], uint8_t count) {
    uint8_t value;

    TSS463_SELECT();

    delayMicroseconds(1);//at 8MHZ max speed (4 clocks XTAL)

    //At the beginning of a transmission over the serial interface, the first byte is the address of the TSS463C register to be accessed
    value = spi_transfer(address);
    if (value != ADDR_ANSW)
        error++;
    delayMicroseconds(2);//at 8MHZ max speed (8 clocks XTAL)

    //The next byte transmitted is the control byte that determines the direction of the communication
    value = spi_transfer(READ);
    if (value != CMD_ANSW)
        error++;
    delayMicroseconds(4);//at 8MHZ max speed (15 clocks XTAL)

    //When the master (CPU) conducts a read, it sends an address byte, a control byte and dummy characters (0xFF for instance) on its MOSI line

    // TSS463 has auto-increment of address-pointer
    for (uint8_t i = 0; i < count; i++)
    {
        values[i] = spi_transfer(0xFF);
        delayMicroseconds(3);//at 8MHZ max speed (12 clocks XTAL)
    }

    TSS463_UNSELECT();

    return value;
}
