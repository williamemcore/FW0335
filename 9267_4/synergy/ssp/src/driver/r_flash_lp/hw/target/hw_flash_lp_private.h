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
 * File Name    : hw_flash_lp_private.h
 * Description  : FLASH LLD header file for Low power Flash.
 **********************************************************************************************************************/
#include "r_flash_lp.h"


/*******************************************************************************************************************//**
 * @addtogroup R_FLASH_LP
 * @{
 **********************************************************************************************************************/

#ifndef HW_FLASH_LP_PRIVATE_H
#define HW_FLASH_LP_PRIVATE_H


/**********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#if defined(BSP_MCU_GROUP_S3A7)                                                      /// NPP7
#define FLASH_NUM_BLOCKS_CF        (512)                                             ///< Total number of Code Flash
                                                                                     // Blocks
#define FLASH_MIN_PGM_SIZE_CF      (8)                                               ///< Minimum program size for Code
                                                                                     // Flash (64 bits)
#define FLASH_CODE_SMALL_BLOCK_SZ  (2048)                                            ///< Smallest Code Flash block size
#define FLASH_CODE_LARGE_BLOCK_SZ  (2048)                                            ///< Largest Code Flash block size
#define FLASH_CODE_BLANK_CHECK_MAX (FLASH_NUM_BLOCKS_CF * FLASH_CODE_SMALL_BLOCK_SZ) ///< Max # of bytes that may be
                                                                                     // blank check for Code Flash

#define FLASH_NUM_BLOCKS_DF        (16)                                              ///< Total number of Data Flash
                                                                                     // Blocks
#define FLASH_MIN_PGM_SIZE_DF      (1)                                               ///< Minimum program size for Data
                                                                                     // Flash
#define FLASH_DATA_BLOCK_SZ        (1024)                                            ///< Data Block size
#define FLASH_DATA_BLANK_CHECK_MAX (FLASH_NUM_BLOCKS_DF * FLASH_DATA_BLOCK_SZ)       ///< Max # of bytes that may be
                                                                                     // blank check for Data Flash
#define FLASH_CF_BLOCK_END         0x00FFFFFU                                        ///<   End of Code Flash Area
#define FLASH_DF_BLOCK_END         0x40103FFFU                                       ///<   End of Data Flash Area
#endif // if defined(BSP_MCU_GROUP_S3A7)

#if defined(BSP_MCU_GROUP_S124)
#define FLASH_NUM_BLOCKS_CF        (128)       ///< Total number of Code Flash Blocks (Max part sz == 128Kb) there is also 64Kb
#define FLASH_MIN_PGM_SIZE_CF      (4)         ///< Minimum program size for Code Flash (32 bits)
#define FLASH_CODE_SMALL_BLOCK_SZ  (1024)      ///< Smallest Code Flash block size
#define FLASH_CODE_LARGE_BLOCK_SZ  (1024)      ///< Largest Code Flash block size

#define FLASH_NUM_BLOCKS_DF        (4)         ///< Total number of Data Flash Blocks
#define FLASH_MIN_PGM_SIZE_DF      (1)         ///< Minimum program size for Data Flash
#define FLASH_DATA_BLOCK_SZ        (1024)      ///< Data Block size
#define FLASH_DATA_BLANK_CHECK_MAX (4096)      ///< Max # of bytes that may be blank check for Data Flash
#define FLASH_DATA_BLANK_CHECK_MIN (4)         ///< Min # of bytes that may be blank check for Data Flash
#define FLASH_CF_BLOCK_END         0x001FFFFU  ///<   End of Code Flash Area
#define FLASH_DF_BLOCK_END         0x40100FFFU ///<   End of Data Flash Area
#endif // if defined(BSP_MCU_GROUP_S124)

#if defined(BSP_MCU_GROUP_S128)
#define FLASH_NUM_BLOCKS_CF        (256)       ///< Total number of Code Flash Blocks (Max part sz == 256Kb) there is also 128Kb
#define FLASH_MIN_PGM_SIZE_CF      (4)         ///< Minimum program size for Code Flash (32 bits)
#define FLASH_CODE_SMALL_BLOCK_SZ  (1024)      ///< Smallest Code Flash block size
#define FLASH_CODE_LARGE_BLOCK_SZ  (1024)      ///< Largest Code Flash block size

#define FLASH_NUM_BLOCKS_DF        (4)         ///< Total number of Data Flash Blocks
#define FLASH_MIN_PGM_SIZE_DF      (1)         ///< Minimum program size for Data Flash
#define FLASH_DATA_BLOCK_SZ        (1024)      ///< Data Block size
#define FLASH_DATA_BLANK_CHECK_MAX (4096)      ///< Max # of bytes that may be blank check for Data Flash
#define FLASH_DATA_BLANK_CHECK_MIN (4)         ///< Min # of bytes that may be blank check for Data Flash
#define FLASH_CF_BLOCK_END         0x003FFFFU  ///<   End of Code Flash Area
#define FLASH_DF_BLOCK_END         0x40100FFFU ///<   End of Data Flash Area
#endif // if defined(BSP_MCU_GROUP_S128)


#define FLASH_CF_BLOCK_0 0                      ///<   Start of Code Flash Area
#define FLASH_DF_BLOCK_0 0x40100000U            ///<   Start of Data Flash area

/** The maximum timeout for commands is 100usec when FCLK is 16 MHz i.e. 1600 FCLK cycles.
 * Assuming worst case of ICLK at 240 MHz and FCLK at 4 MHz, and optimization set to max such that
 * each count decrement loop takes only 5 cycles, then ((240/4)*1600)/5 = 19200 */
