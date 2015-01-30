#include "adc_tim.h"
#include "stm_lib/inc/stm32f0xx_gpio.h"
#include "stm_lib/inc/stm32f0xx_rcc.h"
#include "stm_lib/inc/stm32f0xx_adc.h"
#include "stm_lib/inc/stm32f0xx_tim.h"
#include "stm_lib/inc/stm32f0xx_misc.h"
#include "stm_lib/inc/stm32f0xx_syscfg.h"
volatile uint8_t full_frameADC_received;
volatile uint8_t num_buf;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////                 ADC Configuration         /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ADC1_COMP_IRQHandler(void) {
	//if (ADC1->ISR & ADC_ISR_EOC) { // jeli stan wysoki
	//GPIOC->ODR ^= GPIO_ODR_9; // mrygam zielona co 1 sek

	static uint8_t sample_num;
	ADC1ConvertedValue = ADC1->DR;
	// flaga uzywana w mainie
//////	DAC->DHR12R1 = ADC1ConvertedValue;   // zarzucam na DAC
	buf_ADC[num_buf][sample_num++] = ADC1ConvertedValue;
	if (sample_num > 31) {
		sample_num = 0;
		num_buf ^= 1; // zmieniam bufor poprzedni wysy³am
		full_frameADC_received = 1;
	}
///	DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;
	// tutaj tablica z próbkami 12 bitowymi z ADC, wyrownane do prawej strony
//	sprintf(buf, "wart ADC: %u", ADC1ConvertedValue);
//	if (!Send_Packet(W_TX_PAYLOAD_NOACK_CMD, &buf[0], strlen(buf))) {
//		GPIOC->ODR ^= GPIO_Pin_9;
//	}
	//ADC1->ISR &= ~(ADC_ISR_EOC);
//	}

}

/////////////////////////////////////////////////////////////////////////////

void ADC_TIM_Config(void) // inicjalizacja timera i ADC wyzwalanego prze niego
{
	ADC_InitTypeDef ADC_InitStructure;
	///GPIO_InitTypeDef GPIO_InitStructure;
	// GPIOC Periph clock enable
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

	// ADC1 and Periph clock enable
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	// Configure ADC Channel11 as analog input
	GPIOC->MODER |= GPIO_MODER_MODER1; // mode_analog, No pullup and pulldown

//TIM2_Conf(); // TIM2 Configuration
	TIM1_Conf(); // TIM1 Configuration

	ADC_DeInit(ADC1);
	ADC_StructInit(&ADC_InitStructure); // Initialize ADC structure //
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; // Configure the ADC1 in continous mode withe a resolutuion equal to 12 bits
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC4;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_ChannelConfig(ADC1, ADC_Channel_11, ADC_SampleTime_28_5Cycles); // Convert the ADC1 Channel 11 with 239.5 Cycles as sampling time//
	ADC1->IER |= ADC_IER_EOCIE; // odpalam przerwanie
	ADC_GetCalibrationFactor(ADC1); // ADC Calibration//
	ADC_Cmd(ADC1, ENABLE); // Enable ADCperipheral[PerIdx]
	while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN))
		; // Wait the ADCEN falg
	ADC_StartOfConversion(ADC1); // ADC1 regular Software Start Conv//
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_COMP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////                     TIMER 1            ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TIM1_CC_IRQHandler(void) { // obsluga przerwania od timera 1
	if (TIM1->SR & TIM_SR_CC4IF) { // jeli wyst¹pi przerwanie to
//ADC_StartOfConversion(ADC1); // ADC1 regular Software Start Conv//
	}
	TIM1->SR &= ~(TIM_SR_CC4IF); // czyszcze wszystkie flagi
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TIM1_Conf() { ///konfiguracja timera 1 co sekunda palic go bede taktowany 44,1 Khz
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	TIM1->PSC = 479 / 10; // CLK =48MHz/ (7999+1) = 100KHz
	//TIM1->CR1 |= TIM_CR1_ARPE; // ARR is buffered
	TIM1->BDTR |= TIM_BDTR_MOE; // Enable the TIM Main Output
	//TIM1->CR1 |= 0; // clock division
	TIM1->ARR = 10; // period czyli wartosc do ktorej zlicza licznik / po tym sie automatycznie przepelnia
	TIM1->CCR4 = 9;
	TIM1->DIER |= TIM_DIER_CC4IE; // zezwolenie na przerwania
	TIM1->CCMR2 |= TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1; //PWM mode 1 - In upcounting, channel 1 is active as long as TIMx_CNT<TIMx_CCR1 	else inactive.
	//TIM1->CCER|=TIM_CCER_CC4P;  // output polarity deafault : active High
	TIM1->CCER |= TIM_CCER_CC4E; //On - OC1N signal is output on the corresponding output pin depending on MOE, OSSI, OSSR, OIS1, OIS1N and CC1E bits.
	TIM1->CR1 |= TIM_CR1_CEN; //  counter enable
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//static __INLINE void NVIC_EnableIRQ(IRQn_Type IRQn)
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////                                  TIMER 2                         //////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TIM2_IRQHandler(void) { // obsluga przerwania od timera 2

	if (TIM2->SR & TIM_SR_CC1IF) {
		GPIOC->ODR ^= GPIO_ODR_9; // mrygam zielona co 1 sek
	}
	TIM2->SR &= ~(TIM_SR_CC1IF);

	if (TIM2->SR & TIM_SR_TIF) {
		//GPIOC->ODR ^= GPIO_ODR_9; // mrygam zielona co 1 sek
	}
	TIM2->SR &= ~(TIM_SR_TIF);

	if (TIM2->SR & TIM_SR_UIF) {
		//GPIOC->ODR ^= GPIO_ODR_9; // mrygam zielona co 1 sek
	}
	TIM2->SR &= ~(TIM_SR_UIF);

}

