#include "UART.h"
#include<math.h>
#include <stdio.h>
#include "dac.h"
#include "adc.h"
#include "ProcessCmd.h"
#include "flashHandle.h"
#include "tc_fc_table.h"

#include "tx_api.h"
#include "alarm.h"
#include "timer.h"

extern uint32_t flashB2;


extern uint8_t getRequire;
extern char freqRun;

char counResend=0;

extern uint8_t flashRun;
extern void wait(int value);


void UART0_Callback(uart_callback_args_t * p_args)// UART ISR
{

    if ( p_args->event == UART_EVENT_RX_CHAR )
    {

        RxData[ptrIn] = (uint8_t)p_args->data;
        ptrIn++;

        if ( ptrIn >=( UART_BUF_MAX -1)){
            ptrIn = 0;
        }
        if ((ptrIn >13)&&(RxData[ptrIn-2]==0x74)&&(RxData[ptrIn-1]==0x03)){
                if(flashRun==0)
                    EmcoreCommands();

            ptrIn = 0;
        }

    }


    if ( p_args->event == UART_EVENT_TX_COMPLETE )
    {

        flgDummy=false;

    }

}



