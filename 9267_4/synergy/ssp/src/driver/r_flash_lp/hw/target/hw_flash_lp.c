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
 * File Name    : hw_flash_lp.c
 * Description  : LLD Interface for the Low power Flash peripheral on SC32 MCUs.
 **********************************************************************************************************************/


/*******************************************************************************************************************//**
 * @ingroup HAL_Library
 * @addtogroup FLASH_LP_API
 * @brief FLASH API
 *
 * This module supports the low level interface for the Low Power Flash peripheral.
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "bsp_api.h"

#if defined(BSP_MCU_GROUP_S124) || defined(BSP_MCU_GROUP_S3A7) || defined(BSP_MCU_GROUP_S128)
#include "r_flash_lp.h"
#include "hw_flash_lp_private.h"
#include "r_flash_cfg.h"
#include "r_cgc_api.h"
#include "r_cgc.h"
#include "flash_lp_core\hw_dataflash.h"
#include "flash_lp_core\hw_codeflash.h"
#include "flash_lp_core\hw_codeflash_extra.h"

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
static current_parameters_t * gp_flash_parameters; ///< passed in via flash_init()

#if (BSP_IRQ_CFG_FCU_FRDYI != BSP_IRQ_DISABLED)
static void (* flash_ready_isr_handler)(flash_callback_args_t *callback_ptr);    ///< Function pointer for Flash Ready ISR
#endif

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief   This function enables or disables the Flash interrupt. The flash interrupt, if enabled, is called when a
 *          Flash operation completes, or when an Error condition is detected. If the caller has provided a callback
 *          function as part of the provided p_cfg pointer, then that function will be called as a result of the
 *          interrupt.
 * @param[in]  state        Control for the FLASH device context.
 * @param[in]  p_cfg        Pointer to the FLASH configuration structure.
 * @retval SSP_SUCCESS              Successful.
 * @retval SSP_ERR_INVALID_PTR      p_cfg is null.
 * @retval SSP_ERR_IRQ_BSP_DISABLED Caller is requesting BGO but the Flash interrupts are not enabled.
 **********************************************************************************************************************/
ssp_err_t HW_FLASH_LP_irq_cfg (bool state, flash_cfg_t const * const p_cfg)
{
    SSP_PARAMETER_NOT_USED(state);          /// Prevent compiler 'unused' warning

    /** If BGO is being used then we require the Flash Rdy interrupt be enabled. If it
     *  is not enabled then we are not going to be doing BGO and we won't generate an ISR routine */
    #if (BSP_IRQ_CFG_FCU_FRDYI != BSP_IRQ_DISABLED)

    /** Enable the Interrupt */
    if (true == state)
    {
        /** Assign the callback if not NULL*/
        if (NULL == p_cfg->p_callback)
        {
            return SSP_ERR_INVALID_POINTER;
        }

        flash_ready_isr_handler = p_cfg->p_callback;

        /** Enable FCU interrupts if callback is not null, clear the Interrupt Request bit */
        NVIC_ClearPendingIRQ(FCU_FRDYI_IRQn);
        R_BSP_IrqStatusClear(FCU_FRDYI_IRQn);
        NVIC_EnableIRQ(FCU_FRDYI_IRQn);
    }
    /** Disable the Interrupts */
    else if (false == state)
    {
        /** Disable interrupt in ICU*/
        NVIC_DisableIRQ(FCU_FRDYI_IRQn);

        /** Clear the Interrupt Request bit */
        R_BSP_IrqStatusClear(FCU_FRDYI_IRQn);
    }

    return SSP_SUCCESS;
#else
   /** The Flash Interrupt has not been enabled. If the caller is requesting BGO then we'll flag this as an error. */
   if (p_cfg->data_flash_bgo == true)
       return SSP_ERR_IRQ_BSP_DISABLED;
   else
       return SSP_SUCCESS;
#endif
}

/*******************************************************************************************************************//**
 * @brief   This function will initialize the FCU, copy the FCU firmware to FCURAM and
 *         set the FCU Clock based on the current FCLK frequency.
 * @param[in, out]  p_current_parameters        Pointer to the g_current_parameters structure created by the HLD layer.
 **********************************************************************************************************************/
