/*
 * rfm70.c
 *
 *  rozmiar biblioteki - ok 1312 bajtów FLASH , 0 bajtów RAM
 *
 *  Created on: 2011-10-17
 *       Autor: Miros³aw Kardaœ
 *       przerobka na STM32- Lukasz USZKO
 */
#include "stm_lib/inc/stm32f0xx_gpio.h"
#include "stm_lib/inc/stm32f0xx_rcc.h"
#include "stm_lib/inc/stm32f0xx_exti.h"
#include "stm_lib/inc/stm32f0xx_syscfg.h"
#include"stm_lib/inc/stm32f0xx_misc.h"
//#include "stm_lib/inc/stm32f0xx_spi.h"
#include "rfm70.h"

uint8_t rf70_rx_buf[MAX_PACKET_LEN + 1];
uint8_t delay;
//Bank1 register initialization value
const uint32_t Bank1_Reg0_13[] = { //latest config txt
		0xE2014B40, 0x00004BC0, 0x028CFCD0, 0x41390099, 0x0B869Ef9, 0xA67F0624,
				0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
				0x00000000, 0x00127300, 0x36B48000 };

const uint8_t Bank1_Reg14[] = { 0x41, 0x20, 0x08, 0x04, 0x81, 0x20, 0xCF, 0xF7,
		0xFE, 0xFF, 0xFF };

//Bank0 register initialization value
const uint8_t Bank0_Reg[][2] = { { 0, 0x0F }, //reflect RX_DR\TX_DS\MAX_RT,Enable CRC ,2byte,POWER UP,PRX
		{ 1, 0x3F }, //Enable auto acknowledgement data pipe5\4\3\2\1\0
		{ 2, 0x3F }, //Enable RX Addresses pipe5\4\3\2\1\0
		{ 3, 0x03 }, //RX/TX address field width 5byte
		{ 4, 0xff }, //auto retransmission dalay (4000us),auto retransmission count(15)
		{ 5, 0x17 }, //23 channel
		{ 6, 0x17 }, //air data rate-1M,out power 0dbm,setup LNA gain
		{ 7, 0x07 }, //
		{ 8, 0x00 }, //
		{ 9, 0x00 }, { 12, 0xc3 }, //only LSB Receive address data pipe 2, MSB bytes is equal to RX_ADDR_P1[39:8]
		{ 13, 0xc4 }, //only LSB Receive address data pipe 3, MSB bytes is equal to RX_ADDR_P1[39:8]
		{ 14, 0xc5 }, //only LSB Receive address data pipe 4, MSB bytes is equal to RX_ADDR_P1[39:8]
		{ 15, 0xc6 }, //only LSB Receive address data pipe 5, MSB bytes is equal to RX_ADDR_P1[39:8]
		{ 17, 0x20 }, //Number of bytes in RX payload in data pipe0(32 byte)
		{ 18, 0x20 }, //Number of bytes in RX payload in data pipe1(32 byte)
		{ 19, 0x20 }, //Number of bytes in RX payload in data pipe2(32 byte)
		{ 20, 0x20 }, //Number of bytes in RX payload in data pipe3(32 byte)
		{ 21, 0x20 }, //Number of bytes in RX payload in data pipe4(32 byte)
		{ 22, 0x20 }, //Number of bytes in RX payload in data pipe5(32 byte)
		{ 23, 0x00 }, //fifo status
		{ 28, 0x3F }, //Enable dynamic payload length data pipe5\4\3\2\1\0
		{ 29, 0x07 } //Enables Dynamic Payload Length,Enables Payload with ACK,Enables the W_TX_PAYLOAD_NOACK command
};

const uint8_t RX0_Address[] = { 0x34, 0x43, 0x10, 0x10, 0x01 }; //Receive address data pipe 0
const uint8_t RX1_Address[] = { 0x39, 0x38, 0x37, 0x36, 0xc2 }; //Receive address data pipe 1

// wskaŸnik do funkcji callback dla zdarzenia RFM70_RX_EVENT()
static void (*rfm70_rx_event_callback)(void * pBuf, uint8_t len);

