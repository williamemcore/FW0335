/*
 * flashHandle.h
 *
 *  Created on: Jun 10, 2016
 *      Author: c0878324
 */

#ifndef FLASHHANDLE_H_
#define FLASHHANDLE_H_



void readFlash(uint32_t addr,int len);
void readFlashPage(uint32_t addr,int len);
uint8_t readFlashDac[46];
void writeFlash(uint32_t addr,uint8_t  *data,int len);

void writeInt(uint32_t addr,int *dataP,int len );
void readWord(uint32_t addr,int len);
void iniFlash(void);
uint8_t srcPage[1024];
uint8_t  dstPage[1024];
uint8_t  dst[121];

uint8_t src[121];


#define proInfoSize       100


#endif /* FLASHHANDLE_H_ */

