/*
 * IO_Task.c
 *
 * Created: 2016/8/18 下午 03:57:05
 *  Author: tom hsieh
 */ 

#include <avr/io.h>
#include "IO_Task.h"

// IO initialize.
void IO_Init( void )
{
	// PE2 output port for SPI #1 /SS.
	// PE3 output port for SPI #1 MOSI.
	DDRE = 0x0c;
	// PC1 output port for SPI #1 SCK.
	DDRC = 0x02;
	// PD7 output port for SX1272 RX_Switch.
	DDRD = 0x80;
	// PB0 output port for SX1272 TX_Switch.
	// PB1 input port for SX1272 IRQ.
	DDRB = 0x01;

	// SS output high, inactive.
	PORTE |= 0x04;	
	// TX_Switch output low.
	PORTB &= 0xfe;
	// RX_Switch output high.
	PORTD |= 0x80;	
}

