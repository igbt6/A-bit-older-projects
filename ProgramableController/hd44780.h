/*
 * hd44780.h
 *
 *  Created on: 2011-07-10
 *       Autor: £ukasz Uszko // Biblioteka MIRKA KARDASIA
 *        */

#ifndef HD44780_H_
#define HD44780_H_


// rozdzielczoœæ wyœwietlacza LCD (wiersze/kolumny)
#define LCD_Y 2		// iloœæ wierszy wyœwietlacza LCD
#define LCD_X 16	// iloœæ kolumn wyœwietlacza LCD

// tu ustalam za pomoc¹ zera lub jedynki czy steruje pinem RW
//	0 - pin RW pod³¹czony na sta³e do GND
//	1 - pin RW pod³¹czony do mikrokontrolera
#define USE_RW 1

//----------------------------------------------------------------------------------------
//
//		Ustawienia sprzêtowe po³¹czeñ sterownika z mikrokontrolerem
//
//----------------------------------------------------------------------------------------
// tu konfigurujemy port i piny do jakich pod³¹czymy linie D7..D4 LCD
#define LCD_D7PORT  A
#define LCD_D7 3
#define LCD_D6PORT  A
#define LCD_D6 2
#define LCD_D5PORT  A
#define LCD_D5 1
#define LCD_D4PORT  A
#define LCD_D4 0


// tu definiujemy piny procesora do których pod³¹czamy sygna³y RS,RW, E
#define LCD_RSPORT B
#define LCD_RS 0

#define LCD_RWPORT B
#define LCD_RW 1

#define LCD_EPORT B
#define LCD_E 2



//****************************************************************************************
//*																						 *
//*		U S T A W I E N I A   KOMPILACJI												 *
//*																						 *
//*		W³¹czam kompilacjê komend u¿ywanych lub wy³¹czam nieu¿ywanych					 *
//*		(dziêki temu reguluje zajêtoœæ pamiêci FLASH po kompilacji)					 *
//*																						 *
//*		1 - oznacza W£¥CZENIE do kompilacji												 *
//*		0 - oznacza wy³¹czenie z kompilacji (funkcja niedostêpna)						 *
//*																						 *
//****************************************************************************************
//----------------------------------------------------------------------------------------

#define USE_LCD_LOCATE	1			// ustawia kursor na wybranej pozycji Y,X (Y=0-3, X=0-n)

#define USE_LCD_CHAR 	1			// wysy³a pojedynczy znak jako argument funkcji

#define USE_LCD_STR_P 	1			// wysy³a string umieszczony w pamiêci FLASH
#define USE_LCD_STR_E 	1			// wysy³a string umieszczony w pamiêci FLASH

#define USE_LCD_INT 	1			// wyœwietla liczbê dziesietn¹ na LCD
#define USE_LCD_HEX 	1			// wyœwietla liczbê szesnastkow¹ na LCD

#define USE_LCD_DEFCHAR		1		// wysy³a zdefiniowany znak z pamiêci RAM
#define USE_LCD_DEFCHAR_P 	1		// wysy³a zdefiniowany znak z pamiêci FLASH
#define USE_LCD_DEFCHAR_E 	1		// wysy³a zdefiniowany znak z pamiêci EEPROM

#define USE_LCD_CURSOR_ON 		1	// obs³uga w³¹czania/wy³¹czania kursora
#define USE_LCD_CURSOR_BLINK 	1	// obs³uga w³¹czania/wy³¹czania migania kursora
#define USE_LCD_CURSOR_HOME 	1	// ustawia kursor na pozycji pocz¹tkowej

//------------------------------------------------  koniec ustawieñ kompilacji ---------------




// definicje adresów w DDRAM dla ró¿nych wyœwietlaczy
// inne s¹ w wyœwietlaczach 2wierszowych inne w 4wierszowych (od razu na przysz³osc bedzie)
#if ( (LCD_Y == 4) && (LCD_X == 16) )
#define LCD_LINE1 0x00		// adres 1 znaku 1 wiersza
#define LCD_LINE2 0x28		// adres 1 znaku 2 wiersza
#define LCD_LINE3 0x14  	// adres 1 znaku 3 wiersza
#define LCD_LINE4 0x54  	// adres 1 znaku 4 wiersza
#else
#define LCD_LINE1 0x00		// adres 1 znaku 1 wiersza
#define LCD_LINE2 0x40		// adres 1 znaku 2 wiersza
#define LCD_LINE3 0x10  	// adres 1 znaku 3 wiersza
#define LCD_LINE4 0x50  	// adres 1 znaku 4 wiersza
#endif




