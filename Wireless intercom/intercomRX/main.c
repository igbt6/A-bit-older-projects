/*
 * domofon bezprzewodowy stm32f051r8 , zegar HSI 48MHz
 */

#include "stm_lib/inc/stm32f0xx_gpio.h"
#include "stm_lib/inc/stm32f0xx_rcc.h"
#include "stm_lib/inc/stm32f0xx_exti.h"
#include "stm_lib/inc/stm32f0xx_syscfg.h"
#include "stm_lib/inc/stm32f0xx_misc.h"
#include "stm_lib/inc/stm32f0xx_adc.h"
#include "stm_lib/inc/stm32f0xx_tim.h"
#include "rfm70.h"
#include "adc_tim.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stm32f0xx_dma.h"
#include <stdbool.h>
void my_receive_packet(void * pBuf, uint8_t len); // deklaracja  wĹ‚asnej funkcji do analizy odebranych pakietĂłw
uint32_t i;
volatile uint8_t full_frameADC_received;
volatile uint8_t num_buf;
volatile bool TX_RX;
int main(void) {

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	GPIOC->MODER |= GPIO_MODER_MODER8_0 | GPIO_MODER_MODER9_0; // ustawiam wyjscie na diode PC8
	//GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_8); // to tak dla jaj ze push pull wyjscie
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8 | GPIO_OSPEEDER_OSPEEDR9; // 50 MHz
	GPIOC->ODR |= GPIO_Pin_8 | GPIO_Pin_9; //1 na wyjsciu GPIOC 8
	RFM70_Init();
	//ADC1_DMA_Config();
	DAC_DMA_Config();
	//ADC_TIM_Config(); // inicjalizacja ADC wyzwalanego timerem
	//DAC_Conf();

	SetChannelNum(41); //  wybor kanalu czsstotliwosc 2400 - 2483.5 GHz
	register_rfm70_rx_event_callback(my_receive_packet); //rejestrujemy wĹ‚asnÄ… funkcjÄ™ do analizy/odbioru pakietĂłw

	//sprintf(buf, "wart ADC: %u", ADC1ConvertedValue);
	//Send_Packet(W_TX_PAYLOAD_NOACK_CMD, &buf[0], strlen(buf));
	while (1) {
	/*	switch (TX_RX) {
		case 0: {
			RFM70_RX_EVENT(); // zdarzenie odbiorcze z RFM70 	// domyslnie caly czas działa odbior
		}

		case 1: {
			if (full_frameADC_received == 1) {
			Send_Packet(W_TX_PAYLOAD_NOACK_CMD, &buf_ADC[num_buf ^ 1][0],
				SIZE_BUF_ADC);
				static uint16_t i;
				i++;
				if (i == 1000) {
					GPIOC->ODR ^= (GPIO_Pin_8); // migamy sobie diod¹ LED
					i = 0;
				}

				full_frameADC_received = 0;
				TX_RX ^= 1;
			} //else TX_RX ^= 1;
		}
		}
		*/
		RFM70_RX_EVENT(); // zdarzenie odbiorcze z RFM70
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// funkcja do analizy odebranych danych z rfma
void my_receive_packet(void * pBuf, uint8_t len) {
	static uint16_t i;
	memcpy(&buf_DAC[num_bufDAC][0], pBuf, len); // kopiuje sobie do mojego buforka
	DMA_Cmd(DMA1_Channel3, ENABLE);
	//flaga = 1;
			num_bufDAC ^= 1;
	TX_RX ^= 1; // przelacz
	i++;
	if (i == 15) {

		GPIOC->ODR ^= (GPIO_Pin_9); // migamy sobie diod¹ LED
		i = 0;
	}

}

