/*
 * tc_fc_table.c
 *
 *
 *
 */
#include "tc_fc_table.h"
#include "UART.h"
#include<math.h>
#include <stdio.h>
#include "dac.h"

#include "ProcessCmd.h"
#include "flashHandle.h"
extern uint32_t flashB2;
int boardTemT=0;

void readTCFC(){
    readFlashPage(flashB2,tcFcSize);//page2
    for(int i=0;i<24;i++){                 //24 DAC channels
         for(int j=0;j<9;j++){
             tcTab[i][j]=(int)(dstPage[i*18+j*2]*256+dstPage[i*18+j*2+1]); //TC
             fcTab[i][j]=(int)(srcPage[fcStart+i*18+j*2]<<8|srcPage[fcStart+i*18+j*2+1]);
         }
         tcEnable[i]=dstPage[tcTabSize+i];
         fcEnable[i]=dstPage[fcStart+fcTabSize+i];

         checkTcFc(i);

    }
}


void checkTcFc(int channel){
    if(dacValue[channel]<0||(dacValue[channel]>dacMax))
        dacValue[channel]=0;

    long sendValue=(long)dacValue[channel];
#if 1
  //TC:

    if(tcEnable[channel]==1){       // TC enable

        checkSetTc(channel);

        sendValue+=(long)tcCurValue[channel];
        sendValue&=0xffff;

    }
#endif
#if 1
  //FC:
    if(fcEnable[channel]==1){       //FC enable
        checkSetFc(channel);
        sendValue+=(long)fcCurValue[channel];
        sendValue&=0xffff;

    }
#endif

    if((sendValue<0)||(sendValue&0x8000)){

        SPI_write_data((uint8_t)channel,0);
        return;
    }

 //send_DAC:
    sendValue/=10;
    sendValue&=0xffff;

    int valueMax=1023;

    switch(channel){
        case 16:                        //FLSR
            valueMax=flsrMax;
            break;
        case 17:                        //RLSR
            valueMax=rlsrMax;
            break;

    }
    if((sendValue>=0)&&(sendValue<=valueMax)){       //send range
           SPI_write_data((uint8_t)channel,(int)sendValue);
    }
    else{            //value over
        if(sendValue>=valueMax){
            SPI_write_data((uint8_t)channel,valueMax);

        }


    }


}




void checkSetTc(int channel)
{

    int tcPoint[9]={-100,0,100,200,300,400,500,600,700};
    long t1=(long)(boardTemAdc);
    t1&=0xffff;

    long t2=(t1*3300)>>12;
    t2=t2&0xffff;
    t2-=500;

    boardTemT=(int)t2;

    if(channel<24){
        if(dacValue[channel]<0)return;

        boardTemT&=0xffff;
        if((boardTemT>=(tcPoint[8]))&&(!(boardTemT&0x8000))){
            getTcValue(channel,8);
        }
        else{

            for(int i=0;i<8;i++){
                if(boardTemT&0x8000)
                    getTcValue(channel,0);
                else{
                    if((boardTemT>=tcPoint[i])&&(boardTemT<tcPoint[i+1])){
                        getTcValue(channel,i);


                    }
                }
            }

        }

    }

}

void checkSetFc(int channel)
{
    if(channel<24){
        if(dacValue[channel]<0)return;


        fibLen&=0xffff;
        if(fibLen>=(fcPoint[8]*10)){
            getFcValue(channel,8);

        }
        else{
            for(int i=0;i<8;i++){
                if((fibLen>=(fcPoint[i]*10))&&(fibLen<(fcPoint[i+1]*10))){
                    getFcValue(channel,i);

                }

            }

        }


    }
}


void getTcValue(int channel,int point){
     int tcPoint[9]={-100,0,100,200,300,400,500,600,700};
     long tcCurValueF=0,vCons=0;
     int k1=0;
     long k=0;



     if((point>=0) && (point<8)){

         k1=tcTab[channel][point+1]-tcTab[channel][point];
         k1&=0xffff;
         if(k1&0x8000){
             k=((long)(~(k1-1)));
             k&=0xffff;
             k=0-k;

         }
         else{
             k=(long)k1;
             k&=0xffff;
         }

         long v1=(long)tcPoint[point];
         v1&=0xffff;
         v1*=k;

         long v2=(long)tcTab[channel][point];
         v2&=0xffff;
         v2*=100;

         vCons=v2-v1;

         long v=(long)boardTemT;
         v&=0xffff;
         v*=k;
         tcCurValueF=v+vCons;

     }
     else{
         if(point==8){
             //k:
               k1=tcTab[channel][point]-tcTab[channel][point-1];
               k1&=0xffff;
               if(k1&0x8000){
                   k=((long)(~(k1-1)));
                   k&=0xffff;
                   k=0-k;

               }
               else{
                   k=(long)k1;
                   k&=0xffff;
               }

             //constant:
               long v1=(long)tcPoint[point-1];
               v1&=0xffff;
               v1*=k;

               long v2=(long)tcTab[channel][point-1];
               v2&=0xffff;
               v2*=100;

               vCons=v2-v1;

            //tc_value_long:
               long v=(long)boardTemT;
               v&=0xffff;
               v*=k;
               tcCurValueF=v+vCons;
         }
         else
             tcCurValueF=0;
     }


   //TC value:
     int t=(int)(round(tcCurValueF/100));
     t&=0xffff;
     tcCurValue[channel]=t;



}


void getFcValue(int channel,int point){

     long fcCurValueF=0,vCons;
     int k1=0;
     long k=0;



     if((point>=0) && (point<8)){
         k1=fcTab[channel][point+1]-fcTab[channel][point];

         k1&=0xffff;
         if(k1&0x8000){
             k=(long)(~(k1-1));
             k&=0xffff;
             k*=100;
             k/=fcPointStep;
             k=0-k;

         }
         else{
             k=(long)k1*100/fcPointStep;
         }
         vCons=(long)(fcTab[channel][point]*100-(int)(k*(long)fcPoint[point]));
         fcCurValueF=(long)((k*(long)(fibLen)+vCons*10));


     }
     else{
         if(point==8){
             k1=fcTab[channel][point]-fcTab[channel][point-1];

             k1&=0xffff;
             if(k1&0x8000){
                 k=((long)(~(k1-1)));
                 k&=0xffff;
                 k*=100;
                 k/=fcPointStep;
                 k=0-k;

             }
             else{
                 k=(long)k1*100/fcPointStep;
             }
             vCons=(long)(fcTab[channel][point-1]*100-(int)(k*(long)fcPoint[point-1]));
             fcCurValueF=(long)((k*(long)(fibLen)+vCons*10));
         }
         else
             return ;
     }


   //fc value:
      fcCurValue[channel]=(int)(fcCurValueF/1000);       //fcCurValue[channel]*10 here

}
