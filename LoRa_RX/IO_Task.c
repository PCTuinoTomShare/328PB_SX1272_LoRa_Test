/*
 * IO_Task.c
 *
 * Created: 2016/8/18 下午 03:57:05
 *  Author: tom hsieh
 */ 

#include <avr/io.h>
#include "IO_Task.h"
#include "var.h"

// IO initialize.
void IO_Init( void )
{
	// PE2 output port for SPI #1 /SS.
	// PE3 output port for SPI #1 MOSI.
	DDRE = 0x0c;
	// PC2 output port for SX1272 RESET.
	// PC1 output port for SPI #1 SCK.
	DDRC = 0x06;
	// PD7 output port for SX1272 RX_Switch.
	// PD6 output port for error LED.
	// PD5 output port for data active LED.
	DDRD = 0xe0;
	// PB0 output port for SX1272 TX_Switch.
	// PB1 input port for SX1272 IRQ.
	DDRB = 0x01;

	// SS output high, inactive.
	PORTE |= 0x04;
	// SX1272 Reset output high.
	PORTC |= 0x04;		
	// TX_Switch output low.
	PORTB &= 0xfe;
	// RX_Switch output high.
	PORTD |= 0x80;
	// LEDs off.
	PORTD &= 0x9f;	
}

// Data LED on.
void IO_Data_LED_On( void )
{
	PORTD |= 0x20;
}

// Data LED off.
void IO_Data_LED_Off( void )
{
	PORTD &= 0xdf;	
}

// Data LED toggle.
void IO_Data_LED_Toggle( void )
{
	temp1 = PORTD;
	temp1 &= 0x20;	
	if( temp1 ){
		PORTD &= 0xdf;
		return;		
	}
	PORTD |= 0x20;	
}

// Error LED on.
void IO_Error_LED_On( void )
{
	PORTD |= 0x20;	
}

// Error LED off.
void IO_Error_LED_Off( void )
{
	PORTD &= 0xdf;	
}