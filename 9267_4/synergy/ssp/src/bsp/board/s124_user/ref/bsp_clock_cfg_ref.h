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
* File Name    : bsp_clock_cfg_dks1.h
* Description  : BSP clock configuration file for S124 device.
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @ingroup BSP_CONFIG_S124
 * @defgroup BSP_CONFIG_S124_CLOCKS Build Time Configurations - Clock
 *
 * This file contains build-time clock configuration options. The BSP will use these macros to setup the MCU's clocks
 * for the user before main() is executed.
 *
 * Clock configuration options.
 * The input clock frequency is specified and then the system clocks are set by specifying the multipliers used. The
 * multiplier settings are used to set the clock registers. An example is shown below for a board with a 8MHz
 * XTAL and CPU clock of 48MHz:
 *
 * <PRE>
 * BSP_CFG_XTAL_HZ = 12000000
 * BSP_CFG_PLL_DIV = 2 (/2)
 * BSP_CFG_PLL_MUL = 12 ((12MHz / 2) x 12 = 48MHz)
 *
 * BSP_CFG_ICK_DIV =  1      : System Clock (ICLK)        =
 *                             (((BSP_CFG_XTAL_HZ/BSP_CFG_PLL_DIV) * BSP_CFG_PLL_MUL) / BSP_CFG_ICK_DIV)  = 48MHz
 * BSP_CFG_PCKA_DIV = 1      : Peripheral Clock A (PCLKA) =
 *                             (((BSP_CFG_XTAL_HZ/BSP_CFG_PLL_DIV) * BSP_CFG_PLL_MUL) / BSP_CFG_PCKA_DIV) = 48MHz
 * BSP_CFG_PCKB_DIV = 2      : Peripheral Clock B (PCLKB) =
 *                             (((BSP_CFG_XTAL_HZ/BSP_CFG_PLL_DIV) * BSP_CFG_PLL_MUL) / BSP_CFG_PCKB_DIV) = 24MHz
 * BSP_CFG_PCKD_DIV = 1      : Peripheral Clock D (PCLKD) =
 *                             (((BSP_CFG_XTAL_HZ/BSP_CFG_PLL_DIV) * BSP_CFG_PLL_MUL) / BSP_CFG_PCKD_DIV) = 48MHz
 * </PRE>
 *
 * @note USB clock does not have a divisor
 *
 * @{
***********************************************************************************************************************/

#ifndef BSP_CLOCK_CFG
#define BSP_CLOCK_CFG

/***********************************************************************************************************************
Configuration Options
***********************************************************************************************************************/
/** Clock source select (CKSEL). The chosen clock will be the base clock provided for the system clock and all
 * peripheral clocks. It is also used for the flash clock and the external bus clocks.
 *
 * Set the macro to one of the enumerations shown.
 *
 * <PRE>
 * Clock                                        -   Enumeration to use for macro
 * High Speed On-Chip Oscillator   (HOCO)       -   CGC_CLOCK_HOCO
 * Middle Speed On-Chip Oscillator (MOCO)       -   CGC_CLOCK_MOCO
 * Low Speed On-Chip Oscillator    (LOCO)       -   CGC_CLOCK_LOCO
 * Main Clock Oscillator                        -   CGC_CLOCK_MAIN_OSC
 * Sub-Clock Oscillator                         -   CGC_CLOCK_SUBCLOCK
 * </PRE>
 */
#define BSP_CFG_CLOCK_SOURCE                        (CGC_CLOCK_HOCO)

/** XTAL - Input clock frequency in Hz */
#define BSP_CFG_XTAL_HZ                             (12000000)

/** The HOCO can operate at several different frequencies. Choose which one using the macro below. The frequency
 * used out of reset depends upon the OFS1.HOCOFRQ0 bits.
 *
 * <PRE>
 * Available frequency settings:
 * 0 = 24MHz
 * 1 = 29.491MHz
 * 2 = 32MHz
 * 4 = 48MHz
 * 5 = 64MHz
 * </PRE>
 *
 * @note Setting a value of 3 is prohibited
 */
#define BSP_CFG_HOCO_FREQUENCY                      (2)

/** PLL Output Frequency Division Ratio Select (PLIDIV).
 *
 * Available divisors = - /1 (no division), - /2, - /4
 *
 * @note Set macro definition to 'CGC_PLL_DIV_' + your divider selection.
 */
#define BSP_CFG_ICK_DIV                             (CGC_SYS_CLOCK_DIV_1)

/** Peripheral Module Clock B Divider (PCKB).
 *
 * Available divisors = /1 (no division), /2, /4, /8, /16, /32, /64
 *
 * @note Set macro definition to 'CGC_SYS_CLOCK_DIV_' + your divider selection.
 */
#define BSP_CFG_PCKB_DIV                            (CGC_SYS_CLOCK_DIV_2)

/** Peripheral Module Clock D Divider (PCKD).
 *
 * Available divisors = /1 (no division), /2, /4, /8, /16, /32, /64
 *
 * @note Set macro definition to 'CGC_SYS_CLOCK_DIV_' + your divider selection.
 */
#define BSP_CFG_PCKD_DIV                            (CGC_SYS_CLOCK_DIV_1)


#endif /* BSP_CLOCK_CFG */

/** @} (end defgroup BSP_CONFIG_S124_CLOCKS) */
