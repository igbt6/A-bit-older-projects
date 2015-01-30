/*
 * ds18b20_obsluga.c
 *
 *  Created on: 2012-02-23
 *       Autor: £ukasz Uszko
 */

#include <avr/io.h>
#include <util/delay.h>
#include "1Wire/ds18x20.h"

#include "ds18b20_obsluga.h"
#include "hd44780.h"
#include "harddef.h"

/*************************************************************************************************************
 * wyœwietlanie temperatury na pozycji X w drugiej linii LCD
 * @param x
 ************************************************************************************************************/

void display_temp(uint8_t x) {
	lcd_cls();

	lcd_locate(0, 0);
	lcd_put_str("> TEMPERATURA:");
	lcd_locate(1, x);
	lcd_put_str("> ");
	if (subzero)
		lcd_put_str("-"); /* jeœli subzero==1 wyœwietla znak minus (temp. ujemna) */
	else
		lcd_put_str(" "); /* jeœli subzero==0 wyœwietl spacjê zamiast znaku minus (temp. dodatnia) */
	lcd_int(cel); /* wyœwietl dziesiêtne czêœci temperatury  */
	lcd_put_str("."); /* wyœwietl kropkê */
	lcd_int(cel_fract_bits); /* wyœwietl dziesiêtne czêœci stopnia */
	lcd_put_str(" C "); /* wyœwietl znak jednostek (C - stopnie Celsiusza) */
}
/********************************************************************************************************
 * funkcja wyszukuj¹ca czujnik na magistrali i sprawdzaj¹ca czy jest nie uszkodzony np.
 ******************************************************************************************/

void first_search_sensors(void) { /* sprawdzamy ile czujników DS18xxx widocznych jest na magistrali */
	czujniki_cnt = search_sensors();

	/* wysy³amy rozkaz wykonania pomiaru temperatury
	 * do wszystkich czujników na magistrali 1Wire
	 * zak³adaj¹c, ¿e zasilane s¹ w trybie NORMAL,
	 * gdyby by³ to tryb Parasite, nale¿a³oby u¿yæ
	 * jako pierwszego prarametru DS18X20_POWER_PARASITE */
	DS18X20_start_meas(DS18X20_POWER_EXTERN, NULL);

	/* czekamy 800ms na dokonanie konwersji przez pod³¹czone czujniki */
	//Delay_100ms(8);
	_delay_ms(800);
	/* dokonujemy odczytu temperatury z pierwszego czujnika o ile zosta³ wykryty */
	/* wyœwietlamy temperaturê gdy czujnik wykryty */
	/*wyswietlam temperaturê z pierwszego czujnika*/
	if (DS18X20_OK == DS18X20_read_meas(gSensorIDs[0], &subzero, &cel,
			&cel_fract_bits)) {
		return;
	}
	//display_temp(0);
	else {
		lcd_locate(1, 0);
		lcd_put_str("    error ");
		return;//
	}

}

/********************************************************************************************
 * funkcja sprawdzajaca w cyklu odczyt danych z czujnika i wyœwietlajaca go na wyswietlaczu
 *
 ********************************************************************************************/
void read_temp_from_sensors(void) {
	czujniki_cnt = search_sensors();
	DS18X20_start_meas(DS18X20_POWER_EXTERN, NULL);
	if (DS18X20_OK == DS18X20_read_meas(gSensorIDs[0], &subzero, &cel,
			&cel_fract_bits)) {
		display_temp(0);
	} else {
		lcd_cls();
		lcd_locate(1, 0);
		lcd_put_str("  error ");
	}

}

/***********************************************************************************************************************************************
 * funkcja sprawdzaj¹ca czy zosta³a przekroczona temperatura maksymalna z czujnika 1 , jeœli tak to uruchamiane
 * jest wyjscie np wiatrak  i gdy  temp spadnie ponizej temp min to wiatrak siê wy³¹czy , taka histereza
 * ma³a przydatne jak sterownik pieca, z dok³adnosci¹ do jednego stopnia
 * @param temp_min ->>>
 * @param temp_max ->>>
 * @param up_or_down)->>> jeœli podam UP (logiczna 1) to po osi¿gniêciu temp_max wyjœcie siê w³¹cza i po opadniêciu ponizej temp_min  wy³¹cza siê
 *                   ->>>  -||- -||-  DOWN (logiczne 0) to po spadku poni¿ej temp_min uk³ad siê w³¹cza
 * @return bedzie zwracaæ jakieœ wartoœci mówiace ,pewnie dwie o tym w jakim stanie sie znajduje
 ************************************************************************************************************************************************/
bool alarm_from_sensor1(int temp_min, int temp_max, bool up_or_down) {

	static bool histereza;
	//czujniki_cnt = search_sensors();
	//DS18X20_start_meas(DS18X20_POWER_EXTERN, NULL);
	//if (DS18X20_OK == DS18X20_read_meas(gSensorIDs[0], &subzero, &cel,
	//	&cel_fract_bits)) {}
	int temp_zmierzona = 0;
	if (!subzero) {
		temp_zmierzona = cel;
	} else if (subzero) {
		temp_zmierzona = -cel;
	} // jeœli zmierzona temp  bêdzie znakiem ujemnym to dajê - przedz stopnieami;

	if (up_or_down) {
		if (histereza == false && temp_zmierzona >= temp_max) {
			DDRD |= 1 << DDD3;
			PORTD |= 1 << PORTD3;
			histereza = true;
			return 1;
		} else if (histereza == true && temp_zmierzona <= temp_min - 1) {
			PORTD &= ~(1 << PORTD3);
			histereza = false;
			return 0;
		}
	} else if (!up_or_down) {
		if (histereza == false && temp_zmierzona <= temp_min-1) {
			DDRD |= 1 << DDD3;
			PORTD |= 1 << PORTD3;
			histereza = true;
			return 1;
		} else if (histereza == true && temp_zmierzona >= temp_max) {
			PORTD &= ~(1 << PORTD3);
			histereza = false;
			return 0;
		}
	}

}

