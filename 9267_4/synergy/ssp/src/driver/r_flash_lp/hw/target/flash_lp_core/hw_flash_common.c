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
 * File Name    : hw_flash_common.c
 * Description  : Common functions used by both the Code and Data Low Power Flash modules
 **********************************************************************************************************************/


/*******************************************************************************************************************//**
 * @ingroup HAL_Library
 * @addtogroup FLASH_LP_API
 * @brief FLASH API
 *
 * This module implements Common functions used by both the Code and Data Flash modules.
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
 * Macro definitions
 ******************************************************************************/
#if defined(__ICCARM__)
#define BSP_ATTRIBUTE_STACKLESS __stackless
#elif defined(__GNUC__)
/*LDRA_INSPECTED 293 s - This is an allowed exception to LDRA standard 293 S "Non ANSI/ISO construct used. "*/
#define BSP_ATTRIBUTE_STACKLESS __attribute__((naked))
#endif

/******************************************************************************
 * Private global variables and functions
 ******************************************************************************/

BSP_ATTRIBUTE_STACKLESS static void HW_FLASH_LP_delay_loop (unsigned n) PLACE_IN_RAM_SECTION;

// Roughly 4 cycles per loop
#define DELAY_LOOP_CYCLES 4UL
BSP_ATTRIBUTE_STACKLESS static void HW_FLASH_LP_delay_loop (unsigned n)
{
    SSP_PARAMETER_NOT_USED(n);      /// prevent compiler 'unused' warning
    __asm volatile ("label1:\n"
#if defined(__ICCARM__)
                    "   subs r0, #1         \n"     ///< 1 cycle
#elif defined(__GNUC__)
                    "   sub r0, r0, #1      \n"     ///< 1 cycle
#endif
                    "cmp r0, #0\n"              //  1 cycle
/* CM0 has a different instruction set */					
#ifdef __CORE_CM0PLUS_H_GENERIC         
                    "   bne label1   \n"     ///< 2 cycles
#else
                    "   bne.n label1 \n"     ///< 2 cycles
#endif
                    "bx lr\n");                 // ~2 cycles
}


/*******************************************************************************
 * Outline      : Function that specifies the execution time
 * Header       : none
 * Function Name: r_flash_delay_us
 * Description  : The number of loops is calculated based on the execution time (Î¼s)
 *              : and the sytem clock (ICLK) frequency, and the intrinsic function
 *              : that specifies the number of loops is called.
 * Arguments    : us  : Execution time
 *             : mhz : ICLK frequency (in Mhz)
 * Return Value : none
 *******************************************************************************/
void HW_FLASH_LP_delay_us (unsigned long us, uint32_t mhz)
{
    signed long loop_cnt;

    // @12 MHz, one loop is 332 ns. A delay of 5 us would require 15 loops. 15 * 332 = 4980 ns or ~ 5us
    /* Calculation of a loop count */
    loop_cnt = (signed long)((us * mhz) / DELAY_LOOP_CYCLES);

    if (loop_cnt > 0)
    {
        HW_FLASH_LP_delay_loop((unsigned int) loop_cnt);
    }
}
#endif /* if defined(BSP_MCU_GROUP_S3A7) || defined(BSP_MCU_GROUP_S124) || defined(BSP_MCU_GROUP_S128) */

/*******************************************************************************************************************//**
 * @} (end defgroup FLASH)
 **********************************************************************************************************************/
