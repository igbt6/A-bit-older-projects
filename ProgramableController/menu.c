/*
 * menu.c
 *
 * plik zawieraj�cy 2 struktury:
 * -->pierwsza to menu g��wne
 * -->druga to pod menu
 *
 *  Created on: 2012-02-26
 *       Autor: �ukasz Uszko
 */

#include <avr/io.h>
#include "menu.h"
#include "hd44780.h"
#include "harddef.h"
#include <util/delay.h>

//aktualna pozycja w menu gl�wnym


// definicje znak�w specjalnych dla wyswietlacza LCD
/*
 *
 char CGRom[65] = { 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, // "kratka"			0x00
 0xC0, 0xC0, 0xFF, 0xF1, 0xF1, 0xF1, 0xFF, 0xC0, // pusty kwadrat	0x01
 0xC0, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, // kwadrat zacz.	0x02
 0xE0, 0xFF, 0xFF, 0xFF, 0xE0, 0xE0, 0xE0, 0xE0, // linia				0x03
 0xFF, 0xFF, 0xF9, 0xF3, 0xE7, 0xF3, 0xF9, 0xFF, // znak w lewo		0x04
 0xFF, 0xFF, 0xF3, 0xF9, 0xFC, 0xF9, 0xF3, 0xFF, // znak w prawo	0x05
 0xFF, 0xFF, 0xFB, 0xF1, 0xE4, 0xEE, 0xFF, 0xFF, // znak w g�re		0x06
 0xFF, 0xFF, 0xFF, 0xEE, 0xE4, 0xF1, 0xFB, 0xFF, // znak w d�l		0x07
 0x00 };
 */
typedef struct {
	char name[maxlength]; //napisy menu
	uint8_t y0; //polozenie znacznika wyboru w poziomie
	uint8_t retval; //wartosc zwracana w wyniku wyboru
} PODPODMENU;
typedef struct {
	char name[maxlength]; //napisy menu
	uint8_t y0; //polozenie znacznika wyboru w poziomie
	PODPODMENU *nextpodmenu;
	uint8_t retval; //wartosc zwracana w wyniku wyboru
} PODMENU;

//definicja dla pozycji menu gl�wnego
typedef struct {
	char name[maxlength]; //napisy menu
	uint8_t y0; //polozenie znacznika wyboru w poziomie u mnie zaawsze 1 bo tak bedzie w menu g��wnym
	PODMENU *nextmenu; //wskaznik do tablicy z wykazem opcji podmenu
	uint8_t return_value; //wartos zwracana w wyniku wyboru // m�wi mi o pozycji w menu
} MENU_GLOWNE;

PODPODMENU menu1_ALARM[3] = { // pod-menu pierwszego PODMENU
		{ "DOBOWY", 1, 110 }, { "TYGODNIOWY", 1, 111 }, { "ROCZNY", 1, 112 } };

PODMENU menu1[liczba_1_podmenu1] = { //pod-menu 1//ustawienia
		{ "CZAS I DATA", 1, 0, 10 }, { "ALARM", 1, menu1_ALARM, 11 }, {
				"podmenu 1.3 ", 1, 0, 12 } };

PODMENU menu2[liczba_1_podmenu2] = { //pod-menu 2
		{ "podmenu 2.0 ", 1, 0, 20 }, { "podmenu 2.1 ", 1, 0, 21 }, {
				"podmenu 2.2 ", 1, 0, 22 }, { "podmenu 2.3 ", 1, 0, 23 }, {
				"podmenu 2.4", 1, 0, 24 }, { "podmenu 2.5 ", 1, 0, 25 }, {
				"podmenu 2.6 ", 1, 0, 26 } };

PODMENU menu3[liczba_1_podmenu3] = { //pod-menu 3
		{ "podmenu 3.0", 1, 0, 30 }, { "podmenu 3.1 ", 1, 0, 31 }, };

MENU_GLOWNE main_menu[liczba_menu_gl] = { //menu gl�wne
		{ "USTAWIENIA", 1, menu1, 1 }, { "TRYB PRACY", 1, menu2, 2 }, { // tutaj mam zwracane 0 bo zadnej funkcji wtkonywa� nie b�d�
				"TRANSMISJA RF", 1, menu3, 3 }, { "FIRMWARE", 1, 0, 4 } };

