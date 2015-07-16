/*
 * init_modules.h
 *
 *  Created on: Dec 23, 2011
 *      Author: lukasz
 */
#include "stm32f10x.h"
#ifndef INIT_MODULES_H_
#define INIT_MODULES_H_

vu32 TimeDelay;
void Systick_Config(void);  ///deklaracje funkcji
void Delay(__IO uint32_t Time);
void RCC_Config(void);
void GPIO_Config(void);
void EXTI_Config(void);
void TIMER_Config(void);
void TIMER_PWM_Config(void);
void NVIC_Config(void);
#endif /* INIT_MODULES_H_ */
