// tss46x_van.h

#ifndef _TSS46x_VAN_h
#define _TSS46x_VAN_h

#include "tss46x_definitions.h"
#include "tss46x_register_structs.h"
#include "itss46x.h"

#if defined(ARDUINO) && ARDUINO >= 100
    #include <Arduino.h>
#else
    #include "WProgram.h"
#endif

typedef struct ChannelSetup {
    uint8_t MessageLengthAndStatusRegisterValue;
    uint8_t MemoryLocation;
    uint8_t MessageLength;
    uint16_t Identifier;
    bool IsOccupied;
};

enum VAN_SPEED {
    VAN_62K5BPS,
    VAN_125KBPS,
};

class TSS46X_VAN
{
private:
    const uint8_t NOT_ENOUGH_MEMORY_FOR_DATA = 0xFF;
    const uint8_t TSS463C_RAM_SIZE_IN_BYTES = 128;

    // interface of the hardware layer
    ITss46x* _sender;

    ChannelSetup channels[14];
    volatile uint8_t next_free_memory_address;

    uint8_t _lineControl;
    void tss_init();
    void setup_channel(uint8_t channelId, uint16_t identifier, uint8_t id1, uint8_t id2, uint8_t id2AndCommand, uint8_t messagePointer, uint8_t lengthAndStatus);
    uint8_t get_memory_address_to_use(uint8_t channelId, uint8_t messageLength);
    bool is_valid_channel(uint8_t channelId, uint16_t identifier);
public:

    TSS46X_VAN(ITss46x* sender, VAN_SPEED vanSpeed);
    bool set_channel_for_transmit_message(uint8_t channelId, uint16_t identifier, const uint8_t values[], uint8_t messageLength, uint8_t ack);
    bool set_channel_for_receive_message(uint8_t channelId, uint16_t identifier, uint8_t messageLength, uint8_t setAck);
    bool set_channel_for_reply_request_message_without_transmission(uint8_t channelId, uint16_t identifier, uint8_t messageLength);
    bool set_channel_for_reply_request_message(uint8_t channelId, uint16_t identifier, uint8_t messageLength, uint8_t requireAck);
    bool set_channel_for_immediate_reply_message(uint8_t channelId, uint16_t identifier, const uint8_t values[], uint8_t messageLength);
    bool set_channel_for_deferred_reply_message(uint8_t channelId, uint16_t identifier, const uint8_t values[], uint8_t messageLength, uint8_t setAck);
    bool set_channel_for_reply_request_detection_message(uint8_t channelId, uint16_t identifier, uint8_t messageLength);
    bool reactivate_channel(uint8_t channelId);
    void reset_channels();
    MessageLengthAndStatusRegister message_available(uint8_t channelId);
    void read_message(uint8_t channelId, uint8_t*length, uint8_t buffer[]);
    uint8_t get_last_channel();
    void set_value_in_channel(uint8_t channelId, uint8_t index0, uint8_t value);
    void begin();
    void disable_channel(uint8_t channelId);
};
#endif
