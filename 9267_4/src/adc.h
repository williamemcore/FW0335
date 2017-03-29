/*
 * inputs.h
 *
 *  Created on: May 20, 2016
 *      Author: Dave
 */

#ifndef INPUTS_H_
#define INPUTS_H_

#include "bsp_api.h"

#include "hal_data.h"
void adc_thread_entry(void);
#include "r_adc.h"
#include "r_adc_api.h"

extern const adc_instance_t g_adc0;
#ifdef NULL
#define ADC_CALLBACK_USED_g_adc0 (0)
#else
#define ADC_CALLBACK_USED_g_adc0 (1)
#endif
#if ADC_CALLBACK_USED_g_adc0
void NULL(adc_callback_args_t * p_args);



#include "adc.h"
void inputs_get_sensor_values(adc_data_size_t *p_pot_data,adc_data_size_t *p_light_data, adc_data_size_t *p_temp_data);


#endif /* INPUTS_H_ */

void getAadcValues(adc_data_size_t *p_pot_data);


#endif /* ADC_THREAD_H_ */
#define counAdc     16

void adcIni();
void adcRead();
void adcSendUart();
int adcSend[12];
uint8_t lasPCaliGui[2];

