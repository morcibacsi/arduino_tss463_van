// tss46x_definitions.h

#ifndef tss46x_definitions
#define tss46x_definitions

#pragma region TSS46x internal register adresses - Figure 22 (TSS463C)
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

#endif