// tss463_van.h

#ifndef _TSS463_VAN_h
#define _TSS463_VAN_h

#include "tss463_channel_registers_struct.h"

#if defined(ARDUINO) && ARDUINO >= 100
    #include <Arduino.h>
    #include <inttypes.h>
    #include <SPI.h>
#else
    #include "WProgram.h"
#endif

#define TSS463_SELECT()   digitalWrite(SPICS, LOW)
#define TSS463_UNSELECT() digitalWrite(SPICS, HIGH)

#define MOTOROLA_MODE 0x00
#define WRITE         0xE0
#define READ          0x60
#define ADDR_ANSW     0xAA
#define CMD_ANSW      0x55

#define ROKE  (1)
#define RNOKE (0)

#pragma region TSS463C internal register adresses - Figure 22
                                 // R/W?  - Default value on init
                                 //------------------------------
#define LINECONTROL       0x00   // r/w   - 0x00
#define TRANSMITCONTROL   0x01   // r/w   - 0x02
#define DIAGNOSISCONTROL  0x02   // r/w   - 0x00
#define COMMANDREGISTER   0x03   // w     - 0x00
#define LINESTATUS        0x04   // r     - 0bx01xxx00
#define TRANSMITSTATUS    0x05   // r     - 0x00
#define LASTMESSAGESTATUS 0x06   // r     - 0x00
#define LASTERRORSTATUS   0x07   // r     - 0x00
#define INTERRUPTSTATUS   0x09   // r     - 0x80
#define INTERRUPTENABLE   0x0A   // r/w   - 0x80
#define INTERRUPTRESET    0x0B   // w
#pragma endregion

#define TSS_8MHz_62k5BPS 0x30
#define TSS_8MHz_125kBPS 0x20

// Channels
#define CHANNEL_ADDR(x) (0x10 + (0x08 * x))
#define CHANNELS 14
// Mailbox - data register
#define GETMAIL(x) (0x80 + x)

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

class TSS463_VAN
{
private:
    const uint8_t NOT_ENOUGH_MEMORY_FOR_DATA = 0XFF;
    const uint8_t TSS463C_RAM_SIZE_IN_BYTES = 128;

    ChannelSetup channels[14];
    uint8_t next_free_memory_address;

    volatile int error = 0; // TSS463C out of sync error
    SPIClass *SPI;
    uint8_t SPICS;
    uint8_t _lineControl;
    void tss_init();
    void motorolla_mode();
    uint8_t spi_transfer(volatile uint8_t data);
    void register_set(uint8_t address, uint8_t value);
    uint8_t register_get(uint8_t address);
    uint8_t registers_get(uint8_t address, volatile uint8_t values[], uint8_t count);
    void registers_set(uint8_t address, const uint8_t values[], uint8_t n);
    void setup_channel(uint8_t channelId, uint16_t identifier, uint8_t id1, uint8_t id2, uint8_t id2AndCommand, uint8_t messagePointer, uint8_t lengthAndStatus);
    uint8_t get_memory_address_to_use(uint8_t channelId, uint8_t messageLength);
    bool is_valid_channel(uint8_t channelId, uint16_t identifier);
public:

    TSS463_VAN(uint8_t _CS, SPIClass *_SPI, VAN_SPEED vanSpeed);
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

extern TSS463_VAN VAN;

#endif
