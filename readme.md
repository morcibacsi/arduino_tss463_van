# Arduino TSS463/TSS461 VAN interface library

This is an Arduino library for the Atmel TSS463/TSS461 VAN Datalink Controller.

[VAN bus][van_bus] is pretty similar to CAN bus. It was used in many cars (Peugeot, Citroen) made by PSA.

It is capable of reading and writing the VAN bus. To understand the VAN protocol and the library I **strongly recommend** to read the datasheet of the TSS463C or the TSS461 VAN Datalink Controller. It is included in the repository inside the extras folder. The library contains references to the pages of the document to have a better understanding on what is going on.

### Message types
Understanding the various message types is essential (see page 19-21 and 42-45 in the datasheet of the TSS463C). Some messages are pretty straightforward: they have a source and destination(s), but there are others which are like queries: a device (for example the display) asks another one (like the BSI or the CD changer or the head unit) for some data. So when you are testing and a message does not appear it could mean that you don't have the initiator in your setup (or maybe you are missing the device which should answer for the query). Or it could also mean that you misconfigured the receiver channels.

Because of the various message types and the way the TSS46x works, configuring the channels properly is very important. You can easily miss messages if you don't pay attention to the message types and the channel configuration.

So if you just want to have a working VAN bus reader and don't want to have your hands dirty I recommend my [VAN bus reader library for ESP32][esp32_van_reader] as it just dumps everything from the bus regardless of the message types and states. And also that requires less (and more easy to obtain) hardware parts. But that library has reading capabilities only compared to this one which also can transmit messages on the bus safely.

### Schematics

To have the library working, you need to build a shield first as such thing does not exists on the market for the VAN bus. To build the hardware you need to buy a TSS463C VAN controller and a REMQ 0339 VAN line driver (this is also known as Alcatel 2840). Unfortunately these are pretty hard to find but if you are lucky you can buy them on aliexpress (or you can also extract them from an old headunit or display).

#### TSS463C + Remq0339 VAN line driver

![schema_tss463c_remq0339](https://github.com/morcibacsi/arduino_tss463_van/raw/master/extras/schema/schema_tss463c_remq0339.png)

#### TSS463C + MCP2551 CAN transceiver

Instead of the Remq 0339 it is also possible to use a CAN transceiver, for example the MCP2551

![schema_tss463c_mcp2551](https://github.com/morcibacsi/arduino_tss463_van/raw/master/extras/schema/schema_tss463c_mcp2551.png)

#### TSS461C + TJA1040 CAN transceiver

The library supports the TSS461 controller with a parallel port in Intel mode as well. You can use the schematics below. You need to connect the AD0-AD7, ALE, CS, WR, RD, RES, IRQ pins either to your microcontroller or you can also use an MCP23S17 port extender as well. When you use the port extender then you have to have [Majenko's excellent library][majenko] installed

![schema_tss461_mcp23s17_intel_mode_tja_1040](https://github.com/morcibacsi/arduino_tss463_van/raw/master/extras/schema/schema_tss461_mcp23s17_intel_mode_tja_1040.jpg)

### Initialization examples
In version 2.0.0 support was added for TSS461. To have a common codebase for the register setup the actual hardware handling was abstracted away. This means an extra step is required to instantiate the hardware which causes a breaking change from prior versions.

#### TSS463
```C
#include <SPI.h>
#include <itss46x.h>
#include <tss46x_van.h>
#include <tss463.h>

SPIClass* spi;
ITss46x* vanSender;
TSS46X_VAN* VANInterface;

void setup()
{
  SPIClass* spi;
  spi = new SPIClass();
  spi->begin();

  vanSender = new Tss463(VAN_PIN, spi);
  VANInterface = new TSS46X_VAN(vanSender, VAN_125KBPS);
  VANInterface->begin();
}
```
#### TSS461

```C
#include <itss46x.h>
#include <tss46x_van.h>
#include <tss461_intel.h>

ITss46x* vanSender;
TSS46X_VAN* VANInterface;

void setup()
{
    pinSetup = new TSSPinSetup();
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
    
    vanSender = new Tss461Intel(pinSetup);  
    VANInterface = new TSS46X_VAN(vanSender, VAN_125KBPS);
    VANInterface->begin();
}
```

#### TSS461 with MCP23S17

```C
#include <SPI.h>
#include <itss46x.h>
#include <tss46x_van.h>
#include "tss461_with_mcp23s17_intel.h"

SPIClass* spi;
ITss46x* vanSender;
TSS46X_VAN* VANInterface;

void setup()
{
  SPIClass* spi;
  spi = new SPIClass();
  spi->begin();

  //check tss461_intel.h file for pin assignment
  vanSender = new Tss461WithMcp23s17Intel(VAN_PIN, spi);
  VANInterface = new TSS46X_VAN(vanSender, VAN_125KBPS);
  VANInterface->begin();
}
```

### TODO
Currently reading of the registers does not work on the TSS461. A pull request is very welcome for this.

### Installing
Copy the files to your **documents\Arduino\libraries\tss46x_van** folder or install it via the Arduino library manager

Check the **tss463_van_monitor** and **tss463_van_dashboard_experiment** folders inside the extras folder for examples on how to read and write messages on the bus.

### Tested boards
- Arduino UNO/Nano/Pro Mini
- ESP32
- STM32 BluePill

### See also
- [VAN Analyzer for Saleae Logic Analyzer][van_analyzer]
- [VAN bus reader library for ESP32][esp32_van_reader]
- [Majenko's MCP23S10 library][majenko]

### Thanks
I would like to thank [lazarov-g][lazarov-g] without his help and [sketch][lazarov_reader] this library would never exists.

[van_bus]: https://en.wikipedia.org/wiki/Vehicle_Area_Network
[van_network]: https://en.wikipedia.org/wiki/Vehicle_Area_Network
[van_analyzer]: https://github.com/morcibacsi/VanAnalyzer/
[esp32_van_reader]: https://github.com/morcibacsi/esp32_rmt_van_rx
[lazarov_reader]: https://github.com/lazarov-g/vanreader
[lazarov-g]: https://github.com/lazarov-g
[majenko]: https://github.com/MajenkoLibraries/MCP23S17
