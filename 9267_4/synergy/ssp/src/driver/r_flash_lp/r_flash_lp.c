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
 * File Name    : r_flash_lp.c
 * Description  : HLD Interface for the Low power FLASH peripheral on SC32 MCUs.
 **********************************************************************************************************************/


/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "bsp_api.h"

#if defined(BSP_MCU_GROUP_S3A7) || defined(BSP_MCU_GROUP_S124)|| defined(BSP_MCU_GROUP_S128)
#include "r_flash_lp.h"
#include "hw\target\hw_flash_lp_private.h"
#include "r_flash_lp_private_api.h"
/* Configuration for this package. */
#include "r_flash_cfg.h"
#include "r_cgc_api.h"
#include "r_cgc.h"
#include "hw\target\flash_lp_core\hw_dataflash.h"
#include "hw\target\flash_lp_core\hw_codeflash_extra.h"
#include "hw\target\flash_lp_core\hw_codeflash.h"

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
#define MINIMUM_SUPPORTED_FCLK_FREQ 4U            /// Minimum FCLK for Flash Operations

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/** Macro for error logger. */
#ifndef FLASH_ERROR_RETURN
#define FLASH_ERROR_RETURN(a, err) SSP_ERROR_RETURN((a), (err), g_module_name, &g_flash_lp_version)
#endif

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
static flash_block_info_t g_block_info;             /// Structure holding block info about an address.
static bool               g_flash_api_open = false; /// true once API has been successfully opened.
bsp_lock_t                g_flash_Lock;             /// Flash commands software lock


/** Structure that holds the parameters for current operations*/
current_parameters_t  g_current_parameters;

/** State variable for the Flash API. */
static flash_states_t g_flash_state = FLASH_STATE_UNINITIALIZED;

/** Internal functions. */
/*LDRA_INSPECTED 219 s - This is an allowed exception to LDRA standard 219 S "User name starts with underscore."*/
static ssp_err_t      flash_lock_state (flash_states_t new_state) PLACE_IN_RAM_SECTION;

/*LDRA_INSPECTED 219 s - This is an allowed exception to LDRA standard 219 S "User name starts with underscore."*/
static ssp_err_t      setup_for_pe_mode (flash_type_t flash_type, flash_states_t flash_state) PLACE_IN_RAM_SECTION;

static ssp_err_t      flash_setup (void) PLACE_IN_RAM_SECTION;


/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
const flash_api_t g_flash_on_flash_lp =
{
    .open              = R_FLASH_LP_Open,
    .close             = R_FLASH_LP_Close,
    .write             = R_FLASH_LP_Write,
    .read              = R_FLASH_LP_Read,
    .erase             = R_FLASH_LP_Erase,
    .blankCheck        = R_FLASH_LP_BlankCheck,
    .statusGet         = R_FLASH_LP_StatusGet,
    .accessWindowSet   = R_FLASH_LP_AccessWindowSet,
    .accessWindowClear = R_FLASH_LP_AccessWindowClear,
    .reset             = R_FLASH_LP_Reset,
    .startupAreaSelect = R_FLASH_LP_StartUpAreaSelect,
    .updateFlashClockFreq = R_FLASH_LP_UpdateFlashClockFreq,
    .versionGet        = R_FLASH_LP_VersionGet
};

#if defined(__GNUC__)
/* This structure is affected by warnings from the GCC compiler bug https://gcc.gnu.org/bugzilla/show_bug.cgi?id=60784
 * This pragma suppresses the warnings in this structure only, and will be removed when the SSP compiler is updated to
 * v5.3.*/
/*LDRA_INSPECTED 69 S */
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
/** Version data structure used by error logger macro. */
static const ssp_version_t g_flash_lp_version =
{
    .api_version_minor  = FLASH_API_VERSION_MINOR,
    .api_version_major  = FLASH_API_VERSION_MAJOR,
    .code_version_major = FLASH_LP_CODE_VERSION_MAJOR,
    .code_version_minor = FLASH_LP_CODE_VERSION_MINOR
};
#if defined(__GNUC__)
/* Restore warning settings for 'missing-field-initializers' to as specified on command line. */
/*LDRA_INSPECTED 69 S */
#pragma GCC diagnostic pop
#endif

/** Name of module used by error logger macro */
#if BSP_CFG_ERROR_LOG != 0
static const char          g_module_name[] = "r_flash_lp";
#endif

#if defined(BSP_MCU_GROUP_S124)

/*******************************************************************************************************************//**
 * User (ROM) area for NPP4 is 128 blocks of 1K for a total of 128K.
 **********************************************************************************************************************/
static const flash_block_info_t g_flash_RomBlockSizes[NUM_BLOCK_ROM_TABLE_ENTRIES] =
{
        {.is_code_flash_addr = true, .num_blocks = (uint32_t)128, .block_section_end_addr = (uint32_t)0x1FFFF, .this_block_end_addr = (uint32_t)0x1FFFF,
         .block_section_st_addr  = (uint32_t)0,  .block_size = (uint32_t)1024, .this_block_st_addr = (uint32_t)0, .this_block_number = (uint32_t)0},

        {.is_code_flash_addr = false, .num_blocks = (uint32_t)0, .block_section_end_addr = (uint32_t)0, .this_block_end_addr = (uint32_t)0,
         .block_section_st_addr  = (uint32_t)0,  .block_size = (uint32_t)0, .this_block_st_addr = (uint32_t)0, .this_block_number = (uint32_t)0},
};


/*******************************************************************************************************************//**
 * Data area for NPP4 is 4 blocks of 1K for a total of 4K.
 **********************************************************************************************************************/
static const flash_block_info_t g_flash_DataBlockSizes[NUM_BLOCK_DATA_TABLE_ENTRIES] =
{
        {.is_code_flash_addr = false, .num_blocks = (uint32_t)4, .block_section_end_addr = (uint32_t)0x40100FFF, .this_block_end_addr = (uint32_t)0x40100FFF,
         .block_section_st_addr  = (uint32_t)0x40100000,  .block_size = (uint32_t)1024, .this_block_st_addr = (uint32_t)0x40100000, .this_block_number = (uint32_t)0},

        {.is_code_flash_addr = false, .num_blocks = (uint32_t)0, .block_section_end_addr = (uint32_t)0, .this_block_end_addr = (uint32_t)0,
         .block_section_st_addr  = (uint32_t)0,  .block_size = (uint32_t)0, .this_block_st_addr = (uint32_t)0, .this_block_number = (uint32_t)0},
};



#endif // if defined(BSP_MCU_GROUP_S124)

#if defined(BSP_MCU_GROUP_S128)

/*******************************************************************************************************************//**
 * User (ROM) area for S128 is 256 blocks of 1K for a total of 256K.
 **********************************************************************************************************************/
static const flash_block_info_t g_flash_RomBlockSizes[NUM_BLOCK_ROM_TABLE_ENTRIES] =
{
        {.is_code_flash_addr = true, .num_blocks = (uint32_t)256, .block_section_end_addr = (uint32_t)0x3FFFF, .this_block_end_addr = (uint32_t)0x3FFFF,
         .block_section_st_addr  = (uint32_t)0,  .block_size = (uint32_t)1024, .this_block_st_addr = (uint32_t)0, .this_block_number = (uint32_t)0},

        {.is_code_flash_addr = false, .num_blocks = (uint32_t)0, .block_section_end_addr = (uint32_t)0, .this_block_end_addr = (uint32_t)0,
         .block_section_st_addr  = (uint32_t)0,  .block_size = (uint32_t)0, .this_block_st_addr = (uint32_t)0, .this_block_number = (uint32_t)0},
};


