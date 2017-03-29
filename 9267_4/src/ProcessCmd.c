#include "ProcessCmd.h"
#include "PROTOCOL.h"
#include "Global.h"
#include "UART.h"
#include "flashHandle.h"
#include "dac.h"
#include "adc.h"
#include "tc_fc_table.h"
#include "alarm.h"
#include<math.h>
#include <stdio.h>
#include "alarm.h"
#include "boardInfo.h"
#include "rfGain.h"

extern uint32_t flashB1,flashB2,flashB3,flashB4;
uint8_t uartTx[alarmTableSize+11]={SOT};;
uint8_t getRequire=0;

uint16_t    nSource;
 uint8_t     bChannel;
float       fVoltage;

int    nValue;//, temSet=0xffff;
extern uint8_t adcP[65];
int sendData[22];
unsigned int freq;
char freqRun=0;//,saveRun=0,saveTcRun=0;
extern void initSBSSPIport(unsigned int sbsFreqMhz);
extern uint8_t adcContinue, adcRefresh;
extern uint8_t firmV[5];
uint32_t  processTxLen=0;

uint8_t agc=0;

void EmcoreCommands(void)
{
    uint8_t channel;

  //MsgType
    switch(RxData[5]){
        case 0x02://single channel set DAC
            Dac_Save_Data[sParseInfo.DataBuf[0]].Enable=true;

            bChannel=sParseInfo.DataBuf[0];
            Dac_Save_Data[sParseInfo.DataBuf[0]].bChannel=bChannel;

            fVoltage=ArrayToF32_BigEndian(&sParseInfo.DataBuf[1]);
            Dac_Save_Data[sParseInfo.DataBuf[0]].fVolt=fVoltage;


          //channel:
            bChannel=(uint16_t)RxData[8];
            if(bChannel>23)bChannel=23;

         //value:
            nValue=RxData[10];      //high-byte

            nValue<<=8;
            nValue&=0xFF00;
            nValue +=(uint16_t)RxData[9];//low-byte
            dacValue[bChannel]=nValue;

       //set_DAC:
            if(tcEnable[bChannel]==1)      //TC
                nValue+=tcCurValue[bChannel];///10;
            if(fcEnable[bChannel]==1)      //FC
                nValue+=fcCurValue[bChannel];///10;

            SPI_write_data(bChannel,nValue/10);


            if(bChannel==17){
                adcP[1]=bChannel;
                adcP[2]=tcEnable[0];
                adcP[3]=(uint8_t)(nValue>>8);
                adcP[4]=(uint8_t)(nValue&0xff);
                adcP[6]=(uint8_t)((nValue/10)>>8);
                adcP[7]=(uint8_t)((nValue/10)&0xff);
            }
            RxData[5]=0;
            break;
        case 4: //set all dac
        //update DAC array:
            for(int i=0;i<25;i++){
                dacValue[i]=((RxData[8+i*2]<<8)|RxData[9+i*2+1]);
                if(dacValue[i]>dacMax)
                    dacValue[i]=0;

            }

            readTCFC();     //reset 24-channel DAC

            freqRun=1;      //frequency
            freq=(unsigned int)RxData[8+48]*256+RxData[9+48];

            if((freq>=2400) && (freq<=2725)){
                initSBSSPIport(freq);        //57=compensate

            }
            break;

        case 0x06://save dac

#if 1       //flash <- RX
             for(int i=0;i<50;i++){
                 src[i]=RxData[8+i];
            }
#endif

        //checksum:
            if(checkSum(RxData)!=RxData[RxData[2]+2])break;



            writeFlash(flashB1,&src[0],50);
            uartTx[5]=0x07;
            processTxLen=14;

            RxData[5]=0;


            break;
        case 8: //get dac
            readFlash(flashB1,50);//page1
            for(int i=0;i<50;i++){
                uartTx[8+i]=dst[i];
            }

        //update DAC array:
            for(int i=0;i<25;i++){
                dacValue[i]=((dst[i*2]<<8)|dst[i*2+1]);
                if(dacValue[i]>dacMax)
                    dacValue[i]=0;

            }

         //set_send:
            uartTx[1]=0;
            uartTx[2]=56;
            uartTx[58]=checkSum(uartTx);

            uartTx[59]=0x74;
            uartTx[60]=0x03;


            uartTx[5]=9;
            processTxLen=61;

            readTCFC();

            freqRun=1;      //frequency
            freq=(unsigned int)uartTx[8+48]*256+uartTx[9+48];

            if((freq>=2400) && (freq<=2725)){
                initSBSSPIport(freq);        //57=compensate

            }
            break;
        case 0x0a: //frequency set

            freqRun=1;
            freq=(unsigned int)RxData[8]*256+RxData[9];


            RxData[5]=0;
            if((freq>=2400) && (freq<=2725)){
                initSBSSPIport(freq);        //57=compensate

            }

            adcP[3]=(uint8_t)(freq>>8);
            adcP[4]=(uint8_t)(freq&0xff);
            break;


        case 0x10://Save TC table to board
            if(checkSum(RxData)!=RxData[RxData[2]+2])break;
            channel=RxData[8];

            if(channel<24){
                readFlashPage(flashB2,tcFcSize);//page2
                for(int i=0;i<tcFcSize;i++){
                    srcPage[i]=dstPage[i];

                }

                for(int i=0;i<18;i++){      //TC table
                    srcPage[channel*18+i]=RxData[9+i];
                }

                srcPage[tcTabSize+channel]=RxData[9+18];      //enable

                writeFlash(flashB2,&srcPage[0],tcFcSize);
            }
            processTxLen=14;
            uartTx[5]=0x11;


          //updata TC array:
            for(int i=0;i<9;i++){//point1- point9
                tcTab[channel][i]=((int)(srcPage[channel*18+i*2]<<8|srcPage[channel*18+i*2+1]))/10;//note high-low bytes not invert
            }

         //update_TC_channel_enable:
            tcEnable[channel]=RxData[9+18];

            RxData[5]=0;
            break;
        case 0x12://require TC table from board
            channel=RxData[8];

            if(channel<24){
                readFlash(flashB2+ (uint32_t)(channel*18),18);               //TC
            }

            uartTx[8]=channel;
            for(int i=0;i<18;i++){
                uartTx[9+i]=dst[i];
            }

            readFlash(flashB2+tcTabSize+channel,1);     //channel enable
            uartTx[9+18]=dst[0];

            uartTx[5]=0x13;
            uartTx[1]=0x12;
            adcP[6]=uartTx[9+18];

            processTxLen=31;
            break;
        case 0x14:                                                      //Require TC data
            channel=RxData[8];
            uartTx[8]=  channel;                                           //channel
            uartTx[9]=(uint8_t)(boardTemAdc>>8);                          //module temp
            uartTx[10]=(uint8_t)(boardTemAdc&0xff);

            uartTx[11]=(uint8_t)(dacValue[channel]>>8);
            uartTx[12]=(uint8_t)(dacValue[channel]&0xff);

            uartTx[13]=(uint8_t)(tcCurValue[channel]>>8);
            uartTx[14]=(uint8_t)(tcCurValue[channel]&0xff);

            uartTx[5]=0x15;
            uartTx[1]=0x14;//(uint8_t)(boardTemAdc>>8);
            uartTx[2]=(uint8_t)(boardTemAdc&0xff);
            uartTx[3]=(uint8_t)(tcCurValue[channel]>>8);
            uartTx[4]=(uint8_t)(tcCurValue[channel]&0xff);
            uartTx[6]=RxData[8];
            uartTx[7]=channel;

            processTxLen=19;


            break;
        case 0x16://TC single channel on/off
            channel=RxData[8];

            break;
        case 0x18://save TC all channel on/off
            if(checkSum(RxData)!=RxData[RxData[2]+2])break;
            readFlashPage(flashB2,tcFcSize);//page2
            for(int i=0;i<tcFcSize;i++){
                srcPage[i]=dstPage[i];

            }

            for(int i=0;i<24;i++){      //TC
                srcPage[tcTabSize+i]=RxData[8+i];
            }
            writeFlash(flashB2,&srcPage[0],tcFcSize);



            uartTx[5]=0x19;
            processTxLen=14;

          //updata TC enable array:
            for(int i=0;i<24;i++){//
                tcEnable[i]=srcPage[tcTabSize+i];
            }


            adcP[1]=0x18;//(uint8_t)(tcEnable[0]<<8);
            adcP[2]=(uint8_t)(tcEnable[1]&0xff);
            adcP[3]=(uint8_t)(tcEnable[2]<<8);
            adcP[4]=(uint8_t)(tcEnable[3]&0xff);

            RxData[5]=0;

            break;


        case 0x1a://Require TC all channels on/off

             readFlash(flashB2+tcTabSize,24);//page2

             for(int i=0;i<24;i++){
                 uartTx[8+i]=dst[i];
             }

             uartTx[5]=0x1b;
             processTxLen=36;

            break;
        case 0x20://Save fc table to board
            if(checkSum(RxData)!=RxData[RxData[2]+2])break;
            channel=RxData[8];

            if(channel<24){
                readFlashPage(flashB2,tcFcSize);//page2
                for(int i=0;i<tcFcSize;i++){
                    srcPage[i]=dstPage[i];

                }

                for(int i=0;i<18;i++){      //fc table
                    srcPage[fcStart+channel*18+i]=RxData[9+i];
                }

                srcPage[fcStart+fcTabSize+channel]=RxData[9+18];      //enable
                srcPage[tcFcSize-2]=RxData[9+19];      //fib Length
                srcPage[tcFcSize-1]=RxData[9+20];      //fib Length

                writeFlash(flashB2,&srcPage[0],tcFcSize);
            }

         //updata fc array:
            for(int i=0;i<9;i++){//point1- point9
                fcTab[channel][i]=((int)(srcPage[fcStart+channel*18+i*2]<<8|srcPage[fcStart+channel*18+i*2+1]));///10;//note high-low bytes not invert
            }

         //update_fc_channel_enable:
            fcEnable[channel]=RxData[9+18];

         //update fib length:
            fibLen=(int)((srcPage[tcFcSize-2]<<8)+(srcPage[tcFcSize-1]&0xff));

            uartTx[5]=0x21;

            processTxLen=14;

            RxData[5]=0;
            break;
        case 0x22://require fc table from board

            channel=RxData[8];

            if(channel<24){
                readFlash(flashB2+ (uint32_t)(fcStart+channel*18),18);//page2
            }

            uartTx[8]=channel;                                    //channel#
            for(int i=0;i<18;i++){                              //FC
                uartTx[9+i]=dst[i];
            }

            readFlash(flashB2+fcStart+fcTabSize+channel,1);     //channel enable
            uartTx[9+18]=dst[0];

            readFlash(flashB2+tcFcSize-2,2);     //fiber length
            fibLen=(int)((dst[0]<<8)|(dst[1]&0xff));
            uartTx[9+18+1]=(uint8_t)(fibLen>>8);
            uartTx[9+18+2]=(uint8_t)(fibLen&0xff);

            uartTx[5]=0x23;

            processTxLen=31;
            break;
        case 0x24:                                                      //Require fc data
            channel=RxData[8];

            uartTx[8]=  channel;                                           //channel#
            uartTx[9]=(uint8_t)(fibLen>>8);                               //fiber length
            uartTx[10]=(uint8_t)(fibLen&0xff);


            uartTx[11]=(uint8_t)(dacValue[channel]>>8);                   //DAC raw value
            uartTx[12]=(uint8_t)(dacValue[channel]&0xff);

            uartTx[13]=(uint8_t)(fcCurValue[channel]>>8);                 //fc correction: realValue*10
            uartTx[14]=(uint8_t)(fcCurValue[channel]&0xff);

            uartTx[5]=0x25;
            processTxLen=19;


            break;
        case 0x26://fc single channel on/off
            channel=RxData[8];

            break;
        case 0x28://save fc all channel on/off
            if(checkSum(RxData)!=RxData[RxData[2]+2])break;
            readFlashPage(flashB2,tcFcSize);//page2
            for(int i=0;i<tcFcSize;i++){
                srcPage[i]=dstPage[i];

            }

            for(int i=0;i<24;i++){      //fc enable
                srcPage[fcStart+fcTabSize+i]=RxData[8+i];
            }
            writeFlash(flashB2,&srcPage[0],tcFcSize);



            uartTx[5]=0x29;
            processTxLen=14;
          //updata fc enable array:
            for(int i=0;i<24;i++){//
                fcEnable[i]=srcPage[fcStart+fcTabSize+i];
            }



            RxData[5]=0;

            break;


        case 0x2a://Require fc enable
             readFlash(flashB2+fcStart+fcTabSize,24);//page2

             for(int i=0;i<24;i++){
                 uartTx[8+i]=dst[i];
             }

             uartTx[5]=0x2b;
             processTxLen=36;
            break;
        case 0x30://Save alarm table to board
            if(checkSum(RxData)!=RxData[RxData[2]+2])break;

            for(int i=0;i<alarmTableSize;i++){
                srcPage[i]=RxData[8+i];
            }

            writeFlash(flashB3,&srcPage[0],alarmTableSize);

         //updata alarm table:
            for(int i=0;i<10;i++){
                for(int j=0;j<5;j++)
                    alarmTable[i][j]=srcPage[i*11+j*2]<<8|srcPage[i*11+j*2+1];   //alarm value


                alarmEna[i]=srcPage[i*11+10];                                   //alarm enable
            }

            uartTx[5]=0x31;
            processTxLen=14;
            RxData[5]=0;
            break;
        case 0x32://require alarm table from board
            readFlash(flashB3,alarmTableSize);//page3
            for(int i=0;i<alarmTableSize;i++){                              //FC
                uartTx[8+i]=dst[i];
            }

            uartTx[1]=uartTx[12];
            uartTx[2]=uartTx[13];
            uartTx[3]=uartTx[14];
            uartTx[4]=uartTx[15];
            uartTx[6]=uartTx[16];
            uartTx[7]=uartTx[17];


            uartTx[0]=SOT;
            uartTx[5]=0x33;

            processTxLen=132;

            break;

        case 0x34://require alarm calibration from board
            for(int i=0;i<4;i++){
                uartTx[8+i*2]=(uint8_t)(adcSend[i]>>8);
                uartTx[9+i*2]=(uint8_t)(adcSend[i]&0xff);
            }
            for(int i=4;i<10;i++){                          //no SBS-MON
                uartTx[8+i*2]=(uint8_t)(adcSend[i+1]>>8);
                uartTx[9+i*2]=(uint8_t)(adcSend[i+1]&0xff);
            }
            uartTx[28]=lasPCaliGui[0];        //power meter value
            uartTx[29]=lasPCaliGui[1];


            uartTx[0]=SOT;
            uartTx[5]=0x35;
            processTxLen=33;
            break;
        case 0x42://Single refresh

            uartTx[5]=0x41;
            adcRefresh=1;

            break;
        case 0x44://Continue/discontinue
            if(RxData[8]==1)
                adcContinue=1;
            else
                adcContinue=0;

            break;
        case 0x50://Save board info to board
            for(int i=0;i<infoSize;i++){       //general info
                srcPage[i]=RxData[8+i];
            }

            srcPage[lasPCaliAdd]=lasPCaliGui[0];
            srcPage[lasPCaliAdd+1]=lasPCaliGui[1];

            writeFlash(flashB4,&srcPage[0],boardInfoSize);

            uartTx[5]=0x51;
            processTxLen=14;
            RxData[5]=0;
            break;
        case 0x52://require board infofrom board
            readFlash(flashB4,infoSize);

            for(int i=0;i<infoSize;i++){               //general info
                uartTx[8+i]=dst[i];
            }

            for(int i=0;i<5;i++){                           //firmware version
                uartTx[8+infoSize+i]=firmV[i];
            }

            uartTx[119]=0x74;
            uartTx[120]=0x03;
            uartTx[5]=0x53;
            processTxLen=121;

            break;
        case 0x60://Calibrate laser power &  Save
         //check data:
            lasPCaliGui[0]=RxData[8];
            lasPCaliGui[1]=RxData[9];

         //write to flash:
            readFlash(flashB4,boardInfoSize);

            for(int i=0;i<infoSize;i++){                        //general info
                srcPage[i]=dst[i];
            }

            srcPage[lasPCaliAdd]=lasPCaliGui[0];
            srcPage[lasPCaliAdd+1]=lasPCaliGui[1];

            writeFlash(flashB4,&srcPage[0],boardInfoSize);


            uartTx[8]=lasPCaliGui[0];
            uartTx[9]=lasPCaliGui[1];
            uartTx[12]=0x74;
            uartTx[13]=0x03;
            uartTx[5]=0x61;

            processTxLen=14;

            break;
        case 0x64://Laser initial type

            break;

        case 0x70://Gain Control   Save
         //check data:
            if(((uint8_t) RxData[8])==0x01)
                agc=1;
            else
                agc=0;


        //checksum:
            if(checkSum(RxData)!=RxData[RxData[2]+2])break;

         //write to flash:
            readFlash(flashB4,boardInfoSize);

            for(int i=0;i<infoSize;i++){                        //general info
                srcPage[i]=dst[i];
            }

            srcPage[gainControlAdd]=agc;
            srcPage[gainSetAdd]=RxData[9];
            srcPage[gainSetAdd+1]=RxData[10];
            writeFlash(flashB4,&srcPage[0],boardInfoSize);


            uartTx[12]=0x74;
            uartTx[13]=0x03;
            uartTx[5]=0x71;

            processTxLen=14;

            break;
        case 0x72://Gain Control   get
         //read flash:
            readFlash(flashB4+gainControlAdd,3);

            agc=dst[0];
            uartTx[8]=agc;
            uartTx[9]=dst[1];
            uartTx[10]=dst[2];


            uartTx[12]=0x74;
            uartTx[13]=0x03;
            uartTx[5]=0x73;

            processTxLen=14;

            break;
        case 0x74://Gain Control single step
            setGainDb=RxData[9]<<8|RxData[10];

            adcP[6]=RxData[9];
            adcP[7]=RxData[10];
            uartTx[12]=0x74;
            uartTx[13]=0x03;
            uartTx[5]=0x75;

            processTxLen=14;

            break;
    }



}// End of function EmcoreCommands

float ArrayToF32_BigEndian(const uint8_t* const array)
{
    union{
        unsigned char b[4];
        float     f;
    }funion;

    uint8_t n;


    for(n=0; n<4;++n)
    {
        funion.b[3-n]=array[n];

    }
    return funion.f;
}

uint8_t checkSum( uint8_t* package){
    int  len1=package[2];
    unsigned int sum1=0;
    for(int k=3;k<(len1+2);k++)
        sum1+=package[k];

    adcP[4]=(uint8_t)len1;
    adcP[6]=(uint8_t)(sum1&0xff);
    adcP[7]=package[package[2]+2];
    return (uint8_t)(sum1&0xff);

}
