/*
 * alarm.c
 *
 *  Created on: Nov 29, 2016
 *      Version:  A1010
 */

#include "alarm.h"
#include <stdio.h>
#include "hal_data.h"

uint8_t alarmLed=0;
uint8_t counAlarm[8]={0,0,0,0,0,0,0,0};

uint8_t *alarmCheck(uint8_t *checkedAlarm,int *valueAdc){
    int valueAdc1=0;
    alarmLed=0;
    g_ioport.p_api->pinWrite(IOPORT_PORT_04_PIN_07, IOPORT_LEVEL_HIGH);//LED
    g_ioport.p_api->pinWrite(IOPORT_PORT_01_PIN_03, IOPORT_LEVEL_LOW); //alarm output

 //check_alarm_holde_time:
    for(int i=0;i<8;i++){
        if((checkedAlarm[i]==0x01)||(checkedAlarm[i]==0x02)|| (checkedAlarm[i]==0x04)||(checkedAlarm[i]==0x08)){//LA,LW,HA,HW
            if(counAlarm[i]<30){
                counAlarm[i]++;
            }
        }
     }


 //take_off_SBS-MON:
    int valueAdcAl[10];
    for(int i=0;i<4;i++)
        valueAdcAl[i]=valueAdc[i];
    for(int i=4;i<10;i++)
        valueAdcAl[i]=valueAdc[i+1];

  //+5v/+24v:
    valueAdcAl[8]*=10;
    valueAdcAl[9]*=10;

  //check_channlels:
    for(int i=0;i<8;i++){
        if(alarmEna[i]==1){     //channel enable
            valueAdcAl[i]&=0xffff;
            valueAdc1=valueAdcAl[i]<<4;

            if(i!=2){           //regular
                if(valueAdc1<= alarmTable[i][0]){
                    checkedAlarm[i]=0x01;                     //LA
                    g_ioport.p_api->pinWrite(IOPORT_PORT_01_PIN_03, IOPORT_LEVEL_HIGH);//alarm pin
                    alarmLed=1;

                }
                else{
                    if(valueAdc1>= alarmTable[i][4]){
                        checkedAlarm[i]=0x08;                 //HA
                        g_ioport.p_api->pinWrite(IOPORT_PORT_01_PIN_03, IOPORT_LEVEL_HIGH);//alarm pin
                        alarmLed=1;

                    }

                    else{
                        if(valueAdc1<= alarmTable[i][1]){
                            if((checkedAlarm[i]==1)||(checkedAlarm[i]==8)){
                                if(counAlarm[i]>=30){
                                    counAlarm[i]=0;
                                    checkedAlarm[i]=0x02;
                                }
                            }
                            else
                                checkedAlarm[i]=0x02;                 //LW
                        }
                        else{
                            if(valueAdc1>= alarmTable[i][3]){
                                if((checkedAlarm[i]==1)||(checkedAlarm[i]==8)){
                                    if(counAlarm[i]>=30){
                                        counAlarm[i]=0;
                                        checkedAlarm[i]=0x04;
                                    }
                                }
                                else
                                    checkedAlarm[i]=0x04;             //HW

                            }
                            else{
                                if((checkedAlarm[i]==1)||(checkedAlarm[i]==8)||(checkedAlarm[i]==2)||(checkedAlarm[i]==4)){
                                    if(counAlarm[i]>=30){
                                        counAlarm[i]=0;
                                        checkedAlarm[i]=0;
                                    }
                                }
                                else
                                    checkedAlarm[i]=0;            //OK


                            }
                        }
                    }
                }

            }
            else{
                if(valueAdc1>= alarmTable[i][0]){
                    checkedAlarm[i]=0x01;                     //LA
                    g_ioport.p_api->pinWrite(IOPORT_PORT_01_PIN_03, IOPORT_LEVEL_HIGH);//alarm pin
                    alarmLed=1;

                }
                else{
                    if(valueAdc1>= alarmTable[i][1]){
                        if((checkedAlarm[i]==1)||(checkedAlarm[i]==8)){
                            if(counAlarm[i]>=30){
                                counAlarm[i]=0;
                                checkedAlarm[i]=0x02;
                            }
                        }
                        else
                            checkedAlarm[i]=0x02;                 //LW

                    }
                    else{
                        if(valueAdc1<= alarmTable[i][4]){
                            checkedAlarm[i]=0x08;                 //HA
                            g_ioport.p_api->pinWrite(IOPORT_PORT_01_PIN_03, IOPORT_LEVEL_HIGH);//alarm pin
                            alarmLed=1;
                        }
                        else{
                            if(valueAdc1<= alarmTable[i][3]){
                                if((checkedAlarm[i]==1)||(checkedAlarm[i]==8)){
                                    if(counAlarm[i]>=30){
                                        counAlarm[i]=0;
                                        checkedAlarm[i]=0x04;
                                    }
                                }
                                else
                                    checkedAlarm[i]=0x04;             //HW
                            }
                            else{
                                if((checkedAlarm[i]==1)||(checkedAlarm[i]==8)||(checkedAlarm[i]==2)||(checkedAlarm[i]==4)){
                                    if(counAlarm[i]>=30){
                                        counAlarm[i]=0;
                                        checkedAlarm[i]=0;
                                    }
                                }
                                else
                                    checkedAlarm[i]=0;            //OK
                            }
                        }
                    }
                }

            }


        }
        else{
            checkedAlarm[i]=0;    //channel disable
        }


    }

    return checkedAlarm;
}
