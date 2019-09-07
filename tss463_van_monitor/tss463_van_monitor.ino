#include <SPI.h>
#include "VanMessageSender.h"

const int VAN_PIN = 7;

AbstractVanMessageSender *VANInterface;

uint8_t vanMessageLength;
uint8_t vanMessage[32];
SPIClass* spi;

uint8_t headerByte = 0x80;

char incomingByte;

void setup()
{
    Serial.begin(230400);

    Serial.println("Arduino VAN bus monitor using TSS463C");

    // initialize SPI
    spi = new SPIClass();
    spi->begin();

    // instantiate the VanMessageSender class passing the CS pin and the SPI instance as a dependency
    VANInterface = new VanMessageSender(VAN_PIN, spi);
    VANInterface->begin();

    // IMPORTANT - it does matter in which order the channels are set up for the various request types - need to investigate why
    van_setup_channel(0); //#1 reply request message
    van_setup_channel(1); //#2 receive message
    van_setup_channel(2); //#3 reply request without transmission message
    van_setup_channel(3); //#4 deferred reply request detection message
}

void van_setup_channel(int channel)
{
    switch (channel)
    {
        case 0:
            VANInterface->set_channel_for_reply_request_message_without_transmission(channel, 0x00, 30);
            break;
        case 1:
            VANInterface->set_channel_for_receive_message(channel, 0x00, 30, 0);
            break;
        case 2:
            VANInterface->set_channel_for_reply_request_message_without_transmission(channel, 0x00, 30);
            break;
        case 3:
            VANInterface->set_channel_for_reply_request_detection_message(channel, 0x00, 30);
            break;
        default:
            break;
    }
}

void ShowPopupMessage(int messageId)
{
    uint8_t packet[14] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, messageId, 0xFF, 0xFF, 0xFF, 0xFF };
    VANInterface->set_channel_for_transmit_message(4, 0x524, packet, 14, 0);
    VANInterface->disable_channel(4);
}

void SendExternalTemperature(int temperature)
{
    uint8_t packet[7] = { 0x0F, 0x07, 0x00, 0x00, 0x00, 0x00, temperature * 2  + 0x50};
    VANInterface->set_channel_for_transmit_message(4, 0x8A4, packet, 7, 0);
    VANInterface->disable_channel(4);
}

/* 
    The message id 0x564 is asked by the multifunction display. If we remove the display the message dissappears.
    With the help of method we can query the info from the BSI without having the display in the car.
    We need to ask periodically.
*/
void QueryCarStatusWithTripInfo()
{
    VANInterface->set_channel_for_reply_request_message(3, 0x564, 29, 1);
}

/*
    The display asks for the presence of the CD changer. 
    With the help of this method we can fool the display that there is a CD changer installed.
    We need to set this periodically.
*/
void AnswerToCDC()
{
    headerByte = (headerByte == 0x87) ? 0x80 : headerByte + 1;

    uint8_t status = 0xC3; //playing
    uint8_t cartridge = 0x16;
    uint8_t minutes = 0x01;
    uint8_t seconds = 0x56;
    uint8_t trackNo = 0x17;
    uint8_t cdNo = 0x02;
    uint8_t trackCount = 0x21;
   
    uint8_t packet[12] = { headerByte, 0x00, status, cartridge, minutes, seconds, trackNo, cdNo, trackCount, 0x3f, 0x01, headerByte };
    VANInterface->set_channel_for_immediate_reply_message(3, 0x4EC, packet, 12);
}

void loop() {
    if (Serial.available()>0)
    {
        int inChar = Serial.read();
        switch (inChar)
        {
            case 'd': {
                ShowPopupMessage(0x09);
                break;
            }
            case 'f': {
                ShowPopupMessage(0X5F);
                break;
            }
            case 't': {
                SendExternalTemperature(10);
                break;
            }
            case 'q': {
                QueryCarStatusWithTripInfo();
                break;
            }
            case 'a': {
                AnswerToCDC();
                break;
            }
            default:
                break;
        }
    }
    for (uint8_t channel = 0; channel < 4; channel++)
    {
        MessageLengthAndStatusRegister messageAvailable = VANInterface->message_available(channel);
        if (messageAvailable.data.CHRx || messageAvailable.data.CHTx /*|| messageAvailable.data.CHER*/)
        {
            VANInterface->readMsgBuf(channel, &vanMessageLength, vanMessage);

            if (vanMessage[0] == 0x00)
            {
                continue;
            }

            Serial.print("Channel: ");
            Serial.print(channel, DEC);
            Serial.print(": ");

            char tmp[3];

            for (size_t i = 0; i < vanMessageLength; i++)
            {
                snprintf(tmp, 3, "%02X", vanMessage[i]);
                Serial.print(" ");
                Serial.print(tmp);
            }
            Serial.println();

            van_setup_channel(channel);
        }
    }
}