// funkcja do rejestracji funkcji zwrotnej w zdarzeniu RFM70_RX_EVENT()
void register_rfm70_rx_event_callback(
		void (*callback)(void * pBuf, uint8_t len)) {
	rfm70_rx_event_callback = callback;
}

// Zdarzenie do odbioru danych radiowych
void RFM70_RX_EVENT(void) {
	uint8_t len;

#if RF70_UseIRQ == 1
	if (rf70_rx_flag) {
#endif
		len = Receive_Packet(rf70_rx_buf);
		if (len && rfm70_rx_event_callback)
			(*rfm70_rx_event_callback)(rf70_rx_buf, len);

#if RF70_UseIRQ == 1
		rf70_rx_flag = 0;
	}
#endif
}

/**************************************************
 Function: SPI_RW(); - podstawowy zapis/odczyt soft SPI
 **************************************************/
static uint8_t SPI_RW(uint8_t value) {
	register uint8_t i;

	for (i = 0; i < 8; i++) {
//		for(delay=0;delay<10;delay++); ////////////////////////////delay/////////////////////////////
		if (value & 0x80) {
			SPI_PORT |= MOSI; // MOSI=1;
		} else {
			SPI_PORT &= ~MOSI; // MOSI=0;
		}

		value = (value << 1); // shift next bit into MSB..
//		for(delay=0;delay<10;delay++); ////////////////////////////delay 1 us/////////////////////////////
		if (SPI_PIN & MISO)
			value |= 1; // check MISO bit
//		for(delay=0;delay<10;delay++); ////////////////////////////delay 1 us/////////////////////////////

		SPI_PORT |= SCK; // SCK = 1;
//		for(delay=0;delay<10;delay++); ////////////////////////////delay 1 us/////////////////////////////
		SPI_PORT &= ~SCK; // SCK = 0;
//		for(delay=0;delay<10;delay++); ////////////////////////////delay 1 us/////////////////////////////
	}
	return value; //
}


/**************************************************
 Function: SPI_Write_Reg(); - zapis wartoœci value do rejestru reg
 **************************************************/
void SPI_Write_Reg(uint8_t reg, uint8_t value) {
	SPI_PORT &= ~CSN; // CSN = 0;   init SPI transaction
	SPI_RW(reg); // select register
	SPI_RW(value); // ..and write value to it..
	SPI_PORT |= CSN; // CSN = 1;  terminate SPI communication
}

/**************************************************
 Function: SPI_Read_Reg(); - odczyt rejestru reg
 **************************************************/
uint8_t SPI_Read_Reg(uint8_t reg) {
	uint8_t value;
	SPI_PORT &= ~CSN; // CSN = 0;   initialize SPI communication...
	SPI_RW(reg); // Select register to read from..
	value = SPI_RW(0); // ..then read register value
	SPI_PORT |= CSN; // CSN = 1;  terminate SPI communication

	return value; // return register value
}

/**************************************************
 Function: SPI_Read_Buf(); - odczyt danych z rejestru reg do bufora pBuf o d³ugoœci len
 **************************************************/
void SPI_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len) {
	uint8_t status, i;

	SPI_PORT &= ~CSN; // CSN = 0;
	status = SPI_RW(reg); // Select register to write, and read status uint8_t

	for (i = 0; i < len; i++)
		pBuf[i] = SPI_RW(0); // Perform SPI_RW to read uint8_t from RFM70

	SPI_PORT |= CSN; // CSN = 1;

}

/**************************************************
 Function: SPI_Write_Buf(); zapis do rejestru reg bufor *pBuf o d³ugoœci len
 **************************************************/
void SPI_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len) {
	uint8_t i;

	SPI_PORT &= ~CSN; // CSN = 0;
	SPI_RW(reg); // Select register to write to and read status uint8_t
	for (i = 0; i < len; i++) // then write all uint8_t in buffer(*pBuf)
		SPI_RW(*pBuf++);
	SPI_PORT |= CSN; // CSN = 1;

}

/**************************************************
 Function: SwitchToRxMode(); - prze³¹cz w tryb odbioru
 **************************************************/
