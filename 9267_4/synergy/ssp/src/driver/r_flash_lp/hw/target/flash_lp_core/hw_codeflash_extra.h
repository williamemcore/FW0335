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
 * File Name    : hw_codeflash_extra.h
 * Description  : Flash Control Access window and swap control processing for Low Power Flash.
 **********************************************************************************************************************/


/*******************************************************************************************************************//**
 * @addtogroup FLASH
 * @{
 **********************************************************************************************************************/

#ifndef R_CODEFLASH_EXTRA_H
#define R_CODEFLASH_EXTRA_H

/******************************************************************************
 * Macro definitions
 ******************************************************************************/
/*  operation definition (FEXCR Register setting)*/
#define FEXCR_STARTUP      (0x81)
#define FEXCR_AW           (0x82)
#define FEXCR_CLEAR        (0x00)

#define DUMMY              (0xFFFFFFFF)
#define DEFAULT_AREA       (1)
#define DEFAULT_AREA_VALUE (0xFFFF)
#define STARTUP_AREA_VALUE (0xFEFF)

#define SAS_DEFAULT_AREA   0x02                 ///< Bit value for FSUAC register specifying DEFAULT area
#define SAS_ALTERNATE_AREA 0x03                 ///< Bit value for FSUAC register specifying ALTERNATE area
#define SAS_PER_BTFLG_AREA 0x00                 ///< Bit value for FSUAC register specifying use BTFLG area

/******************************************************************************
 * Exported global functions (to be accessed by other files)
 ******************************************************************************/

/**   FLASH operation command values */
typedef enum e_flash_command
{
    FLASH_COMMAND_ACCESSWINDOW,               /**< Flash access window command */
    FLASH_COMMAND_STARTUPAREA                 /**< Flash change startup area command */
} r_flash_command_t;

ssp_err_t   HW_FLASH_LP_access_window_set (uint32_t const start_addr, uint32_t const end_addr) PLACE_IN_RAM_SECTION;

ssp_err_t   HW_FLASH_LP_access_window_clear (void) PLACE_IN_RAM_SECTION;

/*LDRA_INSPECTED 219 s - This is an allowed exception to LDRA standard 219 S "User name starts with underscore."*/
ssp_err_t   HW_FLASH_LP_set_startup_area_temporary (flash_startup_area_swap_t swap_type) PLACE_IN_RAM_SECTION;

/*LDRA_INSPECTED 219 s - This is an allowed exception to LDRA standard 219 S "User name starts with underscore."*/
ssp_err_t   HW_FLASH_LP_set_startup_area_boot (flash_startup_area_swap_t swap_type) PLACE_IN_RAM_SECTION;

/*LDRA_INSPECTED 219 s - This is an allowed exception to LDRA standard 219 S "User name starts with underscore."*/
void HW_FLASH_LP_extra_operation (const uint32_t    start_addr_startup_value,
                                         const uint32_t    end_addr,
                                         r_flash_command_t command) PLACE_IN_RAM_SECTION;

ssp_err_t HW_FLASH_LP_extra_check (void) PLACE_IN_RAM_SECTION;

ssp_err_t HW_FLASH_LP_set_startup_area (uint32_t value) PLACE_IN_RAM_SECTION;
#endif /* R_CODEFLASH_EXTRA_H */

/*******************************************************************************************************************//**
 * @} (end addtogroup FLASH)
 **********************************************************************************************************************/