// Makra upraszczaj¹ce dostêp do portów
// *** PORT
#define PORT(x) XPORT(x)
#define XPORT(x) (PORT##x)
// *** PIN
#define PIN(x) XPIN(x)
#define XPIN(x) (PIN##x)
// *** DDR
#define DDR(x) XDDR(x)
#define XDDR(x) (DDR##x)


// Komendy steruj¹ce
#define LCDC_CLS					0x01
#define LCDC_HOME					0x02
#define LCDC_ENTRY					0x04
	#define LCDC_ENTRYR					0x02
	#define LCDC_ENTRYL					0
	#define LCDC_MOVE					0x01
#define LCDC_ONOFF					0x08
	#define LCDC_DISPLAYON				0x04
	#define LCDC_CURSORON				0x02
	#define LCDC_CURSOROFF				0
	#define LCDC_BLINKON				0x01
#define LCDC_SHIFT					0x10
	#define LCDC_SHIFTDISP				0x08
	#define LCDC_SHIFTR					0x04
	#define LCDC_SHIFTL					0
#define LCDC_FUNC					0x20
	#define LCDC_FUNC8B					0x10
	#define LCDC_FUNC4B					0
	#define LCDC_FUNC2L					0x08
	#define LCDC_FUNC1L					0
	#define LCDC_FUNC5x10				0x04
	#define LCDC_FUNC5x7				0
#define LCDC_SET_CGRAM				0x40
#define LCDC_SET_DDRAM				0x80





// deklaracje funkcji na potrzeby innych modu³ów
void lcd_init(void);								// W£¥CZONA na sta³e do kompilacji
void lcd_cls(void);									// W£¥CZONA na sta³e do kompilacji
void lcd_put_str(char * str);							// W£¥CZONA na sta³e do kompilacji

void lcd_locate(uint8_t y, uint8_t x);				// domyœlnie W£¥CZONA z kompilacji w pliku lcd.c

void lcd_char(char c);								// domyœlnie wy³¹czona z kompilacji w pliku lcd.c
void lcd_str_P(char * str);							// domyœlnie wy³¹czona z kompilacji w pliku lcd.c
void lcd_str_E(char * str);							// domyœlnie wy³¹czona z kompilacji w pliku lcd.c
void lcd_int(int val);								// domyœlnie wy³¹czona z kompilacji w pliku lcd.c
void lcd_hex(int val);								// domyœlnie wy³¹czona z kompilacji w pliku lcd.c
void lcd_defchar(uint8_t nr, uint8_t *def_znak);	// domyœlnie wy³¹czona z kompilacji w pliku lcd.c
void lcd_defchar_P(uint8_t nr, uint8_t *def_znak);	// domyœlnie wy³¹czona z kompilacji w pliku lcd.c
void lcd_defchar_E(uint8_t nr, uint8_t *def_znak);	// domyœlnie wy³¹czona z kompilacji w pliku lcd.c

void lcd_home(void);								// domyœlnie wy³¹czona z kompilacji w pliku lcd.c
void lcd_cursor_on(void);							// domyœlnie wy³¹czona z kompilacji w pliku lcd.c
void lcd_cursor_off(void);							// domyœlnie wy³¹czona z kompilacji w pliku lcd.c
void lcd_blink_on(void);							// domyœlnie wy³¹czona z kompilacji w pliku lcd.c
void lcd_blink_off(void);							// domyœlnie wy³¹czona z kompilacji w pliku lcd.c
void move_char_on_lcd(char* tab1,char* tab2);        // Funkcja przesuwaj¹ca obraz na wyswietlaczu od lewej do prawej i z powrotem

//teksty

extern char  PL_tab1[];
extern char PL_tab2[];
extern char PL_cont[];
extern char PL_save[];
extern char PL_error[];
extern char PL_powitanie[];
extern char PL_wersja[];
extern char PL_oprog[];
extern char PL_next_alarm[];



#endif /* HD44780_H_ */