void SwitchToRxMode() {
	uint8_t value;

	SPI_Write_Reg(FLUSH_RX, 0); //flush Rx

	value = SPI_Read_Reg(STATUS); // read register STATUS's value
	SPI_Write_Reg(WRITE_REG | STATUS, value); // clear RX_DR or TX_DS or MAX_RT interrupt flag

	SPI_PORT &= ~CE; // CE=0;

	value = SPI_Read_Reg(CONFIG); // read register CONFIG's value

//PRX
	value = value | 0x01; //set bit 1
	SPI_Write_Reg(WRITE_REG | CONFIG, value); // Set PWR_UP bit, enable CRC(2 length) & Prim:RX. RX_DR enabled..
	SPI_PORT |= CE; // CE=1;
}

/**************************************************
 Function: SwitchToTxMode(); - prze³¹cz w tryb nadawania
 **************************************************/
void SwitchToTxMode() {
	uint8_t value;
	SPI_Write_Reg(FLUSH_TX, 0); //flush Tx

	SPI_PORT &= ~CE; // CE=0;
	value = SPI_Read_Reg(CONFIG); // read register CONFIG's value
//PTX
	value = value & 0xfe; //set bit 0
	SPI_Write_Reg(WRITE_REG | CONFIG, value); // Set PWR_UP bit, enable CRC(2 length) & Prim:RX. RX_DR enabled.

	SPI_PORT |= CE; // CE=1;
}

/**************************************************
 Function: SwitchCFG(); - prze³¹cz bank rejestrów Bank1 lub Bank0

 Parametr:
 _cfg      1:register bank1
 0:register bank0
 **************************************************/
//1:Bank1 0:Bank0
void SwitchCFG(char _cfg) {
	uint8_t Tmp;

	Tmp = SPI_Read_Reg(7);
	Tmp = Tmp & 0x80;

	if (((Tmp) && (_cfg == 0)) || (((Tmp) == 0) && (_cfg)))
		SPI_Write_Reg(ACTIVATE_CMD, 0x53);
}

/**************************************************
 Function: SetChannelNum(); - ustaw kana³ ch
 **************************************************/
/*
 * 	zakres czêstotliwoœci od 2400 - 2483.5 MHz
 *
 *  zmiana kana³u nastêpuje co 1MHz
 *  dostêpne s¹ 83 kana³y
 *
 */
void SetChannelNum(uint8_t ch) {
	SPI_Write_Reg((uint8_t)(WRITE_REG | 5), (uint8_t)(ch));
}

#if RF70_UseIRQ == 1

volatile uint8_t rf70_rx_flag;

void EXTI0_1_IRQHandler(void) { // obsluga przerwania z wyjscia IRQ, czyli wtedy gdy odbiore cala ramke
	if (EXTI->PR & EXTI_PR_PR0) // jesli przerwanie wystapi
	{
		register uint8_t value; // gegister bo potrzebuje szybkiego odbioru danych
		value = SPI_Read_Reg(READ_REG | STATUS);
		if ((value & STATUS_RX_DR)) {
			rf70_rx_flag = 1;
		}
		///	GPIOC->ODR ^= GPIO_Pin_8;
		EXTI->PR |= EXTI_PR_PR0; //Clear the EXTI line 0 pending bit
	}
}
#endif

/**************************************************
 Function: RFM70_Initialize(); - pe³na inicjalizacja soft SPI oraz modu³u RFM70
 **************************************************/
