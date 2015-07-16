/**
 *****************************************************************************
 **
 **  File        : main.c
 **
 **  Abstract    : main function.
 **
 **  Functions   : main
 **
 **  Environment : Atollic TrueSTUDIO/STM32
 **                STMicroelectronics STM32F10x Standard Peripherals Library
 **
 **  Distribution: The file is distributed “as is,” without any warranty
 **                of any kind.
 **
 **  (c)Copyright Atollic AB.
 **  You may use this file as-is or modify it according to the needs of your
 **  project. Distribution of this file (unmodified or modified) is not
 **  permitted. Atollic AB permit registered Atollic TrueSTUDIO(R) users the
 **  rights to distribute the assembled, compiled & linked contents of this
 **  file as part of an application binary file, provided that it is built
 **  using the Atollic TrueSTUDIO(R) toolchain.
 **
 **
 *****************************************************************************
 */

/* Includes */

/*
 *OPIS POSZCZEGÓLNYCH POŁĄCZEŃ UKŁADU STM32 DISCCOVERY_VALUE z wyswietlaczem SPL501C oraz switchami
 *
 *SWITCHE: sprawdzany stan GND jak wciniete, kazdy zbocznikowany kond. ceramicznym 4,7 nF , oraz przez rezystor 100 OM do masy  czyli GND->100oM->SWITCH(rownolegle capacitor)->PORT WEJSCOIWY
 *       -> LEWO: PC13
 *		 -> PRAWO: PC1
 *		 -> DOL: PC4
 *       -> GORA: PC3
 *		 -> OK: PC5 ,chyba sprawdzic jak cos
 *
 *LCD: zasilanie z 3,3V z plytki
 *	-> 1: GND
 *	-> 2: 3v3
 *	-> 3: GND
 *	-> 4: PA7
 *	-> 5: PA6
 *	-> 6: PA5
 *	-> 7: PA4
 *	-> 8: PA3
 *	-> 9: PA2
 *	-> 10: PA1
 *	-> 11: PA0
 *	-> 12: PB8
 *	-> 13: PB9
 *	-> 14: PB10
 *	-> 15: PB11
 *	-> 16: PB12
 *	->	17 i 18 : not connected
 * */


#include "stm32f10x.h"
#include "my_library/init_modules.h"
#include "splc501c.h"
#include "grafika.h"
#include "stdlib.h"
#define INTRO 1
#define GRA 2
#define GAMEOVER 3
#define PAUZA 4

#define POFFX 30//wsp. X gornego lewego rogu planszy
#define POFFY 0		//wsp. Y górnego lewego rogu planszy
#define NKOFFX 75	//wsp. X górnego lewego rogu miejsca wyœwietlania nastepnego klocka
#define NKOFFY 48	//wsp. Y górnego lewego rogu miejsca wyœwietlania nastepnego klocka
#define WOFFX 100	//wsp. X wyniku
#define WOFFY 1//wsp. Y wyniku
#define LOFFX 100//wsp. X ilosci linii
#define LOFFY 3	//wsp. Y ilosci linii
#define TOFFX 101// wsp x i y timera
#define TOFFY 6
#define LEVOFFX 12// wsp i y levela
#define LEVOFFY 1

unsigned char Kolizja(unsigned char x, unsigned char y, unsigned char r);
void KlocekObroc(void);
void KlocekWklej(void);
void KlocekNowy(void);
void KlocekLewo(void);
void KlocekPrawo(void);
void KlocekDol(void);
void RysujEkran(void);
void CzyscPlansze(void);
void GLCD_ClearPreviousKlocek(void);
void GLCD_ClearNextKlocek(void);
void OdswiezEkran(void);