/*******************************************************************************************************************//**
 * Data area for S128 is 4 blocks of 1K for a total of 4K.
 **********************************************************************************************************************/
static const flash_block_info_t g_flash_DataBlockSizes[NUM_BLOCK_DATA_TABLE_ENTRIES] =
{
        {.is_code_flash_addr = false, .num_blocks = (uint32_t)4, .block_section_end_addr = (uint32_t)0x40100FFF, .this_block_end_addr = (uint32_t)0x40100FFF,
         .block_section_st_addr  = (uint32_t)0x40100000,  .block_size = (uint32_t)1024, .this_block_st_addr = (uint32_t)0x40100000, .this_block_number = (uint32_t)0},

        {.is_code_flash_addr = false, .num_blocks = (uint32_t)0, .block_section_end_addr = (uint32_t)0, .this_block_end_addr = (uint32_t)0,
         .block_section_st_addr  = (uint32_t)0,  .block_size = (uint32_t)0, .this_block_st_addr = (uint32_t)0, .this_block_number = (uint32_t)0},
};



#endif // if defined(BSP_MCU_GROUP_S128)

#if defined(BSP_MCU_GROUP_S3A7)

/*******************************************************************************************************************//**
 * User (ROM) area for NPP7 is 512 blocks of 2K for a total of 1M.
 **********************************************************************************************************************/

static const flash_block_info_t g_flash_RomBlockSizes[NUM_BLOCK_ROM_TABLE_ENTRIES] =
{
        {.is_code_flash_addr = true, .num_blocks = (uint32_t)512, .block_section_end_addr = (uint32_t)0xFFFFF, .this_block_end_addr = (uint32_t)0xFFFFF,
         .block_section_st_addr  = (uint32_t)0,  .block_size = (uint32_t)2048, .this_block_st_addr = (uint32_t)0, .this_block_number = (uint32_t)0},

        {.is_code_flash_addr = false, .num_blocks = (uint32_t)0, .block_section_end_addr = (uint32_t)0, .this_block_end_addr = (uint32_t)0,
         .block_section_st_addr  = (uint32_t)0,  .block_size = (uint32_t)0, .this_block_st_addr = (uint32_t)0, .this_block_number = (uint32_t)0},
};



/*******************************************************************************************************************//**
 * Data area for NPP7 is 16 blocks of 1K for a total of 16K.
 **********************************************************************************************************************/
static const flash_block_info_t g_flash_DataBlockSizes[NUM_BLOCK_DATA_TABLE_ENTRIES] =
{
        {.is_code_flash_addr = false, .num_blocks = (uint32_t)16, .block_section_end_addr = (uint32_t)0x40103FFF, .this_block_end_addr = (uint32_t)0x40103FFF,
         .block_section_st_addr  = (uint32_t)0x40100000,  .block_size = (uint32_t)1024, .this_block_st_addr = (uint32_t)0x40100000, .this_block_number = (uint32_t)0},

        {.is_code_flash_addr = false, .num_blocks = (uint32_t)0, .block_section_end_addr = (uint32_t)0, .this_block_end_addr = (uint32_t)0,
         .block_section_st_addr  = (uint32_t)0,  .block_size = (uint32_t)0, .this_block_st_addr = (uint32_t)0, .this_block_number = (uint32_t)0},
};

#endif // if defined(BSP_MCU_GROUP_S3A7)
/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @addtogroup FLASH
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief  Initialize the Low Power flash peripheral. Implements flash_api_t::open.
 *
 * The Open function initializes the Flash. It first copies the FCU firmware to FCURAM
 * and sets the FCU Clock based on the current FCLK frequency. In addition, if Code Flash programming is enabled,
 * the API code responsible for Code Flash programming will be copied to RAM.
 *
 * This function must be called once prior to calling any other FLASH API functions.
 * If a user supplied callback function is supplied, then the Flash Ready interrupt will be configured to
 * call the users callback routine with an Event type describing the source of the interrupt for Data Flash
 * operations.
 * Subsequent to successfully completing this call g_flash_api_open will be true.
 *
 * @note Providing a callback function in the supplied p_cfg->callback field, automatically configures the
 *       Flash for Data Flash to operate in non-blocking (BGO) mode.
 *
 * Subsequent to a successful Open(), the Flash is ready to process additional Flash commands.
 *
 * @retval SSP_SUCCESS               Initialization was successful and timer has started.
 * @retval SSP_ERR_ASSERTION         NULL provided for p_ctrl or p_cfg.
 * @retval SSP_ERR_INVALID_ARGUMENT  Invalid argument provided to CGC call from flash_setup(). Should never occur...
 * @retval SSP_ERR_HW_LOCKED         Hardware lock for Low Power Flash peripheral is already taken.
 * @retval SSP_ERR_IRQ_BSP_DISABLED  Caller is requesting BGO but the Flash interrupt is not enabled.
 *
 **********************************************************************************************************************/