void RFM70_Init() {
	uint16_t i, j;
	uint8_t WriteArr[12];

	 #if RF70_UseIRQ == 1

	EXTI0_Config();

	 #endif

	//! Init soft SPI ********************
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	GPIOB->MODER |= GPIO_MODER_MODER8_0 | GPIO_MODER_MODER9_0
			| GPIO_MODER_MODER10_0 | GPIO_MODER_MODER12_0; // ustawiam wyjscie na odpowiednie porty CE,CSN, SCK, MOSI
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8 | GPIO_OSPEEDER_OSPEEDR9
			| GPIO_OSPEEDER_OSPEEDR10 |GPIO_OSPEEDER_OSPEEDR11| GPIO_OSPEEDER_OSPEEDR12; // 50 MHz
	GPIOB->ODR |= CSN; // CSN=1
	GPIOB->ODR &= ~(MOSI | CE | SCK); // MOSI,CE,SCK=0


//! Init soft SPI end ****************

	for( i=0; i<6500;i++){}; // delay okolo 30ms

	SwitchCFG(0);

	for (i = 0; i < 20; i++) {
		SPI_Write_Reg((WRITE_REG | (Bank0_Reg[i][0])), (Bank0_Reg[i][1]));
	}

//reg 10 - Rx0 addr
	for (j = 0; j < 5; j++) {
		WriteArr[j] = (RX0_Address[j]);
	}

	SPI_Write_Buf((WRITE_REG | 10), &(WriteArr[0]), 5);

//REG 11 - Rx1 addr
	for (j = 0; j < 5; j++) {
		WriteArr[j] = (RX1_Address[j]);
	}
	SPI_Write_Buf((WRITE_REG | 11), &(WriteArr[0]), 5);
//REG 16 - TX addr
	for (j = 0; j < 5; j++) {
		WriteArr[j] = (RX0_Address[j]);
	}
	SPI_Write_Buf((WRITE_REG | 16), &(WriteArr[0]), 5);

	i = SPI_Read_Reg(29); //read Feature Register Payload With ACK (REG28,REG29).
	if (i == 0) // i!=0 showed that chip has been actived.so do not active again.
		SPI_Write_Reg(ACTIVATE_CMD, 0x73); // Active
	for (i = 22; i >= 21; i--) {
		SPI_Write_Reg((WRITE_REG | (Bank0_Reg[i][0])), (Bank0_Reg[i][1]));
	}

//********************Write Bank1 register******************
	SwitchCFG(1);
	//reverse
	for (i = 0; i <= 8; i++) {
		for (j = 0; j < 4; j++)
			WriteArr[j] = ((Bank1_Reg0_13[i]) >> (8 * (j))) & 0xff;
		SPI_Write_Buf((WRITE_REG | i), &(WriteArr[0]), 4);
	}

	for (i = 9; i <= 13; i++) {
		for (j = 0; j < 4; j++)
			WriteArr[j] = ((Bank1_Reg0_13[i]) >> (8 * (3 - j))) & 0xff;
		SPI_Write_Buf((WRITE_REG | i), &(WriteArr[0]), 4);
	}

	for (j = 0; j < 11; j++) {
		WriteArr[j] = (Bank1_Reg14[j]);
	}
	SPI_Write_Buf((WRITE_REG | 14), &(WriteArr[0]), 11);

//toggle REG4<25,26>
	for (j = 0; j < 4; j++)
		WriteArr[j] = ((Bank1_Reg0_13[4]) >> (8 * (j))) & 0xff;

	WriteArr[0] = WriteArr[0] | 0x06;
	SPI_Write_Buf((WRITE_REG | 4), &(WriteArr[0]), 4);

	WriteArr[0] = WriteArr[0] & 0xf9;
	SPI_Write_Buf((WRITE_REG | 4), &(WriteArr[0]), 4);

	for(i=0; i<4000;i++){}; // delay okolo 20ms

//********************switch back to Bank0 register access******************
	SwitchCFG(0);
	SwitchToRxMode(); //switch to RX mode
}

/**************************************************
 Function: Send_Packet
 Description:
 fill FIFO to send a packet
 Parameter:
 type: WR_TX_PLOAD or  W_TX_PAYLOAD_NOACK_CMD
 pbuf: a buffer pointer
 len: packet length -----> MAX 32 !!!!
 Return:
 0 - OK
 1 - TIMEOUT
 **************************************************/
uint8_t Send_Packet(uint8_t type, void* pbuf, uint8_t len) {
	uint16_t tmo = 0xffff;
	while ((SPI_Read_Reg(FIFO_STATUS) & FIFO_STATUS_TX_FULL) && tmo--)
		; //if not full, send data (write buff)
	SwitchToTxMode(); //switch to tx mode
	SPI_Write_Buf(type, pbuf, len); // Writes data to buffer
	tmo = 0xffff;
	while (!((SPI_Read_Reg(FIFO_STATUS) & FIFO_STATUS_TX_EMPTY)) && tmo--)
		;
	SwitchToRxMode();
	if (tmo)
		return 0; // OK
	else
		return 1; // TIMEOUT (error)
}

