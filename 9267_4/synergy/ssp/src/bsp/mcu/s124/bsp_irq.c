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
* File Name    : bsp_irq.c
* Description  : This module configures certain ELC events so that they can trigger NVIC interrupts.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "bsp_api.h"

#if defined(BSP_MCU_GROUP_S124)

/** ELC event definitions. */
#include "r_elc_api.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/* Used for accessing bitfields in IELSR registers. */
typedef struct
{
    __IO uint32_t  IELS       :  9;               /* [0..8] Event selection to NVIC */
         uint32_t  res0       :  7;
    __IO uint32_t  IR         :  1;               /* [16..16] Interrupt Status Flag */
         uint32_t  res1       :  7;
    __IO uint32_t  DTCE       :  1;               /* [24..24] DTC Activation Enable */
         uint32_t  res2       :  7;
} bsp_prv_ielsr_t;

/***********************************************************************************************************************
Exported global variables (to be accessed by other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @addtogroup BSP_MCU_IRQ
 *
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief Clear the interrupt status flag (IR) for a given interrupt. When an interrupt is triggered the IR bit
 *        is set. If it is not cleared in the ISR then the interrupt will trigger again immediately.
 *
 * @param[in] irq Interrupt for which to clear the IR bit.
 *
 * @note This does not work for system exceptions where the IRQn_Type value is < 0.
 **********************************************************************************************************************/
void R_BSP_IrqStatusClear (IRQn_Type irq)
{
    /* This does not work for system exceptions where the IRQn_Type value is < 0 */
    if (((int32_t)irq) >= 0)
    {
        /* Clear the IR bit in the selected IELSR register. */
        ((bsp_prv_ielsr_t *)&R_ICU->IELSRn)[(uint32_t)irq].IR = 0UL;
    }
}

/** @} (end addtogroup BSP_MCU_IRQ) */

/*******************************************************************************************************************//**
 * @brief This function will initialize the ICU so that certain ELC events will trigger interrupts in the NVIC.
 *        It will also set the NVIC priority levels for all enabled interrupts. Which ELC events are enabled and
 *        what priority levels are used depends on the macro settings in bsp_irq_cfg.h.
 *
 * @note This does not work for system exceptions where the IRQn_Type value is < 0.
 **********************************************************************************************************************/
