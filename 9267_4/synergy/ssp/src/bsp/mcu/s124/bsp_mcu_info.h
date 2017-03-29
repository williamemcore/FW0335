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
/***********************************************************************************************************************
* File Name    : bsp_mcu_info.h
* Description  : Information about the MCU on this board
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @ingroup BSP_MCUs
 * @defgroup BSP_MCU_S124 S124
 * @brief Code that is common to S124 MCUs.
 *
 * Implements functions that are common to S124 MCUs.
 *
 * @{
***********************************************************************************************************************/

#ifndef BSP_MCU_INFO_H_
#define BSP_MCU_INFO_H_

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Gets MCU configuration information. */
#include "bsp_cfg.h"
#include "bsp_clock_cfg.h"
#include "bsp_irq_cfg.h"

/* BSP Common Includes. */
#include "../../src/bsp/mcu/all/bsp_common.h"
#include "../../inc/ssp_common_api.h"

#if defined(__GNUC__)
/* CMSIS-CORE currently generates 2 warnings when compiling with GCC. One in core_cmInstr.h and one in core_cm4_simd.h.
 * We are not modifying these files so we will ignore these warnings temporarily. */
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif

/* CMSIS-CORE Renesas Device Files. */
#include "../../src/bsp/cmsis/Device/RENESAS/S124/Include/S124.h"
#include "../../src/bsp/cmsis/Device/RENESAS/S124/Include/system_S124.h"

/* CMSIS-CORE Standard Files. */
#include "core_cm0plus.h"
#include "core_cmInstr.h"
#include "core_cmFunc.h"

#if defined(__GNUC__)
/* Restore warning settings for 'conversion' and 'sign-conversion' to as specified on command line. */
#pragma GCC diagnostic pop
#endif

/* BSP MCU Specific Includes. */
#include "../../src/bsp/mcu/s124/bsp_register_protection.h"
#include "../../src/bsp/mcu/s124/bsp_locking.h"
#include "../../src/bsp/mcu/s124/bsp_hw_locks.h"
#include "../../src/bsp/mcu/s124/bsp_irq.h"
#include "../../src/bsp/mcu/s124/bsp_group_irq.h"
#include "../../src/bsp/mcu/s124/bsp_clocks.h"
#include "../../src/bsp/mcu/s124/bsp_elc.h"
#include "../../src/bsp/mcu/s124/bsp_ioport.h"
#include "../../src/bsp/mcu/s124/bsp_cache.h"

/* BSP Common Includes (Other than bsp_common.h) */
#include "../../src/bsp/mcu/all/bsp_common_leds.h"
#include "../../src/bsp/mcu/all/bsp_delay.h"

#include "../../src/bsp/mcu/s124/bsp_mcu_api.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* MCU Group name. */
#if   BSP_CFG_MCU_PART_CORE == 0x02
    #define BSP_MCU_GROUP_S124              (1)     ///< CM0, No FPU, 32MHz
#else
    #error "ERROR - BSP_CFG_MCU_PART_CORE - Unknown MCU Group chosen in r_bsp_config.h"
#endif

/* Memory size of your MCU. */
#if   BSP_CFG_MCU_PART_MEMORY_SIZE == 0x06
    #define BSP_ROM_SIZE_BYTES              (65536)     ///< 64KB of ROM
    #define BSP_RAM_SIZE_BYTES              (16384)     ///< 16KB of RAM
    #define BSP_DATA_FLASH_SIZE_BYTES       (4096)      ///< 4KB of Data Flash
#elif BSP_CFG_MCU_PART_MEMORY_SIZE == 0x07
    #define BSP_ROM_SIZE_BYTES              (131072)    ///< 128KB of ROM
    #define BSP_RAM_SIZE_BYTES              (24576)     ///< 24KB of RAM
    #define BSP_DATA_FLASH_SIZE_BYTES       (8192)      ///< 8KB of Data Flash
#endif

/* Package and pin count. */
#if   BSP_CFG_MCU_PART_PACKAGE == 0x0
    #define BSP_PACKAGE_BGA
    #define BSP_PACKAGE_PINS        (121)
#elif BSP_CFG_MCU_PART_PACKAGE == 0x1
    #define BSP_PACKAGE_LQFP
    #define BSP_PACKAGE_PINS        (64)
#elif BSP_CFG_MCU_PART_PACKAGE == 0x2
    #define BSP_PACKAGE_LQFP
    #define BSP_PACKAGE_PINS        (100)
#elif BSP_CFG_MCU_PART_PACKAGE == 0x3
    #define BSP_PACKAGE_LQFP
    #define BSP_PACKAGE_PINS        (144)
#elif BSP_CFG_MCU_PART_PACKAGE == 0x4
    #define BSP_PACKAGE_LGA
    #define BSP_PACKAGE_PINS        (100)
#elif BSP_CFG_MCU_PART_PACKAGE == 0x5
    #define BSP_PACKAGE_LGA
    #define BSP_PACKAGE_PINS        (145)
#elif BSP_CFG_MCU_PART_PACKAGE == 0x6
    #define BSP_PACKAGE_WQFN
    #define BSP_PACKAGE_PINS        (64)
#endif

/* These macros define clock speeds for fixed speed clocks. */
#define BSP_LOCO_HZ                 (32768)         ///< Low-speed On-Chip Oscillator
#define BSP_MOCO_HZ                 (8000000)       ///< Middle-speed On-Chip Oscillator
#define BSP_SUB_CLOCK_HZ            (32768)         ///< Sub-Clock Oscillator

#if   BSP_CFG_HOCO_FREQUENCY == 0
    #define BSP_HOCO_HZ             (24000000)      ///< High-speed On-Chip Oscillator
#elif BSP_CFG_HOCO_FREQUENCY == 1
    #define BSP_HOCO_HZ             (29492000)      ///< High-speed On-Chip Oscillator
#elif BSP_CFG_HOCO_FREQUENCY == 2
    #define BSP_HOCO_HZ             (32000000)      ///< High-speed On-Chip Oscillator
#elif BSP_CFG_HOCO_FREQUENCY == 4
    #define BSP_HOCO_HZ             (48000000)      ///< High-speed On-Chip Oscillator
#elif BSP_CFG_HOCO_FREQUENCY == 5
    #define BSP_HOCO_HZ             (64000000)      ///< High-speed On-Chip Oscillator
#else
    #error "Invalid HOCO frequency chosen (BSP_CFG_HOCO_FREQUENCY) in bsp_clock_cfg.h"
#endif

/* Number of NVIC slots. Includes 16 fixed exception vectors at the beginning. */
#define BSP_VECTOR_TABLE_ENTRIES    (48)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/

#endif /* BSP_MCU_INFO_H_ */

/** @} (end defgroup BSP_MCU_S124) */
