/*
 * timer.h
 *
 *  Created on: Jan 12, 2017
 *      Author: c0879252
 */

#ifndef TIMER_H_
#define TIMER_H_

void gTimer_ISR(timer_callback_args_t * p_args);

void powerLed();
void lasIni(int counter);
#endif /* TIMER_H_ */