void bsp_irq_cfg (void)
{
    uint32_t * base_addr;

    base_addr = (uint32_t *)&R_ICU->IELSRn;

    /* This next line does not emit any code. It is used to suppress compiler warnings about base_addr not being
     * used when no interrupts are enabled. */
    ((void)(base_addr));

#if (BSP_IRQ_CFG_ICU_IRQ0 != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)ICU_IRQ0_IRQn) = ELC_EVENT_ICU_IRQ0;
    NVIC_SetPriority(ICU_IRQ0_IRQn, BSP_IRQ_CFG_ICU_IRQ0);
#endif
#if (BSP_IRQ_CFG_ICU_IRQ1 != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)ICU_IRQ1_IRQn) = ELC_EVENT_ICU_IRQ1;
    NVIC_SetPriority(ICU_IRQ1_IRQn, BSP_IRQ_CFG_ICU_IRQ1);
#endif
#if (BSP_IRQ_CFG_ICU_IRQ2 != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)ICU_IRQ2_IRQn) = ELC_EVENT_ICU_IRQ2;
    NVIC_SetPriority(ICU_IRQ2_IRQn, BSP_IRQ_CFG_ICU_IRQ2);
#endif
#if (BSP_IRQ_CFG_ICU_IRQ3 != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)ICU_IRQ3_IRQn) = ELC_EVENT_ICU_IRQ3;
    NVIC_SetPriority(ICU_IRQ3_IRQn, BSP_IRQ_CFG_ICU_IRQ3);
#endif
#if (BSP_IRQ_CFG_ICU_IRQ4 != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)ICU_IRQ4_IRQn) = ELC_EVENT_ICU_IRQ4;
    NVIC_SetPriority(ICU_IRQ4_IRQn, BSP_IRQ_CFG_ICU_IRQ4);
#endif
#if (BSP_IRQ_CFG_ICU_IRQ5 != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)ICU_IRQ5_IRQn) = ELC_EVENT_ICU_IRQ5;
    NVIC_SetPriority(ICU_IRQ5_IRQn, BSP_IRQ_CFG_ICU_IRQ5);
#endif
#if (BSP_IRQ_CFG_ICU_IRQ6 != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)ICU_IRQ6_IRQn) = ELC_EVENT_ICU_IRQ6;
    NVIC_SetPriority(ICU_IRQ6_IRQn, BSP_IRQ_CFG_ICU_IRQ6);
#endif
#if (BSP_IRQ_CFG_ICU_IRQ7 != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)ICU_IRQ7_IRQn) = ELC_EVENT_ICU_IRQ7;
    NVIC_SetPriority(ICU_IRQ7_IRQn, BSP_IRQ_CFG_ICU_IRQ7);
#endif
#if (BSP_IRQ_CFG_DTC_COMPLETE != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)DTC_COMPLETE_IRQn) = ELC_EVENT_DTC_COMPLETE;
    NVIC_SetPriority(DTC_COMPLETE_IRQn, BSP_IRQ_CFG_DTC_COMPLETE);
#endif
#if (BSP_IRQ_CFG_DTC_END != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)DTC_END_IRQn) = ELC_EVENT_DTC_END;
    NVIC_SetPriority(DTC_END_IRQn, BSP_IRQ_CFG_DTC_END);
#endif
#if (BSP_IRQ_CFG_ICU_SNOOZE_CANCEL != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)ICU_SNOOZE_CANCEL_IRQn) = ELC_EVENT_ICU_SNOOZE_CANCEL;
    NVIC_SetPriority(ICU_SNOOZE_CANCEL_IRQn, BSP_IRQ_CFG_ICU_SNOOZE_CANCEL);
#endif
#if (BSP_IRQ_CFG_FCU_FRDYI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)FCU_FRDYI_IRQn) = ELC_EVENT_FCU_FRDYI;
    NVIC_SetPriority(FCU_FRDYI_IRQn, BSP_IRQ_CFG_FCU_FRDYI);
#endif
#if (BSP_IRQ_CFG_LVD_LVD1 != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)LVD_LVD1_IRQn) = ELC_EVENT_LVD_LVD1;
    NVIC_SetPriority(LVD_LVD1_IRQn, BSP_IRQ_CFG_LVD_LVD1);
#endif
#if (BSP_IRQ_CFG_LVD_LVD2 != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)LVD_LVD2_IRQn) = ELC_EVENT_LVD_LVD2;
    NVIC_SetPriority(LVD_LVD2_IRQn, BSP_IRQ_CFG_LVD_LVD2);
#endif
#if (BSP_IRQ_CFG_CGC_MOSC_STOP != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)CGC_MOSC_STOP_IRQn) = ELC_EVENT_CGC_MOSC_STOP;
    NVIC_SetPriority(CGC_MOSC_STOP_IRQn, BSP_IRQ_CFG_CGC_MOSC_STOP);
#endif
#if (BSP_IRQ_CFG_LPM_SNOOZE_REQUEST != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)LPM_SNOOZE_REQUEST_IRQn) = ELC_EVENT_LPM_SNOOZE_REQUEST;
    NVIC_SetPriority(LPM_SNOOZE_REQUEST_IRQn, BSP_IRQ_CFG_LPM_SNOOZE_REQUEST);
#endif
#if (BSP_IRQ_CFG_AGT0_INT != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)AGT0_INT_IRQn) = ELC_EVENT_AGT0_INT;
    NVIC_SetPriority(AGT0_INT_IRQn, BSP_IRQ_CFG_AGT0_INT);
#endif
#if (BSP_IRQ_CFG_AGT0_COMPARE_A != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)AGT0_COMPARE_A_IRQn) = ELC_EVENT_AGT0_COMPARE_A;
    NVIC_SetPriority(AGT0_COMPARE_A_IRQn, BSP_IRQ_CFG_AGT0_COMPARE_A);
#endif
#if (BSP_IRQ_CFG_AGT0_COMPARE_B != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)AGT0_COMPARE_B_IRQn) = ELC_EVENT_AGT0_COMPARE_B;
    NVIC_SetPriority(AGT0_COMPARE_B_IRQn, BSP_IRQ_CFG_AGT0_COMPARE_B);
#endif
#if (BSP_IRQ_CFG_AGT1_INT != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)AGT1_INT_IRQn) = ELC_EVENT_AGT1_INT;
    NVIC_SetPriority(AGT1_INT_IRQn, BSP_IRQ_CFG_AGT1_INT);
#endif
#if (BSP_IRQ_CFG_AGT1_COMPARE_A != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)AGT1_COMPARE_A_IRQn) = ELC_EVENT_AGT1_COMPARE_A;
    NVIC_SetPriority(AGT1_COMPARE_A_IRQn, BSP_IRQ_CFG_AGT1_COMPARE_A);
#endif
#if (BSP_IRQ_CFG_AGT1_COMPARE_B != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)AGT1_COMPARE_B_IRQn) = ELC_EVENT_AGT1_COMPARE_B;
    NVIC_SetPriority(AGT1_COMPARE_B_IRQn, BSP_IRQ_CFG_AGT1_COMPARE_B);
#endif
#if (BSP_IRQ_CFG_IWDT_UNDERFLOW != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)IWDT_UNDERFLOW_IRQn) = ELC_EVENT_IWDT_UNDERFLOW;
    NVIC_SetPriority(IWDT_UNDERFLOW_IRQn, BSP_IRQ_CFG_IWDT_UNDERFLOW);
#endif
#if (BSP_IRQ_CFG_WDT_UNDERFLOW != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)WDT_UNDERFLOW_IRQn) = ELC_EVENT_WDT_UNDERFLOW;
    NVIC_SetPriority(WDT_UNDERFLOW_IRQn, BSP_IRQ_CFG_WDT_UNDERFLOW);
#endif
#if (BSP_IRQ_CFG_RTC_ALARM != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)RTC_ALARM_IRQn) = ELC_EVENT_RTC_ALARM;
    NVIC_SetPriority(RTC_ALARM_IRQn, BSP_IRQ_CFG_RTC_ALARM);
#endif
#if (BSP_IRQ_CFG_RTC_PERIOD != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)RTC_PERIOD_IRQn) = ELC_EVENT_RTC_PERIOD;
    NVIC_SetPriority(RTC_PERIOD_IRQn, BSP_IRQ_CFG_RTC_PERIOD);
#endif
#if (BSP_IRQ_CFG_RTC_CARRY != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)RTC_CARRY_IRQn) = ELC_EVENT_RTC_CARRY;
    NVIC_SetPriority(RTC_CARRY_IRQn, BSP_IRQ_CFG_RTC_CARRY);
#endif
#if (BSP_IRQ_CFG_ADC0_SCAN_END != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)ADC0_SCAN_END_IRQn) = ELC_EVENT_ADC0_SCAN_END;
    NVIC_SetPriority(ADC0_SCAN_END_IRQn, BSP_IRQ_CFG_ADC0_SCAN_END);
#endif
#if (BSP_IRQ_CFG_ADC0_SCAN_END_B != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)ADC0_SCAN_END_B_IRQn) = ELC_EVENT_ADC0_SCAN_END_B;
    NVIC_SetPriority(ADC0_SCAN_END_B_IRQn, BSP_IRQ_CFG_ADC0_SCAN_END_B);
#endif
#if (BSP_IRQ_CFG_ADC0_WINDOW_A != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)ADC0_WINDOW_A_IRQn) = ELC_EVENT_ADC0_WINDOW_A;
    NVIC_SetPriority(ADC0_WINDOW_A_IRQn, BSP_IRQ_CFG_ADC0_WINDOW_A);
#endif
#if (BSP_IRQ_CFG_ADC0_WINDOW_B != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)ADC0_WINDOW_B_IRQn) = ELC_EVENT_ADC0_WINDOW_B;
    NVIC_SetPriority(ADC0_WINDOW_B_IRQn, BSP_IRQ_CFG_ADC0_WINDOW_B);
#endif
#if (BSP_IRQ_CFG_ADC0_COMPARE_MATCH != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)ADC0_COMPARE_MATCH_IRQn) = ELC_EVENT_ADC0_COMPARE_MATCH;
    NVIC_SetPriority(ADC0_COMPARE_MATCH_IRQn, BSP_IRQ_CFG_ADC0_COMPARE_MATCH);
#endif
#if (BSP_IRQ_CFG_ADC0_COMPARE_MISMATCH != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)ADC0_COMPARE_MISMATCH_IRQn) = ELC_EVENT_ADC0_COMPARE_MISMATCH;
    NVIC_SetPriority(ADC0_COMPARE_MISMATCH_IRQn, BSP_IRQ_CFG_ADC0_COMPARE_MISMATCH);
#endif
#if (BSP_IRQ_CFG_COMP_LP_0 != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)COMP_LP_0_IRQn) = ELC_EVENT_COMP_LP_0;
    NVIC_SetPriority(COMP_LP_0_IRQn, BSP_IRQ_CFG_COMP_LP_0);
#endif
#if (BSP_IRQ_CFG_COMP_LP_1 != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)COMP_LP_1_IRQn) = ELC_EVENT_COMP_LP_1;
    NVIC_SetPriority(COMP_LP_1_IRQn, BSP_IRQ_CFG_COMP_LP_1);
#endif
#if (BSP_IRQ_CFG_USBFS_INT != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)USBFS_INT_IRQn) = ELC_EVENT_USBFS_INT;
    NVIC_SetPriority(USBFS_INT_IRQn, BSP_IRQ_CFG_USBFS_INT);
#endif
#if (BSP_IRQ_CFG_USBFS_RESUME != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)USBFS_RESUME_IRQn) = ELC_EVENT_USBFS_RESUME;
    NVIC_SetPriority(USBFS_RESUME_IRQn, BSP_IRQ_CFG_USBFS_RESUME);
#endif
#if (BSP_IRQ_CFG_IIC0_RXI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)IIC0_RXI_IRQn) = ELC_EVENT_IIC0_RXI;
    NVIC_SetPriority(IIC0_RXI_IRQn, BSP_IRQ_CFG_IIC0_RXI);
#endif
#if (BSP_IRQ_CFG_IIC0_TXI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)IIC0_TXI_IRQn) = ELC_EVENT_IIC0_TXI;
    NVIC_SetPriority(IIC0_TXI_IRQn, BSP_IRQ_CFG_IIC0_TXI);
#endif
#if (BSP_IRQ_CFG_IIC0_TEI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)IIC0_TEI_IRQn) = ELC_EVENT_IIC0_TEI;
    NVIC_SetPriority(IIC0_TEI_IRQn, BSP_IRQ_CFG_IIC0_TEI);
#endif
#if (BSP_IRQ_CFG_IIC0_ERI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)IIC0_ERI_IRQn) = ELC_EVENT_IIC0_ERI;
    NVIC_SetPriority(IIC0_ERI_IRQn, BSP_IRQ_CFG_IIC0_ERI);
#endif
#if (BSP_IRQ_CFG_IIC0_WUI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)IIC0_WUI_IRQn) = ELC_EVENT_IIC0_WUI;
    NVIC_SetPriority(IIC0_WUI_IRQn, BSP_IRQ_CFG_IIC0_WUI);
#endif
#if (BSP_IRQ_CFG_IIC1_RXI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)IIC1_RXI_IRQn) = ELC_EVENT_IIC1_RXI;
    NVIC_SetPriority(IIC1_RXI_IRQn, BSP_IRQ_CFG_IIC1_RXI);
#endif
#if (BSP_IRQ_CFG_IIC1_TXI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)IIC1_TXI_IRQn) = ELC_EVENT_IIC1_TXI;
    NVIC_SetPriority(IIC1_TXI_IRQn, BSP_IRQ_CFG_IIC1_TXI);
#endif
#if (BSP_IRQ_CFG_IIC1_TEI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)IIC1_TEI_IRQn) = ELC_EVENT_IIC1_TEI;
    NVIC_SetPriority(IIC1_TEI_IRQn, BSP_IRQ_CFG_IIC1_TEI);
#endif
#if (BSP_IRQ_CFG_IIC1_ERI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)IIC1_ERI_IRQn) = ELC_EVENT_IIC1_ERI;
    NVIC_SetPriority(IIC1_ERI_IRQn, BSP_IRQ_CFG_IIC1_ERI);
#endif
#if (BSP_IRQ_CFG_CTSU_WRITE != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)CTSU_WRITE_IRQn) = ELC_EVENT_CTSU_WRITE;
    NVIC_SetPriority(CTSU_WRITE_IRQn, BSP_IRQ_CFG_CTSU_WRITE);
#endif
#if (BSP_IRQ_CFG_CTSU_READ != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)CTSU_READ_IRQn) = ELC_EVENT_CTSU_READ;
    NVIC_SetPriority(CTSU_READ_IRQn, BSP_IRQ_CFG_CTSU_READ);
#endif
#if (BSP_IRQ_CFG_CTSU_END != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)CTSU_END_IRQn) = ELC_EVENT_CTSU_END;
    NVIC_SetPriority(CTSU_END_IRQn, BSP_IRQ_CFG_CTSU_END);
#endif
#if (BSP_IRQ_CFG_KEY_INT != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)KEY_INT_IRQn) = ELC_EVENT_KEY_INT;
    NVIC_SetPriority(KEY_INT_IRQn, BSP_IRQ_CFG_KEY_INT);
#endif
#if (BSP_IRQ_CFG_DOC_INT != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)DOC_INT_IRQn) = ELC_EVENT_DOC_INT;
    NVIC_SetPriority(DOC_INT_IRQn, BSP_IRQ_CFG_DOC_INT);
#endif
#if (BSP_IRQ_CFG_CAC_FREQUENCY_ERROR != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)CAC_FREQUENCY_ERROR_IRQn) = ELC_EVENT_CAC_FREQUENCY_ERROR;
    NVIC_SetPriority(CAC_FREQUENCY_ERROR_IRQn, BSP_IRQ_CFG_CAC_FREQUENCY_ERROR);
#endif
#if (BSP_IRQ_CFG_CAC_MEASUREMENT_END != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)CAC_MEASUREMENT_END_IRQn) = ELC_EVENT_CAC_MEASUREMENT_END;
    NVIC_SetPriority(CAC_MEASUREMENT_END_IRQn, BSP_IRQ_CFG_CAC_MEASUREMENT_END);
#endif
#if (BSP_IRQ_CFG_CAC_OVERFLOW != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)CAC_OVERFLOW_IRQn) = ELC_EVENT_CAC_OVERFLOW;
    NVIC_SetPriority(CAC_OVERFLOW_IRQn, BSP_IRQ_CFG_CAC_OVERFLOW);
#endif
#if (BSP_IRQ_CFG_CAN0_ERROR != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)CAN0_ERROR_IRQn) = ELC_EVENT_CAN0_ERROR;
    NVIC_SetPriority(CAN0_ERROR_IRQn, BSP_IRQ_CFG_CAN0_ERROR);
#endif
#if (BSP_IRQ_CFG_CAN0_FIFO_RX != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)CAN0_FIFO_RX_IRQn) = ELC_EVENT_CAN0_FIFO_RX;
    NVIC_SetPriority(CAN0_FIFO_RX_IRQn, BSP_IRQ_CFG_CAN0_FIFO_RX);
#endif
#if (BSP_IRQ_CFG_CAN0_FIFO_TX != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)CAN0_FIFO_TX_IRQn) = ELC_EVENT_CAN0_FIFO_TX;
    NVIC_SetPriority(CAN0_FIFO_TX_IRQn, BSP_IRQ_CFG_CAN0_FIFO_TX);
#endif
#if (BSP_IRQ_CFG_CAN0_MAILBOX_RX != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)CAN0_MAILBOX_RX_IRQn) = ELC_EVENT_CAN0_MAILBOX_RX;
    NVIC_SetPriority(CAN0_MAILBOX_RX_IRQn, BSP_IRQ_CFG_CAN0_MAILBOX_RX);
#endif
#if (BSP_IRQ_CFG_CAN0_MAILBOX_TX != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)CAN0_MAILBOX_TX_IRQn) = ELC_EVENT_CAN0_MAILBOX_TX;
    NVIC_SetPriority(CAN0_MAILBOX_TX_IRQn, BSP_IRQ_CFG_CAN0_MAILBOX_TX);
#endif
#if (BSP_IRQ_CFG_IOPORT_EVENT_1 != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)IOPORT_EVENT_1_IRQn) = ELC_EVENT_IOPORT_EVENT_1;
    NVIC_SetPriority(IOPORT_EVENT_1_IRQn, BSP_IRQ_CFG_IOPORT_EVENT_1);
#endif
#if (BSP_IRQ_CFG_IOPORT_EVENT_2 != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)IOPORT_EVENT_2_IRQn) = ELC_EVENT_IOPORT_EVENT_2;
    NVIC_SetPriority(IOPORT_EVENT_2_IRQn, BSP_IRQ_CFG_IOPORT_EVENT_2);
#endif
#if (BSP_IRQ_CFG_ELC_SOFTWARE_EVENT_0 != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)ELC_SOFTWARE_EVENT_0_IRQn) = ELC_EVENT_ELC_SOFTWARE_EVENT_0;
    NVIC_SetPriority(ELC_SOFTWARE_EVENT_0_IRQn, BSP_IRQ_CFG_ELC_SOFTWARE_EVENT_0);
#endif
#if (BSP_IRQ_CFG_ELC_SOFTWARE_EVENT_1 != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)ELC_SOFTWARE_EVENT_1_IRQn) = ELC_EVENT_ELC_SOFTWARE_EVENT_1;
    NVIC_SetPriority(ELC_SOFTWARE_EVENT_1_IRQn, BSP_IRQ_CFG_ELC_SOFTWARE_EVENT_1);
#endif
#if (BSP_IRQ_CFG_POEG0_EVENT != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)POEG0_EVENT_IRQn) = ELC_EVENT_POEG0_EVENT;
    NVIC_SetPriority(POEG0_EVENT_IRQn, BSP_IRQ_CFG_POEG0_EVENT);
#endif
#if (BSP_IRQ_CFG_POEG1_EVENT != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)POEG1_EVENT_IRQn) = ELC_EVENT_POEG1_EVENT;
    NVIC_SetPriority(POEG1_EVENT_IRQn, BSP_IRQ_CFG_POEG1_EVENT);
#endif
#if (BSP_IRQ_CFG_GPT0_CAPTURE_COMPARE_A != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT0_CAPTURE_COMPARE_A_IRQn) = ELC_EVENT_GPT0_CAPTURE_COMPARE_A;
    NVIC_SetPriority(GPT0_CAPTURE_COMPARE_A_IRQn, BSP_IRQ_CFG_GPT0_CAPTURE_COMPARE_A);
#endif
#if (BSP_IRQ_CFG_GPT0_CAPTURE_COMPARE_B != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT0_CAPTURE_COMPARE_B_IRQn) = ELC_EVENT_GPT0_CAPTURE_COMPARE_B;
    NVIC_SetPriority(GPT0_CAPTURE_COMPARE_B_IRQn, BSP_IRQ_CFG_GPT0_CAPTURE_COMPARE_B);
#endif
#if (BSP_IRQ_CFG_GPT0_COMPARE_C != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT0_COMPARE_C_IRQn) = ELC_EVENT_GPT0_COMPARE_C;
    NVIC_SetPriority(GPT0_COMPARE_C_IRQn, BSP_IRQ_CFG_GPT0_COMPARE_C);
#endif
#if (BSP_IRQ_CFG_GPT0_COMPARE_D != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT0_COMPARE_D_IRQn) = ELC_EVENT_GPT0_COMPARE_D;
    NVIC_SetPriority(GPT0_COMPARE_D_IRQn, BSP_IRQ_CFG_GPT0_COMPARE_D);
#endif
#if (BSP_IRQ_CFG_GPT0_COUNTER_OVERFLOW != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT0_COUNTER_OVERFLOW_IRQn) = ELC_EVENT_GPT0_COUNTER_OVERFLOW;
    NVIC_SetPriority(GPT0_COUNTER_OVERFLOW_IRQn, BSP_IRQ_CFG_GPT0_COUNTER_OVERFLOW);
#endif
#if (BSP_IRQ_CFG_GPT0_COUNTER_UNDERFLOW != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT0_COUNTER_UNDERFLOW_IRQn) = ELC_EVENT_GPT0_COUNTER_UNDERFLOW;
    NVIC_SetPriority(GPT0_COUNTER_UNDERFLOW_IRQn, BSP_IRQ_CFG_GPT0_COUNTER_UNDERFLOW);
#endif
#if (BSP_IRQ_CFG_GPT1_CAPTURE_COMPARE_A != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT1_CAPTURE_COMPARE_A_IRQn) = ELC_EVENT_GPT1_CAPTURE_COMPARE_A;
    NVIC_SetPriority(GPT1_CAPTURE_COMPARE_A_IRQn, BSP_IRQ_CFG_GPT1_CAPTURE_COMPARE_A);
#endif
#if (BSP_IRQ_CFG_GPT1_CAPTURE_COMPARE_B != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT1_CAPTURE_COMPARE_B_IRQn) = ELC_EVENT_GPT1_CAPTURE_COMPARE_B;
    NVIC_SetPriority(GPT1_CAPTURE_COMPARE_B_IRQn, BSP_IRQ_CFG_GPT1_CAPTURE_COMPARE_B);
#endif
#if (BSP_IRQ_CFG_GPT1_COMPARE_C != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT1_COMPARE_C_IRQn) = ELC_EVENT_GPT1_COMPARE_C;
    NVIC_SetPriority(GPT1_COMPARE_C_IRQn, BSP_IRQ_CFG_GPT1_COMPARE_C);
#endif
#if (BSP_IRQ_CFG_GPT1_COMPARE_D != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT1_COMPARE_D_IRQn) = ELC_EVENT_GPT1_COMPARE_D;
    NVIC_SetPriority(GPT1_COMPARE_D_IRQn, BSP_IRQ_CFG_GPT1_COMPARE_D);
#endif
#if (BSP_IRQ_CFG_GPT1_COUNTER_OVERFLOW != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT1_COUNTER_OVERFLOW_IRQn) = ELC_EVENT_GPT1_COUNTER_OVERFLOW;
    NVIC_SetPriority(GPT1_COUNTER_OVERFLOW_IRQn, BSP_IRQ_CFG_GPT1_COUNTER_OVERFLOW);
#endif
#if (BSP_IRQ_CFG_GPT1_COUNTER_UNDERFLOW != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT1_COUNTER_UNDERFLOW_IRQn) = ELC_EVENT_GPT1_COUNTER_UNDERFLOW;
    NVIC_SetPriority(GPT1_COUNTER_UNDERFLOW_IRQn, BSP_IRQ_CFG_GPT1_COUNTER_UNDERFLOW);
#endif
#if (BSP_IRQ_CFG_GPT2_CAPTURE_COMPARE_A != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT2_CAPTURE_COMPARE_A_IRQn) = ELC_EVENT_GPT2_CAPTURE_COMPARE_A;
    NVIC_SetPriority(GPT2_CAPTURE_COMPARE_A_IRQn, BSP_IRQ_CFG_GPT2_CAPTURE_COMPARE_A);
#endif
#if (BSP_IRQ_CFG_GPT2_CAPTURE_COMPARE_B != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT2_CAPTURE_COMPARE_B_IRQn) = ELC_EVENT_GPT2_CAPTURE_COMPARE_B;
    NVIC_SetPriority(GPT2_CAPTURE_COMPARE_B_IRQn, BSP_IRQ_CFG_GPT2_CAPTURE_COMPARE_B);
#endif
#if (BSP_IRQ_CFG_GPT2_COMPARE_C != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT2_COMPARE_C_IRQn) = ELC_EVENT_GPT2_COMPARE_C;
    NVIC_SetPriority(GPT2_COMPARE_C_IRQn, BSP_IRQ_CFG_GPT2_COMPARE_C);
#endif
#if (BSP_IRQ_CFG_GPT2_COMPARE_D != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT2_COMPARE_D_IRQn) = ELC_EVENT_GPT2_COMPARE_D;
    NVIC_SetPriority(GPT2_COMPARE_D_IRQn, BSP_IRQ_CFG_GPT2_COMPARE_D);
#endif
#if (BSP_IRQ_CFG_GPT2_COUNTER_OVERFLOW != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT2_COUNTER_OVERFLOW_IRQn) = ELC_EVENT_GPT2_COUNTER_OVERFLOW;
    NVIC_SetPriority(GPT2_COUNTER_OVERFLOW_IRQn, BSP_IRQ_CFG_GPT2_COUNTER_OVERFLOW);
#endif
#if (BSP_IRQ_CFG_GPT2_COUNTER_UNDERFLOW != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT2_COUNTER_UNDERFLOW_IRQn) = ELC_EVENT_GPT2_COUNTER_UNDERFLOW;
    NVIC_SetPriority(GPT2_COUNTER_UNDERFLOW_IRQn, BSP_IRQ_CFG_GPT2_COUNTER_UNDERFLOW);
#endif
#if (BSP_IRQ_CFG_GPT3_CAPTURE_COMPARE_A != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT3_CAPTURE_COMPARE_A_IRQn) = ELC_EVENT_GPT3_CAPTURE_COMPARE_A;
    NVIC_SetPriority(GPT3_CAPTURE_COMPARE_A_IRQn, BSP_IRQ_CFG_GPT3_CAPTURE_COMPARE_A);
#endif
#if (BSP_IRQ_CFG_GPT3_CAPTURE_COMPARE_B != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT3_CAPTURE_COMPARE_B_IRQn) = ELC_EVENT_GPT3_CAPTURE_COMPARE_B;
    NVIC_SetPriority(GPT3_CAPTURE_COMPARE_B_IRQn, BSP_IRQ_CFG_GPT3_CAPTURE_COMPARE_B);
#endif
#if (BSP_IRQ_CFG_GPT3_COMPARE_C != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT3_COMPARE_C_IRQn) = ELC_EVENT_GPT3_COMPARE_C;
    NVIC_SetPriority(GPT3_COMPARE_C_IRQn, BSP_IRQ_CFG_GPT3_COMPARE_C);
#endif
#if (BSP_IRQ_CFG_GPT3_COMPARE_D != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT3_COMPARE_D_IRQn) = ELC_EVENT_GPT3_COMPARE_D;
    NVIC_SetPriority(GPT3_COMPARE_D_IRQn, BSP_IRQ_CFG_GPT3_COMPARE_D);
#endif
#if (BSP_IRQ_CFG_GPT3_COUNTER_OVERFLOW != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT3_COUNTER_OVERFLOW_IRQn) = ELC_EVENT_GPT3_COUNTER_OVERFLOW;
    NVIC_SetPriority(GPT3_COUNTER_OVERFLOW_IRQn, BSP_IRQ_CFG_GPT3_COUNTER_OVERFLOW);
#endif
#if (BSP_IRQ_CFG_GPT3_COUNTER_UNDERFLOW != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT3_COUNTER_UNDERFLOW_IRQn) = ELC_EVENT_GPT3_COUNTER_UNDERFLOW;
    NVIC_SetPriority(GPT3_COUNTER_UNDERFLOW_IRQn, BSP_IRQ_CFG_GPT3_COUNTER_UNDERFLOW);
#endif
#if (BSP_IRQ_CFG_GPT4_CAPTURE_COMPARE_A != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT4_CAPTURE_COMPARE_A_IRQn) = ELC_EVENT_GPT4_CAPTURE_COMPARE_A;
    NVIC_SetPriority(GPT4_CAPTURE_COMPARE_A_IRQn, BSP_IRQ_CFG_GPT4_CAPTURE_COMPARE_A);
#endif
#if (BSP_IRQ_CFG_GPT4_CAPTURE_COMPARE_B != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT4_CAPTURE_COMPARE_B_IRQn) = ELC_EVENT_GPT4_CAPTURE_COMPARE_B;
    NVIC_SetPriority(GPT4_CAPTURE_COMPARE_B_IRQn, BSP_IRQ_CFG_GPT4_CAPTURE_COMPARE_B);
#endif
#if (BSP_IRQ_CFG_GPT4_COMPARE_C != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT4_COMPARE_C_IRQn) = ELC_EVENT_GPT4_COMPARE_C;
    NVIC_SetPriority(GPT4_COMPARE_C_IRQn, BSP_IRQ_CFG_GPT4_COMPARE_C);
#endif
#if (BSP_IRQ_CFG_GPT4_COMPARE_D != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT4_COMPARE_D_IRQn) = ELC_EVENT_GPT4_COMPARE_D;
    NVIC_SetPriority(GPT4_COMPARE_D_IRQn, BSP_IRQ_CFG_GPT4_COMPARE_D);
#endif
#if (BSP_IRQ_CFG_GPT4_COUNTER_OVERFLOW != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT4_COUNTER_OVERFLOW_IRQn) = ELC_EVENT_GPT4_COUNTER_OVERFLOW;
    NVIC_SetPriority(GPT4_COUNTER_OVERFLOW_IRQn, BSP_IRQ_CFG_GPT4_COUNTER_OVERFLOW);
#endif
#if (BSP_IRQ_CFG_GPT4_COUNTER_UNDERFLOW != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT4_COUNTER_UNDERFLOW_IRQn) = ELC_EVENT_GPT4_COUNTER_UNDERFLOW;
    NVIC_SetPriority(GPT4_COUNTER_UNDERFLOW_IRQn, BSP_IRQ_CFG_GPT4_COUNTER_UNDERFLOW);
#endif
#if (BSP_IRQ_CFG_GPT5_CAPTURE_COMPARE_A != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT5_CAPTURE_COMPARE_A_IRQn) = ELC_EVENT_GPT5_CAPTURE_COMPARE_A;
    NVIC_SetPriority(GPT5_CAPTURE_COMPARE_A_IRQn, BSP_IRQ_CFG_GPT5_CAPTURE_COMPARE_A);
#endif
#if (BSP_IRQ_CFG_GPT5_CAPTURE_COMPARE_B != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT5_CAPTURE_COMPARE_B_IRQn) = ELC_EVENT_GPT5_CAPTURE_COMPARE_B;
    NVIC_SetPriority(GPT5_CAPTURE_COMPARE_B_IRQn, BSP_IRQ_CFG_GPT5_CAPTURE_COMPARE_B);
#endif
#if (BSP_IRQ_CFG_GPT5_COMPARE_C != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT5_COMPARE_C_IRQn) = ELC_EVENT_GPT5_COMPARE_C;
    NVIC_SetPriority(GPT5_COMPARE_C_IRQn, BSP_IRQ_CFG_GPT5_COMPARE_C);
#endif
#if (BSP_IRQ_CFG_GPT5_COMPARE_D != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT5_COMPARE_D_IRQn) = ELC_EVENT_GPT5_COMPARE_D;
    NVIC_SetPriority(GPT5_COMPARE_D_IRQn, BSP_IRQ_CFG_GPT5_COMPARE_D);
#endif
#if (BSP_IRQ_CFG_GPT5_COUNTER_OVERFLOW != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT5_COUNTER_OVERFLOW_IRQn) = ELC_EVENT_GPT5_COUNTER_OVERFLOW;
    NVIC_SetPriority(GPT5_COUNTER_OVERFLOW_IRQn, BSP_IRQ_CFG_GPT5_COUNTER_OVERFLOW);
#endif
#if (BSP_IRQ_CFG_GPT5_COUNTER_UNDERFLOW != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT5_COUNTER_UNDERFLOW_IRQn) = ELC_EVENT_GPT5_COUNTER_UNDERFLOW;
    NVIC_SetPriority(GPT5_COUNTER_UNDERFLOW_IRQn, BSP_IRQ_CFG_GPT5_COUNTER_UNDERFLOW);
#endif
#if (BSP_IRQ_CFG_GPT6_CAPTURE_COMPARE_A != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT6_CAPTURE_COMPARE_A_IRQn) = ELC_EVENT_GPT6_CAPTURE_COMPARE_A;
    NVIC_SetPriority(GPT6_CAPTURE_COMPARE_A_IRQn, BSP_IRQ_CFG_GPT6_CAPTURE_COMPARE_A);
#endif
#if (BSP_IRQ_CFG_GPT6_CAPTURE_COMPARE_B != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT6_CAPTURE_COMPARE_B_IRQn) = ELC_EVENT_GPT6_CAPTURE_COMPARE_B;
    NVIC_SetPriority(GPT6_CAPTURE_COMPARE_B_IRQn, BSP_IRQ_CFG_GPT6_CAPTURE_COMPARE_B);
#endif
#if (BSP_IRQ_CFG_GPT6_COMPARE_C != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT6_COMPARE_C_IRQn) = ELC_EVENT_GPT6_COMPARE_C;
    NVIC_SetPriority(GPT6_COMPARE_C_IRQn, BSP_IRQ_CFG_GPT6_COMPARE_C);
#endif
#if (BSP_IRQ_CFG_GPT6_COMPARE_D != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT6_COMPARE_D_IRQn) = ELC_EVENT_GPT6_COMPARE_D;
    NVIC_SetPriority(GPT6_COMPARE_D_IRQn, BSP_IRQ_CFG_GPT6_COMPARE_D);
#endif
#if (BSP_IRQ_CFG_GPT6_COUNTER_OVERFLOW != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT6_COUNTER_OVERFLOW_IRQn) = ELC_EVENT_GPT6_COUNTER_OVERFLOW;
    NVIC_SetPriority(GPT6_COUNTER_OVERFLOW_IRQn, BSP_IRQ_CFG_GPT6_COUNTER_OVERFLOW);
#endif
#if (BSP_IRQ_CFG_GPT6_COUNTER_UNDERFLOW != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)GPT6_COUNTER_UNDERFLOW_IRQn) = ELC_EVENT_GPT6_COUNTER_UNDERFLOW;
    NVIC_SetPriority(GPT6_COUNTER_UNDERFLOW_IRQn, BSP_IRQ_CFG_GPT6_COUNTER_UNDERFLOW);
#endif
#if (BSP_IRQ_CFG_OPS_UVW_EDGE != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)OPS_UVW_EDGE_IRQn) = ELC_EVENT_OPS_UVW_EDGE;
    NVIC_SetPriority(OPS_UVW_EDGE_IRQn, BSP_IRQ_CFG_OPS_UVW_EDGE);
#endif
#if (BSP_IRQ_CFG_SCI0_RXI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)SCI0_RXI_IRQn) = ELC_EVENT_SCI0_RXI;
    NVIC_SetPriority(SCI0_RXI_IRQn, BSP_IRQ_CFG_SCI0_RXI);
#endif
#if (BSP_IRQ_CFG_SCI0_TXI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)SCI0_TXI_IRQn) = ELC_EVENT_SCI0_TXI;
    NVIC_SetPriority(SCI0_TXI_IRQn, BSP_IRQ_CFG_SCI0_TXI);
#endif
#if (BSP_IRQ_CFG_SCI0_TEI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)SCI0_TEI_IRQn) = ELC_EVENT_SCI0_TEI;
    NVIC_SetPriority(SCI0_TEI_IRQn, BSP_IRQ_CFG_SCI0_TEI);
#endif
#if (BSP_IRQ_CFG_SCI0_ERI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)SCI0_ERI_IRQn) = ELC_EVENT_SCI0_ERI;
    NVIC_SetPriority(SCI0_ERI_IRQn, BSP_IRQ_CFG_SCI0_ERI);
#endif
#if (BSP_IRQ_CFG_SCI0_AM != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)SCI0_AM_IRQn) = ELC_EVENT_SCI0_AM;
    NVIC_SetPriority(SCI0_AM_IRQn, BSP_IRQ_CFG_SCI0_AM);
#endif
#if (BSP_IRQ_CFG_SCI0_RXI_OR_ERI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)SCI0_RXI_OR_ERI_IRQn) = ELC_EVENT_SCI0_RXI_OR_ERI;
    NVIC_SetPriority(SCI0_RXI_OR_ERI_IRQn, BSP_IRQ_CFG_SCI0_RXI_OR_ERI);
#endif
#if (BSP_IRQ_CFG_SCI1_RXI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)SCI1_RXI_IRQn) = ELC_EVENT_SCI1_RXI;
    NVIC_SetPriority(SCI1_RXI_IRQn, BSP_IRQ_CFG_SCI1_RXI);
#endif
#if (BSP_IRQ_CFG_SCI1_TXI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)SCI1_TXI_IRQn) = ELC_EVENT_SCI1_TXI;
    NVIC_SetPriority(SCI1_TXI_IRQn, BSP_IRQ_CFG_SCI1_TXI);
#endif
#if (BSP_IRQ_CFG_SCI1_TEI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)SCI1_TEI_IRQn) = ELC_EVENT_SCI1_TEI;
    NVIC_SetPriority(SCI1_TEI_IRQn, BSP_IRQ_CFG_SCI1_TEI);
#endif
#if (BSP_IRQ_CFG_SCI1_ERI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)SCI1_ERI_IRQn) = ELC_EVENT_SCI1_ERI;
    NVIC_SetPriority(SCI1_ERI_IRQn, BSP_IRQ_CFG_SCI1_ERI);
#endif
#if (BSP_IRQ_CFG_SCI1_AM != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)SCI1_AM_IRQn) = ELC_EVENT_SCI1_AM;
    NVIC_SetPriority(SCI1_AM_IRQn, BSP_IRQ_CFG_SCI1_AM);
#endif
#if (BSP_IRQ_CFG_SCI9_RXI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)SCI9_RXI_IRQn) = ELC_EVENT_SCI9_RXI;
    NVIC_SetPriority(SCI9_RXI_IRQn, BSP_IRQ_CFG_SCI9_RXI);
#endif
#if (BSP_IRQ_CFG_SCI9_TXI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)SCI9_TXI_IRQn) = ELC_EVENT_SCI9_TXI;
    NVIC_SetPriority(SCI9_TXI_IRQn, BSP_IRQ_CFG_SCI9_TXI);
#endif
#if (BSP_IRQ_CFG_SCI9_TEI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)SCI9_TEI_IRQn) = ELC_EVENT_SCI9_TEI;
    NVIC_SetPriority(SCI9_TEI_IRQn, BSP_IRQ_CFG_SCI9_TEI);
#endif
#if (BSP_IRQ_CFG_SCI9_ERI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)SCI9_ERI_IRQn) = ELC_EVENT_SCI9_ERI;
    NVIC_SetPriority(SCI9_ERI_IRQn, BSP_IRQ_CFG_SCI9_ERI);
#endif
#if (BSP_IRQ_CFG_SCI9_AM != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)SCI9_AM_IRQn) = ELC_EVENT_SCI9_AM;
    NVIC_SetPriority(SCI9_AM_IRQn, BSP_IRQ_CFG_SCI9_AM);
#endif
#if (BSP_IRQ_CFG_SPI0_RXI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)SPI0_RXI_IRQn) = ELC_EVENT_SPI0_RXI;
    NVIC_SetPriority(SPI0_RXI_IRQn, BSP_IRQ_CFG_SPI0_RXI);
#endif
#if (BSP_IRQ_CFG_SPI0_TXI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)SPI0_TXI_IRQn) = ELC_EVENT_SPI0_TXI;
    NVIC_SetPriority(SPI0_TXI_IRQn, BSP_IRQ_CFG_SPI0_TXI);
#endif
#if (BSP_IRQ_CFG_SPI0_IDLE != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)SPI0_IDLE_IRQn) = ELC_EVENT_SPI0_IDLE;
    NVIC_SetPriority(SPI0_IDLE_IRQn, BSP_IRQ_CFG_SPI0_IDLE);
#endif
#if (BSP_IRQ_CFG_SPI0_ERI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)SPI0_ERI_IRQn) = ELC_EVENT_SPI0_ERI;
    NVIC_SetPriority(SPI0_ERI_IRQn, BSP_IRQ_CFG_SPI0_ERI);
#endif
#if (BSP_IRQ_CFG_SPI0_TEI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)SPI0_TEI_IRQn) = ELC_EVENT_SPI0_TEI;
    NVIC_SetPriority(SPI0_TEI_IRQn, BSP_IRQ_CFG_SPI0_TEI);
#endif
#if (BSP_IRQ_CFG_SPI1_RXI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)SPI1_RXI_IRQn) = ELC_EVENT_SPI1_RXI;
    NVIC_SetPriority(SPI1_RXI_IRQn, BSP_IRQ_CFG_SPI1_RXI);
#endif
#if (BSP_IRQ_CFG_SPI1_TXI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)SPI1_TXI_IRQn) = ELC_EVENT_SPI1_TXI;
    NVIC_SetPriority(SPI1_TXI_IRQn, BSP_IRQ_CFG_SPI1_TXI);
#endif
#if (BSP_IRQ_CFG_SPI1_IDLE != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)SPI1_IDLE_IRQn) = ELC_EVENT_SPI1_IDLE;
    NVIC_SetPriority(SPI1_IDLE_IRQn, BSP_IRQ_CFG_SPI1_IDLE);
#endif
#if (BSP_IRQ_CFG_SPI1_ERI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)SPI1_ERI_IRQn) = ELC_EVENT_SPI1_ERI;
    NVIC_SetPriority(SPI1_ERI_IRQn, BSP_IRQ_CFG_SPI1_ERI);
#endif
#if (BSP_IRQ_CFG_SPI1_TEI != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)SPI1_TEI_IRQn) = ELC_EVENT_SPI1_TEI;
    NVIC_SetPriority(SPI1_TEI_IRQn, BSP_IRQ_CFG_SPI1_TEI);
#endif
#if (BSP_IRQ_CFG_AES_WRREQ != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)AES_WRREQ_IRQn) = ELC_EVENT_AES_WRREQ;
    NVIC_SetPriority(AES_WRREQ_IRQn, BSP_IRQ_CFG_AES_WRREQ);
#endif
#if (BSP_IRQ_CFG_AES_RDREQ != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)AES_RDREQ_IRQn) = ELC_EVENT_AES_RDREQ;
    NVIC_SetPriority(AES_RDREQ_IRQn, BSP_IRQ_CFG_AES_RDREQ);
#endif
#if (BSP_IRQ_CFG_TRNG_RDREQ != BSP_IRQ_DISABLED)
    *(base_addr + (uint32_t)TRNG_RDREQ_IRQn) = ELC_EVENT_TRNG_RDREQ;
    NVIC_SetPriority(TRNG_RDREQ_IRQn, BSP_IRQ_CFG_TRNG_RDREQ);
#endif
}

#endif



