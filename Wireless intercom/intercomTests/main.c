#include "stm_lib/inc/stm32f0xx_gpio.h"
#include "stm_lib/inc/stm32f0xx_rcc.h"
#include "stm_lib/inc/stm32f0xx_exti.h"
#include "stm_lib/inc/stm32f0xx_syscfg.h"
#include"stm_lib/inc/stm32f0xx_misc.h"
#include "stm_lib/inc/stm32f0xx_usart.h"
#include <stdio.h>
#include <string.h>
#include "rfm70.h"
#include "usart.h"
//#include "adc_tim.h"
void USART_SendString(char * string);
void my_receive_packet(void * pBuf, uint8_t len); // deklarujemy w³asn¹ funkcjê do analizy odebranych pakietów
void TIM1_Conf(void);
uint8_t i;
char buf[] = { "the reason why you're stupid is too much sun" };
int main(void) {

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	GPIOC->MODER |= GPIO_MODER_MODER8_0 | GPIO_MODER_MODER9_0; // ustawiam wyjscie na diode PC8, a PC9 mi tylko sygnalizuje kiedy wysylam ramke
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8 | GPIO_OSPEEDER_OSPEEDR9; // 50 MHz
	GPIOC->ODR |= GPIO_Pin_8 | GPIO_Pin_9; //1 na wyjsciu GPIOC 8
	TIM1_Conf();
	HyperTerminal_Interrupt(); // inicjalizacja terminala
	RFM70_Init();
	SetChannelNum(41);  // ustawiam kanal
	register_rfm70_rx_event_callback(my_receive_packet); // rejestrujemy w³asn¹ funkcjê do analizy/odbioru pakietów
	while (1) {
		RFM70_RX_EVENT(); // zdarzenie odbiorcze z RFM70 ca³y czas dzia³a odbiór
	}

}
// funkcja do analizy odebranych pakietów////////////////////////////////////////////////////////////////////////////////////////////////
void my_receive_packet(void * pBuf, uint8_t len) {
	GPIOC->ODR ^= GPIO_Pin_8; // migamy se diod¹ LED
	USART_SendString(pBuf); // ka¿dy odebrany string przesy³amy na UART
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//funkcja wysylajaca stringi po usarcienr 1
void USART_SendString(char * string) {
	char c;
	while ((c = *string++)) { // dopóki nie napotkasz 0 wysy³aj znak
		for (i = 0; i < 255; i++) {
		}; // petla opozniajaca bo tak to mi putty sie dupcy
		USART_SendData(USART1, c);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
void TIM1_CC_IRQHandler(void) { // obsluga przerwania od timera 1
	if (TIM1->SR &= TIM_SR_CC1IF) { // jeli wystapi przerwanie to
		GPIOC->ODR ^= GPIO_Pin_9;
		Send_Packet(W_TX_PAYLOAD_NOACK_CMD, &buf[0],32);
		TIM1->SR &= ~(TIM_SR_CC1IF); // czyszcze flage
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TIM1_Conf() { ///konfiguracja timera 1 co sekunda palic go bede
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	//TIM1->CR1 &= ~(TIM_CR1_DIR); // upcounter
	//TIM1->CNT=0;
	TIM1->PSC = 7999; // CLK =48MHz/ (7999+1) = 8KHz
	//TIM1->CR1 |= TIM_CR1_ARPE; // ARR is buffered
	TIM1->BDTR = 0;
	//TIM1->CR1 |= 0; // clock division
	TIM1->ARR = 1000; // period czyli wartosc do ktorej zlicza licznik / po tym sie automatycznie przepelnia // czyli co 1/8 sekundy
	//TIM1->CCR1 = 100; // czyli tyknie co sekunde;
	TIM1->DIER |= TIM_DIER_CC1IE; // zezwolenie na przerwania
	TIM1->CCMR1 |= TIM_CCMR1_OC1PE; //tryb compare match
	//TIM1->EGR|=TIM_EGR_CC1G; //pale tryb compare match
	TIM1->CR1 |= TIM_CR1_CEN; //  counter enable
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
*/
