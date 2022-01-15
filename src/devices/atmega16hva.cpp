#include "../jtag.h"

namespace {

constexpr gdb_io_reg_def_type atmega16hva_io_registers[] = {
    {"PINA", 0x20, 0x00},    {"DDRA", 0x21, 0x00},          {"PORTA", 0x22, 0x00},
    {"PINB", 0x23, 0x00},    {"DDRB", 0x24, 0x00},          {"PORTB", 0x25, 0x00},
    {"PINC", 0x26, 0x00},    {"PORTC", 0x28, 0x00},         {"TIFR0", 0x35, 0x00},
    {"TIFR1", 0x36, 0x00},   {"OSICSR", 0x37, 0x00},        {"EIFR", 0x3c, 0x00},
    {"EIMSK", 0x3d, 0x00},   {"GPIOR0", 0x3e, 0x00},        {"EECR", 0x3f, 0x00},
    {"EEDR", 0x40, 0x00},    {"EEAR -- EEARL", 0x41, 0x00}, {"GTCCR", 0x43, 0x00},
    {"TCCR0A", 0x44, 0x00},  {"TCCR0B", 0x45, 0x00},        {"TCNT0L", 0x46, 0x00},
    {"TCNT0H", 0x47, 0x00},  {"OCR0A", 0x48, 0x00},         {"OCR0B", 0x49, 0x00},
    {"GPIOR1", 0x4a, 0x00},  {"GPIOR2", 0x4b, 0x00},        {"SPCR", 0x4c, 0x00},
    {"SPSR", 0x4d, 0x00},    {"SPDR", 0x4e, 0x00},          {"DWDR", 0x51, 0x00},
    {"SMCR", 0x53, 0x00},    {"MCUSR", 0x54, 0x00},         {"MCUCR", 0x55, 0x00},
    {"SPMCSR", 0x57, 0x00},  {"SPL", 0x5d, 0x00},           {"SPH", 0x5e, 0x00},
    {"SREG", 0x5f, 0x00},    {"WDTCSR", 0x60, 0x00},        {"CLKPR", 0x61, 0x00},
    {"PRR0", 0x64, 0x00},    {"FOSCCAL", 0x66, 0x00},       {"EICRA", 0x69, 0x00},
    {"TIMSK0", 0x6e, 0x00},  {"TIMSK1", 0x6f, 0x00},        {"VADCL", 0x78, 0x00},
    {"VADCH", 0x79, 0x00},   {"VADCSR", 0x7a, 0x00},        {"VADMUX", 0x7c, 0x00},
    {"DIDR0", 0x7e, 0x00},   {"TCCR1A", 0x80, 0x00},        {"TCCR1B", 0x81, 0x00},
    {"TCNT1L", 0x84, 0x00},  {"TCNT1H", 0x85, 0x00},        {"OCR1A", 0x88, 0x00},
    {"OCR1B", 0x89, 0x00},   {"ROCR", 0xc8, 0x00},          {"BGCCR", 0xd0, 0x00},
    {"BGCRR", 0xd1, 0x00},   {"CADAC0", 0xe0, 0x00},        {"CADAC1", 0xe1, 0x00},
    {"CADAC2", 0xe2, 0x00},  {"CADAC3", 0xe3, 0x00},        {"CADCSRA", 0xe4, 0x00},
    {"CADCSRB", 0xe5, 0x00}, {"CADRC", 0xe6, 0x00},         {"CADICL", 0xe8, 0x00},
    {"CADICH", 0xe9, 0x00},  {"FCSR", 0xf0, 0x00},          {"BPIMSK", 0xf2, 0x00},
    {"BPIFR", 0xf3, 0x00},   {"BPSCD", 0xf5, 0x00},         {"BPDOCD", 0xf6, 0x00},
    {"BPCOCD", 0xf7, 0x00},  {"BPDHCD", 0xf8, 0x00},        {"BPCHCD", 0xf9, 0x00},
    {"BPSCTR", 0xfa, 0x00},  {"BPOCTR", 0xfb, 0x00},        {"BPHCTR", 0xfc, 0x00},
    {"BPCR", 0xfd, 0x00},    {"BPPLR", 0xfe, 0x00},         {nullptr, 0, 0}};

[[maybe_unused]] const jtag_device_def_type atmega16hva{
    "atmega16hva",
    0x940C,
    128,
    128, // 16384 bytes flash
    4,
    64,     // 256 bytes EEPROM
    21 * 4, // 21 interrupt vectors
    DEVFL_MKII_ONLY,
    atmega16hva_io_registers,
    false,
    0x07,
    0x0000, // fuses
    0x66,   // osccal
    1,      // OCD revision
    {
        0 // no mkI support
    },
    {
        CMND_SET_DEVICE_DESCRIPTOR,
        {0x7F, 0x01, 0xE0, 0xF0, 0xFB, 0x3F, 0xB8, 0xE0}, // ucReadIO
        {0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00}, // ucReadIOShadow
        {0x37, 0x01, 0x00, 0xE0, 0xFB, 0x1F, 0xA8, 0xE0}, // ucWriteIO
        {0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00}, // ucWriteIOShadow
        {0x53, 0xC2, 0x00, 0x57, 0x33, 0x03, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x01, 0x03, 0x00, 0x7F, 0x03, 0xED, 0x7F}, // ucReadExtIO
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // ucReadIOExtShadow
        {0x50, 0xC2, 0x00, 0x50, 0x33, 0x03, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x70, 0x00, 0xED, 0x3F}, // ucWriteExtIO
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // ucWriteIOExtShadow
        0x00,                                                         // ucIDRAddress
        0x57,                                                         // ucSPMCRAddress
        0,                                                            // ucRAMPZAddress
        fill_b2(128),                                                 // uiFlashPageSize
        4,                                                            // ucEepromPageSize
        fill_b4(0),                                                   // ulBootAddress
        fill_b2(0xFE),                                                // uiUpperExtIOLoc
        fill_b4(16384),                                               // ulFlashSize
        {0xBD, 0xF2, 0xBD, 0xE1, 0xBB, 0xCF, 0xB4, 0x00, 0xBE, 0x01,
         0xB6, 0x01, 0xBC, 0x00, 0xBB, 0xBF, 0x99, 0xF9, 0xBB, 0xAF}, // ucEepromInst
        {0xB6, 0x01, 0x11},                                           // ucFlashInst
        0x3E,                                                         // ucSPHaddr
        0x3D,                                                         // ucSPLaddr
        fill_b2(16384 / 128),                                         // uiFlashpages
        0x31,                                                         // ucDWDRAddress
        0x00,                                                         // ucDWBasePC
        0x00,                                                         // ucAllowFullPageBitstream
        fill_b2(0x00),  // uiStartSmallestBootLoaderSection
        1,              // EnablePageProgramming
        0,              // ucCacheType
        fill_b2(0x100), // uiSramStartAddr
        0,              // ucResetType
        0,              // ucPCMaskExtended
        0,              // ucPCMaskHigh
        0,              // ucEindAddress
        fill_b2(0x1F),  // EECRAddress
    },
    nullptr
};

} // namespace
