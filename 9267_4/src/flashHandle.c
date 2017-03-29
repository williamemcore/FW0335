/*
 * flash_entry.c
 *
 *  Created on: Sep 16, 2016
 *      Author: c0878324
 */
#include "hal_data.h"
#include "bsp_api.h"
#include "flashHandle.h"
#include "UART.h"
uint32_t flashB1= 0x40100000,flashB2= 0x40100400,flashB3= 0x40100800,flashB4= 0x40100c00;

uint8_t srcFreq[3] = "123";
uint8_t dstInt[10];

uint8_t flashRun=0;
char alarm1=0;
void iniFlash(void)
{

    err = g_flash0.p_api->open(g_flash0.p_ctrl, g_flash0.p_cfg);
    if(err != SSP_SUCCESS)
    {
        alarm1=1;
    }
    do
    {
        err = g_flash0.p_api->statusGet(g_flash0.p_ctrl);
    } while (err == SSP_ERR_IN_USE);
}

void writeFlash(uint32_t addr, uint8_t  *data,int len )
{
    flashRun=1;

    err = g_flash0.p_api->open(g_flash0.p_ctrl, g_flash0.p_cfg);
    if(err != SSP_SUCCESS)
    {
        alarm1=1;
    }
    do
    {
        err = g_flash0.p_api->statusGet(g_flash0.p_ctrl);
    } while (err == SSP_ERR_IN_USE);

 //erase:
    err = g_flash0.p_api->erase(g_flash0.p_ctrl, addr, 1);
    if(err != SSP_SUCCESS)
    {
        alarm1=1;
    }
    do
    {
        err = g_flash0.p_api->statusGet(g_flash0.p_ctrl);
    } while (err == SSP_ERR_IN_USE);

    uint32_t dataP=(uint32_t)data;      //convert data

    err = g_flash0.p_api->write(g_flash0.p_ctrl, dataP,addr, (uint32_t)len);
    if(err != SSP_SUCCESS)
    {
        alarm1=1;
    }
    do
    {
        err = g_flash0.p_api->statusGet(g_flash0.p_ctrl);
    } while (err == SSP_ERR_IN_USE);



    err = g_flash0.p_api->close(g_flash0.p_ctrl);
    if(err != SSP_SUCCESS)
    {
        alarm1=1;
    }
    do
    {
        err = g_flash0.p_api->statusGet(g_flash0.p_ctrl);
    } while (err == SSP_ERR_IN_USE);
    flashRun=0;
}
void writeInt(uint32_t addr,int *data,int len )
{
    flashRun=1;

    err = g_flash0.p_api->open(g_flash0.p_ctrl, g_flash0.p_cfg);
    if(err != SSP_SUCCESS)
    {
        alarm1=1;
    }
    do
    {
        err = g_flash0.p_api->statusGet(g_flash0.p_ctrl);
    } while (err == SSP_ERR_IN_USE);

 //erase:
    err = g_flash0.p_api->erase(g_flash0.p_ctrl, addr, 1);
    if(err != SSP_SUCCESS)
    {
        alarm1=1;
    }
    do
    {
        err = g_flash0.p_api->statusGet(g_flash0.p_ctrl);
    } while (err == SSP_ERR_IN_USE);

    uint32_t dataP=(uint32_t)data;
    err = g_flash0.p_api->write(g_flash0.p_ctrl, dataP,addr, (uint32_t)len);

    if(err != SSP_SUCCESS)
    {
        alarm1=1;
    }
    do
    {
        err = g_flash0.p_api->statusGet(g_flash0.p_ctrl);
    } while (err == SSP_ERR_IN_USE);



    err = g_flash0.p_api->close(g_flash0.p_ctrl);
    if(err != SSP_SUCCESS)
    {
        alarm1=1;
    }
    do
    {
        err = g_flash0.p_api->statusGet(g_flash0.p_ctrl);
    } while (err == SSP_ERR_IN_USE);
    flashRun=0;
}


void readFlash(uint32_t addr,int len)
{
    //ssp_err_t err;
    err = g_flash0.p_api->open(g_flash0.p_ctrl, g_flash0.p_cfg);
    if(err != SSP_SUCCESS)
    {
        alarm1=1;
    }
    do
    {
        err = g_flash0.p_api->statusGet(g_flash0.p_ctrl);
    } while (err == SSP_ERR_IN_USE);


    //err = g_flash0.p_api->read(g_flash0.p_ctrl, &readFlashDac[0], addr, len);
    err = g_flash0.p_api->read(g_flash0.p_ctrl, &dst[0], addr, (uint32_t)len);
    if(err != SSP_SUCCESS)
    {
        alarm1=1;
    }
    do
    {
        err = g_flash0.p_api->statusGet(g_flash0.p_ctrl);
    } while (err == SSP_ERR_IN_USE);


    err = g_flash0.p_api->close(g_flash0.p_ctrl);
    if(err != SSP_SUCCESS)
    {
        alarm1=1;
    }
    do
    {
        err = g_flash0.p_api->statusGet(g_flash0.p_ctrl);
    } while (err == SSP_ERR_IN_USE);


}

#if 1

void readFlashPage(uint32_t addr,int len)
{
    //ssp_err_t err;
    err = g_flash0.p_api->open(g_flash0.p_ctrl, g_flash0.p_cfg);
    if(err != SSP_SUCCESS)
    {
        alarm1=1;
    }
    do
    {
        err = g_flash0.p_api->statusGet(g_flash0.p_ctrl);
    } while (err == SSP_ERR_IN_USE);


    //err = g_flash0.p_api->read(g_flash0.p_ctrl, &readFlashDac[0], addr, len);
    err = g_flash0.p_api->read(g_flash0.p_ctrl, &dstPage[0], addr, (uint32_t)len);
    if(err != SSP_SUCCESS)
    {
        alarm1=1;
    }
    do
    {
        err = g_flash0.p_api->statusGet(g_flash0.p_ctrl);
    } while (err == SSP_ERR_IN_USE);


    err = g_flash0.p_api->close(g_flash0.p_ctrl);
    if(err != SSP_SUCCESS)
    {
        alarm1=1;
    }
    do
    {
        err = g_flash0.p_api->statusGet(g_flash0.p_ctrl);
    } while (err == SSP_ERR_IN_USE);


}
#endif


