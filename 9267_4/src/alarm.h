/*
 * alarm.h
 *
 *  Created on: Nov 29, 2016
 *      Version:  A1010
 */

#ifndef ALARM_H_
#define ALARM_H_
#include <stdio.h>



#define alarmTableSize    110

int alarmTable[10][5];
uint8_t alarmEna[10];
uint8_t *alarmCheck(uint8_t *valueAlarm,int *valueAdc);


#endif /* ALARM_H_ */