void Lcd_show_Menu_Glowne(uint8_t pozycja_menu_gl/*, uint8_t pozycja_sub_menu*/) {

	MENU_GLOWNE *wsk_menu_glowne = main_menu; //adres zerowego elementu tablicy zawieraj�cej struktury menu g��wnego
	//PODMENU *wsk_podmenu = (wsk_menu_glowne + pozycja_menu_gl)->nextmenu; // tutaj przypisuje wska�nik PODMENU na wska�nik menu g��wnego
	if (pozycja_menu_gl >= liczba_menu_gl) {
		pozycja_menu_gl = 0;
	}
	//	if (pozycja_sub_menu == 111) { // je�li pozycja sub menu r�na od 0
	lcd_cls();
	lcd_locate(0, 0);
	lcd_put_str("------MENU------");
	lcd_locate(wsk_menu_glowne->y0, 0);
	lcd_put_str("<");
	lcd_int((wsk_menu_glowne + (pozycja_menu_gl))->return_value); // przesuwam wska�nik na kolejne tablice struktur
	lcd_put_str(">");
	lcd_locate((wsk_menu_glowne)->y0, 4);
	lcd_put_str((wsk_menu_glowne + (pozycja_menu_gl))->name);

}

/****************************************************************************************************************
 *funkcja wy�wietlaj�ca pierwsze PODMENU
 * @param pozycja_menu_gl
 * @param pozycja_sub_menu
 *******************************************************************************************************************/
void Lcd_show_Sub_Menu_1(uint8_t pozycja_menu_gl, uint8_t pozycja_sub_menu) {
	MENU_GLOWNE *wsk_menu_glowne = main_menu; //adres zerowego elementu tablicy zawieraj�cej struktury menu g��wnego
	PODMENU *wsk_podmenu = (wsk_menu_glowne + pozycja_menu_gl)->nextmenu; // tutaj przypisuje wska�nik PODMENU na wska�nik menu g��wnego
	lcd_cls();
	lcd_locate(0, 0);
	lcd_put_str("-> ");
	lcd_put_str((wsk_menu_glowne + pozycja_menu_gl)->name);
	lcd_locate(wsk_podmenu->y0, 0);
	lcd_put_str("<");
	lcd_int((wsk_podmenu + pozycja_sub_menu)->retval % 10 + 1); //po to �eby zacyzna od pierwsyej poyzcji(by by�a taka wysietlana
	lcd_put_str(">");
	lcd_locate((wsk_podmenu)->y0, 4);
	lcd_put_str((wsk_podmenu + pozycja_sub_menu)->name);
}
/****************************************************************************************************************
 *funkcja wy�wietlaj�ca drugie PODMENU
 * @param pozycja_sub_menu
 * @param pozycja_subsub_menu
 *******************************************************************************************************************/
void Lcd_show_Sub_Menu_2(uint8_t pozycja_menu_gl, uint8_t pozycja_sub_menu,
		uint8_t pozycja_subsub_menu) {
	MENU_GLOWNE *wsk_menu_glowne = main_menu; //adres zerowego elementu tablicy zawieraj�cej struktury menu g��wnego
	PODMENU *wsk_podmenu = (wsk_menu_glowne + pozycja_menu_gl)->nextmenu; // tutaj przypisuje wska�nik PODMENU na wska�nik menu g��wnego
	PODPODMENU *wsk_podpodmenu = (wsk_podmenu + pozycja_sub_menu)->nextpodmenu;
	lcd_cls();
	lcd_locate(0, 0);
	lcd_put_str("-> ");
	lcd_put_str((wsk_podmenu + pozycja_sub_menu)->name);
	lcd_locate(wsk_podpodmenu->y0, 0); // y0 to jedynka
	lcd_put_str("<");
	lcd_int((wsk_podpodmenu + pozycja_subsub_menu)->retval % 10 + 1); //po to �eby zacyzna od pierwsyej poyzcji(by by�a taka wysietlana
	lcd_put_str(">");
	lcd_locate(wsk_podpodmenu->y0, 4);
	lcd_put_str((wsk_podpodmenu + pozycja_subsub_menu)->name);
}
/****************************************************************************************************************
 * Funkcja wyswietlaj�ca i obs�uguj�ca ca�e menu  zwraca mi Return Value kt�re to powraca z POD_MEN�W kolejnych
 * @return Return_Value
 ***************************************************************************************************/

