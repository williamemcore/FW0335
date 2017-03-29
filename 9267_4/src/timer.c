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
#include "rfGain.h"
extern uint8_t alarmLed;

extern uint8_t agc;

char counTimer40=0,counTimer400=0, counTimer5s=0;

extern uint8_t getRequire;
extern char freqRun;
extern void wait(int value);

int counIni=0;


void gTimer_ISR(timer_callback_args_t * p_args) // 40 MS TIMER
{

    if(p_args->event ==0)
     {
         flgDummy=false;
     }


  //power_on_Initial:
    if((counIni<40)||(alarmLed==1)){
        counIni++;
        powerLed();
    }

//send_DAC_to_GUI:
    if(getRequire!=0){
         getRequire=0;
     }
     else{

      //set_frequency_running:
         if(freqRun!=0){
             freqRun=0;
             return;
         }
         else{


#if 0
             if(counTimer400>=12){
                 counTimer400=0;

               //read_TCFC:
                 readTCFC();                             //4.8s
                 if(agc==1)
                     checkGain();
                 return;
             }
             else{
                 adcRead();                             //40 ms

                 if(counTimer40<11){
                     counTimer40++;

                     return;
                 }
                 else{
                      counTimer40=0;
                      counTimer400++;

                  //ADC_send:
                      adcSendUart();                    //400ms

                 }

             }
#endif
        }


   }

}
void powerLed(){
    if(counIni%2==0){
        if(counIni%4==0)
             g_ioport.p_api->pinWrite(IOPORT_PORT_04_PIN_07, IOPORT_LEVEL_HIGH);
        else
            g_ioport.p_api->pinWrite(IOPORT_PORT_04_PIN_07, IOPORT_LEVEL_LOW);

    }

}

void Toggle_Indicator(void)
{
    flgTimerToggle ^= 1;
    if(!flgTimerToggle)
    {

    }else
    {

    }

}
