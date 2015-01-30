 /*2
 * menu.h
 *
 *  Created on: 2012-02-26
 *       Autor: �ukasz Uszko
 */

#ifndef MENU_H_
#define MENU_H_
#define maxlength 16 //maksymalna d�ugo��  stringa w menu
#define liczba_menu_gl 4
//definiuj� liczb� element�w tablicy(rozmiar tych tablic) struktur PODMENU
 //jak b�d� dodawa� nowe podmena to musz� doda� te� w funkcji  int liczba_1_podmenu(int pozycja);
#define liczba_1_podmenu1 3
#define liczba_1_podmenu2 7
#define liczba_1_podmenu3 2
//#define liczba_1_podmenu4
//definicja dla pozycji pod-menu
int liczba_1_podmenu(int pozycja); //funkcja wyliczaj�ca mi granic� przekroczenia ilo�ci wy�wietlanych podmen�w
void Lcd_show_Menu_Glowne(uint8_t pozycja_menu_gl/*, uint8_t pozycja_sub_menu*/); // funkcja wy�wietlajaca menu
void Lcd_show_Sub_Menu_1(uint8_t pozycja_menu_gl, uint8_t pozycja_sub_menu);
uint8_t Menu_Glowne(void); // funkcja obs�uguj�ca przyciski w menu g��wnym wyboru
uint8_t Pod_Menu1(void); // funkcja obs�uguj�ca przyciski w  1pod_menu i wy�wietlaj�ca to PODMEN, jak bede chcia� do�ozy� kolejne Podmena  to tak samo
uint8_t Pod_Menu2(void); //jak poprzednie funkcje tylko �e 2 Podmenu
void show_version_firmware(void);  // wyswietla wersj� firmwaru
extern int POS_IN_MAIN_MENU ;//aktualna pozycja w pod-menu
extern int POS_IN_SUBMENU ;//taka warto�� pocz�tkowa czyli jestem tylko w menu g��wnym
extern int POS_IN_SUBSUBMENU;
#endif /* MENU_H_ */