uint8_t Menu_Glowne(void) { // funkcja s�u��ca do poruszania si� po menu g��wnym
	POS_IN_MAIN_MENU = 0;
	Delay_sec(1); //czekam se 1 sekund�
	MENU_GLOWNE *wsk_menu_glowne = main_menu;
	uint8_t Return_Value = 0;

	uint8_t key_menu = 0;
	Lcd_show_Menu_Glowne(POS_IN_MAIN_MENU);
	while (1) {

		key_menu = Read_Pin_Input_Switch();
		switch (key_menu) {
		case SWITCH_DOL: {
			POS_IN_MAIN_MENU++;
			//wsk_menu_glowne++;
			if (POS_IN_MAIN_MENU > (liczba_menu_gl - 1)) {
				POS_IN_MAIN_MENU = 0;
				//wsk_menu_glowne = main_menu;
				//Return_Value = wsk_menu_glowne->return_value;

			}
			Lcd_show_Menu_Glowne(POS_IN_MAIN_MENU);
			Delay_100ms(4);

			break;
		}
		case SWITCH_GORA: {
			POS_IN_MAIN_MENU--;
			if (POS_IN_MAIN_MENU < 0) {
				POS_IN_MAIN_MENU = (liczba_menu_gl - 1);
				wsk_menu_glowne = &main_menu[liczba_menu_gl - 1];
				//Return_Value = wsk_menu_glowne->return_value;
			}
			Lcd_show_Menu_Glowne(POS_IN_MAIN_MENU);
			Delay_100ms(4);
			//_delay_ms(400);
			break;
		}
		case SWITCH_OK: {
			Delay_ms(400);
			lcd_cls();
			//Return_Value = Pod_Menu1(); //tutaj wchodz� w opcje pierwszego podmenu i warto�� Return Value
			if ((wsk_menu_glowne + POS_IN_MAIN_MENU)->nextmenu != 0) { // je�li pokazuje na warto�� r�zn� od 0
				Return_Value = Pod_Menu1(); //tutaj wchodz� w opcje pierwszego podmenu i zwracam warto�� Return Value
				if (Return_Value) {
					lcd_cls();
					return Return_Value;
				}
			} else { // je�li ma warto�� 0 czyli nie ma ju� zadnego podmenu to zwracam warto�� t� z menu g��wnego
				Return_Value
						= ((wsk_menu_glowne + POS_IN_MAIN_MENU)->return_value);
				return Return_Value;
			}

			break;
		}
		case SWITCH_LEWO: {      // klawiszem w lewo opuszczam  dane menu
			return 0 ;break;
		}

		}
	}
}
/***************************************************************************************************
 * funkcja obs�uguj�ca pierwsze  PODMENU bedzie wywo�ywana w MENUG��WNYM
 * @return uint8_t Return_Value
 **************************************************************************************************/

uint8_t Pod_Menu1(void) {
	MENU_GLOWNE *wsk_menu_glowne = main_menu;
	PODMENU *wsk_podmenu = (wsk_menu_glowne + POS_IN_MAIN_MENU)->nextmenu; //tutaj przypisuje wska�nik na sub_menu do tablicy na kt�r� wskazuje element MENU
	uint8_t Return_Value = 0;

	//wsk_podmenu = wsk_menu_glowne->nextmenu;
	uint8_t key_submenu = 0; // zeruje key sub-menu dodatkowy cykl
	Lcd_show_Sub_Menu_1(POS_IN_MAIN_MENU, POS_IN_SUBMENU);
	while (1) {
		key_submenu = Read_Pin_Input_Switch();
		switch (key_submenu) {
		case SWITCH_DOL: {
			POS_IN_SUBMENU++;
			//wsk_podmenu++; //jad� po kolejnych elementach tablicy
			if (POS_IN_SUBMENU > (liczba_1_podmenu(POS_IN_MAIN_MENU) - 1)) {//liczba_1_podmenu(wsk_menu_glowne + POS_IN_MAIN_MENU)/******to tak na razie to tylko dla 1 przypadku ustawie�*/) {
				POS_IN_SUBMENU = 0;
			}
			Lcd_show_Sub_Menu_1(POS_IN_MAIN_MENU, POS_IN_SUBMENU);
			Delay_100ms(4);
			break;
		}
		case SWITCH_GORA: {
			POS_IN_SUBMENU--;
			//wsk_podmenu--; //jad� po kolejnych elementach tablicy danego podmenu
			if (POS_IN_SUBMENU < 0) {
				POS_IN_SUBMENU = (liczba_1_podmenu(POS_IN_MAIN_MENU) - 1);
			}
			Lcd_show_Sub_Menu_1(POS_IN_MAIN_MENU, POS_IN_SUBMENU);
			Delay_100ms(4);
			//Return_Value = (wsk_podmenu + POS_IN_SUBMENU)->retval;
			break;
		}
		case SWITCH_LEWO: {   // klawiszem w lewo opuszczam  dane menu
			Delay_100ms(4);
			return 0;
			break;
		}
		case SWITCH_OK: {
			Delay_100ms(4);
			if ((wsk_podmenu + POS_IN_SUBMENU)->nextpodmenu != 0) { // je�li pokazuje na warto�� r�zn� od 0
				Return_Value = Pod_Menu2(); //tutaj wchodz� w opcje drugiego podmenu (funkcja go obs�uguj�ca) i zwracam warto�� Return Value
				if (Return_Value) {
					lcd_cls();
					return Return_Value;
				}
			} else { // je�li ma warto�� 0 czyli nie ma ju� zadnego podmenu to zwracam warto�� t� z menu g��wnego
				Return_Value = (wsk_podmenu + POS_IN_SUBMENU)->retval;
				return Return_Value;
			}

			break;
		}
		}
	}
}
/***************************************************************************************************
 * funkcja obs�uguj�ca drugie  PODMENU (struktura PODPODMENU) bedzie wywo�ywana w PODMENU1
 * @return uint8_t Return_Value
 **************************************************************************************************/
