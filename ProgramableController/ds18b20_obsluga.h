/*
 * ds18b20_obsluga.h
 *
 *  Created on: 2012-02-23
 *       Autor: £ukasz Uszko
 */
#include "stdbool.h"

#ifndef DS18B20_OBSLUGA_H_
#define DS18B20_OBSLUGA_H_
uint8_t subzero, cel, cel_fract_bits;
uint8_t czujniki_cnt; /* iloœæ czujników na magistrali */
void display_temp(uint8_t x);
void first_search_sensors(void);
void read_temp_from_sensors(void); // funkcja zwraca wskaŸnik na tablicê 3 elementowa w której s¹ wartoœci temperatury znak "-" dziesi¹tki  stopnia i ca³e stopnie
bool alarm_from_sensor1(int temp_min, int temp_max,bool up_or_down); //funkcja uruchamiaj¹ca dane wyjscie ze wzgldêdu na zmierzon¹ temperaturê
#endif /* DS18B20_OBSLUGA_H_ */
