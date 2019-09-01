
# Arduino TSS463C VAN interface library

This is an Arduino library for the Atmel TSS463C VAN Datalink Controller with SPI.

[VAN bus][van_bus] is pretty similar to CAN bus. It was used in many cars (Peugeot, Citroen) made by PSA.

This library is unfinished, so contributions are very welcome. It is capable of reading and writing the VAN bus. To understand the VAN protocol and the library I **strongly recommend** to read the datasheet of the TSS463C VAN Datalink Controller. It is included in the repository. The library contains references to the pages of the document to have a better understanding on what is going on.

### Message types
Understanding the various message types is essential (see page 19-21 and 42-45 in the datasheet). Some messages are pretty straightforward: they have a source and destination(s), but there are others which are like queries: a device (for example the display) asks another one (like the BSI or the CD changer or the head unit) for some data. So when you are testing and a message does not appear it could mean that you don't have the initiator in your setup (or maybe you are missing the device which should answer for the query). Or it could also mean that you misconfigured the receiver channels.

If you just want to have a working VAN bus reader and don't want to have your hands dirty I recommend my [VAN bus reader library for ESP32][esp32_van_reader] as it is faster and requires less (and more easy to obtain) hardware (and it also captures those messages which does not have the response part in a query-like message). But for now that library only has reading capabilities.

### Schematics

To have the library working, you need to build a shield first as such thing does not exists on the market for the VAN bus. To build the hardware you need to buy a TSS463C VAN controller and a REMQ 0339 VAN line driver (this is also known as Alcatel 2840). Unfortunately these are pretty hard to find but if you are lucky you can buy them on aliexpress (or you can also extract them from an old headunit or display).

The schematics for the hardware looks like following:


![schema](https://github.com/morcibacsi/arduino_tss463_van/raw/master/schema/schema.png)
### Installing
Copy the following files to your **documents\Arduino\libraries\tss463_van** folder
  - tss463_channel_registers_struct.h
  - tss463_van.h
  - tss463_van.cpp
  - keywords.txt

Check the **tss463_van_monitor** folder for examples on how to read and write messages on the bus.

## See also
- [VAN Analyzer for Saleae Logic Analyzer][van_analyzer]
- [VAN bus reader library for ESP32][esp32_van_reader]

## Thanks
I would like to thank [lazarov-g][lazarov-g] without his help and [sketch][lazarov_reader] this library would never exists.

[van_bus]: https://en.wikipedia.org/wiki/Vehicle_Area_Network
[van_network]: https://en.wikipedia.org/wiki/Vehicle_Area_Network
[van_analyzer]: https://github.com/morcibacsi/VanAnalyzer/
[esp32_van_reader]: https://github.com/morcibacsi/esp32_rmt_van_rx
[lazarov_reader]: https://github.com/lazarov-g/vanreader
[lazarov-g]: https://github.com/lazarov-g
