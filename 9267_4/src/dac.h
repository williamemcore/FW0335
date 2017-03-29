/*///////////////////////////////////////////////////
 *  Project: LEML Mini Tx
 *  File: dac.h
 *
 *  Created on: 17 Oct 2016
 *  Reversion:   V1012
 *///////////////////////////////////////////////////
#include "hal_data.h"

#ifndef DAC_H_
#define DAC_H_
ssp_err_t R_SPI_Init(spi_instance_t * p_spi_instance);


void SPI_write_data (uint8_t channel,int value);
#endif /* DAC_H_ */
void wait(int value);

typedef struct {
    int Attn;
    int MpTiltAdj;
    int PeakAdjMp;
    int Vamp;
    int TpAttn;
    int VSbs;
    int Vxt;

    int Vcso2;
    int Vxmod;
    int ModBias;
    int Vctb;
    int Vctb3;
    int Vcso4;
    int Vclamp;
    int Vcso3;

    int FlsrBias;
    int RlsrBias;
    int LsrTemp;
    int TpTiltAdj;
    int PeakAdjTp;
    int Vadj;
    int DitherCorr;

    int freq;

}dacContr;
int dacValue[25];
dacContr dacContr1;

#define dacMax      10230
#define flsrMax     750
#define rlsrMax     375
