#include "adc_tim.h"
#include "stm_lib/inc/stm32f0xx_gpio.h"
#include "stm_lib/inc/stm32f0xx_rcc.h"
#include "stm_lib/inc/stm32f0xx_adc.h"
#include "stm_lib/inc/stm32f0xx_tim.h"
#include "stm_lib/inc/stm32f0xx_misc.h"
#include "stm_lib/inc/stm32f0xx_dac.h"
#include "stm32f0xx_dma.h"
#include "rfm70.h"
//volatile uint8_t full_frameADC_received;

volatile uint8_t num_buf;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////                 ADC Configuration         /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
void ADC1_COMP_IRQHandler(void) {

	static uint8_t sample_num;
	sample_num++;
	;
	if (sample_num > SIZE_BUF_DACADC - 1) {
		sample_num = 0;
		num_buf ^= 1; // zmieniam bufor poprzedni wysyÅ‚am
		full_frameADC_received = 1;
	}
	buf_ADC[num_buf][sample_num] = ADC1->DR; // ADC1ConvertedValue;

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
	TIM1_Conf(); // TIM1 Configuration

	ADC_DeInit(ADC1);
	ADC_StructInit(&ADC_InitStructure); // Initialize ADC structure //
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_8b; // Configure the ADC1 in continous mode withe a resolutuion equal to 12 bits
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; ///////----------------------------------------
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC4;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_ChannelConfig(ADC1, ADC_Channel_11, ADC_SampleTime_1_5Cycles); // Convert the ADC1 Channel 11 with 239.5 Cycles as sampling time//
	ADC1->IER |= ADC_IER_EOCIE; //----------------------------------------------------------------------------------------------------------- // odpalam przerwanie
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
void TIM1_IRQHandler(void) { // obsluga przerwania od timera 2

	if (TIM1->SR & TIM_SR_CC4IF) {
		static uint8_t sample_num;
		sample_num++;
		;
		if (sample_num > SIZE_BUF_DACADC - 1) {
			sample_num = 0;
			num_buf ^= 1; // zmieniam bufor poprzedni wysyÅ‚am
			full_frameADC_received = 1;
		}
		buf_ADC[num_buf][sample_num] = ADC1->DR; // ADC1ConvertedValue;
		TIM1->SR &= ~(TIM_SR_CC4IF);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TIM1_Conf() { ///konfiguracja timera 1 co sekunda palic go bede taktowany 44,1 Khz
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	TIM1->PSC = 0; // CLK =48MHz/ (7999+1) = 100KHz                   /////////////////////////////////////////////////////////////yu BYLO RUSZNE!!!!!!!!!!!!!!!!!!!!!!!
	//TIM1->CR1 |= TIM_CR1_ARPE; // ARR is buffered

	TIM1->BDTR |= TIM_BDTR_MOE; // Enable the TIM Main Output
	//TIM1->CR1 |= 0; // clock division
	TIM1->ARR = 2160; // period czyli wartosc do ktorej zlicza licznik / po tym sie automatycznie przepelnia
	TIM1->CCR4 = 2159;
	//TIM1->DIER |= TIM_DIER_CC4IE; // -------------------------------------------zezwolenie na przerwania
	TIM1->CCMR2 |= TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1; //PWM mode 1 - In upcounting, channel 1 is active as long as TIMx_CNT<TIMx_CCR1 	else inactive.
	//TIM1->CCER|=TIM_CCER_CC4P;  // output polarity deafault : active High
	TIM1->CCER |= TIM_CCER_CC4E; //On - OC1N signal is output on the corresponding output pin depending on MOE, OSSI, OSSR, OIS1, OIS1N and CC1E bits.
	TIM1->CR1 |= TIM_CR1_CEN; //  counter enable

//	 NVIC_InitTypeDef NVIC_InitStructure;
//	 NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
//	 NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
//	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	 NVIC_Init(&NVIC_InitStructure);

	//static __INLINE void NVIC_EnableIRQ(IRQn_Type IRQn)
}
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////        DAC_Conf             ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DAC_Conf(void) { // bedzie na PA4 trzeba ustawiÃ¦ je na AIN3
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC | RCC_APB1Periph_TIM6, ENABLE); // tim6 dla DAC
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	//conf tim6 , it'll be trigger in 44,1kHz and enable to DAC

	TIM6->PSC = 0; // CLK =48MHz/ (107+1) = 444,44KHz
	TIM6->ARR = FSAMPLE; // period czyli wartosc do ktorej zlicza licznik / po tym sie automatycznie przepelnia czest 44.1 KHZ
	TIM6->CR2 |= TIM_CR2_MMS_1; // The update event is selected as a trigger output (TRGO).)
	TIM6->DIER |= TIM_DIER_UIE;
	//TIM6->CR1 |= TIM_CR1_CEN; // | TIM_CR1_URS; //  counter enable ,,Only counter overflow/underflow generates an update interrupt or DMA request if enabled.
	GPIOA->MODER |= GPIO_MODER_MODER4; // uruchamiam na PA4 DAC
	//DAC->CR |= DAC_CR_TEN1; // trigger from underflow from timer6 , enable output bufor
	DAC->CR |= DAC_CR_EN1; // channel1 enabled
	Enable_TIM6_DAC();
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void Enable_TIM6_DAC(void) {
	//DAC->CR |= DAC_CR_EN1;
	TIM6->CR1 |= TIM_CR1_CEN; // channel1 enabled
}
void Disable_TIM6_DAC(void) {
	//DAC->CR &= ~(DAC_CR_EN1);
	TIM6->CR1 &= ~(TIM_CR1_CEN); // channel1 enabled}
}

void TIM6_DAC_IRQHandler(void) {
	static uint8_t sample_numDAC;
	if (TIM6->SR & TIM_SR_UIF) {
		TIM6->SR &= ~(TIM_SR_UIF);
		if (flaga == 1) {
			sample_numDAC++;
			if (sample_numDAC > SIZE_BUF_DAC - 1) {
				flaga = 0;
				sample_numDAC = 0;
			}
			register uint16_t value;
			value = buf_DAC[num_bufDAC ^ 1][sample_numDAC];
			DAC->DHR8R1 = value;
		}
	}
//
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////       ADC DMA      ///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ADC1_DMA_Config(void) {
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	// ADC1 DeInit //
	ADC_DeInit(ADC1);

	// GPIOC Periph clock enable //
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

	// ADC1 Periph clock enable //
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	// DMA1 clock enable //
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	// TIM2 Periph clock enable //
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/*obliczenia dotycz¹ce czêstotliwoci probkowania
	 * ADCCLK=14 MHz
	 * sampling time ADC= 1,5 taktu
	 * total Measure time ADC=  18us
	 * czyli Total fs=8KHz
	 *
	 * ustawienia poni¿ej
	 *
	 *
	 * */
	//Time base configuration//
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period = FSAMPLE; // fs=8kHz
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	// TIM2 TRGO selection: update event is selected as trigger for DAC//
	TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
	TIM_Cmd(TIM2, ENABLE);
	// Configure ADC Channel11 as analog input //
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	////////////////////////////////////////////////////////////////////////////////////////////
	DMA_StructInit(&DMA_InitStructure);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (ADC1->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) & buf_ADC[num_buf][0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = SIZE_BUF_ADC;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	DMA_Cmd(DMA1_Channel1, ENABLE);
	ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);
	ADC_DMACmd(ADC1, ENABLE);
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
	NVIC_InitTypeDef NVIC_InitStructure;
	/* Setup NVIC for DMA channel 1 interrupt request */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	ADC_Init(ADC1, &ADC_InitStructure);
	/// Configure the ADC1 in continous mode withe a resolutuion equal to 8 bits
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_8b;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConv_T2_TRGO;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Backward;
	ADC_Init(ADC1, &ADC_InitStructure);
	// Convert the ADC1 Channel 1 with 1.5 Cycles as sampling time
	ADC_ChannelConfig(ADC1, ADC_Channel_11, ADC_SampleTime_1_5Cycles);
	/// ADC Calibration //
	ADC_GetCalibrationFactor(ADC1);
	/// Enable ADC1 //
	ADC_Cmd(ADC1, ENABLE);

	/// Wait the ADCEN falg //
	while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN))
		;
	ADC_StartOfConversion(ADC1); // ADC1 regular Software Start Conv//

}

