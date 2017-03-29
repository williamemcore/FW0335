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
* File Name    : bsp_cfg_dks1.h
* Description  : BSP configuration file for S124 device.
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @ingroup BSP_CONFIG_S124
 * @defgroup BSP_CONFIG_S124_GENERAL Build Time Configurations - General
 *
 * This file contains compile-time configuration options for general BSP settings. Things such as stack sizes and ROM
 * registers are configured in this file.
 *
 * @{
 **********************************************************************************************************************/

#ifndef BSP_BOARD_S124_DK
#define BSP_BOARD_S124_DK

/***********************************************************************************************************************
Configuration Options
***********************************************************************************************************************/
/** 
 * @name Part Number Information
 *
 * Enter the product part number for your MCU. This information will be used to obtain information about your MCU such
 * as package and memory size.
 * To help parse this information, the part number will be defined using multiple macros.
 *
 * <PRE>
 * R7FS 1 2 4 7 7 3 A01 C FM
 * |    | | | | | | |   | |  Macro Name                      Description
 * |    | | | | | | |   | |__BSP_CFG_MCU_PART_PACKAGE      = Package type, number of pins
 * |    | | | | | | |   |____not used                      = Quality ID
 * |    | | | | | | |________not used                      = Software ID
 * |    | | | | | |__________not used                      = Operating range
 * |    | | | | |____________BSP_CFG_MCU_PART_MEMORY_SIZE  = ROM, RAM, and Data Flash Capacity
 * |    | | | |______________BSP_CFG_MCU_PART_FEATURE_SET  = Superset, no encryption
 * |    | | |________________not used                      = Document index
 * |    | |__________________BSP_CFG_MCU_PART_CORE         = Core & frequency (CM0, 32MHz)
 * |    |____________________BSP_CFG_MCU_PART_SERIES       = Performance category (High-performance, Low power)
 * |_________________________not used                      = Renesas Synergy MCU
 * </PRE>
 */

/** Package type. Set the macro definition based on values below:
 * <PRE>
 * Character(s) = Value for macro = Package Type/Number of Pins/Other Info
 * BJ           = 0x0             = BGA/121
 * FM           = 0x1             = LQFP/64
 * FP           = 0x2             = LQFP/100
 * FB           = 0x3             = LQFP/144
 * LJ           = 0x4             = LGA/100/7x7
 * LK           = 0x5             = LGA/145
 * NB           = 0x6             = WQFN/64
 * </PRE>
 */
#define BSP_CFG_MCU_PART_PACKAGE                    (0x1)

/** ROM, RAM, and Data Flash Capacity.
 * <PRE>
 * Character(s) = Value for macro = ROM Size/Ram Size/Data Flash Size
 * 6            = 0x06             = 64KB/16KB/4KB
 * 7            = 0x07             = 128KB/16KB/4KB
 * </PRE>
*/
#define BSP_CFG_MCU_PART_MEMORY_SIZE                (0x07)

/** Core & Frequency.
 * <PRE>
 * Character(s) = Value for macro = Description
 * 2            = 0x02            = CM0+, 32MHz
 * </PRE>
*/
#define BSP_CFG_MCU_PART_CORE                       (0x02)

/** Series
 * <PRE>
 * Character(s) = Value for macro = Description
 * 1            = 0x1             = Ultra-low Power (up to 32MHz)
 * </PRE>
*/
#define BSP_CFG_MCU_PART_SERIES                     (0x1)

/** @} (end of 'Part Number Information') */

/** @{
 * @name Stack & Heap Size Settings
 */

/** The Main Stack size in bytes. This is the stack that is used out of reset and is required. Exceptions always use
 * the main stack.
 */
#define BSP_CFG_STACK_MAIN_BYTES                    (0x1000)

/** The Process Stack size in bytes. Use of the Process Stack is optional. The BSP does not use the Process Stack.
 * If the user wishes to use the Process Stack in their application they will need to set the SPSEL bit in the
 * CONTROL register.
 */
#define BSP_CFG_STACK_PROCESS_BYTES                 (0)

/** Heap size in bytes. */
#define BSP_CFG_HEAP_BYTES                          (0x400)

/** @} (end of 'Stack & Heap Size Settings')*/

/** @{
 * @name Option-Setting Memory (ROM registers) Settings
 *
 * Certain registers are stored in ROM and used to configure the MCU out of reset.
 *
 * @note To use the default values for a register, leave the macro as all 0xFF's.
 * @note ALL OPTIONS ARE DISABLED BY DEFAULT.
*/

/** Configure WDT and IWDT settings.
 * OFS0 - Option Function Select Register 0
 * - b31     Reserved (set to 1)
 * - b30     WDTSTPCTL - WDT Stop Control - (0=counting continues, 1=stop when entering sleep mode)
 * - b29     Reserved (set to 1)
 * - b28     WDTRSTIRQS - WDT Reset Interrupt Request - What to do on underflow (0=take interrupt, 1=reset MCU)
 * - b27:b26 WDTRPSS - WDT Window Start Position Select - (0=25%, 1=50%, 2=75%, 3=100%,don't use)
 * - b25:b24 WDTRPES - WDT Window End Position Select - (0=75%, 1=50%, 2=25%, 3=0%,don't use)
 * - b23:b20 WDTCKS - WDT Clock Frequency Division Ratio - (1=/4, 4=/64, 0xF=/128, 6=/512, 7=/2048, 8=/8192)
 * - b19:b18 WDTTOPS - WDT Timeout Period Select - (0=1024 cycles, 1=4096, 2=8192, 3=16384)
 * - b17     WDTSTRT - WDT Start Mode Select - (0=auto-start after reset, 1=halt after reset)
 * - b16:b15 Reserved (set to 1)
 * - b14     IWDTSTPCTL - IWDT Sleep Stop Control - (0=counting continues, 1=stop w/some low power modes)
 * - b13     Reserved (set to 1)
 * - b12     IWDTRSTIRQS - IWDT Reset Interrupt Request - What to do on underflow (0=take interrupt, 1=reset MCU)
 * - b11:b10 IWDTRPSS - IWDT Window Start Position Select - (0=25%, 1=50%, 2=75%, 3=100%,don't use)
 * - b9:b8   IWDTRPES - IWDT Window End Position Select - (0=75%, 1=50%, 2=25%, 3=0%,don't use)
 * - b7:b4   IWDTCKS - IWDT Clock Frequency Division Ratio - (0=none, 2=/16, 3 = /32, 4=/64, 0xF=/128, 5=/256)
 * - b3:b2   IWDTTOPS - IWDT Timeout Period Select - (0=128 cycles, 1=512, 2=1024, 3=2048)
 * - b1      IWDTSTRT - IWDT Start Mode Select - (0=auto-start after reset, 1=halt after reset)
 * - b0      Reserved (set to 1)
 *
 *  @note A value of 0xFFFFFFFF is the default and will disable all features
 */
#define BSP_CFG_ROM_REG_OFS0                    (0xFFFFFFFF)

/** Configure whether voltage detection 0 circuit and HOCO are enabled after reset.
 * OFS1 - Option Function Select Register 1
 * - b31:b15 Reserved (set to 1)
 * - b14:b12 HOCOFRQ1 - HOCO frequency setting (Set using BSP_CFG_HOCO_FREQUENCY in bsp_clock_cfg.h)
 * - b11:b9  Reserved (set to 1)
 * - b8      HOCOEN - Enable/disable HOCO oscillation after a reset (0=enable, 1=disable)
 * - b7:b3   Reserved (set to 1)
 * - b2      LVDAS - Choose to enable/disable Voltage Detection 0 Circuit after a reset (0=enable, 1=disable)
 * - b1:b0   VDSEL - Voltage Detection 0 Level Select (1=2.94V, 2=2.87V, 3=2.80V)
 *
 * @note A value of 0xFFFFFFFF is the default and will disable all features
 */
#define BSP_CFG_ROM_REG_OFS1                    (0xFFFFFFFF)

/** @} (end of 'Option-Setting Memory (ROM registers) Settings') */



/** @{
 * @name ID Code Protection
 *
 * Set your desired ID code. If you set this value and program it into the MCU then you will need to remember
 * the ID code because the debugger will ask for it when trying to connect. The ID Code is 16 bytes long.
 * The macro below define the ID Code in 4-byte sections.
 *
 * @note Leave at the default (all 0xFF's) if you do not wish to use an ID code (Protection Disabled).
 * @warning Please consult the hardware manual for your MCU on valid ID Code settings.
 */
/** Lowest 4-byte section, address 0x40120050. From MSB to LSB: ID Code 16, ID code 15, ID code 14, ID code 13. */
#define BSP_CFG_ID_CODE_LONG_1                      (0xFFFFFFFF)
/** 2nd ID Code section, address 0x40120054. From MSB to LSB: ID code 12, ID code 11, ID code 10, ID code 9. */
#define BSP_CFG_ID_CODE_LONG_2                      (0xFFFFFFFF)
/** 3rd ID Code section, address 0x40120058. From MSB to LSB: ID code 8, ID code 7, ID code 6, ID code 5. */
#define BSP_CFG_ID_CODE_LONG_3                      (0xFFFFFFFF)
/** 4th ID Code section, address 0x4012005C. From MSB to LSB: ID code 4, ID code 3, ID code 2, ID code 1. */
#define BSP_CFG_ID_CODE_LONG_4                      (0xFFFFFFFF)

/** @} (end of 'Other Hardware Options') */

/** @{
 * @name Other Hardware Options
 */

/** This macro is used to define the voltage that is supplied to the MCU (Vcc). This macro is defined in millivolts.
 * This macro does not actually change anything on the MCU. Some AMS modules need this information so it is
 * defined here.
 */
#define BSP_CFG_MCU_VCC_MV                          (3300)

/** @} (end of 'Other Hardware Options') */

/** @{
 *  @name Project-Wide Software Options
 */

/** By default AMS modules will check input parameters to be valid. This is helpful during development but some users
 * will want to disable this for production code. The reason for this would be to save execution time and code space.
 * This macro is a global setting for enabling or disabling parameter checking. Each AMS module will also have its
 * own local macro for this same purpose. By default the local macros will take the global value from here though
 * they can be overridden. Therefore, the local setting has priority over this global setting. Disabling parameter
 * checking should only used when inputs are known to be good and the increase in speed or decrease in code space is
 * needed.
 * - 0 = Global setting for parameter checking is disabled.
 * - 1 = Global setting for parameter checking is enabled (Default).
 */
#define BSP_CFG_PARAM_CHECKING_ENABLE               (1)

/** Specify which RTOS is being used
 * - 0 = No RTOS
 * - 1 = ThreadX
 * - 2 = Other
 */
#define BSP_CFG_RTOS                                (0)

/** @} (end of 'Project-Wide Software Options') */

#endif /* BSP_BOARD_S124_DK */

/** @} (end defgroup BSP_CONFIG_S124_GENERAL) */
