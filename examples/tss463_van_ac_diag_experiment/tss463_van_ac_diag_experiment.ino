/*
    Experiments with the Air Conditioning related diagnostic messages (Peugeot 206, MUX, 2003)
*/
#include <Arduino.h>
#include <SPI.h>
#include "VanMessageSender.h"

const int SCK_PIN = 25;
const int MISO_PIN = 5;
const int MOSI_PIN = 33;
//const int VAN_CS_PIN = 32; //ESP32
const int VAN_CS_PIN = 7; //Pro Mini
const VAN_NETWORK NETWORK = VAN_COMFORT;

AbstractVanMessageSender* VANInterface;
unsigned long currentTime = millis();
unsigned long previousTime = millis();

volatile bool runDiagSession = true;
volatile uint8_t headerByte = 0x80;

volatile uint8_t b1 = 0x00;
volatile uint8_t b2 = 0x00;

uint8_t vanMessageLength;
uint8_t vanMessage[32];

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

// Get air conditioning manufacture info (also enters the device into diag mode)
void GetAirConManufacturer(uint8_t channelId)
{
    uint8_t packet[2] = { 0x21, 0x80 };
    VANInterface->set_channel_for_transmit_message(channelId, 0xA5C, packet, 2, 1);
}

// Get air conditioning settings
void GetAirConSettings(uint8_t channelId)
{
    uint8_t packet[2] = { 0x21, 0xA0 };
    VANInterface->set_channel_for_transmit_message(channelId, 0xA5C, packet, 2, 1);
}

// Get air conditioning parameters (live data)
void GetAirConLiveData(uint8_t channelId)
{
    uint8_t packet[2] = { 0x21, 0xC0 };
    VANInterface->set_channel_for_transmit_message(channelId, 0xA5C, packet, 2, 1);
}

void GetActuatorStates(uint8_t channelId)
{
    uint8_t packet[2] = { 0x21, 0xC1 };
    VANInterface->set_channel_for_transmit_message(channelId, 0xA5C, packet, 2, 1);
}

void GetButtonStates(uint8_t channelId)
{
    uint8_t packet[2] = { 0x21, 0xC2 };
    VANInterface->set_channel_for_transmit_message(channelId, 0xA5C, packet, 2, 1);
}

void ReadFaultCodes(uint8_t channelId)
{
    uint8_t packet[2] = { 0x21, 0xC4 };
    VANInterface->set_channel_for_transmit_message(channelId, 0xA5C, packet, 2, 1);
}

void ClearFaultCodes(uint8_t channelId)
{
    uint8_t packet[3] = { 0x14, 0xFF, 0x00 };
    VANInterface->set_channel_for_transmit_message(channelId, 0xA5C, packet, 3, 1);
}

void DisplayTest(uint8_t channelId)
{
    uint8_t packet[4] = { 0x30, 0x89, 0x20, 0x01 };
    VANInterface->set_channel_for_transmit_message(channelId, 0xA5C, packet, 4, 1);
}

void DisplayTestOff(uint8_t channelId)
{
    uint8_t packet[4] = { 0x30, 0x89, 0x20, 0x00 };
    VANInterface->set_channel_for_transmit_message(channelId, 0xA5C, packet, 4, 1);
}

void DisplayTestUnknown(uint8_t channelId)
{
    uint8_t packet[4] = { 0x30, 0x89, 0x01, 0x00 };
    VANInterface->set_channel_for_transmit_message(channelId, 0xA5C, packet, 4, 1);
}

// We need to reserve a buffer for the answer from the Air Conditioner ECU
void QueryAirConData(uint8_t channelId)
{
    VANInterface->set_channel_for_reply_request_message(channelId, 0xADC, 26 + 2, 1);
}

void AnswerADC_manufacture_info(uint8_t channelId)
{
    uint8_t packet[26] = { headerByte, 0x61, 0x80, 0x96, 0x46, 0x61, 0x24, 0x80, 0x00, 0x19, 0x96, 0x46, 0x61, 0x24, 0x80, 0x04, 0x00, 0x05, b1, b2, 0x00, 0x00, 0x00, 0x00, 0x00, headerByte };
    VANInterface->set_channel_for_immediate_reply_message(channelId, 0xADC, packet, 26);
}

