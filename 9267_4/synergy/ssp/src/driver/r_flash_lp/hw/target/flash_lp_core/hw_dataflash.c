/***********************************************************************************************************************
 * Copyright [2015] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 * 
 * This file is part of Renesas SynergyTM Software Package (SSP)
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas SSP license agreement. Unless otherwise agreed in an SSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : hw_dataflash.c
 * Description  : Data Flash Control processing for Low Power Flash
 **********************************************************************************************************************/


/*******************************************************************************************************************//**
 * @ingroup HAL_Library
 * @addtogroup FLASH_LP_API
 * @brief FLASH API
 *
 * This module implements Data Flash control processing for the Low power FLASH peripheral.
 * @{
 **********************************************************************************************************************/

/******************************************************************************
 * Includes   <System Includes> , “Project Includes”
 ******************************************************************************/
#include "bsp_api.h"
#if defined(BSP_MCU_GROUP_S3A7) || defined(BSP_MCU_GROUP_S124) || defined(BSP_MCU_GROUP_S128)
#include "r_flash_lp.h"
#include "..\hw_flash_lp_private.h"
#include "r_flash_cfg.h"
#include "hw_flash_common.h"
#include "hw_dataflash.h"

/******************************************************************************
 * Macro definitions
 ******************************************************************************/

/******************************************************************************
 * Private global variables and functions
 ******************************************************************************/
static current_parameters_t * gp_flash_settings;

static void                HW_FLASH_LP_write_fpmcr (uint8_t value);

/*******************************************************************************************************************//**
 * @brief   Enable Data Flash Access and wait for the Data Flash STOP recovery time.
 * @param[in]  none
 * @retval     none
 **********************************************************************************************************************/
void HW_FLASH_LP_dataflash_enable (void)
{
    /* Data Flash Access enable */
    R_FACI->DFLCTL = 1UL;

    if (1UL == R_FACI->DFLCTL)
    {
        __NOP();
    }

    /** Wait for 5us over (tDSTOP) */
    HW_FLASH_LP_delay_us(WAIT_TDSTOP, gp_flash_settings->system_clock_freq);

}

/*******************************************************************************************************************//**
 * @brief   Transition to Data Flash P/E mode.
 * @param[in]  none
 * @retval     none
 **********************************************************************************************************************/
void HW_FLASH_LP_dataflash_enter_pe_mode (void)
{
	R_FACI->FENTRYR = FENTRYR_DATAFLASH_PE_MODE;
    if (gp_flash_settings->bgo_enabled_df == true)
    {
#if (BSP_IRQ_CFG_FCU_FRDYI != BSP_IRQ_DISABLED)
        NVIC_EnableIRQ(FCU_FRDYI_IRQn);           ///< We are supporting Flash Rdy interrupts for Data Flash operations
#endif
    }

    HW_FLASH_LP_delay_us(WAIT_TDSTOP, gp_flash_settings->system_clock_freq);

    if (R_SYSTEM->OPCCR_b.OPCM == 0)                               ///< High speed mode?
    {
        HW_FLASH_LP_write_fpmcr(DATAFLASH_PE_MODE);
    }
    else
    {
        HW_FLASH_LP_write_fpmcr((uint8_t)DATAFLASH_PE_MODE | (uint8_t)LVPE_MODE);
    }

    R_FACI->FISR_b.PCKA = (uint8_t)((uint8_t)(gp_flash_settings->flash_clock_freq - 1UL) & 0x1FUL);

}

/*******************************************************************************************************************//**
 * @brief   This function switches the peripheral from P/E mode for Data Flash to Read mode.
 * @param[in]     none
 * @param[out]    none
 * @retval SSP_SUCCESS              Successfully entered read mode.
 * @retval SSP_ERR_TIMEOUT    Timed out waiting for confirmation of transition to read mode
 *
 **********************************************************************************************************************/
