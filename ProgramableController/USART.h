/*
 * USART.h
 *
 *  Created on: 2012-03-08
 *       Autor: £ukasz Uszko
 */

#ifndef USART_H_
#define USART_H_



#define UART_BAUD 9600		// tu definiujemy interesuj¹c¹ nas prêdkoœæ
#define __UBRR F_CPU/16/UART_BAUD-1  // obliczamy UBRR dla U2X=0

// definicje na potrzeby RS485
#define UART_DE_PORT PORTD
#define UART_DE_DIR DDRD
#define UART_DE_BIT (1<<PD2)

#define UART_DE_ODBIERANIE  UART_DE_PORT |= UART_DE_BIT
#define UART_DE_NADAWANIE  UART_DE_PORT &= ~UART_DE_BIT


#define UART_RX_BUF_SIZE 32 // definiujemy bufor o rozmiarze 32 bajtów
// definiujemy maskê dla naszego bufora
#define UART_RX_BUF_MASK ( UART_RX_BUF_SIZE - 1)

#define UART_TX_BUF_SIZE 16 // definiujemy bufor o rozmiarze 16 bajtów
// definiujemy maskê dla naszego bufora
#define UART_TX_BUF_MASK ( UART_TX_BUF_SIZE - 1)




#endif /* USART_H_ */
