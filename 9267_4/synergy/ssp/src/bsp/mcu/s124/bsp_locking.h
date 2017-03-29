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
* File Name    : bsp_locking.h
* Description  : This module implements atomic locking
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @ingroup BSP_MCU_S124
 * @defgroup BSP_MCU_LOCKING Atomic Locking
 *
 * This module implements atomic locking mechanisms.
 *
 * @{
 **********************************************************************************************************************/

/** @} (end defgroup BSP_MCU_LOCKING) */

#ifndef BSP_LOCKING_H_
#define BSP_LOCKING_H_

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Valid bsp_lock_t->lock values. */
#define BSP_LOCK_LOCKED             (0x01)  ///< Lock value when lock is taken
#define BSP_LOCK_UNLOCKED           (0x00)  ///< Lock value when lock is available

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/** Lock structure. Passed into software locking functions such as R_BSP_SoftwareLock() and R_BSP_SoftwareUnLock. */
typedef struct st_bsp_lock
{
    uint8_t lock; ///< A uint8_t is used instead of a enum because the size must be 8-bits.
} bsp_lock_t;

/***********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/
void bsp_init_hardware_locks(void);

/* Public functions defined in bsp.h */

#endif /* BSP_LOCKING_H_ */


