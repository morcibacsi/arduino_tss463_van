#include <SPI.h>
#include <itss46x.h>
#include <tss46x_van.h>

#include <tss463.h>
#include <tss461_intel.h>
#include <tss46x_with_serial.h>

uint8_t VAN_PIN = 7;

TSSPinSetup* pinSetup;

SPIClass* spi;
ITss46x* vanSender;
TSS46X_VAN* VANInterface;

uint8_t vanMessageLength;
uint8_t vanMessage[32];

uint8_t headerByte = 0x80;
uint8_t check = 0x00;

char incomingByte;

void InitTss461()
{
     // instantiate the VAN message sender for a TSS461
    pinSetup = new TSSPinSetup();

    #ifdef ARDUINO_ARCH_STM32
    pinSetup->TSS_ALE_PIN   = PB13;
    pinSetup->TSS_WRITE_PIN = PB15;
    pinSetup->TSS_CS_PIN    = PB12;
    pinSetup->TSS_READ_PIN  = PA8;
    pinSetup->TSS_RESET_PIN = PB14;

    pinSetup->TSS_AD0_PIN   = PA0;
    pinSetup->TSS_AD1_PIN   = PA1;
    pinSetup->TSS_AD2_PIN   = PA2;
    pinSetup->TSS_AD3_PIN   = PA3;
    pinSetup->TSS_AD4_PIN   = PA4;
    pinSetup->TSS_AD5_PIN   = PA5;
    pinSetup->TSS_AD6_PIN   = PA6;
    pinSetup->TSS_AD7_PIN   = PA7;
    #endif
    vanSender = new Tss461Intel(pinSetup);
}

void InitTss463()
{
      // initialize SPI
    spi = new SPIClass();

    #ifdef ARDUINO_ARCH_AVR
        // start spi on Arduino boards
        spi->begin();
    #endif

    #ifdef ARDUINO_ARCH_ESP32
        // You can select which pins to use as SPI on ESP32 boards by passing the SCK, MISO, MOSI, SS as arguments (in this order) to the spi->begin() method
        const uint8_t SCK_PIN = 25;
        const uint8_t MISO_PIN = 5;
        const uint8_t MOSI_PIN = 33;
        VAN_PIN = 32;

        spi->begin(SCK_PIN, MISO_PIN, MOSI_PIN, VAN_PIN);
    #endif

    // instantiate the VAN message sender for a TSS463
    vanSender = new Tss463(VAN_PIN, spi);
}

void InitTssSerial()
{
    // This "VAN message sender" just prints the values of the TSS registers to the Serial port, included as an example for an abstract sender
    vanSender = new Tss46xWithSerial(Serial);
}

void setup()
{
    Serial.begin(500000);
    delay(3000);
    Serial.println("Arduino VAN bus monitor using TSS463C");

    InitTss463();
    //InitTss461();
    //InitTssSerial();

    VANInterface = new TSS46X_VAN(vanSender, VAN_125KBPS);
    VANInterface->begin();
    
    VANInterface->set_channel_for_receive_message(1, 0x00, 30, 0);
}

void loop() { 
    MessageLengthAndStatusRegister messageAvailable = VANInterface->message_available(1);
    if (messageAvailable.data.CHRx || messageAvailable.data.CHTx || messageAvailable.data.CHER)
    {
        VANInterface->read_message(1, &vanMessageLength, vanMessage);

        Serial.print("Channel: ");
        Serial.print(1, DEC);
        Serial.print(": ");

        char tmp[3];

        for (size_t i = 0; i < vanMessageLength; i++)
        {
            snprintf(tmp, 3, "%02X", vanMessage[i]);
            Serial.print(" ");
            Serial.print(tmp);
        }
        Serial.println();

        VANInterface->reactivate_channel(1);
    }
}