//---------------------------------------------------------------------------------------------------------------------------------------
void TIM2_Conf() {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //RCC->APB1ENR|=RCC_APB1ENR_TIM2EN;
	TIM2->PSC = 479; // CLK =8MHz/ (7999+1) = 1KHz
	//TIM2->CR1 |= TIM_CR1_ARPE; // ARR is buffered
	TIM2->BDTR = 0;
	TIM2->ARR = 100; // period czyli wartosc do ktorej zlicza licznik / po tym sie automatycznie przepelnia
	//TIM2->CCR1 = 200; // czyli tyknie co sekunde;
	TIM2->DIER |= TIM_DIER_CC1IE | TIM_DIER_TIE | TIM_DIER_UIE; // odpalam przerwania
	//TIM2->CCMR1 |= TIM_CCMR1_OC1PE; //tryb compare match
	TIM2->CR2 |= TIM_CR2_MMS_1 | TIM_CR2_MMS_0; // Compare Pulse - The trigger output send a positive pulse when the CC1IF flag is to be //set (even if it was already high), as soon as a capture or a compare match occurred. (TRGO)
	//TIM2->CCMR1 |= TIM_CCMR1_OC1M; //PWM mode 2 - In upcounting, channel 1 is inactive as long as TIMx_CNT<TIMx_CCR1 else active.
	//TIM2->CCER|=TIM_CCER_CC1E; //signal is output on the corresponding output pin
	TIM2->CR1 |= TIM_CR1_CEN; // | TIM_CR1_URS; //  counter enable ,,Only counter overflow/underflow generates an update interrupt or DMA request if enabled.
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DAC_Conf(void) { // bedzie na PA4 trzeba ustawiæ je na AIN3
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC | RCC_APB1Periph_TIM6, ENABLE); // tim6 dla DAC
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	//conf tim6 , it'll be trigger in 44,1kHz and enable to DAC

	TIM6->PSC = 107; // CLK =48MHz/ (107+1) = 444,44KHz
	TIM6->ARR = 10; // period czyli wartosc do ktorej zlicza licznik / po tym sie automatycznie przepelnia czest 44.1 KHZ
	//TIM2->CCMR1 |= TIM_CCMR1_OC1PE; //tryb compare match
	TIM6->CR2 |= TIM_CR2_MMS_1; // The update event is selected as a trigger output (TRGO).)
	//TIM2->CCMR1 |= TIM_CCMR1_OC1M; //PWM mode 2 - In upcounting, channel 1 is inactive as long as TIMx_CNT<TIMx_CCR1 else active.
	//TIM2->CCER|=TIM_CCER_CC1E; //signal is output on the corresponding output pin
	//TIM6->CR1 |= TIM_CR1_CEN; // | TIM_CR1_URS; //  counter enable ,,Only counter overflow/underflow generates an update interrupt or DMA request if enabled.
	// TIM6->SR&=~(TIM_SR_UIF); //czyszcze flage po wyst zdazenia
	GPIOA->MODER |= GPIO_MODER_MODER4; // uruchamiam na PA4 DAC
	DAC->CR |= DAC_CR_TEN1; // trigger from underflow from timer6 timerem2, enable output bufor
	//DAC->CR |= DAC_CR_EN1; // channel1 enabled
}
/*
void Enable_TIM6_DAC(void){DAC->CR |= DAC_CR_EN1;
TIM6->CR1 |= TIM_CR1_CEN; // channel1 enabled
}



void TIM6_DAC_IRQHandler(void) {
	if (TIM6->SR & (TIM_SR_UIF)) {
		TIM6->SR &= ~(TIM_SR_UIF);
	}
}
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *
 void DMA_Conf(void) {
 #define DAC_DHR12R1_ADDRESS      0x40007408
 DMA_InitTypeDef            DMA_InitStructure;
 // DMA channel3 Configuration
 DMA_DeInit(DMA1_Channel3);
 DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12R1_ADDRESS;
 DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&Sine12bit;
 DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
 DMA_InitStructure.DMA_BufferSize = 32;
 DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
 DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
 DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
 DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
 DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
 DMA_InitStructure.DMA_Priority = DMA_Priority_High;
 DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
 DMA_Init(DMA1_Channel3, &DMA_InitStructure);

 // Enable DMA1 Channel3 //
 DMA_Cmd(DMA1_Channel3, ENABLE);

 }
 */
