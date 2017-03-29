/*/////////////////////////////////////////////////////////////////////////////
 * Project:     LEML Mini TX
 * Company:     Emcore
 * Date:        12/14/2016
 * Reversion:
 *////////////////////////////////////////////////////////////////////////////
#include "hal_data.h"
#include "GLOBAL.h"
#include "UART.h"
#include "PROTOCOL.h"
#include "unistd.h"  //sleep

#include "dac.h"
#include "tx_api.h"
#include "flashHandle.h"
#include "tc_fc_table.h"
#include "alarm.h"
#include "boardInfo.h"
#include "adc.h"
#include "rfGain.h"
int dacData[61];

extern uint32_t flashB1,flashB2,flashB3,flashB4;
extern void initSBSSPIport(unsigned int sbsFreqMhz);
extern void wait(int value);
//char led400=0;
extern char alarm1;
//uint8_t lasIType=0;
//extern char counTimer40,counTimer400, counTimer5s;
extern uint8_t agc;
//=========================================================
//do not remove the firmware_version records:
//uint8_t firmV[5]={'A','1','0','1','5'};   //1/14/2017
//uint8_t firmV[5]={'A','1','0','1','6'};   //1/17/2017
uint8_t firmV[5]={'A','1','0','1','7'};   //
//=========================================================
void hal_entry(void)
{

  //SPI_initial:
    R_SPI_Init((spi_instance_t *) &g_spi9);
    g_ioport.p_api->pinWrite(IOPORT_PORT_04_PIN_01, IOPORT_LEVEL_LOW);
    g_ioport.p_api->pinWrite(IOPORT_PORT_03_PIN_01, IOPORT_LEVEL_LOW);
    g_ioport.p_api->pinWrite(IOPORT_PORT_02_PIN_06, IOPORT_LEVEL_LOW);


 //set_voltage_adjust:
    SPI_write_data(21,256);


 //TEC_DIS:
    g_ioport.p_api->pinWrite(IOPORT_PORT_04_PIN_00, IOPORT_LEVEL_HIGH);

 //UART:
    g_uart0.p_api->open(g_uart0.p_ctrl, g_uart0.p_cfg);
    g_ioport.p_api->init(g_ioport.p_cfg);

 //timer:
    g_timer0.p_api->open(g_timer0.p_ctrl, g_timer0.p_cfg);
    if (g_timer0.p_api->start(g_timer0.p_ctrl)) {
        alarm1=2;
    }



 //first_set to protect laser:
    SPI_write_data(16,0);
    SPI_write_data(17,0);
    SPI_write_data(18,0);



//read DAC from flash:
    readFlash(flashB1,50);
    if((dst[0]==0xff)||(dst[2]==0xff)||(dst[4]==0xff)){

//initial_data_flash:
        src[0]=(uint8_t)(2500>>8);
        src[1]=(uint8_t)(2500&0xff);
        src[2]=(uint8_t)(5120>>8);
        src[3]=(uint8_t)(5120&0xff);

        src[4]=(uint8_t)(375>>8);
        src[5]=(uint8_t)(375&0xff);
        src[6]=(uint8_t)(5000>>8);
        src[7]=(uint8_t)(5000&0xff);


        src[8]=(uint8_t)(2000>>8);
        src[9]=(uint8_t)(2000&0xff);
        src[10]=(uint8_t)(2125>>8);
        src[11]=(uint8_t)(2125&0xff);

        src[12]=(uint8_t)(1750>>8);
        src[13]=(uint8_t)(1750&0xff);
        src[14]=0;  //res1
        src[15]=0;

    //IC2:
        src[16]=(uint8_t)(5000>>8);
        src[17]=(uint8_t)(5000&0xff);
        src[18]=(uint8_t)(3750>>8);
        src[19]=(uint8_t)(3750&0xff);

        src[20]=(uint8_t)(1450>>8);
        src[21]=(uint8_t)(1450&0xff);
        src[22]=(uint8_t)(5000>>8);
        src[23]=(uint8_t)(5000&0xff);

        src[24]=(uint8_t)(5750>>8);
        src[25]=(uint8_t)(5750&0xff);
        src[26]=(uint8_t)(5000>>8);
        src[27]=(uint8_t)(5000&0xff);

        src[28]=(uint8_t)(4500>>8);
        src[29]=(uint8_t)(4500&0xff);
        src[30]=(uint8_t)(5000>>8);
        src[31]=(uint8_t)(5000&0xff);
     //IC3:
        src[32]=(uint8_t)(1000>>8);
        src[33]=(uint8_t)(1000&0xff);
        src[34]=(uint8_t)(1000>>8);
        src[35]=(uint8_t)(1000&0xff);

        src[36]=(uint8_t)(4250>>8);
        src[37]=(uint8_t)(4250&0xff);
        src[38]=(uint8_t)(2500>>8);
        src[39]=(uint8_t)(2500&0xff);

        src[40]=(uint8_t)(1875>>8);
        src[41]=(uint8_t)(1875&0xff);
        src[42]=(uint8_t)(5000>>8);
        src[43]=(uint8_t)(5000&0xff);

        src[44]=0;//dither=core
        src[45]=0;
        src[46]=(uint8_t)(3000>>8);
        src[47]=(uint8_t)(3000&0xff);

        src[48]=(uint8_t)(2557>>8);
        src[49]=(uint8_t)(2557&0xff);

        writeFlash(flashB1,&src[0],50);

 //initial_TC-FC:
      //initial array:
        for(int i=0;i<24;i++){
             for(int j=0;j<9;j++){
                 tcTab[i][j]=0;
                 fcTab[i][j]=0;
             }
             tcCurValue[i]=0;
             fcCurValue[i]=0;
             tcEnable[i]=0;
             fcEnable[i]=0;
        }
     //write_flash:
        uint8_t tcFcTabC[tcFcSize];
        for(int i=0;i<tcFcSize-1;i++)
            tcFcTabC[i]=0;

        writeFlash(flashB2,&tcFcTabC[0],tcFcSize);

 //laser_calberation:
        lasPCaliGui[0]=0;
        lasPCaliGui[1]=0;
        writeFlash(flashB4+lasPCaliAdd,&lasPCaliGui[0],2);

    }

    int dataFlash[26];
    for(int i=0;i<24;i++){  //get DAC data from flash
        dataFlash[i]=((dst[i*2]<<8)|dst[i*2+1]);
        if(dataFlash[i]>dacMax)dataFlash[i]=0;    //first_power_after_program

        dacValue[i]=dataFlash[i];
    }

    dataFlash[24]=(dst[48]<<8)|dst[49];     //frequency
    dacValue[24]=dataFlash[24];


 //ADC:
    g_adc0.p_api->open(g_adc0.p_ctrl, g_adc0.p_cfg);
    g_adc0.p_api->scanCfg(g_adc0.p_ctrl, g_adc0.p_channel_cfg);





//SBS (2557 MHZ):
        if((dacValue[24]<2400)||(dacValue[24]>2725)){
            initSBSSPIport(2557);       //default

        }
        else{
            initSBSSPIport((unsigned int)dacValue[24]);

        }


//Read TC-FC table to array:
        readFlashPage(flashB2,tcFcSize);//page2

        for(int i=0;i<24;i++)       //TC table
             for(int j=0;j<9;j++){
                 tcTab[i][j]=(int)(dstPage[i*18+j*2]<<8|dstPage[i*18+j*2+1]);
             }


        for(int i=0;i<24;i++){      //TC enable
            tcEnable[i]=dstPage[tcTabSize+i];
        }

        for(int i=0;i<24;i++)       //fc table
             for(int j=0;j<9;j++){
                 fcTab[i][j]=(int)(dstPage[fcStart+i*18+j*2]<<8|dstPage[fcStart+i*18+j*2+1]);
             }


        for(int i=0;i<24;i++){      //fc enable
            fcEnable[i]=dstPage[fcStart+fcTabSize+i];
        }

        for(int j=0;j<9;j++)                //fc points
            fcPoint[j]=j*fcPointStep;       //km

        fibLen=(int)((dstPage[tcFcSize-2]<<8)|(dstPage[tcFcSize-1]&0xff));

   //write_SPI(DAC):
        int dacSend=0;
        for(uint8_t k=0;k<24;k++){
            if((k!=16)&&(k!=17)&&(k!=18)&&(k!=21)){
                //TC:
                 if(tcEnable[k]!=1)      //no TC
                     dacSend=dataFlash[k]/10;
                 else{                   //with TC, FC
                     dacSend=(dacValue[k]+tcCurValue[k])/10;

                  }

                 //FC:
                 if(fcEnable[k]==1){      //with FC
                     if(dacSend+fcCurValue[k])//negtive
                         dacSend=0;
                     else
                         dacSend+=fcCurValue[k]/10;

                 }

                 SPI_write_data(k,dacSend);  //with TC, FC

                 tx_thread_sleep(4);

              }


        }


//Alarm:
        readFlash(flashB3,alarmTableSize);//read data from flash
        for(int i=0;i<10;i++){
              for(int j=0;j<5;j++)
                   alarmTable[i][j]=(int)(dst[i*11+j*2]<<8)|dst[i*11+j*2+1];   //alarm value

              alarmEna[i]=dst[i*11+10];                                   //alarm enable
        }

        g_ioport.p_api->pinWrite(IOPORT_PORT_01_PIN_03, IOPORT_LEVEL_LOW);//alarm pin

//laser_power_calibrate:
        readFlash(flashB4+lasPCaliAdd,2);//read data from flash
        lasPCaliGui[0]=dst[0];
        lasPCaliGui[1]=dst[1];


//RF gain control:
        readFlash(flashB4+gainControlAdd,3);  //read flash
        if(dst[0]==1)
            agc=1;
        else
            agc=0;
        setGainDb=(dst[1]<<8)|dst[2];

        while(1){
          //40 ms:
           if(counTimer==1){

               counTimer=0;
               counTimer40++;

               adcRead();                          


             //400 ms:
               if(counTimer40>10){
                   counTimer40=0;
                   counTimer5s++;
                   adcSendUart();                   //ADC_send
                   if(agc==1)
                       if(abs(adcSend[5]-599)>1)
                           gainAgc();

                 //4.8s:
                   if(counTimer5s>=12){
                       counTimer5s=0;
                       if(counLaserIni>=300)
                           readTCFC();                  //read_TCFC

                   }
               }
           }
           //tx_thread_sleep(1);
           wait(1);
       }

	}


}