void HW_FLASH_LP_init (current_parameters_t * p_current_parameters)
{
    int32_t  expression;

    gp_flash_parameters                    = p_current_parameters; // our copy from now on

    gp_flash_parameters->current_operation = FLASH_OPERATION_FCU_INIT;

    gp_flash_parameters->flash_clock_freq  = gp_flash_parameters->flash_clock_freq / 1000000;  // Scale it down
    gp_flash_parameters->system_clock_freq = gp_flash_parameters->system_clock_freq / 1000000; // Scale it down

    /* Common expression for 60 MHz calculations */
    expression =
         (int32_t) ((60.0 / (gp_flash_parameters->flash_clock_freq)) * (gp_flash_parameters->system_clock_freq));

    /*  According to HW Manual the Max Programming Time for 256 bytes (ROM)
     *  is 6ms.  This is with a FCLK of 60MHz. The calculation below
     *  calculates the number of ICLK ticks needed for the timeout delay.
     *  The 6ms number is adjusted linearly depending on the FCLK frequency.
     */
    gp_flash_parameters->wait_max_write_cf =    (uint32_t) (6000 * expression);

    /*  According to HW Manual the Max Programming Time for 4 bytes
     *  (Data Flash) is 1.7ms.  This is with a FCLK of 60MHz. The calculation
     *  below calculates the number of ICLK ticks needed for the timeout delay.
     *  The 1.7ms number is adjusted linearly depending on the FCLK frequency.
     */
    gp_flash_parameters->wait_max_write_df =    (uint32_t) (1700 * expression);

    /*  According to HW Manual the Max Blank Check time for 4 bytes
     *  (Data Flash) is 30 usec.  This is with a FCLK of 60MHz. The calculation
     *  below calculates the number of ICLK ticks needed for the timeout delay.
     *  The 30 usec number is adjusted linearly depending on the FCLK frequency.
     */
    gp_flash_parameters->wait_max_blank_check = (uint32_t) (30 *   expression);

    /*  According to HW Manual the max timeout value when using the peripheral
     *  clock notification command is 60us. This is with a FCLK of 50MHz. The
     *  calculation below calculates the number of ICLK ticks needed for the
     *  timeout delay. The 60us number is adjusted linearly depending on
     *  the FCLK frequency.
     */
    gp_flash_parameters->wait_max_notify_fcu_clock =
        ((uint32_t) (60.0 *
                    (50.0 / (gp_flash_parameters->flash_clock_freq))) * (gp_flash_parameters->system_clock_freq));

    /*  According to HW Manual the Max Erasure Time for a 32KB block is
     *  around 480ms.  This is with a FCLK of 60MHz. The calculation below
     *  calculates the number of ICLK ticks needed for the timeout delay.
     *  The 480ms number is adjusted linearly depending on the FCLK frequency.
     */
    gp_flash_parameters->wait_max_erase_cf_large_block = (uint32_t)(480000 * expression);

    /*  According to HW Manual the Max Erasure Time for a 8KB block is
     *  around 120ms.  This is with a FCLK of 60MHz. The calculation below
     *  calculates the number of ICLK ticks needed for the timeout delay.
     *  The 120ms number is adjusted linearly depending on the FCLK frequency.
     */
    gp_flash_parameters->wait_max_erase_df_block = (uint32_t) (120000 * expression);

    /*  According to HW Manual the Max Erasure Time for a 64 byte Data Flash block is
     *  around 10ms.  This is with a FCLK of 60MHz. The calculation below
     *  calculates the number of ICLK ticks needed for the timeout delay.
     *  The 10ms number is adjusted linearly depending on the FCLK frequency.
     */
    gp_flash_parameters->wait_max_erase_cf_small_block = (uint32_t) (10000 * expression);

    /** Per the Flash spec, update the FLWAIT register if FCLK is being changed */
    R_FACI->FLWAITR = 0;
}

