/*
 * init_modules.c
 *
 *  Created on: Dec 23, 2011
 *      Author: lukasz
 */
#include "init_modules.h"
//****************************************************************************************************
//funkcja konfiguracji licznika SysTick
void Systick_Config(void) {
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	/* Sygnal‚ zegarowy rdzenia bedzie dzielony
	 * przez to co w nawiasie */
	SysTick_Config(2400000); //tu mam konfiguracje przerwania co 0.1 sekundy dla taktowania 24MHz
}
//****************************************************************************************************

//funkcja obslugi przerwania , najlepiej umieciÄ‡ w pliku "stm32f10x_it.c"
// jescze trzeba dodac w pliku Tam gdzie przerwanie czyli "stm32f10x_it.c" stala vu32 TimeDelay;
// dodać jeszcze nagłówek w pliku
/*
void SysTick_Handler(void) //funkcja obslugi przerwania , najlepiej umiecic w pliku "stm32f10x_it.c"
{

	// ta funkcje musze odkomentowac zeby moc uzywac funkcji void Delay(__IO uint32_t Time);
	static u32 n;
	n = TimeDelay;
	if (n > 0) {
		--n;
		TimeDelay = n;
	}

}
*/
//*****************************************************************************************************

void Delay(__IO uint32_t Time) // funkcja opĂłĹşniajÄ…ca w oparciu o przerwanie sys_Tick
{
	TimeDelay = Time;
	while (TimeDelay != 0)
		;
}
//******************************************************************************************************

//funkcja konfigurujacca zegary , i PLL , do czestotliwoci taktowania 16 MHz
void RCC_Config(void) {
	ErrorStatus HSEStartUpStatus;
// Reset ustawien RCC
	RCC_DeInit();

// Wlacz HSE
	RCC_HSEConfig(RCC_HSE_ON);

// Czekaj az HSE bedzie gotowy
	HSEStartUpStatus = RCC_WaitForHSEStartUp();

	if (HSEStartUpStatus == SUCCESS) {
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

		// zwloka dla pamieci Flash
		FLASH_SetLatency(FLASH_Latency_1);

		// HCLK = SYSCLK
		RCC_HCLKConfig(RCC_SYSCLK_Div1);

		// PCLK2 = HCLK
		RCC_PCLK2Config(RCC_HCLK_Div1);

		// PCLK1 = HCLK/2
		RCC_PCLK1Config(RCC_HCLK_Div2);

		// PLLCLK = 8MHz *2 = 16 MHz
		RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_3);

		// Wlacz PLL
		RCC_PLLCmd(ENABLE);

		// Czekaj az PLL poprawnie sie uruchomi
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
			;

		// PLL bedzie zrodlem sygnalu zegarowego
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		// Czekaj az PLL bedzie sygnalem zegarowym systemu
		while (RCC_GetSYSCLKSource() != 0x08)
			;
	}
	RCC_APB2PeriphClockCmd(
			RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO ,
			ENABLE);

}
//LUB TAK:
/*
 void RCC_init(){
 //wlaczenie zegara zewnetrznego
 RCC_CR_HSEON_bb = 1;
 //ustawienie kwarcu zewnetrznego, operacja bitband
 while(!RCC_CR_HSERDY_bb);	//oczekiwanie na flage gotowosci kwarcu
 //konfiguracja flasha
 FLASH->ACR |= (FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_2);			//2 cykle opoznienia flasha, bo f=72MHz, prefetch buffer
 //konfiguracja zegara
 RCC->CFGR |= RCC_CFGR_PLLMULL9 | RCC_CFGR_PLLSRC | RCC_CFGR_PPRE1_DIV2;	//wlaczenie PLL, mnoznik 9 ze zrodla zewnetrznego, szyna APB1 psk2, APB2 i AHB domyslnie psk1
 //wlaczenie PLL i oczekiwanie na flage gotowosci
 RCC_CR_PLLON_bb = 1;
 while(!RCC_CR_PLLRDY_bb);
 RCC->CFGR |= RCC_CFGR_SW_PLL;	//ustawienie PLL jako zrodlo sysclk
 while(((RCC->CFGR) & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);	//oczekiwanie na gotowosc}
 }*/

void GPIO_Config(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	//GPIOC->CRH = GPIO_CRH_MODE9_0 | GPIO_CRH_MODE8_0; //wyjcie push pull GPIOC pin 9 i 8  /10MHz
	///  GPIOA->CRL=GPIO_CRL_CNF0_0;  //wejcie PA0 z podciągnieciem PULLUP/DOWN
/*	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_14
			| GPIO_Pin_13| GPIO_Pin_15; // wejscia joysticka
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // podciagam do plusa
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_13|GPIO_Pin_1|GPIO_Pin_5|GPIO_Pin_3|GPIO_Pin_4; // wejscia joysticka
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // podciagam do plusa
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; // wejscia joysticka
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // podciagam do plusa
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	//GPIO_Init(GPIOB, &GPIO_InitStructure);

}
//************************************************************************************************************************

//******************ustawienie NVIC i EXTERNAL INTERRUPT*************************

//funkcja konfigurujaca GPIO jako zrodlo przerwan (kontroler EXTI)

void EXTI_Config(void) {
	EXTI_InitTypeDef EXTI_InitStructure; //Inicjalizacja struktury konfiguracji przerwan EXTI

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource3); //OK		//Aktywowanie funkcji EXTI pinów switchy do sterowania
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13); //UP
	//GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource4); //DOWN
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource0); //LEFT
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource1); //RIGHT

	EXTI_InitStructure.EXTI_Line = EXTI_Line3 | EXTI_Line13 //| EXTI_Line4
			| EXTI_Line1| EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //Przerwanie przy zboczu opadajacym

	EXTI_InitStructure.EXTI_LineCmd = ENABLE; //Uaktywnienie kontrolera EXTI
	EXTI_Init(&EXTI_InitStructure);
}

//******************************************************************************************************

//Funkcja konfigurujaca kontroler przerwan NVIC
void NVIC_Config(void) {
//Konfigurowanie kontrolera przerwan NVIC
#ifdef  VECT_TAB_RAM
	// Jezeli tablica wektorow w RAM, to ustaw jej adres na 0x20000000
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  // VECT_TAB_FLASH
	// W przeciwnym wypadku ustaw na 0x08000000
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif
	NVIC_InitTypeDef NVIC_InitStructure; //Inicjalizacja struktury konfiguracji kontrolera przerwan

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); //Wybór grupy priorytetow
	NVIC_InitStructure.NVIC_IRQChannel =  EXTI0_IRQn; //Konfiguracja przerwania od GPIO piny od 10 do 15
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel =  EXTI15_10_IRQn; //Konfiguracja przerwania od GPIO piny od 10 do 15
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
//	NVIC_InitStructure.NVIC_IRQChannel =  EXTI9_5_IRQn; //Konfiguracja przerwania od GPIO piny od 5 do 9 dla 2 klawiszy lewo prawo
//NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel =  EXTI1_IRQn; //Konfiguracja przerwania od GPIO piny od 5 do 9 dla 2 klawiszy lewo prawo
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel =  EXTI4_IRQn; //Konfiguracja przerwania od GPIO piny od 5 do 9 dla 2 klawiszy lewo prawo
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel =  EXTI3_IRQn; //Konfiguracja przerwania od GPIO piny od 5 do 9 dla 2 klawiszy lewo prawo
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


}
//******************************************************************************************************
