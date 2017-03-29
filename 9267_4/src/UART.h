#include "hal_data.h"
#include<math.h>
#include<string.h>

#define   UART_BUF_MAX    123
void Process_UART();
enum{
    ParseTime_1,
    ParseTime_2,
    ParseTime_3,
    ParseTime_4,
    ParseTime_5,

    ParseTimeNone

};

uint8_t TxData [64];

uint8_t RxData[256];
uint8_t ptrIn, ptrOut;
uint8_t bParseTimer;
uint8_t bParseRxBuf;
uint8_t bFlashCnt;

//int     nDuration;

ioport_level_t level;
bsp_leds_t leds;
ssp_err_t openResult;
ssp_err_t writeResult;
bool flgDummy;
bool flgTimerToggle;
uint16_t nTimerCnt;
uint8_t  bLedCnt;

ssp_err_t err;

// Function Protocol
void Toggle_Indicator(void);

//ADC:
int boardTemAdc;

