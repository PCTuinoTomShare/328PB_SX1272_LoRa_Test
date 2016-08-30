/*
 * Timer_Task.c
 *
 * Created: 2016/6/27 上午 12:25:47
 *  Author: tom hsieh
 */ 

#include <avr/io.h>
#include "Timer_Task.h"
#include "var.h"

// Timer #0 initialize.
void Timer0_Init( void )
{
	// 8MHz oscillator, 0.125us * 1024 = 128us.
	// period, 128us * 256 = 32.768 ms.	
	
	// CTC mode, no output.
	TCCR0A = 0x02;
	// Prescaler 1024.
	TCCR0B = 0x05; 
	// comparator A.
	// - 10ms / 256us = 38.
	OCR0A = 39;
}

// Timer #0 task.
void Timer0_Task( void )
{
	// Check interrupt flag bit. 
	temp1 = TIFR0;
	temp1 &= 0x02;
	if( temp1 == 0 ){
		return;
	}
	// About 10ms past.
	// Clear flag bit.
	TIFR0 |= 0x02;	
	
	// Delay count.
	if( delay_cnt ){
		--delay_cnt;
	}
}
