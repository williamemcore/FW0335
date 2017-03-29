#include "hal_data.h"



// DAC CONSTANTS

#define      MAX_NUM_DAC_CH             24
#define      DAC                        3
#define      ADC                        5
#define      FLASH                      7
#define      EEPROM                     0xB
#define      LCD                        0xF

#define      DEVICE_INFO                0x10

#define      Query                      0x01
#define      Set                        0x02
#define      Display                    0x03
#define      Error                      0x04



typedef union BYTE_S
{
    uint8_t     a[2];
    int         b;

}bYtes;


// Structure
typedef struct PARSE_INFO
{
    bYtes           Length;
    uint8_t         MsgType; // read: 1, write: 2, display 3, error: 4
    bYtes           MsgId;
    uint8_t         DataBuf[128];
    uint8_t         bDataTotal;
}ParseInfo;



typedef struct DAC_INFO
{
    uint8_t bChannel;
    float fVolt;
    bool  Enable;
}DacInfo;


// TYPE DEFINE
ParseInfo       sParseInfo;

DacInfo         Dac_Save_Data[MAX_NUM_DAC_CH];
bool            flgTxBytes;
