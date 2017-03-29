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
 * File Name    : hw_codeflash.c
 * Description  : Code Flash Control processing for Low Power Flash
 **********************************************************************************************************************/


/*******************************************************************************************************************//**
 * @ingroup HAL_Library
 * @addtogroup FLASH_LP_API
 * @brief FLASH API
 *
 * This module implements Code Flash control processing for the Low power FLASH peripheral.
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
#include "hw_codeflash.h"

/******************************************************************************
 * Private global variables and functions
 ******************************************************************************/

static r_codeflash_data_t g_code_flash_info;
static void               HW_FLASH_LP_codeflash_write_fpmcr (uint8_t value) PLACE_IN_RAM_SECTION;
static current_parameters_t * gp_flash_settings;


/*******************************************************************************************************************//**
 * @brief   Transition to Code Flash P/E mode.
 * @param[in]  none
 * @retval     none
 **********************************************************************************************************************/
void HW_FLASH_LP_codeflash_enter_pe_mode (void)
{
#if (BSP_IRQ_CFG_FCU_FRDYI != BSP_IRQ_DISABLED)
    NVIC_DisableIRQ(FCU_FRDYI_IRQn);           ///< We are not supporting Flash Rdy interrupts for Code Flash operations
#endif
    R_FACI->FENTRYR = FENTRYR_CODEFLASH_PE_MODE;

    HW_FLASH_LP_codeflash_write_fpmcr(DISCHARGE_1);

    /** Wait for 2us over (tDIS) */
    HW_FLASH_LP_delay_us(WAIT_TDIS, gp_flash_settings->system_clock_freq);

    if (R_SYSTEM->OPCCR_b.OPCM == 0)        ///< High speed mode?
    {
        HW_FLASH_LP_codeflash_write_fpmcr(DISCHARGE_2);
        HW_FLASH_LP_codeflash_write_fpmcr(CODEFLASH_PE_MODE);

        /** Wait for 5us over (tMS) */
        HW_FLASH_LP_delay_us(WAIT_TMS_HIGH, gp_flash_settings->system_clock_freq);
    }
    else
    {
        HW_FLASH_LP_codeflash_write_fpmcr(DISCHARGE_2 | LVPE_MODE);
        HW_FLASH_LP_codeflash_write_fpmcr(CODEFLASH_PE_MODE | LVPE_MODE);

        /** Wait for 3us over (tMS) */
        HW_FLASH_LP_delay_us(WAIT_TMS_MID, gp_flash_settings->system_clock_freq);

    }

    R_FACI->FISR_b.PCKA = (uint32_t)((gp_flash_settings->flash_clock_freq - 1UL) & (uint32_t)0x1F);
}

/*******************************************************************************************************************//**
 * @brief   This function switches the peripheral from P/E mode for Code Flash to Read mode.
 * @param[in]     none
 * @param[out]    none
 * @retval SSP_SUCCESS              Successfully entered read mode.
 * @retval SSP_ERR_TIMEOUT    Timed out waiting for confirmation of transition to read mode
 *
 **********************************************************************************************************************/
