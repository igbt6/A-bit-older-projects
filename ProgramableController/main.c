/*
 * main.c
 * Mikrokontroler Atmega 644p kwarc 8MHz
 *
 *
 *  Created on: 2012-02-29
 *       Autor: £ukasz Uszko
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include "triak.h"
#include "hd44780.h"
#include "i2c_twi.h"
#include "harddef.h"
#include "pcf8583.h"
#include "ds18b20_obsluga.h"
#include "menu.h"
#include <math.h>
#include "alarms.h"
int POS_IN_SUBMENU = 0;
int POS_IN_MAIN_MENU = 0;
int POS_IN_SUBSUBMENU = 0;
//uint8_t znacznik_menu; // jest to bufor do którego bede zapisywa³ wartoœæ  Return_value z funkcji Menu_Glowne


int main(void) {
	uint8_t flaga_clear = 0;
	//$#$ ustalam wartoœci po w³¹czeniu
	sekundnik = 0;
	Rtc_Parametry.year = 2012; // s³u¿y po to ¿eby po zaniku napiêcia na wysw pokaza³ mi rok 2012
	//Rtc_Parametry.day_week=0; tu bedzie 0 bo zczytujê z PCFA
	Rtc_Parametry.all_days = 1; // po w³¹czeniu równa siê 1
	Rtc_Parametry.day_of_year = 1;

	DDRB |= (1 << DD4);
	PORTB &= ~(1 << PB4);
	GPIO_Init();
	I2C_init(100000);
	lcd_init();
	INT0_init();
	TIMER0_init();
	first_search_sensors();
	sei();
	write_time_year(&Rtc_Parametry);//sie rok 2012 a nie 0 , muszê za³adowac ten adres do RAM- PCF-a, póŸniej przeniosê to do eproma
	lcd_locate(0, 0);



	//move_char_on_lcd(PL_powitanie,PL_wersja);
	//if	(!Delay_sec(5)){move_char_on_lcd(PL_powitanie,PL_wersja);}
	//	prog_uint8_t litera_L[] = { 8, 10, 12, 8, 24, 15, 0 }; //wzór litery £
	//lcd_defchar_P(0x81, litera_L); // za³adowanie znaku do pamiêci CGRAM
	//prog_uint8_t  strzalka_gora[]  = { 31,27,17,10,27,27,31};
	//move_char_on_lcd(tab1,tab2);

	while (1) {

		///////////////////test_eeprom();







			 if (Read_Input_Switch(SWITCH_OK)) {
			 uint8_t stan = Menu_Glowne();
			 switch (stan) { //bêdê sprawdza³ jak¹ wartoœæ dostanê
			 case 4: {
			 show_version_firmware();
			 break;
			 }
			 case 10: {
			 set_time();
			 break;
			 }
			 case 110:{// dobowy alarm
				 set_alarm(1);
				 break;}
			 default:
			 break;
			 }
			 }

			 if ((sekundnik % 10 <= 3)) {
			 for(flaga_clear=0;flaga_clear<=1;++flaga_clear){
			 read_temp_from_sensors();}// dwa razy siê wykona ta funkcja coby siê ustali³ stan
			 ///flaga_clear = 1;
			 lcd_cls();
			 }


			 else if (sekundnik % 10 > 3 && sekundnik % 10 <= 6) {
			 if (flaga_clear == 1) {
			 lcd_cls();
			 flaga_clear = 2;
			 }
			 show_time();
			 } else if (sekundnik % 10 > 6) {
			 if (flaga_clear == 2) {
			 lcd_cls();
			 flaga_clear = 0;
			 }
			 show_day_week();
			 }

			 if (int0_flag) {
			 PORTB ^= (1 << PB4);
			 int0_flag = 0;
			 }

			 if (alarm_from_sensor1(27, 30, 1)) {
			 lcd_locate(0, 15);
			 lcd_put_str("B");
			 }
			 //	if (Read_Input_Switch(SWITCH_OK)) {
			 //	Menu_Glowne();

			 //	}


		}
		return 0;

	}