#define FLASH_FRDY_CMD_TIMEOUT (19200)

/** Time that it would take for the Data Buffer to be empty (DBFULL Flag) is 90 FCLK cycles.
 * Assuming worst case of ICLK at 240 MHz and FCLK at 4 MHz, and optimization set to max such that
 * each count decrement loop takes only 5 cycles, then ((240/4)*90)/5 = 1080 */
#define FLASH_DBFULL_TIMEOUT (1080)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
/** Flash software lock states */
typedef enum e_flash_states_t
{
    FLASH_STATE_UNINITIALIZED = 0,
    FLASH_STATE_INITIALIZATION,
    FLASH_STATE_READY,
    FLASH_STATE_WRITING,
    FLASH_STATE_ERASING,
    FLASH_STATE_BLANK_CHECK,
    FLASH_STATE_ACCESS_WINDOW,
    FLASH_STATE_STARTUP_AREA,
    FLASH_UPDATE_FCLK
} flash_states_t;

/** Flash types */
typedef enum e_flash_type
{
    FLASH_TYPE_CODE_FLASH = 0,      ///< Operation is referencing Code Flash
    FLASH_TYPE_DATA_FLASH           ///< Operation is referencing Data Flash
} flash_type_t;

/** Possible Flash operation states */
typedef enum e_flash_operation
{
    FLASH_OPERATION_INVALID,
    FLASH_OPERATION_IDLE,
    FLASH_OPERATION_CF_ERASE,
    FLASH_OPERATION_CF_WRITE,
    FLASH_OPERATION_DF_ERASE,
    FLASH_OPERATION_DF_WRITE,
    FLASH_OPERATION_DF_BLANKCHECK,
    FLASH_OPERATION_CF_BLANKCHECK,
    FLASH_OPERATION_DF_BGO_WRITE,
    FLASH_OPERATION_DF_BGO_ERASE,
    FLASH_OPERATION_DF_BGO_BLANKCHECK,
    FLASH_OPERATION_FCU_INIT
} flash_operation_t;

