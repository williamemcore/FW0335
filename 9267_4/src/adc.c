/*///////////////////////////////////////////////////
 *  Project: LEML Mini Tx
 *  File: adc.c
 *
 *  Created on: 17 Oct 2016
 *///////////////////////////////////////////////////********************************************************************************************************************/
#include "adc.h"
#include "UART.h"
#include "alarm.h"
#include "ProcessCmd.h"

static adc_data_size_t pot_data;
long  adcSendS[12][counAdc+1];
uint8_t adcP[65] = {0x7e};
extern int counIni;
uint8_t adcContinue=0, adcRefresh=0;
extern uint32_t processTxLen;
extern uint8_t uartTx[alarmTableSize+11];

void adcIni()
{
    g_adc0.p_api->open(g_adc0.p_ctrl, g_adc0.p_cfg);
    g_adc0.p_api->scanCfg(g_adc0.p_ctrl, g_adc0.p_channel_cfg);

}

/*******************************************************************************************************************//**
 * @brief   Access function to allow other threads to read values.
 *
 *
 * @retval      none
***********************************************************************************************************************/
void getAadcValues(adc_data_size_t *p_pot_data)
{
    *p_pot_data = pot_data;

}

void adcRead(){
 //ADC_read:
    char number=1;
    int adcDa[11][counAdc+1];
    int adcDaS[11]={0,0,0,0,0,0,0,0,0,0};


 //accumulate in 1 timer IRQ:
    for(int i=0;i<number;i++){
        g_adc0.p_api->scanStart(g_adc0.p_ctrl);
        g_adc0.p_api->read(g_adc0.p_ctrl, ADC_REG_CHANNEL_0, (adc_data_size_t * const)&adcDa[0][i]);
        g_adc0.p_api->read(g_adc0.p_ctrl, ADC_REG_CHANNEL_1, (adc_data_size_t * const)&adcDa[1][i]);
        g_adc0.p_api->read(g_adc0.p_ctrl, ADC_REG_CHANNEL_2, (adc_data_size_t * const)&adcDa[2][i]);
        g_adc0.p_api->read(g_adc0.p_ctrl, ADC_REG_CHANNEL_7, (adc_data_size_t * const)&adcDa[3][i]);


        g_adc0.p_api->read(g_adc0.p_ctrl, ADC_REG_CHANNEL_8, (adc_data_size_t * const)&adcDa[4][i]);
        g_adc0.p_api->read(g_adc0.p_ctrl, ADC_REG_CHANNEL_9, (adc_data_size_t * const)&adcDa[5][i]);
        g_adc0.p_api->read(g_adc0.p_ctrl, ADC_REG_CHANNEL_10, (adc_data_size_t * const)&adcDa[6][i]);
        g_adc0.p_api->read(g_adc0.p_ctrl, ADC_REG_CHANNEL_16, (adc_data_size_t * const)&adcDa[7][i]);
        g_adc0.p_api->read(g_adc0.p_ctrl, ADC_REG_CHANNEL_22, (adc_data_size_t * const)&adcDa[8][i]);
        g_adc0.p_api->read(g_adc0.p_ctrl, ADC_REG_CHANNEL_21, (adc_data_size_t * const)&adcDa[9][i]);
        g_adc0.p_api->read(g_adc0.p_ctrl, ADC_REG_CHANNEL_20, (adc_data_size_t * const)&adcDa[10][i]);

    }


    for(int n=0;n<11;n++){
        adcDaS[n]=0;
        for(int i=0;i<number;i++)
            if(n==3)
                adcDaS[n]=+adcDa[n][i];
            else
                adcDaS[n]+=adcDa[n][i];
    }




//accumulate in counAdc timer IRQ:
    long sum=0;
    for(int n=0;n<11;n++){
        for(int i=0;i<(counAdc-1);i++)
            adcSendS[n][i]=adcSendS[n][i+1];

        adcSendS[n][counAdc-1]=adcDaS[n];


        sum=0;
        for(int i=0;i<counAdc;i++)
            sum+=adcSendS[n][i];

        sum/=counAdc;
        adcSend[n]=sum;


    }


    boardTemAdc=adcSend[0];
}
void adcSendUart(){
    for(int i=0;i<11;i++){
         adcP[8+i*2]=(uint8_t)(adcSend[i]>>8);
         adcP[9+i*2]=(uint8_t)(adcSend[i]&0xff);
    }
    adcP[5]=0x41;

 //respond_to_GUI:
    if(processTxLen>0){
        if(processTxLen<=61)
            g_uart0.p_api->write(g_uart0.p_ctrl, uartTx, 61u);
        else
            if(processTxLen<=122)
                g_uart0.p_api->write(g_uart0.p_ctrl, uartTx, 121u);
            else
                g_uart0.p_api->write(g_uart0.p_ctrl, uartTx, 132u);

        processTxLen=0;
        return;
    }

 //alarm:
    uint8_t valueAlarm[12]={0,0,0,0,0,0,0,0,0,0,0,0 };
    uint8_t *alarm=alarmCheck(valueAlarm, &adcSend[0]);

    for(int i=0;i<10;i++){        //11 bytes
        adcP[30+i]=alarm[i];
    }

//laser power calibrate(mW):
    adcP[52]=lasPCaliGui[0];        //power meter value
    adcP[53]=lasPCaliGui[1];

    adcP[59]=0x74;
    adcP[60]=0x03;


//ADC_send:
    if(counIni<20)
        g_uart0.p_api->write(g_uart0.p_ctrl, adcP, 61u);
    else{
        if(adcContinue==1){
            g_uart0.p_api->write(g_uart0.p_ctrl, adcP, 61u);
        }
        else{
            if(adcRefresh==1){
                adcRefresh=0;
                g_uart0.p_api->write(g_uart0.p_ctrl, adcP, 61u);

            }

        }

    }


}