const unsigned char klocki[7][4][4][2] = { //[rodzaj klocka][polozenie klocka][segment klocka][wsp. pustego pixela klocka 0- x, 1- y]
		{ { { 1, 1 }, { 4, 1 }, { 4, 4 }, { 1, 4 } }, { { 1, 1 }, { 4, 1 }, { 4,
				4 }, { 1, 4 } }, { { 1, 1 }, { 4, 1 }, { 4, 4 }, { 1, 4 } }, { {
				1, 1 }, { 4, 1 }, { 4, 4 }, { 1, 4 } } }, //kwadrat

				{ { { 1, 4 }, { 4, 4 }, { 4, 1 }, { 7, 1 } }, { { 1, 1 },
						{ 4, 4 }, { 1, 4 }, { 4, 7 } }, { { 1, 7 }, { 4, 4 }, {
						4, 7 }, { 7, 4 } }, { { 4, 1 }, { 4, 4 }, { 7, 4 }, { 7,
						7 } } }, //eska prawa

				{ { { 1, 1 }, { 4, 4 }, { 4, 1 }, { 7, 4 } }, { { 1, 4 },
						{ 4, 4 }, { 1, 7 }, { 4, 1 } }, { { 1, 4 }, { 4, 4 }, {
						4, 7 }, { 7, 7 } }, { { 4, 7 }, { 4, 4 }, { 7, 1 }, { 7,
						4 } } }, //eska lewa

				{ { { 1, 4 }, { 4, 4 }, { 7, 4 }, { 7, 1 } }, { { 1, 1 },
						{ 4, 4 }, { 4, 1 }, { 4, 7 } }, { { 1, 4 }, { 4, 4 }, {
						1, 7 }, { 7, 4 } }, { { 4, 1 }, { 4, 4 }, { 4, 7 }, { 7,
						7 } } }, //elka prawa

				{ { { 1, 1 }, { 4, 4 }, { 1, 4 }, { 7, 4 } }, { { 1, 7 },
						{ 4, 4 }, { 4, 7 }, { 4, 1 } }, { { 1, 4 }, { 4, 4 }, {
						7, 4 }, { 7, 7 } }, { { 4, 1 }, { 4, 4 }, { 7, 1 }, { 4,
						7 } } }, //elka lewa

				{ { { 1, 4 }, { 4, 4 }, { 4, 1 }, { 7, 4 } }, { { 1, 4 },
						{ 4, 4 }, { 4, 1 }, { 4, 7 } }, { { 1, 4 }, { 4, 4 }, {
						7, 4 }, { 4, 7 } }, { { 4, 1 }, { 4, 4 }, { 4, 7 }, { 7,
						4 } } }, //trojkat

				{ { { 1, 4 }, { 4, 4 }, { 7, 4 }, { 10, 4 } }, { { 4, 1 }, { 4,
						4 }, { 4, 7 }, { 4, 10 } }, { { 1, 4 }, { 4, 4 },
						{ 7, 4 }, { 10, 4 } }, { { 4, 1 }, { 4, 4 }, { 4, 7 }, {
						4, 10 } } } //kryska
		};

// ZMIENNE GLOBALNE W PROGRAMIE
unsigned char plansza[30][60]; //Tablica przechowuj¹ca planszê gry
unsigned char klocekx, kloceky, klocekr; //Zmienne po³o¿enia klocka x,y i obrót s³owo
volatile unsigned char kloceknr; //Zmienna okreœla rodzaj spadaj¹cego klocka
volatile unsigned char nkloceknr; //Zmienna okreœla rodzaj nastêpnego klocka
volatile unsigned char rkloceknr; //Zmienna na potrzeby losowania rodzaju klocka
volatile unsigned char stan_gry; //Zmienna okreœlaj¹ca stan programu
unsigned int punkty; //Punkty
volatile unsigned int linie; //Iloœæ zaliczonych linii
volatile bool refresh; //Wpisanie 1 spowoduje odœwiezenie wyswietlacza czyki funkcje Rysuj Ekran
unsigned char szybkosc; //Predkosc gry
unsigned char linie_etap; //Zmienna odliczajaca linie przed zwiekszeniem predkosci gry
unsigned char level; // level
unsigned char stimer; //Zmienna odlicza czas miêdzy przesuniêciami klocka w dó³
volatile unsigned char sek, min; //zmienne czasu
volatile bool flag_tlo;
vu32 TimeDelay;

