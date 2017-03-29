#include "rfGain.h"
#include "UART.h"
#include<math.h>
#include <stdio.h>
#include "dac.h"
#include "adc.h"

#include "flashHandle.h"

extern uint8_t adcP[65];
int setAttn[21]={0,500,1000,1500,2000,2500,3000,4000,4500,5000,5500,6000,6500,7000,7500,8000,8500,9000,9500,10000,10500};
uint8_t dbIndex=0;
void checkGain(){
  //input:
    float tmvIn=(float)(3300*(adcSend[3]&0xfff)/0x1000);
    float rfVIn =45*(tmvIn-360)/1090;
    float RfDbmIn=rfVIn-45;

  //output:
    float tmvOut=(float)(3300*(adcSend[5]&0xfff)/0x1000);//note:adcSend[4]is SBS-MON
    float rfVOut =45*(tmvOut-360)/1090;
    float RfDbmOut=rfVOut-45;



 //get_current_db:
    int dbCurrent=(int)(1000*log10(RfDbmOut/RfDbmIn));//*100 here

 //check_current_Attn_DAC:
    for(int i=0;i<19;i++){
        if((dacValue[0]>setAttn[i])&&(dacValue[0]<=setAttn[i+1])){

            dbIndex=(uint8_t)(i+1);

            break;
        }
    }

//set_gain(ATTN):
    if((dbCurrent&0x8000)&&(setGainDb&0x8000)){             //both<0
        if(dbCurrent>setGainDb){
            if(dbIndex<19)
                dacValue[0]=setAttn[++dbIndex];
        }
        else{
            if(dbIndex>1)
                dacValue[0]=setAttn[--dbIndex];
        }
    }
    else{
        if((dbCurrent&0x8000)&&((setGainDb&0x8000)==0)){    //dbCurrent<0, setGainDb>0
            if(dbIndex<19)
                dacValue[0]=setAttn[++dbIndex];


        }
        else{
            if(((dbCurrent&0x8000)==0)&&(setGainDb&0x8000)){//dbCurrent>0, setGainDb<0
                if(dbIndex>1)
                    dacValue[0]=setAttn[--dbIndex];

            }
            else{                                           //both>0
                if(dbCurrent<setGainDb){
                    if(dbIndex<19)
                        dacValue[0]=setAttn[++dbIndex];
                }
                else{
                    if(dbIndex>0)
                        dacValue[0]=setAttn[--dbIndex];
                }
            }
        }
    }



}
