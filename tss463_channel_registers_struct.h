// tss463_channel_registers_struct.h
#pragma once

#ifndef _tss463_channel_registers_struct_h
    #define _tss463_channel_registers_struct_h

    #if defined(ARDUINO) && ARDUINO >= 100
        #include "Arduino.h"
    #else
        #include "WProgram.h"
    #endif

/*
Message Length And Status Register
The message length and status register at address (base_address + 0x03) is also 8 bits wide. It indicates the length of reserved for the message in the Message DATA RAM area
Page 39
*/
typedef union
{
    struct
    {
        uint8_t CHRx : 1;
        uint8_t CHTx : 1;
        /*
        As status, this bit is set by the TSS463C when error occurs in transmission or on a received frame. The user must reset it
        */
        uint8_t CHER : 1;
        /*
        The 5 high bits of this register allows the user to specify either the length of the message to be transmitted, or the maximum length of a message receivable in the pointed reception buffer
        The first byte in this register does not contain data, but the length of the message received. This implies that the length value has to be equal to or greater than the maximum length of a message 
        to be received in this buffer (or the length of a message to be transmitted) plus 1
        */
        uint8_t M_L  : 5;
    }data;
    uint8_t Value;
}MessageLengthAndStatusRegister;


/*
Page 38
*/
typedef union
{
    struct
    {
        uint8_t RTR : 1;
        uint8_t RNW : 1;
        uint8_t RAK : 1;
        uint8_t EXT : 1;
        uint8_t ID  : 4;
    }data;
    uint8_t Value;
}Id2AndCommandRegister;

/*
Message Pointer Register
The message pointer register at address (base_address + 0x02) is 8 bits wide. It indicates where in the Message DATA RAM area the message buffer is located.
Page 38
*/
typedef union
{
    struct
    {
        //Message Pointer the value in this register is the offset from 0x80
        uint8_t M_P  : 7;
        /*Disable RAK (Used in ‘Spy Mode’)
        In reception: whatever is the RAK bit of the incoming valid frame, no ACK answer will be set. If the message was successfully received, an IT is set (ROK or RNOK).
        In transmission: no action.
        One: disable active, “spy mode”.
        Zero: disable inactive, normal operation
        */
        uint8_t DRAK : 1;
    }data;
    uint8_t Value;
}MessagePointerRegister;

#endif