void DMA1_Channel1_IRQHandler(void) {
	if (DMA1->ISR & DMA_ISR_TCIF1) {
		DMA1->IFCR |= DMA1_FLAG_TC1; //kasuje flage
		DMA1_Channel1->CMAR = (uint32_t) & buf_ADC[num_buf ^ 1][0];
		full_frameADC_received = 1;
	}

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////      DAC_DMA_Config                            ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DAC_InitTypeDef DAC_InitStructure;
DMA_InitTypeDef DMA_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;

void DAC_DMA_Config(void) {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	// TIM2 Periph clock enable
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

	// Time base configuration
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period = FSAMPLE;
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

	// TIM2 TRGO selection: update event is selected as trigger for DAC
	TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

	// TIM2 enable counter
	TIM_Cmd(TIM6, ENABLE);

	// Enable GPIOA Periph clock --------------------------------------
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	// Configure PA.04 DAC_OUT as analog
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// DAC Periph clock enable
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	// Fill DAC InitStructure
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;

	// DAC channel1 Configuration
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);

	// Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is automatically connected to the DAC converter.
	DAC_Cmd(DAC_Channel_1, ENABLE);

	// Enable DMA for DAC Channel1
	DAC_DMACmd(DAC_Channel_1, ENABLE);

	// Enable DMA1 clock -------------------------------------------------------
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_DeInit(DMA1_Channel3);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (DAC->DHR8R1); //DAC_DHR12R_Address; DAC->DHR8R1;DAC->DHR12R1;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) & buf_DAC[num_buf][0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = SIZE_BUF_DAC;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; // DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);

	// Enable DMA1 Channel1 Transfer Complete interrupt
	DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);

	// Enable DMA1 channel1 IRQ Channel
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	// Enable DMA1 Channel3
	//DMA_Cmd(DMA1_Channel3, ENABLE);///------------------------------------------------------------------------------------------------------------------------------

}

void DMA1_Channel2_3_IRQHandler(void) {
	if (DMA1->ISR & DMA_ISR_TCIF3) {
		DMA1->IFCR |= DMA1_FLAG_TC3; //kasuje flage
		DMA_Cmd(DMA1_Channel3, DISABLE);
		DMA1_Channel3->CMAR = (uint32_t) & buf_DAC[num_buf][0];

	}
}

