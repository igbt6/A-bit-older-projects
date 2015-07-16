/**
 ******************************************************************************
 * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c
 * @author  MCD Application Team
 * @version V3.5.0
 * @date    08-April-2011
 * @brief   Main Interrupt Service Routines.
 *          This file provides template for all exceptions handler and
 *          peripherals interrupt service routine.
 ******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "stdbool.h"
#define INTRO 1
#define GRA 2
#define GAMEOVER 3
#define PAUZA 4
/** @addtogroup STM32F10x_StdPeriph_Template
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief  This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void) {
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void) {
	/* Go to infinite loop when Hard Fault exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void) {
	/* Go to infinite loop when Memory Manage exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void) {
	/* Go to infinite loop when Bus Fault exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void) {
	/* Go to infinite loop when Usage Fault exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
void SVC_Handler(void) {
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void) {
}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval None
 */
void PendSV_Handler(void) {
}
extern void CzyscPlansze(void);
extern void KlocekObroc(void);
extern void KlocekWklej(void);
extern void KlocekNowy(void);
extern void KlocekLewo(void);
extern void KlocekPrawo(void);
extern void KlocekDol(void);
extern void OdswiezEkran(void);
extern unsigned char TimerGry(void);
extern void GLCD_ClearScreen(void); //czyszczenie tla
extern void GLCD_ClearPreviousKlocek(void);

extern volatile unsigned char rkloceknr; //zmienne zainicjalizowane w main.c
extern volatile unsigned char stan_gry;
extern volatile unsigned int punkty, linie, level;
extern volatile unsigned char sek, min;
extern volatile bool flag_tlo;
extern volatile bool refresh;
volatile bool dol = false;
extern vu32 TimeDelay;

//Obsluga przerwania od przyciskow joysticko
void EXTI15_10_IRQHandler(void) {
	EXTI->FTSR &= ~(EXTI_FTSR_TR13); // maskuje przerwanie od tego pinu
	// to do debouncu słuzy // elim dragania styków
	if (EXTI_GetITStatus(EXTI_Line13) != RESET) //sprawdz z ktorego pinu pochodzi przerwanie
			{

		if (stan_gry == GRA) //Jezeli gra wlaczona
		{
			KlocekLewo(); //przesun klocek w lewo
			OdswiezEkran();
		}

		; ///////////////////////////////////////////////////////////////////////////////////////////////////////////
		EXTI_ClearITPendingBit(EXTI_Line13); //zeruj zrodlo przerwania
	}
	EXTI->FTSR |= EXTI_FTSR_TR13; // wlaczam z powrotem

}

void EXTI1_IRQHandler(void) {

	EXTI->FTSR &= ~(EXTI_FTSR_TR1);

	//Obsluga przerwania- przycisk w prawo
	if (EXTI_GetITStatus(EXTI_Line1) != RESET) //sprawdz z którego pinu pochodzi przerwanie
			{

		if (stan_gry == GRA) //Jezeli gra wlaczona
		{
			KlocekPrawo(); //przesun klocek w prawo
			OdswiezEkran();
		}

		EXTI_ClearITPendingBit(EXTI_Line1); //zeruj zrodlo przerwania
	}
	EXTI->FTSR |= EXTI_FTSR_TR1;

}
/*
void EXTI4_IRQHandler(void) {              // to przerwanie jest od opadania , nie uzywam narazie
	dol = true;
	//Obsluga przerwania - przycisk dol
	EXTI->FTSR &= ~(EXTI_FTSR_TR4);
	if (EXTI_GetITStatus(EXTI_Line4) != RESET) //sprawdz z którego pinu pochodzi przerwanie
			{
		if (stan_gry == GRA) //Jezeli gra wlaczona
		{
			KlocekDol(); //przesun klocek w dol
			//OdswiezEkran();
			KlocekDol(); //przesun klocek w dol
			OdswiezEkran();
		}
		EXTI_ClearITPendingBit(EXTI_Line4); //zeruj zrodlo przerwania
	}

	EXTI->FTSR |= EXTI_FTSR_TR4;
	dol = false;
}
*/
void EXTI3_IRQHandler(void) {
	EXTI->FTSR &= ~(EXTI_FTSR_TR3);
	if (EXTI_GetITStatus(EXTI_Line3) != RESET) //sprawdz z którego pinu pochodzi przerwanie
			{

		if (stan_gry == GRA) //Jezeli gra wlaczona
		{
			OdswiezEkran();
			KlocekObroc(); //przesun klocek w prawo
		}
		EXTI_ClearITPendingBit(EXTI_Line3); //zeruj zrodlo przerwania
	}
	EXTI->FTSR |= EXTI_FTSR_TR3;
}

void EXTI0_IRQHandler(void) {
	EXTI->FTSR &= ~(EXTI_FTSR_TR0);
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) //sprawdzam z którego pinu pochodzi przerwanie
			{

		switch (stan_gry) {
		case GRA: //Jezeli gra wlaczona
			GLCD_ClearScreen();

			//flag_tlo=false;
			stan_gry = PAUZA; //wlacz pauze
			break;

		case PAUZA: //Jezeli gra w czasie pauzy
			flag_tlo = false;
			stan_gry = GRA; //wróc do gry
			break;

		case INTRO: //Jezeli wlaczone intro
			//GLCD_ClearScreen();
			flag_tlo = false;
			///	GLCD_bmp(tlo);
			stan_gry = GRA;
			break;

		case GAMEOVER: //Jezeli nastapila przegrana
			CzyscPlansze(); //wyczysc plansze
			KlocekNowy(); //zmien klocek na nowy
			punkty = 0; //wyzeruj licznik punktów
			linie = 0; //i zaliczonych linii
			sek = 0;
			min = 0;
			level = 1;
			flag_tlo = false;
			stan_gry = INTRO; //przejdz do intro
			break;
		}

		EXTI_ClearITPendingBit(EXTI_Line0); //zeruj zrodlo przerwania
		OdswiezEkran();
	}
	EXTI->FTSR |= EXTI_FTSR_TR0;

}
/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */

void SysTick_Handler(void) {
	static uint8_t timer //,licznik
	;

//licznik++;
//if(licznik==2){GLCD_ClearPreviousKlocek();licznik =0; }//czyszczenie tla

	timer++;
	if (timer == 10) {
		sek++; //flag_timer=true;

		if (sek == 60) {
			sek = 0;
			min++;
		}
		timer = 0;
	}
	rkloceknr++; //losowanie klocków
	if (rkloceknr > 6)
		rkloceknr = 0;

//GLCD_ClearPreviousKlocek();
	if (TimerGry() == 1 && dol == false) //Funkcja SpeedTimer zwraca 1 co ustalon¹ ilosc wywolan
	{ //czestosc wywolanreguluje zmienna speed
		if (stan_gry == GRA) {
			OdswiezEkran();
			KlocekDol();
		} // jak jest gra t opousc klocek
	}
	static u32 n;
		n = TimeDelay;
		if (n > 0) {
			--n;
			TimeDelay = n;
		}
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
 * @brief  This function handles PPP interrupt request.
 * @param  None
 * @retval None
 */
/*void PPP_IRQHandler(void)
 {
 }*/

/**
 * @}
 */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