void AnswerADC_parameters(uint8_t channelId)
{
    uint8_t packet[22] = { headerByte, 0x61, 0xC0, b1, b2, b1, b2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x71, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1B, 0x00, headerByte };
    VANInterface->set_channel_for_immediate_reply_message(channelId, 0xADC, packet, 22);
}

void AnswerADC_actuator_status(uint8_t channelId)
{
    uint8_t packet[12] = { headerByte, b1, b2, 0x09, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, headerByte };
    VANInterface->set_channel_for_immediate_reply_message(channelId, 0xADC, packet, 12);
}

void AnswerADC_fault_reading(uint8_t channelId)
{
    uint8_t packet[14] = { headerByte, b1, b2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, headerByte };
    VANInterface->set_channel_for_immediate_reply_message(channelId, 0xADC, packet, 14);
}

void AnswerADC_fault_clearing_success(uint8_t channelId)
{
    uint8_t packet[5] = { headerByte, 0x54, 0xFF, 0x00, headerByte };
    VANInterface->set_channel_for_immediate_reply_message(channelId, 0xADC, packet, 5);
}

void AnswerADC_keep_alive(uint8_t channelId)
{
    uint8_t packet[3] = { headerByte, 0x7E, headerByte };
    VANInterface->set_channel_for_immediate_reply_message(channelId, 0xADC, packet, 3);
}

void setup() {
    Serial.begin(230400);
    Serial.println("TSS463 A/C diag experiment");
    spi = new SPIClass();

#ifdef ARDUINO_ARCH_AVR
    spi->begin();
#endif

#ifdef ARDUINO_ARCH_ESP32
    spi->begin(SCK_PIN, MISO_PIN, MOSI_PIN, VAN_CS_PIN);
#endif

    VANInterface = new VanMessageSender(VAN_CS_PIN, spi, NETWORK);
    VANInterface->begin();

    AnswerADC_keep_alive(4);
}

void loop() {
    currentTime = millis();
    if ((currentTime - previousTime) >= 20)
    {
        previousTime = currentTime;
        IncrementHeader();

        if (runDiagSession)
        {
            GetAirConManufacturer(0);
            //GetActuatorStates(1);
            GetAirConLiveData(1);
            //GetButtonStates(1);

            // Channel 2 will contain the answer from the A/C ECU
            QueryAirConData(2);
        }

        // The following methods are for "emulating" the A/C ECU answers to the diag queries

        //AnswerADC_manufacture_info(3);
        //AnswerADC_actuator_status(3);
        //AnswerADC_fault_reading(3);

        // Keep alive the diag session
        if (runDiagSession)
        {
            VANInterface->reactivate_channel(4);
        }
    }


    // Print the answer data
    MessageLengthAndStatusRegister messageAvailable = VANInterface->message_available(2);
    if (messageAvailable.data.CHRx || messageAvailable.data.CHTx /*|| messageAvailable.data.CHER*/)
    {
        VANInterface->read_message(2, &vanMessageLength, vanMessage);

        if (vanMessage[0] != 0x00)
        {
            Serial.print("Channel: ");
            Serial.print(2, DEC);
            Serial.print(": ");

            char tmp[3];

            for (size_t i = 0; i < vanMessageLength; i++)
            {
                snprintf(tmp, 3, "%02X", vanMessage[i]);
                Serial.print(" ");
                Serial.print(tmp);
            }
            Serial.println();
        }
    }

    // We can increment/decrement two variables to test things out
    if (Serial.available() > 0)
    {
        int inChar = Serial.read();

        switch (inChar)
        {
            case '1':
            {
                //b1=b1*2;
                b1++;
                break;
            }
            case '2':
            {
                //b2 = b2*2;
                b2++;
                break;
            }
            case 'q':
            {
                //b1=b1/2;
                b1--;
                break;
            }
            case 'w':
            {
                //b2=b2/2;
                b2--;
                break;
            }
            case 'a':
            {
                DisplayTest(5);
                break;
            }
            case 's':
            {
                DisplayTestOff(6);
                break;
            }
            case 'd':
            {
                runDiagSession = !runDiagSession;
                break;
            }
            default:
            {
                break;
            }
        }
    }
}