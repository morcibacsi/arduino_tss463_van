/*
    ONLY USE THIS WITHOUT THE HEAD-UNIT OTHERWISE YOU COULD GET STRANGE RESULTS

    sending "p" on the serial monitor fools the display that the state of the radio is toggled (switched ON or OFF)
*/
#include <Arduino.h>
#include <SPI.h>
#include "VanMessageSender.h"

const int SCK_PIN = 25;
const int MISO_PIN = 5;
const int MOSI_PIN = 33;
//const int VAN_PIN = 32; //ESP32
const int VAN_PIN = 7; //Pro Mini
const VAN_NETWORK NETWORK = VAN_COMFORT;

AbstractVanMessageSender* VANInterface;
unsigned long currentTime = millis();
unsigned long previousTime = millis();
volatile uint8_t headerByte = 0x80;
volatile bool isRadioTurnedOn = false;
volatile uint8_t sendCount = 0;

SPIClass* spi;

void IncrementHeader()
{
    if (headerByte == 0x87)
    {
        headerByte = 0x80;
    }
    else
    {
        headerByte++;
    }
}

void Send8C4_1(uint8_t channelId)
{
    uint8_t packet[3] = { 0x8A, 0x21, 0x40 };
    VANInterface->set_channel_for_transmit_message(channelId, 0x8C4, packet, 3, 1);
}

void Send8C4_2(uint8_t channelId)
{
    uint8_t packet[3] = { 0x8A, 0x24, 0x40 };
    VANInterface->set_channel_for_transmit_message(channelId, 0x8C4, packet, 3, 1);
}

void Send8D4_1(uint8_t channelId)
{
    uint8_t packet[2] = { 0x11, 0xC0 };
    VANInterface->set_channel_for_transmit_message(channelId, 0x8D4, packet, 2, 1);
}

void Send8D4_2(uint8_t channelId)
{
    uint8_t packet[2] = { 0x12, 0x01 };
    VANInterface->set_channel_for_transmit_message(channelId, 0x8D4, packet, 2, 1);
}

void AnswerTo554(uint8_t channelId) {
    uint8_t RDS[] = { ' ', 'P', 'e', 'u', 'g', 'e', 'o', 't', ' ', ' ' };
    uint8_t packet[22] = { headerByte, 0xD1, 0x09, 0x80, 0xC2, 0x03, 0x63, 0x60, 0xFF, 0xFF, 0xA1, RDS[0], RDS[1], RDS[2], RDS[3], RDS[4], RDS[5], RDS[6], RDS[7], RDS[8], RDS[9], headerByte };
    VANInterface->set_channel_for_immediate_reply_message(channelId, 0x554, packet, 22);
}

void AnswerTo4D4(uint8_t channelId, uint8_t radioOn) {
    uint8_t packet[11] = { headerByte, 0x0C, radioOn, 0x00, 0x11, 0x0A, 0x3F, 0x3F, 0x3F, 0x3F, headerByte };
    VANInterface->set_channel_for_immediate_reply_message(channelId, 0x4D4, packet, 11);
}

void setup() {
    Serial.begin(230400);
    Serial.println("TSS463 RDS experiment");
    spi = new SPIClass();

#ifdef ARDUINO_ARCH_AVR
    spi->begin();
#endif

#ifdef ARDUINO_ARCH_ESP32
    spi->begin(SCK_PIN, MISO_PIN, MOSI_PIN, VAN_PIN);
#endif

    VANInterface = new VanMessageSender(VAN_PIN, spi, NETWORK);
    VANInterface->begin();
}

void loop() {
    currentTime = millis();
    if ((currentTime - previousTime) >= 50)
    {
        previousTime = currentTime;
        IncrementHeader();

        Send8C4_1(2);
        if (isRadioTurnedOn)
        {
            AnswerTo4D4(0, 1);

            if (sendCount < 3)
            {
                AnswerTo554(1);
                Send8C4_2(2);
                sendCount++;
            }
        }
        else
        {
            AnswerTo4D4(0, 0);
        }
    }

    if (Serial.available() > 0)
    {
        int inChar = Serial.read();

        switch (inChar)
        {
            case 'p':
            {
                if (isRadioTurnedOn)
                {
                    isRadioTurnedOn = false;
                    sendCount = 0;
                    Serial.println("Radio 'turned off'");
                }
                else
                {
                    isRadioTurnedOn = true;
                    Send8D4_1(3);
                    Send8D4_2(4);
                    Serial.println("Radio 'turned on'");
                }
                break;
            }
            default:
            {
                break;
            }
        }
    }
}