/** Parameters reflecting current in progress Flash operation */
typedef struct st_current_parameters
{
    uint32_t           src_addr;                      ///< Source address for operation in progress
    uint32_t           dest_addr;                     ///< Destination address for operation in progress
    uint32_t           total_count;                   ///< total number of bytes to write/erase
    uint32_t           current_count;                 ///< bytes of total completed
    flash_operation_t  current_operation;             ///< Operation in progress, ie. FLASH_OPERATION_CF_ERASE
    uint16_t           min_pgm_size;                  ///< Minimum # of bytes that can be programmed, as defined for the
                                                      // part, usually differs for CF and DF
    uint32_t           wait_cnt;                      ///< Worst case wait time for operation completion
    bool               bgo_enabled_df;                ///< Data Flash BGO operation enable/disable
    uint32_t           wait_max_erase_cf_small_block; ///< Worst case erase time for small CF block
    uint32_t           wait_max_erase_df_block;       ///< Worst case erase time for DF block
    uint32_t           wait_max_erase_cf_large_block; ///< Worst case erase time for large CF block
    uint32_t           wait_max_notify_fcu_clock;     ///< max timeout value when using the peripheral clock
                                                      // notification command
    uint32_t           wait_max_blank_check;          ///< Worst case blankcheck time for minimum blank check size
    uint32_t           wait_max_write_df;             ///< Worst case write time for DF minimum write size
    uint32_t           wait_max_write_cf;             ///< Worst case write time for CF minimum write size
    uint32_t           flash_clock_freq;              ///< Flash clock frequency in Hz
    uint32_t           system_clock_freq;             ///< System clock frequency in Hz
} current_parameters_t;

/** Flash block details */
typedef struct st_flash_block_info
{
    uint32_t  num_blocks;                  ///< number of blocks at this size
    uint32_t  block_size;                  ///< Size of this block
    uint32_t  block_section_st_addr;       ///< starting address for this block section (blocks of this size)
    uint32_t  block_section_end_addr;      ///< ending address for this block section (blocks of this size)
    uint32_t  this_block_st_addr;          ///< starting address for this block (for queried address)
    uint32_t  this_block_end_addr;         ///< ending address for this block (blocks of this size)
    uint32_t  this_block_number;           ///< zero based block number for this address
    bool      is_code_flash_addr;          ///< true if queried address is a Code Flash address
} flash_block_info_t;

/**********************************************************************************************************************
 * Public Functions
 **********************************************************************************************************************/

/** NOTE: The keyword 'PLACE_FLASH_HERE' is defined in r_flash.h and in conjunction with the
 * FLASH_CFG_PARAM_CODE_FLASH_PROGRAMMING_ENABLE config setting, determines if sections of the Flash API will be copied
 * to
 * and executed from RAM. This is required for Data Flash BGO (Background) operation.
 */
ssp_err_t HW_FLASH_LP_irq_cfg (bool state, flash_cfg_t const * const p_cfg) PLACE_IN_RAM_SECTION;

void      HW_FLASH_LP_init (current_parameters_t * p_current_parameters) PLACE_IN_RAM_SECTION;

ssp_err_t HW_FLASH_LP_pe_mode_enter (flash_type_t flash_type) PLACE_IN_RAM_SECTION;

ssp_err_t HW_FLASH_LP_pe_mode_exit (void) PLACE_IN_RAM_SECTION;

ssp_err_t HW_FLASH_LP_erase (uint32_t block_address, uint32_t num_blocks) PLACE_IN_RAM_SECTION;

ssp_err_t HW_FLASH_LP_write (uint32_t const src_start_address, uint32_t dest_start_address,
                             uint32_t num_bytes) PLACE_IN_RAM_SECTION;

ssp_err_t HW_FLASH_LP_blankcheck (uint32_t start_address, uint32_t num_bytes,
                                  flash_result_t * result) PLACE_IN_RAM_SECTION;

void      HW_FLASH_LP_reset (void) PLACE_IN_RAM_SECTION;

ssp_err_t HW_FLASH_HP_stop (void) PLACE_IN_RAM_SECTION;

void      flash_ReleaseState (void) PLACE_IN_RAM_SECTION;

bool      flash_get_block_info (uint32_t addr, flash_block_info_t * p_block_info) PLACE_IN_RAM_SECTION;

/** FRDY ISR is only used for DataFlash operations. For Code flash operations are blocking. Therefore ISR does
 * not need to be located in RAM.
 */
void fcu_frdyi_isr (void);

/** These following tables define the block size and numbers for this parts Code and Data Flash.
 *  flash_get_block_info() is called by various API functions to get information about a particular data or code flash
 * block.
 */
#define NUM_BLOCK_ROM_TABLE_ENTRIES  2
#define NUM_BLOCK_DATA_TABLE_ENTRIES 2


#endif /* HW_FLASH_LP_PRIVATE_H */

/*******************************************************************************************************************//**
 * @} (end addtogroup R_FLASH_LP)
 **********************************************************************************************************************/
