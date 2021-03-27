// itss46x.h

#ifndef _itss46x
#define _itss46x

#include <stdint.h>

/*
    An interface to abstract the hardware specific part of the VAN controller. 
    This enables the support for the TSS463 (which has an SPI interface) and the TSS461 (with multiplexed address and data bus) with the same codebase. 
    This can be done because the register addresses are the same for both controllers.
*/

class ITss46x
{
public:
    virtual void init() = 0;
    virtual void register_set(uint8_t address, uint8_t value) = 0;
    virtual void registers_set(uint8_t address, const uint8_t values[], uint8_t n) = 0;
    virtual uint8_t register_get(uint8_t address) = 0;
    virtual uint8_t registers_get(uint8_t address, volatile uint8_t values[], uint8_t count) = 0;
};

#endif
