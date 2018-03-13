/*
 *  Author: Artur Hadasz
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "bluetooth.h"

/**
Zainicjalizuj transmisjê przez UART na porcie C - a wiêc przez modu³ bluetooth.
*/
void bluetooth_init(){
	//Zgodnie z tutorialem http://morf.lv/modules.php?name=tutorials&lasit=29, aby ustalic baudrate na 9600 bps
	
	//PORTC_OUTSET = PIN3_bm; //Let's make PC7 as TX
	PORTC_DIRSET = PIN3_bm; //TX pin as output
	
	//PORTC_OUTCLR = PIN2_bm;
	PORTC_DIRCLR = PIN2_bm; //PC6 as RX
	
	USARTC0_BAUDCTRLB = 0; 
	USARTC0_BAUDCTRLA = 207; //0xC dla 2 MHz 103 - dla taktowania procesora 16 MHz Dla 32 Mhz - 207
	
	//Brak przerwan
	USARTC0_CTRLA = 0;
	
	//8 data bits, no parity and 1 stop bit
	USARTC0.CTRLC = USART_CHSIZE_8BIT_gc; //| USART_PMODE_DISABLED_gc;
	
	// Turn on transmission and reception.
	USARTC0.CTRLB |= USART_RXEN_bm | USART_TXEN_bm;
}

void send_char(char c){
	while( !(USARTC0_STATUS & USART_DREIF_bm) ); //Wait until DATA buffer is empty
	USARTC0_DATA = c;
}

void send_uint16(uint16_t  i){
	int16_t help;
	char LSB;
	char MSB;
	help = i >> 8;
	i = i - (help << 8);
	LSB = (char) i;
	MSB = (char) help;
	send_char((char) 1);
	send_char((char) 0);
	send_char((char) 0);
	send_char(LSB);
	send_char(MSB);
}


char receiveByte()
{
	while( !(USARTC0_STATUS & USART_RXCIF_bm) ); //Interesting DRIF didn't work.
	return USARTC0_DATA;
}

int uart_getchar(FILE *stream)
{
	while( !(USARTC0_STATUS & USART_RXCIF_bm) ); //Interesting DRIF didn't work.
	char data = USARTC0_DATA;
	if(data == '\r')
	data = '\n';
	uart_putchar(data, stream);
	return data;
}



void sendString(char *text)
{
	while(*text)
	{
		send_char(*text++);
	}
}