ssp_err_t HW_FLASH_LP_dataflash_enter_read_mode (void)
{
    ssp_err_t         err = SSP_SUCCESS;

    /* Timeout counter. */
    volatile uint32_t wait_cnt = FLASH_FRDY_CMD_TIMEOUT;

    HW_FLASH_LP_write_fpmcr(READ_MODE);

    /* Wait for 5us over (tMS) */
    HW_FLASH_LP_delay_us(WAIT_TMS_HIGH, gp_flash_settings->system_clock_freq);

    R_FACI->FENTRYR = FENTRYR_READ_MODE;

    while (0x0000 != R_FACI->FENTRYR)
    {
        /* Confirm that the written value can be read correctly. */
        if (wait_cnt <= (uint32_t)0)
        {
            /* return timeout status*/
            return SSP_ERR_TIMEOUT;
        }
        wait_cnt--;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief   Initiates a Write sequence to the Low Power Data Flash data. Address validation has already been
 *          performed by the caller.
 * @param[in] src_start_address   Start address of the (RAM) area which stores the programming data.
 * @param[in] dest_start_address  Flash Start address which will be written.
 * @param[in] num_bytes           Number of bytes to write.
 * @retval none.
 **********************************************************************************************************************/
void HW_FLASH_LP_dataflash_write (uint32_t const src_start_address, uint32_t dest_start_address, uint32_t num_bytes)
{
    gp_flash_settings->total_count   = num_bytes / FLASH_MIN_PGM_SIZE_DF;  /*  Number of bytes to write */
    gp_flash_settings->dest_addr     = dest_start_address;
    gp_flash_settings->src_addr      = src_start_address;
    gp_flash_settings->current_count = (uint32_t)0;

    R_FACI->FASR_b.EXS               = 0;

    // Pass in Read form address
    HW_FLASH_LP_dataflash_write_operation(src_start_address, dest_start_address);
}

/*******************************************************************************************************************//**
 * @brief   Execute a single Write operation on the Low Power Data Flash data.
 * @param[in] psrc_addr    Source address for data to be written.
 * @param[in] dest_addr    End address (read form) for writing.
 * @retval none.
 **********************************************************************************************************************/
void HW_FLASH_LP_dataflash_write_operation (const uint32_t psrc_addr,  uint32_t dest_addr)
{
    uint32_t dest_addr_idx;
    uint8_t * data8_ptr;
    data8_ptr = (uint8_t *)psrc_addr;

    dest_addr_idx = dest_addr + DATAFLASH_ADDR_OFFSET;  /* Conversion to the P/E address from the read address */

    /** Write start address setting */
    R_FACI->FSARH = (uint16_t) ((dest_addr_idx >> 16) & 0xFFFF);
    R_FACI->FSARL = (uint16_t) (dest_addr_idx & 0xFFFF);

    /* Write data setting */
    R_FACI->FWBL0 = *data8_ptr;                    /// For data flash there are only 8 bits used of the 16 in the reg

    /* Execute Write command */
    R_FACI->FCR = FCR_WRITE;
}

/*******************************************************************************************************************//**
 * @brief   Waits for the write command to be completed and verifies the result of the command execution.
 * @param[in] none.
 * @retval SSP_SUCCESS                Write command successfully completed.
 * @retval SSP_ERR_IN_USE         Write command still in progress.
 * @retval SSP_ERR_TIMEOUT      Timed out waiting for write command completion.
 * @retval SSP_ERR_WRITE_FAILED  Write failed. Flash could be locked, area has not been erased,  or address
 *                                    could be under access window control.
 **********************************************************************************************************************/
ssp_err_t HW_FLASH_LP_dataflash_write_monitor (void)
{
    ssp_err_t status;


    /** Check FRDY Flag bit */
    if (1 != R_FACI->FSTATR1_b.FRDY)
    {
        return SSP_ERR_IN_USE;
    }

    /** Clear FCR register */
    R_FACI->FCR = FCR_CLEAR;

    /** Prepare timeout counter */
    gp_flash_settings->wait_cnt = gp_flash_settings->wait_max_write_df;

    while (0 != R_FACI->FSTATR1_b.FRDY)
    {
        /** Check that execute command is completed. */
        /** Wait until FRDY is 1 unless timeout occurs. */
        if (gp_flash_settings->wait_cnt <= (uint32_t)0)
        {
            /* if FRDY is not set to 0 after max timeout, return error*/
            return SSP_ERR_TIMEOUT;
        }
        gp_flash_settings->wait_cnt--;
    }

    if ((0 != R_FACI->FSTATR2_b.ILGLERR) || (0 != R_FACI->FSTATR2_b.PRGERR1))
    {
        HW_FLASH_LP_reset();
        status = SSP_ERR_WRITE_FAILED;
    }
    else
    {
        /** Begin next write operation */
        gp_flash_settings->src_addr  += FLASH_MIN_PGM_SIZE_DF;
        gp_flash_settings->dest_addr += FLASH_MIN_PGM_SIZE_DF;
        gp_flash_settings->total_count--;

        if (gp_flash_settings->total_count)
        {
            HW_FLASH_LP_dataflash_write_operation(gp_flash_settings->src_addr, gp_flash_settings->dest_addr);
            status = SSP_ERR_IN_USE;
        }
        else
        {
            status = SSP_SUCCESS;
        }
    }

    return status;
}

/*******************************************************************************************************************//**
 * @brief   Initiates the Erase sequence to Erase the # of Data Flash blocks specified by num_blocks, starting with the
 *          Block containing 'address'.
 * @param[in] start_addr - The block containing this address is the first block to be erased.
 * @param[in] num_blocks - The # of blocks to be erased.
 * @retval None.
 **********************************************************************************************************************/
void HW_FLASH_LP_dataflash_erase (uint32_t start_addr, uint32_t num_blocks)
{
    /* Set current operation parameters */
    gp_flash_settings->dest_addr     = start_addr;
    gp_flash_settings->total_count   = num_blocks;
    gp_flash_settings->wait_cnt      = gp_flash_settings->wait_max_erase_df_block;
    gp_flash_settings->current_count = 1;       // Include the one we are doing right here

    R_FACI->FASR_b.EXS               = 0;

    // Pass in Read form address
    HW_FLASH_LP_dataflash_erase_operation(gp_flash_settings->dest_addr);
}

/*******************************************************************************************************************//**
 * @brief   Execute a single Erase operation on the Low Power Data Flash data.
 * @param[in] start_addr  Starting Code Flash address to erase.
 * @retval none.
 **********************************************************************************************************************/
void HW_FLASH_LP_dataflash_erase_operation (const uint32_t start_addr)
{
    uint32_t block_start_addr;
    uint32_t block_end_addr;

    block_start_addr = start_addr + (uint32_t)DATAFLASH_ADDR_OFFSET;  /* Conversion to the P/E address from the read address */
    block_end_addr   = (uint32_t)(block_start_addr + ((uint32_t)FLASH_DATA_BLOCK_SZ - 1));

    /* Erase start address setting */
    R_FACI->FSARH = (uint16_t) ((block_start_addr >> 16) & 0xFFFF);
    R_FACI->FSARL = (uint16_t) (block_start_addr & 0xFFFF);

    /* Erase end address setting */
    R_FACI->FEARH = (uint16_t) ((block_end_addr >> 16) & 0xFFFF);
    R_FACI->FEARL = (uint16_t) (block_end_addr & 0xFFFF);

    /* Execute Erase command */
    R_FACI->FCR = FCR_ERASE;
}

/*******************************************************************************************************************//**
 * @brief   Waits for the erase command to be completed and verifies the result of the command execution.
 * @param[in] none.
 * @retval SSP_SUCCESS            Erase command successfully completed.
 * @retval SSP_ERR_IN_USE         Erase command still in progress.
 * @retval SSP_ERR_TIMEOUT        Timed out waiting for erase command completion.
 * @retval SSP_ERR_WRITE_FAILED  Erase failed. Flash could be locked or address could be under access window
 * control.
 **********************************************************************************************************************/
ssp_err_t HW_FLASH_LP_dataflash_erase_monitor (void)
{
    ssp_err_t status;

    /* Check FRDY Flag bit*/
    if (1 != R_FACI->FSTATR1_b.FRDY)
    {
        return SSP_ERR_IN_USE;
    }

    /* Clear FCR register */
    R_FACI->FCR = FCR_CLEAR;

    /** Worst case timeout */
    gp_flash_settings->wait_cnt = gp_flash_settings->wait_max_erase_df_block;


    while (0 != R_FACI->FSTATR1_b.FRDY)
    {
        /* Check that execute command is completed. */
        /* Wait until FRDY is 1 unless timeout occurs. */
        if (gp_flash_settings->wait_cnt <= (uint32_t)0)
        {
            /* if FRDY is not set to 0 after max timeout, return error*/
            return SSP_ERR_TIMEOUT;
        }
        gp_flash_settings->wait_cnt--;
    }

    if ((0 != R_FACI->FSTATR2_b.ILGLERR) || (0 != R_FACI->FSTATR2_b.ERERR))
    {
        HW_FLASH_LP_reset();
        status = SSP_ERR_WRITE_FAILED;
    }
    else
    {
        /* Next Block */
        gp_flash_settings->dest_addr += FLASH_DATA_BLOCK_SZ;

        if (gp_flash_settings->current_count < gp_flash_settings->total_count)
        {
            HW_FLASH_LP_dataflash_erase_operation(gp_flash_settings->dest_addr);
            status = SSP_ERR_IN_USE;
        }
        else
        {
            status = SSP_SUCCESS;
        }
        gp_flash_settings->current_count++;
    }

    return status;
}

/*******************************************************************************************************************//**
 * @brief   Initiates a Blank check sequence to the Low Power Data Flash data. Address validation has already been
 *          performed by the caller.
 * @param[in] start_addr   Start address of the Data Flash area to blank check.
 * @param[in] end_addr     End address of the Data flash area to blank check This address is included in the blank
 * check.
 * @retval none.
 **********************************************************************************************************************/
void HW_FLASH_LP_dataflash_blank_check (uint32_t start_addr, uint32_t end_addr)
{
    uint32_t start_addr_idx;
    uint32_t end_addr_idx;

    start_addr_idx     = start_addr + DATAFLASH_ADDR_OFFSET; /* Conversion to the P/E address from the read address */
    end_addr_idx       = end_addr + DATAFLASH_ADDR_OFFSET;   /* Conversion to the P/E address from the read address */

    R_FACI->FASR_b.EXS = 0;

    /** BlankCheck start address setting */
    R_FACI->FSARH = (uint16_t) ((start_addr_idx >> 16) & 0xFFFF);
    R_FACI->FSARL = (uint16_t) (start_addr_idx & 0xFFFF);

    /** BlankCheck end address setting */
    R_FACI->FEARH = (uint16_t) ((end_addr_idx >> 16) & 0xFFFF);
    R_FACI->FEARL = (uint16_t) (end_addr_idx & 0xFFFF);

    /** Execute BlankCheck command */
    R_FACI->FCR = FCR_BLANKCHECK;
}

/*******************************************************************************************************************//**
 * @brief   Waits for the blank check command to be completed and verifies the result of the command execution.
 * @param[in] none.
 * @retval SSP_SUCCESS            Blank check command successfully completed.
 * @retval SSP_ERR_IN_USE         Blank check command still in progress.
 * @retval SSP_ERR_TIMEOUT        Timed out waiting for Blank check command completion.
 **********************************************************************************************************************/
ssp_err_t HW_FLASH_LP_dataflash_blank_check_monitor (void)
{
    ssp_err_t err = SSP_SUCCESS;

    /* Check FRDY Flag bit*/
    if (1 != R_FACI->FSTATR1_b.FRDY)
    {
        return SSP_ERR_IN_USE;
    }

    /* Clear FCR register */
    R_FACI->FCR = FCR_CLEAR;

    /** Worst case timeout */
    gp_flash_settings->wait_cnt = gp_flash_settings->wait_max_blank_check;

    while (0 != R_FACI->FSTATR1_b.FRDY)
    {
        /* Check that execute command is completed. */
        /* Wait until FRDY is 1 unless timeout occurs. */
        if (gp_flash_settings->wait_cnt <= (uint32_t)0)
        {
            /* if FRDY is not set to 0 after max timeout, return error*/
            return SSP_ERR_TIMEOUT;
        }
        gp_flash_settings->wait_cnt--;
    }

    if (0 != R_FACI->FSTATR2_b.ILGLERR)
    {
        HW_FLASH_LP_reset();
        err = SSP_ERR_WRITE_FAILED;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief   Sets the FPMCR register, used to place the Flash sequencer in Data Flash P/E mode.
 * @param[in] value - 8 bit value to be written to the FPMCR register.
 * @retval none.
 **********************************************************************************************************************/
static void HW_FLASH_LP_write_fpmcr (uint8_t value)
{
    R_FACI->FPR   = 0xA5;

    R_FACI->FPMCR = value;
    R_FACI->FPMCR = (uint8_t) ~value;
    R_FACI->FPMCR = value;

    if (value == R_FACI->FPMCR)
    {
        __NOP();
    }
}

/*******************************************************************************
 * Outline      : Give the Data Flash HW layer access to the flash settings
 * Header       : none
 * Function Name: set_flash_settings
 * Description  : Give the Data Flash HW layer access to the flash settings
 * Arguments    : current_parameters_t *p_current_parameters - Pointer the settings.
 *             :
 * Return Value : none
 *******************************************************************************/
void HW_FLASH_LP_data_flash_set_flash_settings (current_parameters_t * p_current_parameters)
{
    gp_flash_settings = p_current_parameters;
}

#endif /* if defined(BSP_MCU_GROUP_S3A7) || defined(BSP_MCU_GROUP_S124) || defined(BSP_MCU_GROUP_S128) */

/*******************************************************************************************************************//**
 * @} (end defgroup FLASH)
 **********************************************************************************************************************/