int main(void) {

	RCC_Config();
	EXTI_Config();
	NVIC_Config();
	GPIO_Config();
	Systick_Config();
///SPI_init();
	GLCD_Init(); // inicjalizacja wyswietlacza
	GLCD_ClearScreen();
	CzyscPlansze(); //Czyszczenie planszy
	KlocekNowy(); //Zaladowanie nowego klocka
	punkty = 0; //Zerowanie liczników z punktami
	linie = 0;
	level = 1;
	szybkosc = 10;
	sek = 0, min = 0;
	stan_gry = INTRO;
	refresh = 1;
	flag_tlo = true;

	while (1) {


		switch (stan_gry) //sprawdzanie stanu gry
		{
		case GRA: //gra uruchomiona
			if (flag_tlo == false) {
				GLCD_ClearScreen();
				GLCD_bmp(tlo);
				flag_tlo = true;
			}
			if(!(GPIOC->IDR&GPIO_IDR_IDR4)){        /// ciągłe opadanie klocka w dół // jak trzymam switcha
						{KlocekDol();
						OdswiezEkran();
						Delay(1);
					//	if(!(GPIOC->IDR&GPIO_IDR_IDR4)){continue;}
						//else break;
						}
					}
			if (refresh == 1) {
				refresh = 0;
				RysujEkran();
			}
			break;

		case GAMEOVER: //koniec gry
			GLCD_bmp(game_over);

			break;

		case PAUZA: //pauza
			GLCD_bmp(pause);
			break;

		case INTRO: //intro
			GLCD_bmp(intro);
			sek = 0, min = 0;
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Funkcja zeruje pamiec planszy
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CzyscPlansze(void) {
	unsigned char x, y;

	for (y = 0; y < 60; y++) //odliczanie wierszy
		for (x = 0; x < 30; x++) //odliczanie kolumn
			plansza[x][y] = 0; //wypelnianie komórek zerami
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Funkcja wykrywa kolizje klocka z punktami na planszy
//oraz wykroczenie klocka poza plansze
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char Kolizja(unsigned char x, unsigned char y, unsigned char r) {
	int8_t kx, ky, i, kolizja;
	kolizja = 0;

	for (i = 0; i < 4; i++) //petla odlicza kolejne cztery segmenty klocka
			{ //(kazdy klocek sklada sie z czterech segmentow)
		kx = klocki[kloceknr][r][i][0] + x; //obliczanie wspó³rzêdnych na planszy danego segmentu klocka
		ky = klocki[kloceknr][r][i][1] + y;
		if (kx < 0)
			kolizja = 1;
		if (kx > 29)
			kolizja = 1; //sprawdzenie czy segment jest poza œciankami bocznymi planszy
		else if (ky > 59)
			kolizja = 1; //sprawdzenie czy segment jest poni¿ej dna planszy
		else if (plansza[abs(kx)][ky + 1])
			kolizja = 1; //sprawdzenie czy segment nie naklada sie na istniejacy punkt na planszy
	}
	return kolizja; //jezeli wykryto kolizje  to funkcja zwraca 1 w innym razie 0
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Funkcja przesuwa klocek w lewo
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void KlocekLewo(void) {
	if (Kolizja(klocekx - 3, kloceky, klocekr) == 0) //sprawdzenie czy przesuniêty klocek nie wywo³a kolizji
			{ //jezeli nie to:
		//////////////GLCD_ClearPreviousKlocek();
		klocekx -= 3; //zmniejsz zmienna x po³o¿enia klocka

	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Funkcja przesuwa klocek w prawo
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void KlocekPrawo(void) {
	if (Kolizja(klocekx + 3, kloceky, klocekr) == 0) //sprawdzenie czy przesuniêty klocek nie wywo³a kolizji
			{ //je¿eli nie to:
		/////////GLCD_ClearPreviousKlocek();
		klocekx += 3; //zwiekszam zmienna x polozenia klocka

	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Funkcja realizuje obrót klocka
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void KlocekObroc(void) {
	if (Kolizja(klocekx, kloceky, klocekr + 1) == 0) //sprawdzenie czy obrócony klocek nie wywo³a kolizji
			{
		//////////GLCD_ClearPreviousKlocek();
		klocekr++; //zwiêksz zmienn¹ obrotu klocka
		if (klocekr > 3)
			klocekr = 0;

	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Funkcja kopiuje bie¿¹cy klocek na planszê jako jej stałe punkty
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void KlocekWklej(void) {
	unsigned char x, y, i;
	for (i = 0; i < 4; i++) //pêtla odlicza kolejne 4 segmenty klocka
			{
		x = klocki[kloceknr][klocekr][i][0] + klocekx; //oblicz po³o¿enie segmentu na planszy
		y = klocki[kloceknr][klocekr][i][1] + kloceky;
		plansza[x][y] = 1; //zapalam punkt na planszy w miejscu pustego  segmentu klocka
		plansza[x - 1][y] = 1; //
		plansza[x - 1][y + 1] = 1; //
		plansza[x - 1][y - 1] = 1; // i pozostale otaczajace go wpisuje w ta tablice
		plansza[x][y + 1] = 1; //
		plansza[x][y - 1] = 1; //
		plansza[x + 1][y] = 1; //
		plansza[x + 1][y + 1] = 1; //
		plansza[x + 1][y - 1] = 1;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Funkcja laduje nowy klocek
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void KlocekNowy(void) {
	kloceknr = nkloceknr; //kopiuje rodzaj klocka jaki byl nastepny jako biezacy
	nkloceknr = rkloceknr; //kopiuje rodzaj nastêpnego klocka z zmiennej losujacej
	if (kloceknr == nkloceknr) //jezeli nastêpny klocek jest taki sam jak biezacy
			{
		rkloceknr++; //zwieksz zmienna losujaca
		if (rkloceknr > 6)
			rkloceknr = 0;
		nkloceknr++; //zwieksz zmienna z rodzajem kolejnego klocka
		if (nkloceknr > 6)
			nkloceknr = 0;
	}
	klocekr = 0; //zerowanie polozenia nowego klocka na planszy
	klocekx = 12;
	kloceky = 0;

	if (Kolizja(klocekx, kloceky, klocekr) == 0) //sprawdzanie czy nowy klocek nie powoduje kolizji
			{ //je¿eli nie:
		punkty++; //zwieksz punktacje

	} else //jezeli nowy klocek wywolal od razy kolizje
	{
		///GLCD_ClearScreen();
		stan_gry = GAMEOVER; //koniec gry!
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Funkcja wykrywa pelna linie na planszy i kasuje ja zwiêkszaja c o 10 punkty za kazda
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Redukcja(void) {
	unsigned char x, y, k, yy; // zmienne pomocnicze
	static uint8_t licznik_linii;/// points, lines; // zmienna potrzebna mi do dzielenia punktów i wyniku

	for (y = 0; y < 60; y++) //pêtla oblicza kolejne linie planszy
			{
		k = 1;
		for (x = 0; x < 30; x++) {
			if ((plansza[x][y] == 0)) { //jesli nie mam pelnego rzad zlozony z pelnych 3 pikseli
				k = 0;
			} // else if (plansza[x][y] == 1) {
			  //k = 1;
		}

		if (k == 1) //jezeli biezaca linia jest pelna
				{
			licznik_linii++;
			//linie++; //zwieksz punkty
			//punkty += 10;
			//lines++;
			//points+=10;
			linie_etap++; //zwiekszam zmienna odliczajaca klocki do kolejnego zwiêkszenia predkosci gry
			if (linie_etap >= 15) //sprawdzenie czy zwiekszam juz speed po scieciu 5 linii
					{
				linie_etap = 0;
				if (szybkosc > 0)
					szybkosc--;
				level++;

			}


			for (yy = y; yy > 0; yy--) //petla odlicza kolejne linie od biezacej w góre
					{
				for (x = 0; x < 30; x++) { //pêtla kopiuje liniê yy-1 do linii yy
					plansza[x][yy] = plansza[x][yy - 1];
				} //spowoduje to skasowanie pelnej linii i przesuniecie wszystkich linii ponad o 3 piksele w dół
				  //////////////////////	punkty /= 3;
				  ////////////////////	linie /= 3;
			}
			if (licznik_linii == 3) {
							linie ++;
							punkty+=10;
							licznik_linii = 0;
						}

		}
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Funkcja przesuwa klocek w dol
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void KlocekDol(void) {

	if (Kolizja(klocekx, kloceky + 3, klocekr) == 0) //sprawdzenie czy przesuniety klocek nie wywola kolizji
			{ //jezeli nie to:
		////////	GLCD_ClearPreviousKlocek();

		kloceky += 3; //zwieksz zmienna y polozenia klocka

	} else { //jezeli wykryto kolizje:
		KlocekWklej(); //skopiuj segmenty klocka na plansze
		Redukcja(); //sprawdzam  czy nie ma pelnych linii,,,  redukcja do okodzenia bo mam buga
		GLCD_ClearNextKlocek();
		KlocekNowy(); //zaladuj nowy klocek
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 void GLCD_ClearPreviousKlocek(void) { // funkcją usuwam poprzedniego klocka
 uint8_t x, y;
 if (kloceky >= 5 && kloceky < 56) {
 for (y = kloceky - 5; y < kloceky + 4; y++) {
 //	GLCD_GoTo(POFFX, (POFFY + y));
 for (x = 0; x < 30; x++) {
 //GLCD_WriteChar(' ');
 GLCD_GoTo(POFFX + x, ((POFFY + y) / 8));
 GLCD_WriteData(0);
 }
 }
 } else if (kloceky < 5) {
 for (y = kloceky; y < kloceky + 8; y++) {
 //	GLCD_GoTo(POFFX, (POFFY + y));
 for (x = 0; x < 30; x++) {
 //GLCD_WriteChar(' ');
 GLCD_GoTo(POFFX + x, ((POFFY + y) / 8));
 GLCD_WriteData(0);
 }

 }
 }

 *////////////////////////////////////// na samym dole przed kolejna funkcja jeszcze odkomencic nawias

/*
 if (kloceky >=3 && kloceky < 57) {
 for (y = kloceky -3; y < kloceky +3; y++) {
 //	GLCD_GoTo(POFFX, (POFFY + y));
 for (x = 0; x < 30; x++) {
 //GLCD_WriteChar(' ');
 GLCD_SetPixel(POFFX + x, (POFFY + y), false);
 }
 }
 } else if (kloceky < 3) {
 for (y = 0; y < kloceky +3; y++) {
 //	GLCD_GoTo(POFFX, (POFFY + y));
 for (x = 0; x < 30; x++) {
 //GLCD_WriteChar(' ');
 GLCD_SetPixel(POFFX + x, (POFFY + y), false);
 }
 }
 } else if (kloceky >= 57) {
 for (y = kloceky - 3; y < 60; y++) {
 //	GLCD_GoTo(POFFX, (POFFY + y));
 for (x = 0; x < 30; x++) {
 //GLCD_WriteChar(' ');
 GLCD_SetPixel(POFFX + x, (POFFY + y), false);
 }
 }
 }
 */
/*
 for (i = 0; i < 4; i++) //odliczanie 4 segmentów klocka
 {
 x = klocki[kloceknr][klocekr][i][0] + klocekx; //obliczanie wspolrzêdnych na planszy danego segmentu klocka
 y = klocki[kloceknr][klocekr][i][1] + kloceky;

 GLCD_SetPixel(POFFX + x - 1, POFFY + y, false);
 GLCD_SetPixel(POFFX + x - 1, POFFY + y + 1, false);
 GLCD_SetPixel(POFFX + x - 1, POFFY + y - 1, false);
 GLCD_SetPixel(POFFX + x, POFFY + y + 1, false);
 GLCD_SetPixel(POFFX + x, POFFY + y - 1, false);
 GLCD_SetPixel(POFFX + x + 1, POFFY + y, false);
 GLCD_SetPixel(POFFX + x + 1, POFFY + y + 1, false);
 GLCD_SetPixel(POFFX + x + 1, POFFY + y - 1, false);

 }
 */
////}
void GLCD_ClearNextKlocek(void) {
	uint8_t x, y;
	for (y = 0; y < 8; y++) {
		for (x = 0; x < 12; x++) {
			GLCD_SetPixel(NKOFFX + x, NKOFFY + y, false);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Funkcja rysowania ekranu podczas gry
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RysujEkran(void) {
	unsigned char x, y, i;
	unsigned int val, c, d;
//GLCD_ClearScreen();
//GLCD_bmp(tlo);
///	GLCD_ClearPreviousKlocek();
	for (y = 0; y < 60; y++) { //odliczanie wierszy pamiêci planszy
		for (x = 0; x < 30; x++) //odliczanie kolumn
				{
			if (!(plansza[x][y])) { //// tam gdzie zera w tablicy laduje 0
				GLCD_SetPixel(POFFX + x, POFFY + y, false);
			}

		}

	}

//rysownie spadajacego klocka
	for (i = 0; i < 4; i++) //odliczanie 4 segmentów klocka
			{
		x = klocki[kloceknr][klocekr][i][0] + klocekx; //obliczanie wspolrzêdnych na planszy danego segmentu klocka
		y = klocki[kloceknr][klocekr][i][1] + kloceky;

		GLCD_SetPixel(POFFX + x - 1, POFFY + y, true);
		GLCD_SetPixel(POFFX + x - 1, POFFY + y + 1, true);
		GLCD_SetPixel(POFFX + x - 1, POFFY + y - 1, true);
		GLCD_SetPixel(POFFX + x, POFFY + y + 1, true);
		GLCD_SetPixel(POFFX + x, POFFY + y - 1, true);
		GLCD_SetPixel(POFFX + x + 1, POFFY + y, true);
		GLCD_SetPixel(POFFX + x + 1, POFFY + y + 1, true);
		GLCD_SetPixel(POFFX + x + 1, POFFY + y - 1, true);

	}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//rysownie klocka, który bêdzie nastêpny
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	for (i = 0; i < 4; i++) //odliczanie 4 segmentów klocka
			{
		x = klocki[nkloceknr][0][i][0]; //obliczanie wspó³rzêdnych na ekranie danego segmentu klocka
		y = klocki[nkloceknr][0][i][1];
		GLCD_SetPixel(NKOFFX + x - 1, NKOFFY + y, true); //rysowanie kolejnych 8 pikseli danego  segmentu klocka
		GLCD_SetPixel(NKOFFX + x - 1, NKOFFY + y + 1, true);
		GLCD_SetPixel(NKOFFX + x - 1, NKOFFY + y - 1, true);
		GLCD_SetPixel(NKOFFX + x, NKOFFY + y + 1, true);
		GLCD_SetPixel(NKOFFX + x, NKOFFY + y - 1, true);
		GLCD_SetPixel(NKOFFX + x + 1, NKOFFY + y, true);
		GLCD_SetPixel(NKOFFX + x + 1, NKOFFY + y + 1, true);
		GLCD_SetPixel(NKOFFX + x + 1, NKOFFY + y - 1, true);
	}
//wyswietlanie wyniku
	GLCD_GoTo(WOFFX, WOFFY);
	val = punkty;
	for (d = 10000; d > 0; d = d / 10) //odliczanie kolejnych miejsc dziesietnych
			{
		c = val / d; //dzielenie wyniku na tysi¹ce, setki itd.
		val = val - (c * d); //odejmowanie tysiêcy, setek, itd. od wyniku,
							 //aby mo¿na by³o obliczyæ kolejne miejsce dziesiêtne w kolejnym kroku pêtli
		c = c + '0'; //konwersja wyniku na cyfrê w kodzie ASCII
		GLCD_WriteChar(c); //wyœwietlanie kolejnych cyfr wyniku na ekranie
	}

//Wyswietlanie liczby zaliczonych linii
	GLCD_GoTo(LOFFX, LOFFY);
	val = linie;
	for (d = 10000; d > 0; d = d / 10) {
		c = val / d;
		val = val - (c * d);
		c = c + '0';
		GLCD_WriteChar(c);
	}

//Wyswietlanie time 'u
///if (flag_timer) {
	GLCD_GoTo(TOFFX, TOFFY);

	if (min < 10) {
		GLCD_WriteChar('0');
		GLCD_WriteChar(min + '0');
		GLCD_WriteChar(':');
	} else {
		GLCD_WriteChar((min / 10) + '0');
		GLCD_WriteChar((min % 10) + '0');
		GLCD_WriteChar(':');
	}
	if (sek < 10) {
		GLCD_WriteChar('0');
		GLCD_WriteChar(sek + '0');
	} else {
		GLCD_WriteChar((sek / 10) + '0');
		GLCD_WriteChar((sek % 10) + '0');
	}
//Wyswietlanie levela
	GLCD_GoTo(LEVOFFX, LEVOFFY);
	GLCD_WriteChar(level + '0');
//flag_timer = false;
//}

	//rysowanie stalych punktów na planszy
	for (y = 0; y < 60; y++) { //odliczanie wierszy pamiêci planszy
		for (x = 0; x < 30; x++) //odliczanie kolumn
				{
			if (plansza[x][y]) //tutaj zapalam piksele wokół pustego punktu w poszczegolnym segmencie klocka
			{
				{
					GLCD_SetPixel(POFFX + x, POFFY + y, true);
				}
			}
			//else if(!(plansza[x][y])){GLCD_SetPixel(POFFX + x, POFFY + y, false);}
		}
	}
	/////GLCD_bmp(tlo);
}

//Funkcja ustawia zmienna refresh
void OdswiezEkran(void) {
	refresh = 1;
}

//Funkcja do odliczania czasu do nastepnego przesuniecia klocka w dol
//zmienna speed reguluje ten czas

unsigned char TimerGry(void) {
	unsigned char next = 0;
	stimer++; //zwiêksz zmienn¹ odliczaj¹c¹ czas
	if (stimer > szybkosc) //je¿eli minie kolejny odcinek czasu
			{ //zale¿ny od zmiennej szybkosc
		stimer = 0;
		next = 1;
	}
	return next; //funkcja zwróci wartoœæ 1
}

