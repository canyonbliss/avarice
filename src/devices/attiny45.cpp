#include "../jtag.h"

namespace {

constexpr gdb_io_reg_def_type attiny45_io_registers[] = {{"ADCSRB", 0x23, 0x00},
                                                     {"ADCL", 0x24, IO_REG_RSE},
                                                     {"ADCH", 0x25, IO_REG_RSE},
                                                     {"ADCSRA", 0x26, 0x00},
                                                     {"ADMUX", 0x27, 0x00},
                                                     {"ACSR", 0x28, 0x00},
                                                     {"USICR", 0x2d, 0x00},
                                                     {"USISR", 0x2e, 0x00},
                                                     {"USIDR", 0x2f, 0x00},
                                                     {"USIBR", 0x30, 0x00},
                                                     {"GPIOR0", 0x31, 0x00},
                                                     {"GPIOR1", 0x32, 0x00},
                                                     {"GPIOR2", 0x33, 0x00},
                                                     {"DIDR0", 0x34, 0x00},
                                                     {"PCMSK", 0x35, 0x00},
                                                     {"PINB", 0x36, 0x00},
                                                     {"DDRB", 0x37, 0x00},
                                                     {"PORTB", 0x38, 0x00},
                                                     {"EECR", 0x3c, 0x00},
                                                     {"EEDR", 0x3d, 0x00},
                                                     {"EEARL", 0x3e, 0x00},
                                                     {"EEARH", 0x3f, 0x00},
                                                     {"PRR", 0x40, 0x00},
                                                     {"WDTCR", 0x41, 0x00},
                                                     {"DWDR", 0x42, 0x00},
                                                     {"DTPS1", 0x43, 0x00},
                                                     {"DT1B", 0x44, 0x00},
                                                     {"DT1A", 0x45, 0x00},
                                                     {"CLKPR", 0x46, 0x00},
                                                     {"PLLCSR", 0x47, 0x00},
                                                     {"OCR0B", 0x48, 0x00},
                                                     {"OCR0A", 0x49, 0x00},
                                                     {"TCCR0A", 0x4a, 0x00},
                                                     {"OCR1B", 0x4b, 0x00},
                                                     {"GTCCR", 0x4c, 0x00},
                                                     {"OCR1C", 0x4d, 0x00},
                                                     {"OCR1A", 0x4e, 0x00},
                                                     {"TCNT1", 0x4f, 0x00},
                                                     {"TCCR1", 0x50, 0x00},
                                                     {"OSCCAL", 0x51, 0x00},
                                                     {"TCNT0", 0x52, 0x00},
                                                     {"TCCR0B", 0x53, 0x00},
                                                     {"MCUSR", 0x54, 0x00},
                                                     {"MCUCR", 0x55, 0x00},
                                                     {"SPMCSR", 0x57, 0x00},
                                                     {"TIFR", 0x58, 0x00},
                                                     {"TIMSK", 0x59, 0x00},
                                                     {"GIFR", 0x5a, 0x00},
                                                     {"GIMSK", 0x5b, 0x00},
                                                     {"SPL", 0x5d, 0x00},
                                                     {"SPH", 0x5e, 0x00},
                                                     {"SREG", 0x5f, 0x00},
                                                     {nullptr, 0, 0}};

[[maybe_unused]] const jtag_device_def_type attiny45{
    "attiny45",
    0x9206,
    64,
    64, // 4096 bytes flash
    4,
    64,     // 256 bytes EEPROM
    15 * 2, // 15 interrupt vectors
    DEVFL_MKII_ONLY,
    attiny45_io_registers,
    false,
    0x07,
    0x0000, // fuses
    0x51,   // osccal
    1,      // OCD revision
    {
        0 // no mkI support
    },
    {
        CMND_SET_DEVICE_DESCRIPTOR,
        {0xF8, 0xE1, 0xFF, 0xF1, 0xFB, 0xFF, 0xBF, 0xEF}, // ucReadIO
        {0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00}, // ucReadIOShadow
        {0xC8, 0xE1, 0xFF, 0x71, 0xBB, 0x7F, 0xAD, 0xEB}, // ucWriteIO
        {0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00}, // ucWriteIOShadow
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // ucReadExtIO
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // ucReadIOExtShadow
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // ucWriteExtIO
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // ucWriteIOExtShadow
        0x00,                                                         // ucIDRAddress
        0X00,                                                         // ucSPMCRAddress
        0,                                                            // ucRAMPZAddress
        fill_b2(64),                                                  // uiFlashPageSize
        4,                                                            // ucEepromPageSize
        fill_b4(0x0000),                                              // ulBootAddress
        fill_b2(0x00),                                                // uiUpperExtIOLoc
        fill_b4(4096),                                                // ulFlashSize
        {0xBB, 0xFF, 0xBB, 0xEE, 0xBB, 0xCC, 0xB2, 0x0D, 0xBC, 0x02,
         0xB4, 0x02, 0xBA, 0x0D, 0xBB, 0xBC, 0x99, 0xE1, 0xBB, 0xAC}, // ucEepromInst
        {0xB4, 0x02, 0x12},                                           // ucFlashInst
        0x3E,                                                         // ucSPHaddr
        0x3D,                                                         // ucSPLaddr
        fill_b2(4096 / 64),                                           // uiFlashpages
        0x22,                                                         // ucDWDRAddress
        0x00,                                                         // ucDWBasePC
        0x00,                                                         // ucAllowFullPageBitstream
        fill_b2(0x00), // uiStartSmallestBootLoaderSection
        1,             // EnablePageProgramming
        0,             // ucCacheType
        fill_b2(0x60), // uiSramStartAddr
        0,             // ucResetType
        0,             // ucPCMaskExtended
        0,             // ucPCMaskHigh
        0,             // ucEindAddress
        fill_b2(0x1C), // EECRAddress
    },
    nullptr
};

} // namespace
