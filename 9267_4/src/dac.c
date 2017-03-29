/*///////////////////////////////////////////////////
 *  Project: LEML Mini Tx
 *  File: dac.c
 *
 *  Created on: 17 Oct 2016
 *///////////////////////////////////////////////////
//#include "dac_thread.h"
#include "hal_data.h"
#include "dac.h"
#include <string.h> /* memset */
#include <unistd.h> /* close */
#include "sbs_cmds.h"
static spi_cfg_t        _spi_alt_cfg;
static void _SPI_transfer_callback(spi_callback_args_t * p_args);
static spi_instance_t * p_spi;

int dacWait=50;

ioport_level_t level= IOPORT_LEVEL_HIGH;
uint8_t data_dac[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff};

uint16_t dataMax=1023;

ssp_err_t R_SPI_Init(spi_instance_t * p_spi_instance)
{

    ssp_err_t status;

    if(!p_spi_instance)
        return SSP_ERR_INVALID_POINTER;


    p_spi  = p_spi_instance;
    memcpy(&_spi_alt_cfg, p_spi->p_cfg, sizeof(spi_cfg_t));
    _spi_alt_cfg.p_callback = &_SPI_transfer_callback;


    status = p_spi->p_api->open(p_spi->p_ctrl, &_spi_alt_cfg);
    if(!status)
        return SSP_ERR_INVALID_POINTER;
    //tx_thread_sleep(1);




    return SSP_SUCCESS;
}


void SPI_write_data (uint8_t channel,int value1)
{


//chip selection address:
    char channelSet=(char)(channel+1);
    if((channel>7)&&(channel<16))
        channelSet=(char)(channel-7);
    else{
        if(channel>15)
               channelSet=(char)(channel-15);
    }
//4-bit address:
    data_dac[5]=0xc0;
    for(int i=0;i<4;i++){
        if(channelSet&(0x0001<<i)){
            data_dac[5]|=(uint8_t)(0x0001<<(5-i));
        }
    }

 //value:

    if(value1>dataMax)value1=dataMax;


    for(int i=0;i<2;i++){
        if(value1&(0x0001<<i)){
            data_dac[5]|=(uint8_t)(0x0001<<(1-i));
        }
    }

    data_dac[6]=0x00;
    for(int i=0;i<8;i++){
        if((value1>>2)&(0x0001<<i)){
            data_dac[6]|=(uint8_t)(0x0001<<(7-i));
        }
    }

    p_spi->p_api->write(p_spi->p_ctrl, &data_dac, 7, 1);

    wait(80);

 //chip set:
    if(channel<8)
           g_ioport.p_api->pinWrite(IOPORT_PORT_04_PIN_01, IOPORT_LEVEL_HIGH);
    else{
           if((channel>7)&&(channel<16))
               g_ioport.p_api->pinWrite(IOPORT_PORT_03_PIN_01, IOPORT_LEVEL_HIGH);
           else
               g_ioport.p_api->pinWrite(IOPORT_PORT_02_PIN_06, IOPORT_LEVEL_HIGH);

    }

    wait(8);
    if(channel<8)
        g_ioport.p_api->pinWrite(IOPORT_PORT_04_PIN_01, IOPORT_LEVEL_LOW);
    else{
        if((channel>7)&&(channel<16))
            g_ioport.p_api->pinWrite(IOPORT_PORT_03_PIN_01, IOPORT_LEVEL_LOW);
        else
            g_ioport.p_api->pinWrite(IOPORT_PORT_02_PIN_06, IOPORT_LEVEL_LOW);

    }



}

static void _SPI_transfer_callback(spi_callback_args_t * p_args)
{
    spi_callback_args_t a=*p_args;
    *p_args=a;
}
int a=0;
void wait(int value){
    for(uint8_t i=0;i<value;i++){
        uint8_t j=0;
        while(j++<dacWait);
        a=i;

    }
}

void writeSBS(long cmd) {

    uint8_t txBuf[3];

    txBuf[0]=(uint8_t)((unsigned int)(cmd >> 16) & 0xff);
    txBuf[1]=(uint8_t)((unsigned int)(cmd >> 8) & 0xff);
    txBuf[2]=(uint8_t)((unsigned int)cmd & 0xff);

    p_spi->p_api->write(p_spi->p_ctrl, &txBuf, 3, 1);

    wait(80);
    g_ioport.p_api->pinWrite(IOPORT_PORT_03_PIN_02, IOPORT_LEVEL_HIGH);
    wait(8);
    g_ioport.p_api->pinWrite(IOPORT_PORT_03_PIN_02, IOPORT_LEVEL_LOW);


}