uint8_t Pod_Menu2(void) {
	MENU_GLOWNE *wsk_menu_glowne = main_menu;
	PODMENU *wsk_podmenu = (wsk_menu_glowne + POS_IN_MAIN_MENU)->nextmenu; //tutaj przypisuje wska�nik na sub_menu do tablicy na kt�r� wskazuje element MENU
	PODPODMENU *wsk_podpodmenu = (wsk_podmenu + POS_IN_SUBMENU)->nextpodmenu;// tutaj wska�nik na wywo�ane PODPODMENU z PODMENU
	uint8_t Return_Value = 0;

	uint8_t key_subsubmenu = 0; // zeruje key sub-menu dodatkowy cykl
	Lcd_show_Sub_Menu_2(POS_IN_MAIN_MENU, POS_IN_SUBMENU, POS_IN_SUBSUBMENU);
	while (1) {
		key_subsubmenu = Read_Pin_Input_Switch();
		switch (key_subsubmenu) {
		case SWITCH_DOL: {
			POS_IN_SUBSUBMENU++;
			//wsk_podmenu++; //jad� po kolejnych elementach tablicy
			if (POS_IN_SUBSUBMENU > 2) {//(liczba_2_podmenu(POS_IN_MAIN_MENU))) {//liczba_1_podmenu(wsk_menu_glowne + POS_IN_MAIN_MENU)/******to tak na razie to tylko dla 1 przypadku ustawie�*/) {
				POS_IN_SUBSUBMENU = 0;
			}
			Lcd_show_Sub_Menu_2(POS_IN_MAIN_MENU, POS_IN_SUBMENU,
					POS_IN_SUBSUBMENU);
			Delay_100ms(4);
			break;
		}
		case SWITCH_GORA: {
			POS_IN_SUBSUBMENU--;
			//wsk_podmenu--; //jad� po kolejnych elementach tablicy
			if (POS_IN_SUBSUBMENU < 0) {
				POS_IN_SUBSUBMENU = 2;//(liczba_2_podmenu(POS_IN_MAIN_MENU));
			}
			Lcd_show_Sub_Menu_2(POS_IN_MAIN_MENU, POS_IN_SUBMENU,
					POS_IN_SUBSUBMENU);
			Delay_100ms(4);
			//Return_Value = (wsk_podmenu + POS_IN_SUBMENU)->retval;
			break;
		}
		case SWITCH_LEWO: { // klawiszem w lewo opuszczam  dane menu
			Delay_100ms(4);
			lcd_cls();
			return 0;
			break;
		}
		case SWITCH_OK: {
			Delay_100ms(4);
			Return_Value = (wsk_podpodmenu + POS_IN_SUBSUBMENU)->retval;
			return Return_Value;
			break;
		}
		}
	}
}
/*******************************************************************************************************************************************************
 * ******* wy�wietla wersj� firmwaru
 ********************************************************************************************************************************************************/
void show_version_firmware(void) {
	lcd_cls();
	lcd_locate(0, 0);
	lcd_str_P(PL_oprog);
	lcd_locate(1, 0);
	lcd_str_P(PL_wersja);
	Delay_sec(2);
	lcd_cls();
	return;
}
/***********************************************************************************************************************************************************
 * Funkcja wybieraj�ca rozmiar danego 1podmenu w zale�no�ci od wybranego Podmenu// s�uzy do okre�lania max i min wymiar�w
 * @param pozycja
 * @return
 ********************************************************************************************************************************************************/

int liczba_1_podmenu(int pozycja) {
	switch (pozycja) {
	case 0:
		return liczba_1_podmenu1;
		break;
	case 1:
		return liczba_1_podmenu2;
		break;
	case 2:
		return liczba_1_podmenu3;
		break;
	default:
		return -1;
		break;
	}
}

/***********************************************************************************************************************************************************
 * Funkcja wybieraj�ca rozmiar danego 2podmenu w zale�no�ci od wybranego POdPodmenu// s�uzy do okre�lania max i min wymiar�w
 * @param pozycja
 * @return
 ********************************************************************************************************************************************************/

/*int liczba_2_podmenu(int pozycja) {
 switch (pozycja) {
 case 0:
 return liczba_1_podmenu1;
 break;
 case 1:
 return liczba_1_podmenu2;
 break;
 case 2:
 return liczba_1_podmenu3;
 break;
 default:
 return -1;
 break;
 }
 }*/

