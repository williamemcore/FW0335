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
 * File Name    : hw_codeflash_extra.c
 * Description  : Flash Control Access window and swap control processing for Low Power Flash
 **********************************************************************************************************************/


/*******************************************************************************************************************//**
 * @ingroup HAL_Library
 * @addtogroup FLASH_LP_API
 * @brief FLASH API
 *
 * This module implements Access window and swap control for the Low power FLASH peripheral.
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
#include "hw_codeflash.h"
#include "hw_codeflash_extra.h"

/******************************************************************************
 * Private global variables and functions
 ******************************************************************************/

/*******************************************************************************************************************//**
 * @brief Temporarily sets the startup area to use the DEFAULT or ALTERNATE area as requested.
 * On the next subsequent reset, the startup area will be determined by the state of the BTFLG.
 * This command does NOT modify the configuration via The Configuration Set command, hence these changes are
 * only in effect until the next reset.
 * @param[in]  swap_type - specifies the startup area swap being requested.
 *
 * @retval SSP_SUCCESS                StartUp area temporarily modified.
 **********************************************************************************************************************/
ssp_err_t HW_FLASH_LP_set_startup_area_temporary (flash_startup_area_swap_t swap_type)
{
    ssp_err_t err = SSP_SUCCESS;

    /* Set the SAS bit as requested */
    if (FLASH_STARTUP_AREA_BLOCK0 == swap_type)
    {
        R_FACI->FISR_b.SAS =  SAS_DEFAULT_AREA;
    }
    else if (FLASH_STARTUP_AREA_BLOCK1 == swap_type)
    {
        R_FACI->FISR_b.SAS =  SAS_ALTERNATE_AREA;
    }
    else
    {
        R_FACI->FISR_b.SAS =  (uint32_t)SAS_PER_BTFLG_AREA;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief  Configure an access window for the Code Flash memory using the provided start and end address. An access
 *         window defines a contiguous area in Code Flash for which programming/erase is enabled. This area is
 *         on block boundaries.
 *         The block containing start_addr is the first block. The block containing end_addr is the last block.
 *         The access window then becomes first block - last block inclusive. Anything outside this range
 *         of Code Flash is then write protected.
 *         This command DOES modify the configuration via The Configuration Set command to update the FAWS and FAWE.
 *
 * @param[in]  start_addr Determines the Starting block for the Code Flash access window.
 * @param[in]  end_addr   Determines the Ending block for the Code Flash access window.
 *
 * @retval SSP_SUCCESS                Access window successfully configured.
 * @retval SSP_ERR_ERASE_FAILED        Status is indicating an erase error. Possibly from a prior operation.
 * @retval SSP_ERR_CMD_LOCKED   FCU is in locked state, typically as a result of having received an illegal
 * command.
 * @retval SSP_ERR_WRITE_FAILED  Status is indicating a Programming error for the requested operation.
 * @retval SSP_ERR_TIMEOUT Timed out waiting for the FCU to become ready.
 **********************************************************************************************************************/
ssp_err_t HW_FLASH_LP_access_window_set (uint32_t const start_addr, uint32_t const end_addr)
{
    ssp_err_t err = SSP_SUCCESS;
    uint32_t  start_addr_idx;
    uint32_t  end_addr_idx;

    start_addr_idx = start_addr;
    end_addr_idx   = end_addr;

    /* Select Extra Area */
    R_FACI->FASR_b.EXS = 1;

    HW_FLASH_LP_extra_operation(start_addr_idx, end_addr_idx, FLASH_COMMAND_ACCESSWINDOW);

    do
    {
        err = HW_FLASH_LP_extra_check();
    } while (SSP_ERR_IN_USE == err);

    /* Select User Area */
    R_FACI->FASR_b.EXS = 0;

    return err;
}

/*******************************************************************************************************************//**
 * @brief  Remove any access window that is currently configured in the Code Flash.
 *         This command DOES modify the configuration via The Configuration Set command to update the FAWS and FAWE.
 *         Subsequent to this call all Code Flash is writable.
 *
 * @retval SSP_SUCCESS              Access window successfully removed.
 * @retval SSP_ERR_ERASE_FAILED      Status is indicating an erase error. Possibly from a prior operation.
 * @retval SSP_ERR_CMD_LOCKED   FCU is in locked state, typically as a result of having received an illegal
 * command.
 * @retval SSP_ERR_WRITE_FAILED  Status is indicating a Programming error for the requested operation.
 * @retval SSP_ERR_TIMEOUT      Timed out waiting for the FCU to become ready.
 **********************************************************************************************************************/
ssp_err_t HW_FLASH_LP_access_window_clear (void)
{
    ssp_err_t err = SSP_SUCCESS;

    /* Select Extra Area */
    R_FACI->FASR_b.EXS = 1;

    // Setting the accesswindow to 0,0 will clear any configured access window.
    HW_FLASH_LP_extra_operation(0, 0, FLASH_COMMAND_ACCESSWINDOW);

    do
    {
        err = HW_FLASH_LP_extra_check();
    } while (SSP_ERR_IN_USE == err);

    /* Select User Area */
    R_FACI->FASR_b.EXS = 0;

    return err;
}

/*******************************************************************************************************************//**
 * @brief Modifies the start-up program swap flag (BTFLG) based on the supplied parameters. These changes will
 *        take effect on the next reset. This command DOES modify the configuration via The Configuration
 *        Set command to update the BTFLG.
 *
 * @retval SSP_SUCCESS                Access window successfully removed.
 * @retval SSP_ERR_ERASE_FAILED        Status is indicating an erase error. Possibly from a prior operation.
 * @retval SSP_ERR_CMD_LOCKED   FCU is in locked state, typically as a result of having received an illegal
 * command.
 * @retval SSP_ERR_WRITE_FAILED  Status is indicating a Programming error for the requested operation.
 * @retval SSP_ERR_TIMEOUT Timed out waiting for the FCU to become ready.
 *
 **********************************************************************************************************************/
ssp_err_t HW_FLASH_LP_set_startup_area_boot (flash_startup_area_swap_t swap_type)
{
    ssp_err_t err               = SSP_SUCCESS;
    uint32_t  startup_area_mask = (uint32_t)((uint32_t)swap_type << 8);       // move selection to bit 8

    /* Select Extra Area */
    R_FACI->FASR_b.EXS = 1;

    // Setting the accesswindow to 0,0 will clear any configured access window.
    HW_FLASH_LP_extra_operation(startup_area_mask, 0, FLASH_COMMAND_STARTUPAREA);

    do
    {
        err = HW_FLASH_LP_extra_check();
    } while (SSP_ERR_IN_USE == err);

    /* Select User Area */
    R_FACI->FASR_b.EXS = 0;

    return err;
}

/*******************************************************************************************************************//**
 * @brief  Command processing for the extra area.
 *
 * @param[in]  start_addr Determines the Starting block for the Code Flash access window.
 * @param[in]  end_addr   Determines the Ending block for the Code Flash access window.
 * @param[in]  command  : select from R_FLASH_ACCESSWINDOW or R_FLASH_STARTUPAREA.
 *
 **********************************************************************************************************************/
void HW_FLASH_LP_extra_operation (const uint32_t    start_addr_startup_value,
                                         const uint32_t    end_addr,
                                         r_flash_command_t command)
{
    /** Per the spec: */
    /** Setting data to the FWBL0 register, this command is allowed to select the start-up area from the */
    /** default area (blocks 0-3) to the alternative area (blocks 4-7) and set the security. */
    /** Bit 8 of the FWBL0 register is 0: the alternative area (blocks 4-7) are selected as the start-up area. */
    /** Bit 8 of the FWBL0 register is 1: the default area (blocks 0-3) are selected as the start-up area. */
    /** Bit 14 of the FWBL0 register MUST be 1! Setting this bit to zero will clear the FSPR register and lock the */
    /** FLASH!!! It is not be possible to unlock it. */

    if (FLASH_COMMAND_ACCESSWINDOW == command)
    {
        /* Access Window start address setting */
        /** FWBL0 reg gets the Start Block address. FWBH0 reg gets the end address. */
        /** Convert the addresses to their respective block numbers */
        R_FACI->FWBL0 = (uint16_t) ((start_addr_startup_value >> 10) & 0xFFFF);
        R_FACI->FWBH0 = (uint16_t) ((end_addr >> 10) & 0xFFFF);

        /* Execute Access Window command */
        R_FACI->FEXCR = FEXCR_AW;
    }
    else
    {
        /* Startup Area Flag value setting */
        R_FACI->FWBH0 = (uint16_t) (0);
        R_FACI->FWBL0 = (start_addr_startup_value | 0x4000); ///< B14 (FSPR) must be on!    0100 0000 0000 0000

        /* Execute Startup Area Flag command */
        R_FACI->FEXCR = FEXCR_STARTUP;
    }
}

/*******************************************************************************************************************//**
 * @brief  Verifying the execution result for the extra area.
 *
 *
 * @retval SSP_SUCCESS                Access window successfully removed.
 * @retval SSP_ERR_WRITE_FAILED  Status is indicating a Programming error for the requested operation.
 * @retval SSP_ERR_TIMEOUT      Timed out waiting for completion of extra command.
 **********************************************************************************************************************/
ssp_err_t HW_FLASH_LP_extra_check (void)
{
    /* Timeout counter. */
    volatile uint32_t wait_cnt = FLASH_FRDY_CMD_TIMEOUT;

    /* Check EXFREADY Flag bit*/
    if (1 != R_FACI->FSTATR1_b.EXRDY)
    {
        return SSP_ERR_IN_USE;
    }

    /* Clear FEXCR register */
    R_FACI->FEXCR = FEXCR_CLEAR;

    while (0 != R_FACI->FSTATR1_b.EXRDY)
    {
        /* Check that execute command is completed. */
        /* Confirm that the written value can be read correctly. */
        if (wait_cnt <= (uint32_t)0)
        {
            /* return timeout status*/
            return SSP_ERR_TIMEOUT;
        }
        wait_cnt--;
    }

    if ((0 != R_FACI->FSTATR00_b.EILGLERR) || (0 != R_FACI->FSTATR00_b.PRGERR01))
    {
        HW_FLASH_LP_reset();
        return SSP_ERR_WRITE_FAILED;
    }

    return SSP_SUCCESS;
}
#endif /* if defined(BSP_MCU_GROUP_S3A7) || defined(BSP_MCU_GROUP_S124) || defined(BSP_MCU_GROUP_S128) */

/*******************************************************************************************************************//**
 * @} (end defgroup FLASH)
 **********************************************************************************************************************/
