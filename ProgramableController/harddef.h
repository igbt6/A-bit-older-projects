/*
 * harddef.h
 *
 *  Created on: 2012-02-11
 *       Autor: £ukasz Uszko
 */
#ifndef HARDDEF_H_
#define HARDDEF_H_
#include <stdbool.h>

/////////////////////////////////// Makra upraszczaj¹ce dostêp do portów//////////////////////
// *** PORT
/*
 *
#define PORT(x) XPORT(x)
#define XPORT(x) (PORT##x)
// *** PIN
#define PIN(x) XPIN(x)
#define XPIN(x) (PIN##x)
// *** DDR
#define DDR(x) XDDR(x)
#define XDDR(x) (DDR##x)
*/

//////////////////////////////przyciski////////////////////////////////////////////////////
#define SWITCH_OK 1<<7
#define SWITCH_GORA 1<<6
#define SWITCH_DOL 1<<5
#define SWITCH_LEWO 1<<3
#define SWITCH_PRAWO 1<<4



//////////////////////////////////////////////////////////////////////////////////////////////////////
extern  volatile uint16_t Timer1, Timer2, Timer3; // timery programowe 100Hz
extern volatile uint8_t sekundnik;                                //sekundnik
extern volatile bool int0_flag;

void GPIO_Init(void);
void INT0_init(void);
void TIMER0_init(void);
bool Read_Input_Switch( uint8_t pin);
uint8_t Read_Pin_Input_Switch(void);
bool Delay_ms(uint16_t czas_1_ms);
bool Delay_100ms(uint16_t czas_100_ms);
bool Delay_sec(uint16_t czas_1_sec);
 void Delay_Debounce(void);

#endif /* HARDDEF_H_ */
