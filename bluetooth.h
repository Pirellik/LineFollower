/*
 * bluetooth.h
 *biblioteka do oblugi interfejsu bluetooth
 *  Author: Artur Hadasz
 */ 

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#include <stdio.h>

void bluetooth_init();
void send_char(char c);
void send_uint16(uint16_t i);
char receiveByte();
static int uart_putchar(char c, FILE *stream);
void sendString(char *text);
int uart_getchar(FILE *stream);

static int uart_putchar (char c, FILE *stream)
{
	if (c == '\n')
	uart_putchar('\r', stream);
	
	// Wait for the transmit buffer to be empty
	while (  !(USARTC0_STATUS & USART_DREIF_bm) );
	
	// Put our character into the transmit buffer
	USARTC0_DATA = c;
	
	return 0;
}



#endif /* BLUETOOTH_H_ */