/*******************************************************************************************************************//**
 * @brief   This function erases a specified number of Code or Data Flash blocks
 * @param[in]   block_address   The starting address of the first block to erase.
 * @param[in]   num_blocks      The number of blocks to erase.
 * @retval SSP_SUCCESS            Successfully erased (non-BGO) mode or operation successfully started (BGO).
 * @retval SSP_ERR_ERASE_FAILED    Status is indicating a Erase error.
 * @retval SSP_ERR_CMD_LOCKED   FCU is in locked state, typically as a result of having received an illegal
 *                                    command.
 * @retval SSP_ERR_WRITE_FAILED  Status is indicating a Programming error for the requested operation.
 * @retval SSP_ERR_TIMEOUT Timed out waiting for the FCU to become ready.
 **********************************************************************************************************************/
ssp_err_t HW_FLASH_LP_erase (uint32_t block_address, uint32_t num_blocks)
{
    ssp_err_t err = SSP_SUCCESS;

    /** We're already in the appropriate P/E mode from the caller */

    if (R_FACI->FENTRYR == 0x0080U)
     {
        /** Write the actual registers for first block */
        HW_FLASH_LP_dataflash_erase(block_address, num_blocks);  ///< Set the start and end and do first block

        /** If configured for Blocking mode then don't return until the entire operation is complete */
        if (gp_flash_parameters->bgo_enabled_df == false)
        {
            do
            {
                err = HW_FLASH_LP_dataflash_erase_monitor();
            } while (SSP_ERR_IN_USE == err);
        }
    }

#if (FLASH_CFG_PARAM_CODE_FLASH_PROGRAMMING_ENABLE == 1)

    else if (R_FACI->FENTRYR == 0x0001U)
    {
        /** Write the actual registers for first block */
        HW_FLASH_LP_codeflash_erase(block_address, num_blocks);  // Set the start and end and do first block

        /** Code Flash is always blocking mode */
        do
        {
            err = HW_FLASH_LP_codeflash_erase_monitor();
        } while (SSP_ERR_IN_USE == err);
    }
#endif /* if (FLASH_CFG_PARAM_CODE_FLASH_PROGRAMMING_ENABLE == 1) */
    else
    {
        /** should never get here */
        return SSP_ERR_ERASE_FAILED;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief   This function writes a specified number of bytes to Code or Data Flash.
 * @param[in]   src_start_address   The starting address of the first byte (from source) to write.
 * @param[in]   dest_start_address  The starting address of the Flash (to destination) to write.
 * @param[in]   num_bytes           The number of bytes to write.
 *
 * @retval SSP_SUCCESS            Data successfully written (non-BGO) mode or operation successfully started (BGO).
 * @retval SSP_ERR_IN_USE     Command still executing.
 * @retval SSP_ERR_WRITE_FAILED  Status is indicating a Programming error for the requested operation. This
 *                                    may be returned if the requested Flash area is not blank.
 * @retval SSP_ERR_TIMEOUT  Timed out waiting for the Flash sequencer to become ready.
 * @retval SSP_ERR_PE_FAILURE  Unable to enter Programming mode. Flash may be locked (FSPR bit).
 **********************************************************************************************************************/
ssp_err_t HW_FLASH_LP_write (uint32_t const src_start_address, uint32_t dest_start_address, uint32_t num_bytes)
{
    ssp_err_t err      = SSP_SUCCESS;

    gp_flash_parameters->dest_addr     = dest_start_address;
    gp_flash_parameters->src_addr      = src_start_address;
    gp_flash_parameters->current_count = (uint32_t)0;

    /** We're already in the appropriate P/E mode from the caller */

    if (R_FACI->FENTRYR == 0x0080U)
    {
        gp_flash_parameters->total_count = num_bytes;   ///< NPP7 and NPP4 both have DF write size of 1.

        /** Start the write operation */
        HW_FLASH_LP_dataflash_write(src_start_address, dest_start_address, num_bytes);

        /** If configured for Blocking mode then don't return until the entire operation is complete */
        if (gp_flash_parameters->bgo_enabled_df == false)
        {
            do
            {
                err = HW_FLASH_LP_dataflash_write_monitor();
            } while (SSP_ERR_IN_USE == err);

            HW_FLASH_LP_dataflash_enter_read_mode();         // We're done, return to read mode
        }
    }

#if (FLASH_CFG_PARAM_CODE_FLASH_PROGRAMMING_ENABLE == 1)

    else if (R_FACI->FENTRYR == 0x0001U)
    {
#if defined(BSP_MCU_GROUP_S3A7)
        gp_flash_parameters->total_count = (num_bytes) >> 3; ///< NPP7 - Since 8 bytes will be written at a time
#else
        gp_flash_parameters->total_count = (num_bytes) >> 2; ///< NPP4 - Since 4 bytes will be written at a time
#endif

        /** Start the write operation */
        HW_FLASH_LP_codeflash_write(src_start_address, dest_start_address, num_bytes);

        /** Code Flash is always blocking mode, so we don't return until the entire operation is complete */
        do
        {
            err = HW_FLASH_LP_codeflash_write_monitor();
        } while (SSP_ERR_IN_USE == err);

        HW_FLASH_LP_codeflash_enter_read_mode();         ///< We're done, return to read mode
    }
#endif /* if (FLASH_CFG_PARAM_CODE_FLASH_PROGRAMMING_ENABLE == 1) */
    else
    {
        //Flash locked (FSPR bit?)
        return SSP_ERR_PE_FAILURE;
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief   This function checks if the specified Data Flash address range is blank.
 * @param[in]   src_start_address  The starting address of the Flash area to blank check.
 * @param[in]   num_bytes          Specifies the number of bytes that need to be checked.
 * @param[out]  result             Pointer that will be populated by the API with the results of the blank check
 *                                 operation in non-BGO (blocking) mode. In this case the blank check operation
 *                                 completes here and the result is returned. In Data Flash BGO mode the blank check
 *                                 operation is only started here and the result obtained later when the
 *                                 supplied callback routine is called.
 * @retval SSP_SUCCESS             Blankcheck operation completed with result in result,
 *                                 or blankcheck started and in-progess (BGO mode).
 * @retval SSP_ERR_ERASE_FAILED     Status is indicating an erase error. Possibly from a prior operation.
 * @retval SSP_ERR_CMD_LOCKED   FCU is in locked state, typically as a result of having received an illegal
 * command.
 * @retval SSP_ERR_WRITE_FAILED  Status is indicating a Programming error for the requested operation.
 * @retval SSP_ERR_TIMEOUT Timed out waiting for the FCU to become ready.
 **********************************************************************************************************************/
ssp_err_t HW_FLASH_LP_blankcheck (uint32_t start_address, uint32_t num_bytes, flash_result_t * result)
{
    ssp_err_t err = SSP_SUCCESS;

    /** We're already in the appropriate P/E mode from the caller */

        if (R_FACI->FENTRYR == 0x0080U)
    {
        HW_FLASH_LP_dataflash_blank_check(start_address, (start_address + num_bytes) - 1);     // We want to blank check 0
                                                                                             // - num_bytes-1

        /** if in DF BGO mode, exit here; remaining processing if any will be done in ISR */
        if (gp_flash_parameters->bgo_enabled_df == true)
        {
            *result = FLASH_RESULT_BGO_ACTIVE;
            return err;
        }

        do
        {
            err = HW_FLASH_LP_dataflash_blank_check_monitor();
        } while (SSP_ERR_IN_USE == err);

        if (err == SSP_SUCCESS)
        {
            if (0 != R_FACI->FSTATR00_b.BCERR0)           ///< Tested Flash Area is not blank
            {
                *result = FLASH_RESULT_NOT_BLANK;
            }
            else
            {
                *result = FLASH_RESULT_BLANK;
            }

            HW_FLASH_LP_reset();                     ///< Make sure we clear the BCERR bit that reflects the result of
                                                     // our blank check
            HW_FLASH_LP_dataflash_enter_read_mode(); ///< We're done, return to read mode
        }
    }

    else if (R_FACI->FENTRYR == 0x0001U)
    {
        HW_FLASH_LP_codeflash_blank_check(start_address, (start_address + num_bytes) - 1);   ///< We want to blank check 0
                                                                                           // - num_bytes-1
        do
        {
            err = HW_FLASH_LP_codeflash_blank_check_monitor();
        } while (SSP_ERR_IN_USE == err);

        if (err == SSP_SUCCESS)
        {
            if (0 != R_FACI->FSTATR00_b.BCERR0)           ///< Tested Flash Area is not blank
            {
                *result = FLASH_RESULT_NOT_BLANK;
            }
            else
            {
                *result = FLASH_RESULT_BLANK;
            }

            HW_FLASH_LP_reset();
            HW_FLASH_LP_codeflash_enter_read_mode();         ///< We're done, return to read mode
        }
    }

    return err;       ///< Always succeeds, blank status is in result
}

/*******************************************************************************************************************//**
 * @brief   This function switches the peripheral to P/E mode for Code Flash or Data Flash.
 * @param[in]   flash_type_t : Specifies Code or Data Flash.
 *                             Valid selections are: FLASH_TYPE_CODE_FLASH or FLASH_TYPE_DATA_FLASH
 * @retval SSP_SUCCESS                 Successfully entered P/E mode.
 * @retval SSP_ERR_PE_FAILURE    Failed to enter P/E mode
 * @retval SSP_ERR_INVALID_ARGUMENT    Supplied flash_type was something other than FLASH_TYPE_CODE_FLASH or
 *                                     FLASH_TYPE_DATA_FLASH.
 *
 **********************************************************************************************************************/
ssp_err_t HW_FLASH_LP_pe_mode_enter (flash_type_t flash_type)
{
    ssp_err_t err = SSP_SUCCESS;

    if (flash_type == FLASH_TYPE_DATA_FLASH)
    {
        HW_FLASH_LP_dataflash_enter_pe_mode();            ///< Sets PCKA clock

        /** Verify that we actually did enter DF P/E mode */
        if (R_FACI->FENTRYR != 0x0080U)
        {
            err = SSP_ERR_PE_FAILURE;
        }
    }

#if (FLASH_CFG_PARAM_CODE_FLASH_PROGRAMMING_ENABLE == 1)
    else if (flash_type == FLASH_TYPE_CODE_FLASH)
    {
        HW_FLASH_LP_codeflash_enter_pe_mode();

        /** Verify that we actually did enter CF P/E mode */
        if (R_FACI->FENTRYR != 0x0001U)
        {
            err = SSP_ERR_PE_FAILURE;
        }
    }
#endif /* if (FLASH_CFG_PARAM_CODE_FLASH_PROGRAMMING_ENABLE == 1) */
    else
    {
        err = SSP_ERR_INVALID_ARGUMENT;
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief   This function switches the peripheral from P/E mode for Code Flash or Data Flash to Read mode.
 * @param[in]     none
 * @param[out]    none
 * @retval SSP_SUCCESS                 Successfully entered P/E mode.
 * @retval SSP_ERR_PE_FAILURE    Failed to exited P/E mode
 * @retval SSP_FLASH_ERR_PARAMETERS    Supplied flash_type was something other than FLASH_TYPE_CODE_FLASH or
 *                                     FLASH_TYPE_DATA_FLASH.
 *
 **********************************************************************************************************************/
ssp_err_t HW_FLASH_LP_pe_mode_exit (void)
{
    if (R_FACI->FENTRYR == 0x0080U)
    {
        HW_FLASH_LP_dataflash_enter_read_mode();
    }

#if (FLASH_CFG_PARAM_CODE_FLASH_PROGRAMMING_ENABLE == 1)

    else if (R_FACI->FENTRYR == 0x0001U)
    {
        HW_FLASH_LP_codeflash_enter_read_mode();
    }
#endif
    return (SSP_SUCCESS);
}

/*******************************************************************************************************************//**
 * @brief   This function resets the Flash sequencer.
 * @param[in]     none
 * @param[out]    none
 * @retval        none
 **********************************************************************************************************************/
void HW_FLASH_LP_reset (void)
{
    R_FACI->FRESETR_b.FRESET = 1;
    R_FACI->FRESETR_b.FRESET = (uint32_t)0;

    /** Transition to Read mode */
    HW_FLASH_LP_pe_mode_exit();
}

/** If BGO is being used then we that the Flash RDY interrupt be enabled. If
 *  is not enabled then don't generate the ISR routine */
#if (BSP_IRQ_CFG_FCU_FRDYI != BSP_IRQ_DISABLED)
/*******************************************************************************************************************//**
 * @brief  FLASH ready interrupt routine.
 *
 * @details This function implements the FLASH ready isr. The function clears the interrupt request source on entry
 *          populates the callback structure with the relevant event, and providing
 *          a callback routine has been provided, calls the callback function with the event. <br>
 *
 * @param[in]     none
 * @param[out]    none
 * @retval        none
 **********************************************************************************************************************/
void fcu_frdyi_isr (void)
{
    SF_CONTEXT_SAVE
    flash_callback_args_t cb_data;
    ssp_err_t            result = SSP_SUCCESS;

    /** Clear the Interrupt Request*/
    R_BSP_IrqStatusClear(FCU_FRDYI_IRQn);

    if (FLASH_OPERATION_DF_BGO_WRITE == gp_flash_parameters->current_operation)
    {
        /** We are handling the interrupt indicating the last write operation has completed. */
        /** Whether we are done or not we want to check the status */

        result = HW_FLASH_LP_dataflash_write_monitor();
        if ((result != SSP_SUCCESS) && (result != SSP_ERR_IN_USE))
        {
            HW_FLASH_LP_reset();
            cb_data.event = FLASH_EVENT_ERR_FAILURE;            ///< Pass result back to callback
        }
        else
        {
            /*If there are still bytes to write*/
            if (gp_flash_parameters->total_count > (uint32_t)0)
            {
                /** Exit ISR until next FRDY interrupt to continue operation*/
                SF_CONTEXT_RESTORE
                return;
            }
            /** Done writing all bytes */
            else
            {
                cb_data.event = FLASH_EVENT_WRITE_COMPLETE;
            }
        }
    }
    else if ((FLASH_OPERATION_DF_BGO_ERASE == gp_flash_parameters->current_operation))
    {
        /** We are handling the interrupt indicating the last erase operation has completed. */
        /** Whether we are done or not we want to check the status */
        result = HW_FLASH_LP_dataflash_erase_monitor();
        if ((result != SSP_SUCCESS) && (result != SSP_ERR_IN_USE))
        {
            HW_FLASH_LP_reset();
            cb_data.event = FLASH_EVENT_ERR_FAILURE;            ///< Pass result back to callback
        }
        else
        {
            if (gp_flash_parameters->current_count <= gp_flash_parameters->total_count)
            {
                /** Exit ISR until next FRDY interrupt to continue operation */
                SF_CONTEXT_RESTORE
                return;
            }
            /** If all blocks are erased */
            else
            {
                cb_data.event = FLASH_EVENT_ERASE_COMPLETE;
            }
        }
    }
    else if (FLASH_OPERATION_DF_BGO_BLANKCHECK == gp_flash_parameters->current_operation)
    {
        /** We are handling the interrupt indicating the last blank check operation has completed. */
        /** Whether we are done or not we want to check the status */
        result = HW_FLASH_LP_dataflash_blank_check_monitor();

        /// All of the following are valid returns.
        if ((result != SSP_SUCCESS) && (result != SSP_ERR_IN_USE))
        {
            HW_FLASH_LP_reset();
            cb_data.event = FLASH_EVENT_ERR_FAILURE;        ///< Pass result back to callback
        }
        else
        {
            if (R_FACI->FSTATR00_b.BCERR0 == (uint32_t)1)
            {
                cb_data.event = FLASH_EVENT_NOT_BLANK;
            }
            else
            {
                cb_data.event = FLASH_EVENT_BLANK;
            }
        }
    }
    else
    {
        /** This case could only be reached by a memory corruption of some type. */
        cb_data.event = FLASH_EVENT_ERR_FAILURE;
    }

    /*finished current operation. Exit P/E mode*/
    HW_FLASH_LP_pe_mode_exit();
    /*Release lock and Set current state to Idle*/
    flash_ReleaseState();

    if (NULL != flash_ready_isr_handler)
    {
        /** Set data to identify callback to user, then call user callback. */
        flash_ready_isr_handler(&cb_data);
    }

    SF_CONTEXT_RESTORE
}  /* End of function fcu_frdyi_isr() */
#endif
#endif /* if defined(BSP_MCU_GROUP_S124) || defined(BSP_MCU_GROUP_S3A7) || defined(BSP_MCU_GROUP_S128)*/

/*******************************************************************************************************************//**
 * @} (end @addtogroup FLASH_LP_API)
 **********************************************************************************************************************/