/**************************************************
 Function: Receive_Packet
 Description:
 read FIFO to read a packet
 Parameter:
 pBuf - wskaŸnik na bufor odbiorczy
 Return:
 len - ilosæ odebranych bajtów
 **************************************************/
uint8_t Receive_Packet(void * pBuf) {

	uint8_t len, len1 = 0, sta, fifo_sta;
	uint8_t *wsk = pBuf;

	sta = SPI_Read_Reg(READ_REG | STATUS); // read register STATUS's value

#if RF70_UseIRQ == 0
	// if receive data ready (RX_DR) interrupt
	if( (sta & STATUS_RX_DR) == STATUS_RX_DR ) {
#endif
	do {
		len = SPI_Read_Reg(R_RX_PL_WID_CMD); // read len

		if (len <= MAX_PACKET_LEN) {
			SPI_Read_Buf(RD_RX_PLOAD, pBuf, len); // read receive payload from RX_FIFO buffer
			len1 = len;
		} else {
			SPI_Write_Reg(FLUSH_RX, 0); //flush Rx
		}

		fifo_sta = SPI_Read_Reg(READ_REG | FIFO_STATUS); // read register FIFO_STATUS's value

	} while ((fifo_sta & FIFO_STATUS_RX_EMPTY) == 0); //while not empty

	wsk[len1] = 0; // automatycznie zapisanie zera na potrzeby transmisji tekstu

	SPI_Write_Reg(WRITE_REG | STATUS, sta); // clear RX_DR or TX_DS or MAX_RT interrupt flag
#if RF70_UseIRQ == 0
		}
#endif

	return len1;
}

/**************************************************
 Function: Config _EXTI Interrupt
 Description:
 Configuration module EXTI na wejsciu PB0
 Parameter:
 none
 **************************************************/
void EXTI0_Config() {

		//  przerwanie exti na PB0  ,ustawione wejscie PB0, NoPullDownUp- to juz defaultowo ustawione mam
		RCC->AHBENR |= RCC_AHBPeriph_GPIOB; // zegar na pin B
		RCC->APB2ENR |= RCC_APB2Periph_SYSCFG; // Enable SYSCFG clock
		GPIOB->PUPDR|=GPIO_PUPDR_PUPDR0_0; // pullup na wejsciu
		SYSCFG->EXTICR[0] = SYSCFG_EXTICR1_EXTI0_PB; // Exti in input PB0
		//lub 		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,EXTI_PinSource0);
		EXTI->IMR |= EXTI_IMR_MR0; //Interrupt request from PB0 is not masked
		EXTI->FTSR |= EXTI_FTSR_TR0; // Falling trigger edge
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel = EXTI0_1_IRQn; // Enable and set EXTI0 Interrupt
		NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}
////////////////////////////////////////////////////////////////////////////
/*
void SPI_Config(void)
{
  //konfigurowanie interfejsu SPI
  SPI_InitTypeDef   SPI_InitStructure;

  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;         //transmisja z wykorzystaniem jednej linii, transmisja jednokierunkowa
  SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;                      //tryb pracy SPI
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                //8-bit ramka danych
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                        //stan sygnalu taktujacego przy braku transmisji - niski
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                      //aktywne zbocze sygnalu taktujacego - 1-sze zbocze
  SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;                         //sprzetowa obsluga linii NSS (CS)
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;//prescaler szybkosci tansmisji  36MHz/256=140.625kHz
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                //pierwszy bit w danych najbardziej znaczacy
  SPI_Init(SPI1, &SPI_InitStructure);                               //inicjalizacja SPI
  SPI_SSOutputCmd(SPI1, ENABLE);

  SPI_Cmd(SPI1, ENABLE);  	// Wlacz SPI1
}
*/
