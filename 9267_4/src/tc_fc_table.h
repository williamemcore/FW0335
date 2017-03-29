/*
 * tc_fc_table.h
 *
 *  Created on: Nov 28, 2016
 *      Version:  A1010
 */
#include <stdio.h>
#ifndef TC_FC_TABLE_H_
#define TC_FC_TABLE_H_



void iniTcTable();

//DAC channel:
#define attn            0
#define mp_tilt_adj     1
#define peak_adj_mp     2
#define vamp            3
#define tp_attn         4
#define vsbs            5
#define vxt             6


#define vcso2           8
#define vxmod           9
#define modbias         10
#define vctb            11
#define vctb3           12
#define vcso4           13
#define vclamp          14
#define vcso3           15

#define flsr_bias       16
#define rlasr_bias      17
#define lsr_temp        18
#define tp_tilt         19
#define peak_adj_tp     20
#define vadj            21
#define dither          22
#define ditherTune       23

#define tcTabSize       432
//#define tcEnaSize       24
#define fcTabSize       432
#define fcStart         456
#define tcFcSize        914
#define fcPointMax      160
#define fcPointStep     20
void checkSetTc(int channel);
void checkSetFc(int channel);
void checkTcFc(int channel);



int tcTab[24][9], fcTab[24][9],fibLen;

int tcCurValue[24],fcCurValue[24];
void getTcValue(int channel,int point);
void getFcValue(int channel,int point);
uint8_t tcEnable[24],fcEnable[24];
int  fcPoint[9];
void readTCFC();

#endif /* TC_FC_TABLE_H_ */