ssp_err_t R_FLASH_LP_Open (flash_ctrl_t * const p_ctrl, flash_cfg_t const * const p_cfg)
{
    /** g_flash_lp_version is accessed by the ASSERT macro only, and so compiler toolchain can issue a
     *  warning that it is not accessed. The code below eliminates this warning and also ensures data
     *  structures are not optimized away. */
    SSP_PARAMETER_NOT_USED(g_flash_lp_version);

    /** Eliminate warning if parameter checking is disabled. */
    SSP_PARAMETER_NOT_USED(p_ctrl);
    SSP_PARAMETER_NOT_USED(g_flash_api_open);

    ssp_err_t err = SSP_SUCCESS;
#if (FLASH_CFG_PARAM_CHECKING_ENABLE)
    SSP_ASSERT(NULL != p_cfg);
    SSP_ASSERT(NULL != p_ctrl);
#endif

    /* Take the hardware lock for the Flash */
    FLASH_ERROR_RETURN(!(SSP_SUCCESS != R_BSP_HardwareLock(BSP_HW_LOCK_FACI)), SSP_ERR_HW_LOCKED);

    /** Allow Initialization if not initialized or if no operation is ongoing and re-initialization is desired */
    if ((FLASH_STATE_UNINITIALIZED == g_flash_state) || (FLASH_STATE_READY == g_flash_state))
    {
        /** Check that API is not already Open */
        FLASH_ERROR_RETURN(!(SSP_SUCCESS != flash_lock_state(FLASH_STATE_INITIALIZATION)), SSP_ERR_IN_USE);
    }

    /** Set the parameters struct based on the user supplied settings */
    g_current_parameters.bgo_enabled_df = p_cfg->data_flash_bgo;

    if (g_current_parameters.bgo_enabled_df == true)
    {
        /** Setup the Flash interrupt callback based on the caller's info. If the Flash interrupt is
         * not enabled in the BSP then this will return SSP_ERR_IRQ_BSP_DISABLED */
       err =  HW_FLASH_LP_irq_cfg((bool) (p_cfg->p_callback != NULL), p_cfg);
    }
    else
    {
        /** Make sure Flash interrupts are disabled, they are only used in BGO mode */
        HW_FLASH_LP_irq_cfg(false, p_cfg);
    }

    if (SSP_SUCCESS == err)
    {
        err = flash_setup();    /// Check FCLK, calculate timeout values
    }

    /** Release state so other operations can be performed. */
    flash_ReleaseState();

    /** If we failed to successfully open then return the hardware lock */
    if (SSP_SUCCESS != err)
    {
        /** Return the hardware lock for the Flash */
        R_BSP_HardwareUnlock(BSP_HW_LOCK_FACI);
    }
    else
    {
        g_flash_api_open = true;        /// API is now open
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief  Write to the specified Code or Data Flash memory area. Implements flash_api_t::write.
 *
 * @retval SSP_SUCCESS              Operation successful. If BGO is enabled this means the operation was started
 *                                  successfully.
 * @retval SSP_ERR_IN_USE       Flash is busy with an on-going operation.
 * @retval SSP_FLASH_ERR_FAILURE    Operation failed. Possibly destination address is under access window control.
 * @retval SSP_ERR_TIMEOUT    Timed out waiting for operation to complete.
 * @retval SSP_ERR_INVALID_SIZE      Number of bytes provided was not a multiple of the programming size or exceeded
 *                                  the maximum range.
 * @retval SSP_ERR_INVALID_ADDRESS    Invalid address was input or address not on programming boundary.
 * @retval SSP_ERR_ASSERTION        NULL provided for p_ctrl.
 * @retval SSP_ERR_NOT_OPEN         Flash API has not yet been opened.
 **********************************************************************************************************************/
ssp_err_t R_FLASH_LP_Write (flash_ctrl_t * const p_ctrl,
                            uint32_t const       src_address,
                            uint32_t             flash_address,
                            uint32_t const       num_bytes)
{
    /** Eliminate warning if parameter checking is disabled. */
    SSP_PARAMETER_NOT_USED(p_ctrl);

    ssp_err_t err = SSP_SUCCESS;

    /** Get the block information for this address */
    FLASH_ERROR_RETURN((flash_get_block_info(flash_address, &g_block_info)), SSP_ERR_INVALID_ADDRESS);

#if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1)
    SSP_ASSERT(NULL != p_ctrl);

    /** Ensure num_bytes > 0 */
    FLASH_ERROR_RETURN((0 != num_bytes), SSP_ERR_INVALID_SIZE);

    /** Ensure API has been opened and no operation currently in progress */
    err = R_FLASH_LP_StatusGet(p_ctrl);
    if (SSP_ERR_IN_USE == err)
    {
        FLASH_ERROR_RETURN(0, SSP_ERR_IN_USE);     ///< There's a Flash operation already in progress
    }
    else
    {
        if (SSP_ERR_NOT_OPEN == err)
        {
            FLASH_ERROR_RETURN(0, SSP_ERR_NOT_OPEN);   ///< The Flash API is not open
        }
        else
        {
            /* Do nothing */
        }
    }

    /** Is this a request to Write Code Flash? */
    if (g_block_info.is_code_flash_addr == true)
    {
        /** Ensure start address is valid and on a programming boundary */
        if (((int32_t)flash_address >= FLASH_CF_BLOCK_0) &&  (flash_address <= FLASH_CF_BLOCK_END) && (!(flash_address & (FLASH_MIN_PGM_SIZE_CF - 1))))
        {
            /** Check if there is enough space in the destination, and num_bytes is a multiple of programming size */
            if ((((num_bytes - 1) + flash_address) > (uint32_t) FLASH_CF_BLOCK_END) || (num_bytes & (FLASH_MIN_PGM_SIZE_CF - 1)))

            {
                FLASH_ERROR_RETURN(0, SSP_ERR_INVALID_SIZE);
            }
        }
        else
        {
            FLASH_ERROR_RETURN(0, SSP_ERR_INVALID_ADDRESS);
        }
    }
    else
    {
        /** This is a request to write Data Flash */
        /** Ensure start address is valid */
        /** Check address validity and that it is on a Data Flash programming boundary */
        if ((flash_address >= (uint32_t)FLASH_DF_BLOCK_0) && (flash_address <= (uint32_t)FLASH_DF_BLOCK_END) &&
            (!(flash_address & (uint32_t)(FLASH_MIN_PGM_SIZE_DF - 1))))
        {
            /** Check if there is enough space in the destination, and num_bytes is a multiple of programming size */
            if ((((num_bytes - 1) + flash_address) > (uint32_t)FLASH_DF_BLOCK_END) || (num_bytes & (uint32_t)(FLASH_MIN_PGM_SIZE_DF - 1)))
            {
                FLASH_ERROR_RETURN(0, SSP_ERR_INVALID_SIZE);
            }
        }
        else
        {
            FLASH_ERROR_RETURN(0, SSP_ERR_INVALID_ADDRESS);
        }
    }
#endif /* if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1) */

    /** Update Flash state and enter the respective Code or Data Flash P/E mode */
    if (g_block_info.is_code_flash_addr == true)
    {
        err = setup_for_pe_mode(FLASH_TYPE_CODE_FLASH, FLASH_STATE_WRITING);
    }
    else
    {
        err = setup_for_pe_mode(FLASH_TYPE_DATA_FLASH, FLASH_STATE_WRITING);
    }

    /* Still good to go? */
    if (SSP_SUCCESS == err)
    {
        /* Is this a request to Erase Code Flash? */
        if (g_block_info.is_code_flash_addr == true)
        {
            g_current_parameters.wait_cnt          = g_current_parameters.wait_max_write_cf;
            g_current_parameters.current_operation = FLASH_OPERATION_CF_WRITE;
            g_current_parameters.min_pgm_size      = (FLASH_MIN_PGM_SIZE_CF >> 1);
        }
        else
        {
            /* This is a request to erase Data Flash */
            g_current_parameters.wait_cnt = g_current_parameters.wait_max_write_df;
            g_current_parameters.min_pgm_size = (uint16_t)FLASH_MIN_PGM_SIZE_DF;  ///< For MF3 DF it's always 1


            if (g_current_parameters.bgo_enabled_df == false)
            {
                g_current_parameters.current_operation = FLASH_OPERATION_DF_WRITE;
            }
            else
            {
                g_current_parameters.current_operation = FLASH_OPERATION_DF_BGO_WRITE;
            }
        }

        /** Write the data */
        err = HW_FLASH_LP_write(src_address, flash_address, num_bytes);
        if (SSP_SUCCESS == err)
        {
            /** If in non-BGO mode, then current operation is completed. Exit PE mode and return status */
            if (g_current_parameters.current_operation != FLASH_OPERATION_DF_BGO_WRITE)
            {
                err = HW_FLASH_LP_pe_mode_exit();
            }
        }
    }

    if (SSP_SUCCESS != err)
    {
        /** If there is an error, then reset the Flash sequencer: This will clear error flags and exit the P/E mode */
        HW_FLASH_LP_reset();
    }

    if (g_current_parameters.current_operation != FLASH_OPERATION_DF_BGO_WRITE)
    {
        flash_ReleaseState();
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief  Read the requested number of bytes from the supplied Data or Code Flash memory address.
 *         Implements flash_api_t::read.
 *         @note This function is provided simply for the purposes of maintaining a complete interface.
 *         It is possible (and recommended), to read Flash memory directly.
 *
 * @retval SSP_SUCCESS              Operation successful.
 * @retval SSP_ERR_INVALID_ADDRESS    Invalid Flash address was supplied.
 * @retval SSP_ERR_ASSERTION        NULL provided for p_ctrl or p_dest_address
 * @retval SSP_ERR_NOT_OPEN         Flash API has not yet been opened.
 **********************************************************************************************************************/
ssp_err_t R_FLASH_LP_Read  (flash_ctrl_t * const p_ctrl, uint8_t * p_dest_address, uint32_t const flash_address,
                            uint32_t const num_bytes)
{
    /** Eliminate warning if parameter checking is disabled. */
    SSP_PARAMETER_NOT_USED(p_ctrl);

    ssp_err_t err           = SSP_SUCCESS;
    uint32_t  index;
    uint8_t   * p_flash_ptr = (uint8_t *) flash_address;

#if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1)
    SSP_ASSERT(NULL != p_dest_address);
    SSP_ASSERT(NULL != p_ctrl);

    /** Ensure num_bytes > 0 */
    FLASH_ERROR_RETURN((0 != num_bytes), SSP_ERR_INVALID_SIZE);

    /** Ensure API has been opened */
    FLASH_ERROR_RETURN(!(true != g_flash_api_open), SSP_ERR_NOT_OPEN);

    FLASH_ERROR_RETURN(!(((int32_t)flash_address < FLASH_CF_BLOCK_0) || ((int32_t)flash_address > (int32_t)FLASH_DF_BLOCK_END)), SSP_ERR_INVALID_ADDRESS);
#endif /* if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1) */

    for (index = (uint32_t)0; index < num_bytes; index++)
    {
        p_dest_address[index] = p_flash_ptr[index];
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief  Erase the specified Code or Data Flash blocks. Implements flash_api_t::erase.
 * @retval SSP_SUCCESS                 Successful open.
 * @retval SSP_ERR_INVALID_BLOCKS        Invalid number of blocks specified
 * @retval SSP_ERR_INVALID_ADDRESS       Invalid address specified
 * @retval SSP_ERR_IN_USE          Flash is busy with an on-going operation.
 * @retval SSP_ERR_WRITE_FAILED   Erase failed. Possibly destination address is under access window control or
 *                                     Flash may be locked (FSPR bit).
 * @retval SSP_ERR_ASSERTION           NULL provided for p_ctrl
 * @retval SSP_ERR_NOT_OPEN            Flash API has not yet been opened.
 **********************************************************************************************************************/
ssp_err_t R_FLASH_LP_Erase (flash_ctrl_t * const p_ctrl, uint32_t const address, uint32_t const num_blocks)
{
    /** Eliminate warning if parameter checking is disabled. */
    SSP_PARAMETER_NOT_USED(p_ctrl);

    ssp_err_t err = SSP_SUCCESS;

    /** Get the block information for this address */
    FLASH_ERROR_RETURN((flash_get_block_info(address, &g_block_info)), SSP_ERR_INVALID_ADDRESS);

#if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1)
    SSP_ASSERT(NULL != p_ctrl);

    /** Ensure API has been opened and no operation currently in progress */
    err = R_FLASH_LP_StatusGet(p_ctrl);
    if (SSP_ERR_IN_USE == err)
    {
        FLASH_ERROR_RETURN(0, SSP_ERR_IN_USE);     ///< There's a Flash operation already in progress
    }
    else
    if (SSP_ERR_NOT_OPEN == err)
    {
        FLASH_ERROR_RETURN(0, SSP_ERR_NOT_OPEN);   ///< The Flash API is not open
    }

    /** Is this a request to Erase Code Flash? */
    if (g_block_info.is_code_flash_addr == true)
    {
        if ((num_blocks > FLASH_NUM_BLOCKS_CF) ||
            (num_blocks > (FLASH_NUM_BLOCKS_CF - g_block_info.this_block_number)) ||
            (num_blocks <= (uint32_t)0))
        {
            FLASH_ERROR_RETURN(0, SSP_ERR_INVALID_BLOCKS);
        }
    }
    else
    {
        /** This is a request to erase Data Flash */
        if ((num_blocks > FLASH_NUM_BLOCKS_DF) ||
            (num_blocks > (FLASH_NUM_BLOCKS_DF - g_block_info.this_block_number)) ||
            (num_blocks <= (uint32_t)0))
        {
            FLASH_ERROR_RETURN(0, SSP_ERR_INVALID_BLOCKS);
        }
    }
#endif /* if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1) */

    /** Update Flash state and enter the respective Code or Data Flash P/E mode */
    if (g_block_info.is_code_flash_addr == true)
    {
        err = setup_for_pe_mode(FLASH_TYPE_CODE_FLASH, FLASH_STATE_ERASING);
    }
    else
    {
        err = setup_for_pe_mode(FLASH_TYPE_DATA_FLASH, FLASH_STATE_ERASING);
    }

    /** Still good to go? */
    if (SSP_SUCCESS == err)
    {
        /** Is this a request to Erase Code Flash? */
        if (g_block_info.is_code_flash_addr == true)
        {
            g_current_parameters.wait_cnt          = g_current_parameters.wait_max_erase_cf_large_block;
            g_current_parameters.current_operation = FLASH_OPERATION_CF_ERASE;
        }
        else
        {
            g_current_parameters.wait_cnt = g_current_parameters.wait_max_erase_df_block;
            /** This is a request to erase Data Flash */
            if (g_current_parameters.bgo_enabled_df == false)
            {
                g_current_parameters.current_operation = FLASH_OPERATION_DF_ERASE;
            }
            else
            {
                g_current_parameters.current_operation = FLASH_OPERATION_DF_BGO_ERASE;
            }
        }

        /** Erase the Blocks, give this function the details about this block */
        err = HW_FLASH_LP_erase(g_block_info.this_block_st_addr, num_blocks);
        if (SSP_SUCCESS == err)
        {
            /** If in non-BGO mode, the current operation is complete. Exit PE mode and return status*/
            if (g_current_parameters.current_operation != FLASH_OPERATION_DF_BGO_ERASE)
            {
                err = HW_FLASH_LP_pe_mode_exit();
            }
        }
    }

    if (SSP_SUCCESS != err)
    {
        /** If there is an error, then reset the Flash sequencer: This will clear error flags and exit P/E mode */
        HW_FLASH_LP_reset();
    }

    if (g_current_parameters.current_operation != FLASH_OPERATION_DF_BGO_ERASE)
    {
        flash_ReleaseState();
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief  Perform a blank check on the specified address area. Implements flash_api_t::blankCheck. The
 *         number of bytes for Data Flash blank checking must be between (1 and FLASH_DATA_BLANK_CHECK_MAX).
 *         The number of bytes for Code Flash blank checking must be between (1 and FLASH_CODE_BLANK_CHECK_MAX).
 *         Flash respectively.
 * @retval SSP_SUCCESS              Blankcheck operation completed with result in p_blank_check_result,
 *                                  or blankcheck started and in-progess (BGO mode).
 * @retval SSP_ERR_INVALID_ADDRESS    Invalid data flash address was input
 * @retval SSP_ERR_INVALID_SIZE      'num_bytes' was either too large or not aligned for the CF/DF boundary size.
 * @retval SSP_ERR_IN_USE       Flash is busy with an on-going operation.
 * @retval SSP_ERR_ASSERTION        NULL provided for p_ctrl
 * @retval SSP_ERR_NOT_OPEN         Flash API has not yet been opened.
 **********************************************************************************************************************/
ssp_err_t R_FLASH_LP_BlankCheck (flash_ctrl_t * const p_ctrl, uint32_t const address, uint32_t num_bytes,
                                 flash_result_t * p_blank_check_result)
{
    /** Eliminate warning if parameter checking is disabled. */
    SSP_PARAMETER_NOT_USED(p_ctrl);

    ssp_err_t err         = SSP_SUCCESS;

    /** Get the block information for this address */
    FLASH_ERROR_RETURN((flash_get_block_info(address, &g_block_info)), SSP_ERR_INVALID_ADDRESS);

#if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1)
    SSP_ASSERT(NULL != p_ctrl);

    /** Ensure API has been opened and no operation currently in progress */
    err = R_FLASH_LP_StatusGet(p_ctrl);
    if (SSP_ERR_IN_USE == err)
    {
        FLASH_ERROR_RETURN(0, SSP_ERR_IN_USE);     ///< There's a Flash operation already in progress
    }
    else
    if (SSP_ERR_NOT_OPEN == err)
    {
        FLASH_ERROR_RETURN(0, SSP_ERR_NOT_OPEN);   ///< The Flash API is not open
    }


    /** Is this a request to Blank Check Code Flash? */
    if (g_block_info.is_code_flash_addr == true)
    {
        if ((((int32_t)address >= FLASH_CF_BLOCK_0) && ((int32_t)address <= (int32_t)FLASH_CF_BLOCK_END)))
        {
            /** Check if the range is valid, num_bytes is larger than 0 and up to and including the last CF byte */
            if (((num_bytes + address) > (uint32_t)(FLASH_CF_BLOCK_END + 1)) ||  (num_bytes == (uint32_t)0))
            {
                FLASH_ERROR_RETURN(0, SSP_ERR_INVALID_SIZE);
            }
        }
        else
        {
            FLASH_ERROR_RETURN(0, SSP_ERR_INVALID_ADDRESS);
        }
    }
    else
    {
        if (((address >= FLASH_DF_BLOCK_0) && (address <= FLASH_DF_BLOCK_END)))
        {
            /** Check if the range is valid, num_bytes is a multiple of the programming size, */
            /** is larger than 0 and less than FLASH_DATA_BLANK_CHECK_MAX and up to and including the last DF byte*/
            if (
                ((num_bytes + address) > (int32_t)(FLASH_DF_BLOCK_END + 1)) ||
                (num_bytes & (FLASH_MIN_PGM_SIZE_DF - 1)) ||
                (num_bytes == (uint32_t)0) ||
                (num_bytes > FLASH_DATA_BLANK_CHECK_MAX)
                )
            {
                FLASH_ERROR_RETURN(0, SSP_ERR_INVALID_SIZE);
            }
        }
        else
        {
            FLASH_ERROR_RETURN(0, SSP_ERR_INVALID_ADDRESS);
        }
    }
#endif /* if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1) */

    /** Update Flash state and enter the respective Code or Data Flash P/E mode */
    if (g_block_info.is_code_flash_addr == true)
    {
        err = setup_for_pe_mode(FLASH_TYPE_CODE_FLASH, FLASH_STATE_BLANK_CHECK);
    }
    else
    {
        err = setup_for_pe_mode(FLASH_TYPE_DATA_FLASH, FLASH_STATE_BLANK_CHECK);
    }

    /** Still good to go? */
    if (SSP_SUCCESS == err)
    {
        /** Is this a request to Blank check Code Flash? */
        if (g_block_info.is_code_flash_addr == true)
        {
            /* This is a request to Blank check Code Flash */
            /*No errors in parameters. Enter Code Flash PE mode*/
            /* wait_max_blank_check specifies the wait time for a 4 ROM byte blank check.
             * num_bytes is divided by 4 and then multiplied to calculate the wait time for the entire operation */
            if (num_bytes < (uint32_t)4)
            {
                g_current_parameters.wait_cnt = g_current_parameters.wait_max_blank_check * num_bytes;
            }
            else
            {
                g_current_parameters.wait_cnt = (g_current_parameters.wait_max_blank_check * (num_bytes >> 2));
            }
            g_current_parameters.current_operation = FLASH_OPERATION_CF_BLANKCHECK;
        }
        else
        {
            /* This is a request to Blank check Data Flash */
            /* No errors in parameters. Enter Data Flash PE mode*/
            /* wait_max_blank_check specifies the wait time for a 4 ROM byte blank check. This is the same as the
             * wait time for a 1 byte Data Flash blankcheck.*/
            g_current_parameters.wait_cnt = g_current_parameters.wait_max_blank_check * num_bytes;
            if (g_current_parameters.bgo_enabled_df == false)
            {
                g_current_parameters.current_operation = FLASH_OPERATION_DF_BLANKCHECK;
            }
            else
            {
                g_current_parameters.current_operation = FLASH_OPERATION_DF_BGO_BLANKCHECK;
            }
        }

        /* Start the Blank check operation. If BGO is enabled then the result of the Blank check will be
         * available when the interrupt occurs and p_blank_check_result will contain FLASH_RESULT_BGO_ACTIVE */
        err = HW_FLASH_LP_blankcheck(address, num_bytes, p_blank_check_result);

        if (SSP_SUCCESS == err)
        {
            /** If in non-BGO mode, then current operation is complete. Exit PE mode and return status */
            if (g_current_parameters.current_operation != FLASH_OPERATION_DF_BGO_BLANKCHECK)
            {
                err                                    = HW_FLASH_LP_pe_mode_exit();
                g_current_parameters.current_operation = FLASH_OPERATION_IDLE;
            }
        }
    }

    if (SSP_SUCCESS != err)
    {
        /** If there is an error, then reset the FCU: This will clear error flags and exit the P/E mode */
        HW_FLASH_LP_reset();
    }

    if (g_current_parameters.current_operation != FLASH_OPERATION_DF_BGO_BLANKCHECK)
    {
        flash_ReleaseState();
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief  Query the FLASH for its status. Implements flash_api_t::statusGet.
 *
 * @retval SSP_SUCCESS              Flash is ready and available to accept commands.
 * @retval SSP_ERR_IN_USE       Flash is busy with an on-going operation.
 * @retval SSP_ERR_ASSERTION        NULL provided for p_ctrl
 * @retval SSP_ERR_NOT_OPEN         Flash API has not yet been opened.
 **********************************************************************************************************************/
ssp_err_t R_FLASH_LP_StatusGet (flash_ctrl_t * const p_ctrl)
{
    /** Eliminate warning if parameter checking is disabled. */
    SSP_PARAMETER_NOT_USED(p_ctrl);

#if (FLASH_CFG_PARAM_CHECKING_ENABLE == 1)
    SSP_ASSERT(NULL != p_ctrl);

    /** Ensure API has been opened */
    FLASH_ERROR_RETURN(!(true != g_flash_api_open), SSP_ERR_NOT_OPEN);
#endif

    /** Return flash status */
    if (g_flash_state == FLASH_STATE_READY)
    {
        return SSP_SUCCESS;
    }
    else
    {
        return SSP_ERR_IN_USE;
    }

}

/*******************************************************************************************************************//**
 * @brief  Configure an access window for the Code Flash memory using the provided start and end address. An access
 *         window defines a contiguous area in Code Flash for which programming/erase is enabled. This area is
 *         on block boundaries.
 *         The block containing start_addr is the first block. The block containing end_addr is the last block.
 *         The access window then becomes first block --> last block inclusive. Anything outside this range
 *         of Code Flash is then write protected.
 *         @note If the start address and end address are set to the same value, then the access window
 *         is effectively removed. This accomplishes the same functionality as R_FLASH_LP_AccessWindowClear().
 *
 *         Implements flash_api_t::accessWindowSet.
 *
 * @retval SSP_SUCCESS              Access window successfully configured.
 * @retval SSP_ERR_INVALID_ADDRESS    Invalid settings for start_addr and/or end_addr.
 * @retval SSP_ERR_IN_USE       Flash is busy with an on-going operation.
 * @retval SSP_ERR_ASSERTION        NULL provided for p_ctrl
 * @retval SSP_ERR_WRITE_FAILED   Operation failed. Flash may be locked (FSPR bit).
 * @retval SSP_ERR_NOT_OPEN         Flash API has not yet been opened.
 **********************************************************************************************************************/
ssp_err_t R_FLASH_LP_AccessWindowSet (flash_ctrl_t * const p_ctrl, uint32_t const start_addr,
                                      uint32_t const end_addr)
{
    /** Eliminate warning if parameter checking is disabled. */
    SSP_PARAMETER_NOT_USED(p_ctrl);

    ssp_err_t err = SSP_SUCCESS;

#if (FLASH_CFG_PARAM_CHECKING_ENABLE)
    SSP_ASSERT(NULL != p_ctrl);

    /** Ensure API has been opened and no operation currently in progress */
    err = R_FLASH_LP_StatusGet(p_ctrl);
    if (SSP_ERR_IN_USE == err)
    {
        FLASH_ERROR_RETURN(0, SSP_ERR_IN_USE);     ///< There's a Flash operation already in progress
    }
    else
    if (SSP_ERR_NOT_OPEN == err)
    {
        FLASH_ERROR_RETURN(0, SSP_ERR_NOT_OPEN);   ///< The Flash API is not open
    }
#endif

    /** Note that as the end_addr indicates the address up to but not including the block that contains that address.
     * Therefore in order */
    /** to allow the very last Block to be included in the access window we must allow for FLASH_CF_BLOCK_END+1 */
    if (((int32_t)start_addr >= FLASH_CF_BLOCK_0) && (end_addr <= (uint32_t)(FLASH_CF_BLOCK_END + 1)) && (start_addr <= end_addr))
    {
        /** Update Flash state and enter Code Flash P/E mode */
        err = setup_for_pe_mode(FLASH_TYPE_CODE_FLASH, FLASH_STATE_ACCESS_WINDOW);

        /* Still good to go? */
        if (SSP_SUCCESS == err)
        {
            err = HW_FLASH_LP_access_window_set(start_addr, end_addr);

            if (SSP_SUCCESS == err)
            {
                /** Return to read mode */
                err = HW_FLASH_LP_pe_mode_exit();
            }
        }

        if (SSP_SUCCESS != err)
        {
            /*If there is an error, then reset the Flash. This will clear error flags and exit the P/E mode */
            HW_FLASH_LP_reset();
        }

        flash_ReleaseState();
    }
    else
    {
        err = SSP_ERR_INVALID_ADDRESS;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief  Remove any access window that is currently configured in the Code Flash.
 *         Subsequent to this call all Code Flash is writable.
 *         Implements flash_api_t::accessWindowClear.
 *
 * @retval SSP_SUCCESS                 Access window successfully removed.
 * @retval SSP_ERR_IN_USE          Flash is busy with an on-going operation.
 * @retval SSP_ERR_ASSERTION           NULL provided for p_ctrl
 * @retval SSP_ERR_WRITE_FAILED   Operation failed. Flash may be locked (FSPR bit).
 * @retval SSP_ERR_NOT_OPEN            Flash API has not yet been opened.
 **********************************************************************************************************************/
ssp_err_t R_FLASH_LP_AccessWindowClear (flash_ctrl_t * const p_ctrl)
{
    /** Eliminate warning if parameter checking is disabled. */
    SSP_PARAMETER_NOT_USED(p_ctrl);

    ssp_err_t err = SSP_SUCCESS;

#if (FLASH_CFG_PARAM_CHECKING_ENABLE)
    SSP_ASSERT(NULL != p_ctrl);

    /** Ensure API has been opened and no operation currently in progress */
    err = R_FLASH_LP_StatusGet(p_ctrl);
    if (SSP_ERR_IN_USE == err)
    {
        FLASH_ERROR_RETURN(0, SSP_ERR_IN_USE);     ///< There's a Flash operation already in progress
    }
    else
    if (SSP_ERR_NOT_OPEN == err)
    {
        FLASH_ERROR_RETURN(0, SSP_ERR_NOT_OPEN);   ///< The Flash API is not open
    }
#endif

    /** Update Flash state and enter Code Flash P/E mode */
    err = setup_for_pe_mode(FLASH_TYPE_CODE_FLASH, FLASH_STATE_ACCESS_WINDOW);

    /** Still good to go? */
    if (SSP_SUCCESS == err)
    {
        err = HW_FLASH_LP_access_window_clear();

        if (SSP_SUCCESS == err)
        {
            /** Return to read mode */
            err = HW_FLASH_LP_pe_mode_exit();
        }
    }

    if (SSP_SUCCESS != err)
    {
        /** If there is an error, then reset the Flash. This will clear error flags and exit the P/E mode */
        HW_FLASH_LP_reset();
    }

    flash_ReleaseState();

    return err;
}

/*******************************************************************************************************************//**
 * @brief  Reset the FLASH peripheral. Implements flash_api_t::reset.
 * No attempt is made to grab the Flash software lock before executing the reset since the assumption is that a reset
 * will terminate any existing operation.
 *
 * @retval SSP_SUCCESS        Flash circuit successfully reset.
 * @retval SSP_ERR_ASSERTION  NULL provided for p_ctrl
 * @retval SSP_ERR_NOT_OPEN   Flash API has not yet been opened.
 **********************************************************************************************************************/
ssp_err_t R_FLASH_LP_Reset (flash_ctrl_t * const p_ctrl)
{
    /** Eliminate warning if parameter checking is disabled. */
    SSP_PARAMETER_NOT_USED(p_ctrl);

    ssp_err_t err = SSP_SUCCESS;

#if (FLASH_CFG_PARAM_CHECKING_ENABLE)
    SSP_ASSERT(NULL != p_ctrl);

    /** Ensure API has been opened */
    FLASH_ERROR_RETURN(!(true != g_flash_api_open), SSP_ERR_NOT_OPEN);
#endif

    HW_FLASH_LP_reset();
    flash_ReleaseState();

    return err;
}

/******************************************************************************************************************//**
 * @brief  Select which block - Default (Block 0) or Alternate (Block 1) is used as the startup area block.
 *         The provided parameters determine which block will become the active startup block and whether that
 *         action will be immediate (but temporary), or permanent subsequent to the next reset.
 *         Doing a temporary switch might appear to have limited usefulness. If there is an access window
 *         in place such that Block 0 is write protected, then one could do a temporary switch, update the
 *         block and switch them back without having to touch the access window.
 *         Implements flash_api_t::startupAreaSelect.
 *
 * @retval SSP_SUCCESS              Start-up area successfully toggled.
 * @retval SSP_ERR_HW_LOCKED        FLASH peripheral has already been initialized and is in use.
 * @retval SSP_ERR_IN_USE       Flash is busy with an on-going operation.
 * @retval SSP_ERR_ASSERTION        NULL provided for p_ctrl
 * @retval SSP_ERR_NOT_OPEN         Flash API has not yet been opened.
 **********************************************************************************************************************/
ssp_err_t R_FLASH_LP_StartUpAreaSelect (flash_ctrl_t * const      p_ctrl,
                                        flash_startup_area_swap_t swap_type,
                                        bool                      is_temporary)
{
    /** Eliminate warning if parameter checking is disabled. */
    SSP_PARAMETER_NOT_USED(p_ctrl);

    ssp_err_t err = SSP_SUCCESS;

#if (FLASH_CFG_PARAM_CHECKING_ENABLE)
    SSP_ASSERT(NULL != p_ctrl);

    /** Ensure API has been opened and no operation currently in progress */
    err = R_FLASH_LP_StatusGet(p_ctrl);
    if (SSP_ERR_IN_USE == err)
    {
        FLASH_ERROR_RETURN(0, SSP_ERR_IN_USE);     ///< There's a Flash operation already in progress
    }
    else
    if (SSP_ERR_NOT_OPEN == err)
    {
        FLASH_ERROR_RETURN(0, SSP_ERR_NOT_OPEN);   ///< The Flash API is not open
    }
#endif

    /** This is already the normal reset behavior. There's nothing to do */
    if ((swap_type == FLASH_STARTUP_AREA_BTFLG) && (is_temporary == false))
    {
        return err;
    }

    /** Update Flash state and enter Code Flash P/E mode */
    err = setup_for_pe_mode(FLASH_TYPE_CODE_FLASH, FLASH_STATE_STARTUP_AREA);
    if (err == SSP_SUCCESS)
    {
        if (is_temporary == true)
        {
            err = HW_FLASH_LP_set_startup_area_temporary(swap_type);
        }
        else
        {
            err = HW_FLASH_LP_set_startup_area_boot(swap_type);
        }
    }

    /** Success? */
    if (SSP_SUCCESS == err)
    {
        /** Return to read mode */
        err = HW_FLASH_LP_pe_mode_exit();
    }
    else
    {
        /** If there is an error, then reset the Flash. This will clear error flags and exit the P/E mode */
        HW_FLASH_LP_reset();
    }

    flash_ReleaseState();
    return err;
}


/******************************************************************************************************************//**
 * @brief  Indicate to the already open Flash API, that the FCLK has changed since the Open(). This could be the case if
 *         the application has changed the system clock, and therefore the FCLK. Failure to call this function subsequent
 *         to changing the FCLK could result in damage to the flash macro. This function uses R_CGC_SystemClockFreqGet()
 *         to get the current FCLK frequency.
 *         Implements r_flash_t::updateFlashClockFreq.
 *
 * @retval SSP_SUCCESS              Start-up area successfully toggled.
 * @retval SSP_ERR_IN_USE           Flash is busy with an on-going operation.
 * @retval SSP_ERR_ASSERTION        NULL provided for p_ctrl
 * @retval SSP_ERR_NOT_OPEN         Flash API has not yet been opened.
 **********************************************************************************************************************/
ssp_err_t R_FLASH_LP_UpdateFlashClockFreq (flash_ctrl_t * const  p_ctrl)
{
    /** Eliminate warning if parameter checking is disabled. */
    SSP_PARAMETER_NOT_USED(p_ctrl);

    ssp_err_t err = SSP_SUCCESS;

#if (FLASH_CFG_PARAM_CHECKING_ENABLE)
    SSP_ASSERT(NULL != p_ctrl);

    /** Ensure API has been opened and no operation currently in progress */
    err = R_FLASH_LP_StatusGet(p_ctrl);
    if (SSP_ERR_IN_USE == err)
    {
        FLASH_ERROR_RETURN(0, SSP_ERR_IN_USE);     ///< There's a Flash operation already in progress
    }
    else
    if (SSP_ERR_NOT_OPEN == err)
    {
        FLASH_ERROR_RETURN(0, SSP_ERR_NOT_OPEN);   ///< The Flash API is not open
    }
#endif

    /** Grab the current flash state*/
    if (SSP_SUCCESS != flash_lock_state(FLASH_UPDATE_FCLK))
    {
        /** API already in this state*/
        err = SSP_ERR_IN_USE;
    }

    err = flash_setup();    ///< Check FCLK, calculate timeout values

    flash_ReleaseState();

    return err;
}
/*******************************************************************************************************************//**
 * @brief      Release any resources that were allocated by the Open() or any subsequent Flash operations. Implements
 *             flash_api_t::close.
 *
 * @retval     SSP_SUCCESS       Successful close.
 * @retval SSP_ERR_ASSERTION     NULL provided for p_ctrl or p_cfg.
 **********************************************************************************************************************/
ssp_err_t R_FLASH_LP_Close (flash_ctrl_t * const p_ctrl)
{
    /** Eliminate warning if parameter checking is disabled. */
    SSP_PARAMETER_NOT_USED(p_ctrl);

    ssp_err_t err = SSP_SUCCESS;

#if FLASH_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
#endif
    /** Return the hardware lock for the Flash */
    R_BSP_HardwareUnlock(BSP_HW_LOCK_FACI);

    HW_FLASH_LP_irq_cfg(false, NULL);

    g_flash_api_open = false;        /// API is now closed

    /** Release the lock */
    flash_ReleaseState();

    return err;
}

/*******************************************************************************************************************//**
 * @brief   Get FLASH HAL driver version.
 * @retval  SSP_SUCCESS - operation performed successfully
 * @note This function is reentrant.
 **********************************************************************************************************************/
ssp_err_t R_FLASH_LP_VersionGet (ssp_version_t * const p_version)
{
#if FLASH_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_version);
#endif

    p_version->version_id = g_flash_lp_version.version_id;
    return SSP_SUCCESS;
}  /* End of function R_FLASH_LP_VersionGet() */

/*******************************************************************************************************************//**
 * @} (end addtogroup FLASH)
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief   This function attempts to change the flash state to the new requested state.
 *          This can only happen if we are able to take the FLASH lock. If the lock is currently in use then
 *          we will return FLASH_ERR_BUSY, otherwise we will take the lock and change the state.
 * @param[in]  state        Which state to attempt to transition to.
 * @retval SSP_SUCCESS             Successful.
 * @retval SSP_ERR_IN_USE      Flash is busy with an on-going operation.
 **********************************************************************************************************************/
static ssp_err_t flash_lock_state (flash_states_t new_state)
{
    /** Check to see if lock was successfully taken */
    if (R_BSP_SoftwareLock(&g_flash_Lock) == SSP_SUCCESS)
    {
        /** Lock taken, we can change state */
        g_flash_state = new_state;

        /** Return success */
        return SSP_SUCCESS;
    }
    else
    {
        /** Another operation is on-going */
        return SSP_ERR_IN_USE;
    }
}

/*******************************************************************************************************************//**
 * @brief   This function releases the flash state so another flash operation can take place.
 *          This function is called by both the HLD and LLD layers (interrupt routines).
 * @retval None
 **********************************************************************************************************************/
void flash_ReleaseState (void)
{
    /** Done with current operation */
    g_flash_state = FLASH_STATE_READY;

    /** Release hold on lock */
    R_BSP_SoftwareUnlock(&g_flash_Lock);
}

/*******************************************************************************************************************//**
 * @brief   This function places the flash in the requested Code or Data P/E mode.
 * @param[in]   flash_type          FLASH_TYPE_CODE_FLASH or FLASH_TYPE_DATA_FLASH.
 * @param[in]   p_ctrl              Desired Flash state to transition into (ie FLASH_STATE_WRITING).
 * @retval SSP_SUCCESS              Successful.
 * @retval SSP_ERR_IN_USE       Flash is busy with an on-going operation.
 **********************************************************************************************************************/
static ssp_err_t setup_for_pe_mode (flash_type_t flash_type, flash_states_t flash_state)
{
    ssp_err_t err = SSP_SUCCESS;

    /** Check if API is available*/
    if (g_flash_state != FLASH_STATE_READY)
    {
        /** API busy with an operation*/
        err = SSP_ERR_IN_USE;
    }

    /** Grab the current flash state*/
    if (SSP_SUCCESS != flash_lock_state(flash_state))
    {
        /** API already/still in this state*/
        err = SSP_ERR_IN_USE;
    }

    /** Still good to go? */
    if (SSP_SUCCESS == err)
    {
        /** Configuration Set command requires Data Flash P/E mode */
        err = HW_FLASH_LP_pe_mode_enter(flash_type);
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief   This function will validate the supplied address, and return information about the block in which
 *          it resides.
 * @param[in]   addr    Code or Data Flash address to return info for.
 * @param[out]  p_block_info  Pointer to a caller allocated flash_block_info_t structure which will be filled
 *                            in by this function.
 * @retval false    Supplied address is neither a valid Code or Data Flash address for this part.
 * @retval true     Supplied address is valid and p_block_infocontains the details on this addresses block
 **********************************************************************************************************************/
bool flash_get_block_info (uint32_t addr, flash_block_info_t * p_block_info)
{
    uint16_t                 num_sections = 0;
    uint32_t                 bnum;
    uint32_t                 bcount       = 0;
    uint16_t                 num_tbl_entries;
    const flash_block_info_t * pBInfo;

    /** Determine if this is a Code or Data Flash address, or something else (invalid) */
    if (((int32_t)addr >= FLASH_CF_BLOCK_0) && (addr <= FLASH_CF_BLOCK_END))
    {
        p_block_info->is_code_flash_addr = true;
        num_tbl_entries                  = NUM_BLOCK_ROM_TABLE_ENTRIES;
        pBInfo                           = &g_flash_RomBlockSizes[0];
    }
    else if ((addr >= FLASH_DF_BLOCK_0) && (addr <= FLASH_DF_BLOCK_END))
    {
        p_block_info->is_code_flash_addr = false;
        num_tbl_entries                  = NUM_BLOCK_DATA_TABLE_ENTRIES;
        pBInfo                           = &g_flash_DataBlockSizes[0];
    }
    else
    {
        return false;
    }

    while (num_sections < num_tbl_entries)
    {
        num_sections++;
        /// Is this address within this section of blocks?
        if ((addr >= pBInfo->block_section_st_addr)  && (addr <= pBInfo->block_section_end_addr))
        {
            bnum                                 = ((addr - pBInfo->block_section_st_addr) / pBInfo->block_size);

            p_block_info->this_block_st_addr     = pBInfo->block_section_st_addr + (bnum * pBInfo->block_size);
            p_block_info->this_block_end_addr    = (p_block_info->this_block_st_addr + pBInfo->block_size) - 1;
            p_block_info->this_block_number      = bnum + bcount;
            p_block_info->block_section_st_addr  = pBInfo->block_section_st_addr;
            p_block_info->block_section_end_addr = pBInfo->block_section_end_addr;
            p_block_info->block_size             = pBInfo->block_size;
            p_block_info->num_blocks             = pBInfo->num_blocks;
            break;
        }
        else
        {
            bcount += pBInfo->num_blocks;           /// add in # of blocks in this section
            pBInfo++;
        }
     }

    return true;
}

/*******************************************************************************************************************//**
 * @brief   This function verifies that FCLK falls within the allowable range and calculates the timeout values
 *          based on the current FCLK frequency.
 *
 * @SSP_SUCCESS                         Success
 * @retval SSP_ERR_FCLK           FCLK must be >= 4 MHz.
 * @retval SSP_ERR_INVALID_ARGUMENT     Invalid argument provided to CGC call. Should never occur...
 **********************************************************************************************************************/
static ssp_err_t flash_setup (void)
{
    ssp_err_t err;


    /** We need clock frequencies to calculate the worst case timeout values. */
    err = g_cgc_on_cgc.systemClockFreqGet(CGC_SYSTEM_CLOCKS_FCLK, &g_current_parameters.flash_clock_freq);

    if (SSP_SUCCESS == err)
    {
        /** FCLK must be a minimum of 4 MHz for Flash operations */
        if (g_current_parameters.flash_clock_freq < MINIMUM_SUPPORTED_FCLK_FREQ)
        {
            /** Release state so other operations can be performed. */
            err = SSP_ERR_FCLK;
        }
        else
        {
            err = g_cgc_on_cgc.systemClockFreqGet(CGC_SYSTEM_CLOCKS_ICLK, &g_current_parameters.system_clock_freq);
        }
    }

    if (SSP_SUCCESS == err)
    {
        /** Initialize the flash timeout calculations */
        HW_FLASH_LP_init(&g_current_parameters);
        HW_FLASH_LP_code_flash_set_flash_settings(&g_current_parameters);      /// Give HW layer below access to the configured settings
        HW_FLASH_LP_data_flash_set_flash_settings(&g_current_parameters);      /// Give HW layer below access to the configured settings

        /** Allow access to the DataFlash */
        HW_FLASH_LP_dataflash_enable();

    }

    return err;
}
#endif /* if defined(BSP_MCU_GROUP_S3A7) || defined(BSP_MCU_GROUP_S124) || defined(BSP_MCU_GROUP_S128) */