ssp_err_t HW_FLASH_LP_codeflash_enter_read_mode (void)
{
    ssp_err_t         err = SSP_SUCCESS;

    /** Timeout counter. */
    volatile uint32_t wait_cnt = FLASH_FRDY_CMD_TIMEOUT;

    HW_FLASH_LP_codeflash_write_fpmcr(DISCHARGE_2);

    /** Wait for 2us over (tDIS) */
    HW_FLASH_LP_delay_us(WAIT_TDIS, gp_flash_settings->system_clock_freq);

    HW_FLASH_LP_codeflash_write_fpmcr(DISCHARGE_1);
    HW_FLASH_LP_codeflash_write_fpmcr(READ_MODE);

    /** Wait for 5us over (tMS) */
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
 * @brief   Initiates a Write sequence to the Low Power Code Flash data. Address validation has already been
 *          performed by the caller.
 * @param[in] src_start_address   Start address of the (RAM) area which stores the programming data.
 * @param[in] dest_start_address  Flash Start address which will be written.
 * @param[in] num_bytes           Number of bytes to write.
 * @retval none.
 **********************************************************************************************************************/
void HW_FLASH_LP_codeflash_write (uint32_t const src_start_address, uint32_t dest_start_address, uint32_t num_bytes)
{
    g_code_flash_info.start_addr = src_start_address;                ///< Ram Source for data to write
    g_code_flash_info.end_addr   = dest_start_address;               ///< Flash Start address which will be written
    g_code_flash_info.write_cnt  = num_bytes / FLASH_MIN_PGM_SIZE_CF; ///<  Number of units to write

    /** Select User Area */
    R_FACI->FASR_b.EXS = 0;

    /** Pass in Read form address */
    HW_FLASH_LP_codeflash_write_operation(src_start_address,  dest_start_address);
}

/*******************************************************************************************************************//**
 * @brief   Execute a single Write operation on the Low Power Code Flash data.
 * @param[in] psrc_addr    Source address for data to be written.
 * @param[in] dest_addr    End address (read form) for writing.
 * @retval none.
 **********************************************************************************************************************/
void HW_FLASH_LP_codeflash_write_operation (const uint32_t psrc_addr, const uint32_t dest_addr)
{
    uint8_t * data8_ptr;
    data8_ptr = (uint8_t *)psrc_addr;
    uint16_t load_value;
    uint8_t byteL;
    uint8_t byteH;

    /** Write start address setting */
    R_FACI->FSARH = (uint16_t) ((dest_addr >> 16) & 0xFFFF);
    R_FACI->FSARL = (uint16_t) (dest_addr & 0xFFFF);

    /** Write data setting. Note that the caller could be providing a data buffer that was defined as a char[] buffer. As
     * a result it might not be 16 bit aligned. For the CM0 data accesses that are not aligned will generate
     * a fault. Therefore we will read the data 8 bits at a time.*/
    byteL = *data8_ptr;
    data8_ptr++;
    byteH = *data8_ptr;
    load_value = (uint16_t)((uint16_t)(byteH << 8) | (uint16_t)byteL);
    data8_ptr++;

    R_FACI->FWBL0 = load_value;       // Move to bits 31 - 0 for next write.

    byteL = *data8_ptr;
    data8_ptr++;
    byteH = *data8_ptr;
    load_value = (uint16_t)((uint16_t)(byteH << 8) | (uint16_t)byteL);
    data8_ptr++;
    R_FACI->FWBH0 = load_value;

#if defined(BSP_MCU_GROUP_S3A7)                      /// NPP7
    byteL = *data8_ptr;
    data8_ptr++;
    byteH = *data8_ptr;
    load_value = (uint16_t)((uint16_t)(byteH << 8) | (uint16_t)byteL);
    data8_ptr++;
    R_FACI->FWBL1 = load_value;

    byteL = *data8_ptr;
    data8_ptr++;
    byteH = *data8_ptr;
    load_value = (uint16_t)((uint16_t)(byteH << 8) | (uint16_t)byteL);
    data8_ptr++;
   R_FACI->FWBH1 = load_value;
#endif

    /** Execute Write command */
    R_FACI->FCR = FCR_WRITE;
}

/*******************************************************************************************************************//**
 * @brief   Waits for the write command to be completed and verifies the result of the command execution.
 * @param[in] none.
 * @retval SSP_SUCCESS                Write command successfully completed.
 * @retval SSP_ERR_IN_USE         Write command still in progress.
 * @retval SSP_ERR_TIMEOUT      Timed out waiting for write command completion.
 * @retval SSP_ERR_WRITE_FAILED  Write failed. Flash could be locked, area has not been erased or
 *                                    address could be under access window control.
 **********************************************************************************************************************/
ssp_err_t HW_FLASH_LP_codeflash_write_monitor (void)
{
    ssp_err_t status;

    /** Worst case timeout */
    gp_flash_settings->wait_cnt = gp_flash_settings->wait_max_write_cf;

    /** Check FRDY Flag bit*/
    if (1 != R_FACI->FSTATR1_b.FRDY)
    {
        if (gp_flash_settings->wait_cnt <= (uint32_t)0)
        {
            /* if FRDY is not set to 0 after max timeout, return error*/
            return SSP_ERR_TIMEOUT;
        }
        gp_flash_settings->wait_cnt--;
        return SSP_ERR_IN_USE;
    }

    /** Clear FCR register */
    R_FACI->FCR = FCR_CLEAR;

    /** Worst case timeout */
    gp_flash_settings->wait_cnt = gp_flash_settings->wait_max_write_cf;

    while (0 != R_FACI->FSTATR1_b.FRDY)
    {
        /** Check that execute command is completed. */
        /** Wait until FRDY is 1 unless timeout occurs. */
        if (gp_flash_settings->wait_cnt <= (uint32_t)0)
        {
            /** if FRDY is not set to 0 after max timeout, return error*/
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
        g_code_flash_info.start_addr += FLASH_MIN_PGM_SIZE_CF;
        g_code_flash_info.end_addr   += FLASH_MIN_PGM_SIZE_CF;
        g_code_flash_info.write_cnt--;

        if (g_code_flash_info.write_cnt)
        {
            HW_FLASH_LP_codeflash_write_operation(g_code_flash_info.start_addr, g_code_flash_info.end_addr);
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
 * @brief   Initiates the Erase sequence to Erase the # of Code Flash blocks specified by num_blocks, starting with the
 *          Block containing 'address'.
 * @param[in] start_addr - The block containing this address is the first block to be erased.
 * @param[in] num_blocks - The # of blocks to be erased.
 * @retval None.
 **********************************************************************************************************************/
void HW_FLASH_LP_codeflash_erase (const uint32_t start_addr, const uint32_t num_blocks)
{
    g_code_flash_info.start_addr = start_addr;
    g_code_flash_info.end_addr   = (uint32_t)(start_addr + ((num_blocks * FLASH_CODE_LARGE_BLOCK_SZ) - 1));

    /* Select User Area */
    R_FACI->FASR_b.EXS = 0;

    // Pass in Read form address
    HW_FLASH_LP_codeflash_erase_operation(start_addr);
}

/*******************************************************************************************************************//**
 * @brief   Execute a single Erase operation on the Low Power Code Flash data.
 * @param[in] start_addr  Starting Code Flash address to erase.
 * @retval none.
 **********************************************************************************************************************/
void HW_FLASH_LP_codeflash_erase_operation (const uint32_t start_addr)
{
    uint32_t block_start_addr;
    uint32_t block_end_addr;

    block_start_addr = start_addr;
    block_end_addr   = (uint32_t)(block_start_addr + (FLASH_CODE_LARGE_BLOCK_SZ - 1));

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
 * @retval SSP_SUCCESS                Erase command successfully completed.
 * @retval SSP_ERR_IN_USE         Erase command still in progress.
 * @retval SSP_ERR_TIMEOUT      Timed out waiting for erase command completion.
 * @retval SSP_ERR_WRITE_FAILED  Erase failed. Flash could be locked or address could be under access window
 * control.
 **********************************************************************************************************************/
ssp_err_t HW_FLASH_LP_codeflash_erase_monitor (void)
{
    ssp_err_t status;

    /** Check FRDY Flag bit*/
    if (1 != R_FACI->FSTATR1_b.FRDY)
    {
        return SSP_ERR_IN_USE;
    }

    /** Clear FCR register */
    R_FACI->FCR = FCR_CLEAR;

    /** Prepare worst case timeout */
    gp_flash_settings->wait_cnt = gp_flash_settings->wait_max_erase_cf_large_block;

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

    if ((0 != R_FACI->FSTATR2_b.ILGLERR) || (0 != R_FACI->FSTATR2_b.ERERR))
    {
        HW_FLASH_LP_reset();
        return SSP_ERR_WRITE_FAILED;
    }
    else
    {
        /** Next Block */
        g_code_flash_info.start_addr += (uint32_t)FLASH_CODE_LARGE_BLOCK_SZ;

        if ((g_code_flash_info.start_addr < g_code_flash_info.end_addr) && (g_code_flash_info.start_addr != (uint32_t)0))       //
                                                                                                                      //
                                                                                                                      // Check
                                                                                                                      //
                                                                                                                      // for
                                                                                                                      //
                                                                                                                      // CF0
                                                                                                                      //
                                                                                                                      // overflow
        {
            HW_FLASH_LP_codeflash_erase_operation(g_code_flash_info.start_addr);
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
 * @brief   Initiates a Blank check sequence to the Low Power Code Flash data. Address validation has already been
 *          performed by the caller.
 * @param[in] start_addr   Start address of the Code Flash area to blank check.
 * @param[in] end_addr     End address of the code flash area to blank check This address is included in the blank
 * check.
 * @retval none.
 **********************************************************************************************************************/
void HW_FLASH_LP_codeflash_blank_check (const uint32_t start_addr, const uint32_t end_addr)
{
    uint32_t start_addr_idx;
    uint32_t end_addr_idx;

    start_addr_idx = start_addr;
    end_addr_idx   = end_addr;

    /** Select User Area */
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
 * @retval SSP_SUCCESS                Blank check command successfully completed.
 * @retval SSP_ERR_IN_USE         Blank check command still in progress.
 * @retval SSP_ERR_TIMEOUT      Timed out waiting for Blank check command completion.
 **********************************************************************************************************************/
ssp_err_t HW_FLASH_LP_codeflash_blank_check_monitor (void)
{
    /** Check FRDY Flag bit*/
    if (1 != R_FACI->FSTATR1_b.FRDY)
    {
        return SSP_ERR_IN_USE;
    }

    /** Clear FCR register */
    R_FACI->FCR = FCR_CLEAR;

    /** Prepare worst case timeout */
    gp_flash_settings->wait_cnt = gp_flash_settings->wait_max_blank_check;


    while (0 != R_FACI->FSTATR1_b.FRDY)
    {
        /** Check that execute command is completed. */
        /** Wait until FRDY is 1 unless timeout occurs. */
        if (gp_flash_settings->wait_cnt <= (uint32_t)0)
        {
            /** if FRDY is not set to 0 after max timeout, return error*/
            return SSP_ERR_TIMEOUT;
        }
        gp_flash_settings->wait_cnt--;
    }

    /** Caller will check the FSTATR00 register to determine Blank or not Blank. */
    return SSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief   Sets the FPMCR register, used to place the Flash sequencer in Code Flash P/E mode.
 * @param[in] value - 8 bit value to be written to the FPMCR register.
 * @retval none.
 **********************************************************************************************************************/
static void HW_FLASH_LP_codeflash_write_fpmcr (uint8_t value)
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
 * Outline      : Give the Code Flash HW layer access to the flash settings
 * Header       : none
 * Function Name: set_flash_settings
 * Description  : Give the HW layer access to the flash settings
 * Arguments    : current_parameters_t *p_current_parameters - Pointer the settings.
 *             :
 * Return Value : none
 *******************************************************************************/
void HW_FLASH_LP_code_flash_set_flash_settings (current_parameters_t * p_current_parameters)
{
    gp_flash_settings = p_current_parameters;
}

#endif /* if defined(BSP_MCU_GROUP_S3A7) || defined(BSP_MCU_GROUP_S124) || defined(BSP_MCU_GROUP_S128) */

/*******************************************************************************************************************//**
 * @} (end defgroup FLASH)
 **********************************************************************************************************************/
