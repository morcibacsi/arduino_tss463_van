/*
    DO NOT USE THIS INSIDE A CAR
    THIS IS JUST FOR TESTING THE DASHBOARD ON THE BENCH
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

//uint8_t dashboard_packet[14] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
uint8_t dashboard_packet[14] = { 0x8C, 0x00, 0x02, 0xB9, 0x00, 0x82, 0x8D, 0x4E, 0x59, 0x00, 0xFE, 0x01, 0x00, 0x00 };

//1: dashboards made before 2004
//2: dashboards made after  2004
uint8_t dashboard_version = 1;

SPIClass* spi;

/* For dashboards made before 2004 */
void Send4FC_V1(uint8_t channelId)
{
    VANInterface->set_channel_for_transmit_message(channelId, 0x4FC, dashboard_packet, 11, 1);
}

/* For dashboards made after 2004 */
void Send4FC_V2(uint8_t channelId)
{
    VANInterface->set_channel_for_transmit_message(channelId, 0x4FC, dashboard_packet, 14, 1);
}

void Send824(uint8_t channelId)
{
    uint8_t packet[7] = { 0x4f, 0x9d, 0x2f, 0xff, 0x01, 0x86, 0x00 };
    VANInterface->set_channel_for_transmit_message(channelId, 0x824, packet, 7, 0);
}

/*
    The message id 0x8FC is asked by the BSI. It asks the instrument cluster for its mileage.
    This way if a dashboard with a higher mileage is installed the BSI can update its value with the one from the cluster.
*/
void QueryInstrumentClusterForMileage(uint8_t channelId)
{
    VANInterface->set_channel_for_reply_request_message(channelId, 0x8FC, 7, 1);
}

void Ack664(uint8_t channelId)
{
    VANInterface->set_channel_for_receive_message(channelId, 0x664, 13, 1);
}

void SendExternalTemperature(uint8_t channelId, int temperature)
{
    uint8_t packet[7] = { 0x0F, 0x07, 0x81, 0x1D, 0xA4 ,0x93, temperature * 2 + 0x50 };
    VANInterface->set_channel_for_transmit_message(channelId, 0x8A4, packet, 7, 0);
}

void Send524(uint8_t channelId)
{
    uint8_t packet[16] = { 0xff, 0x20, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    uint8_t messageLength = 16; //V2 dashboard has a message length of 16
    if (dashboard_version == 1)
    {
        messageLength = 14;
    }
    VANInterface->set_channel_for_transmit_message(channelId, 0x524, packet, messageLength, 0);
}

void setup() {
    Serial.begin(230400);
    Serial.println("TSS463 dashboard experiment");
    spi = new SPIClass();

    /*
     *                     SPI pins
     *  BOARD      SCK      MISO         MOSI       SS
     *  Arduino    13        12           11        7
     *  ESP32      12        22           23        13
     */

#ifdef ARDUINO_ARCH_AVR
    spi->begin();
#endif

#ifdef ARDUINO_ARCH_ESP32
    spi->begin(SCK_PIN, MISO_PIN, MOSI_PIN, VAN_CS_PIN);
#endif

    VANInterface = new VanMessageSender(VAN_CS_PIN, spi, NETWORK);
    VANInterface->begin();

    if (dashboard_version == 1)
    {
        Send4FC_V1(0);
    }
    else if (dashboard_version == 2)
    {
        Send4FC_V2(0);
    }
    Send824(1);
    QueryInstrumentClusterForMileage(2);
    Ack664(3);
    SendExternalTemperature(4, 18);
    Send524(5);
}

void loop() {
    currentTime = millis();
    if ((currentTime - previousTime) >= 50)
    {
        previousTime = currentTime;

        for (size_t i = 0; i < 14; i++)
        {
            Serial.print(dashboard_packet[i], HEX);
            Serial.print(" ");
        }
        Serial.println();

        // reactivating a channel is much faster than setting up the whole data every time
        VANInterface->reactivate_channel(0);
        VANInterface->reactivate_channel(1);
        VANInterface->reactivate_channel(2);
        VANInterface->reactivate_channel(3);
        VANInterface->reactivate_channel(4);
        VANInterface->reactivate_channel(5);
    }

    if (Serial.available() > 0)
    {
        int inChar = Serial.read();
        //Serial.println(inChar);
        switch (inChar)
        {
            case '0':
            {
                if (dashboard_packet[0] == 0xC0)
                {
                    dashboard_packet[0] = 0;
                }
                else
                {
                    dashboard_packet[0] = 0xC0;
                }
                break;
            }
            case 'f':
            {
                for (size_t i = 1; i < 14; i++)
                {
                    dashboard_packet[i]++;
                }
                break;
            }
            case 'F':
            {
                for (size_t i = 1; i < 14; i++)
                {
                    dashboard_packet[i]--;
                }
                break;
            }
            case 'r':
            {
                VANInterface->set_value_in_channel(1, 0, 0x18);
                VANInterface->set_value_in_channel(1, 1, 0xF8);
                break;
            }
            case 'R':
            {
                VANInterface->set_value_in_channel(1, 0, 0x4F);
                VANInterface->set_value_in_channel(1, 1, 0x9D);
                break;
            }
        }
        if (dashboard_version == 1)
        {
            Send4FC_V1(0);
        }
        else if (dashboard_version == 2)
        {
            Send4FC_V2(0);
        }
    }
}
