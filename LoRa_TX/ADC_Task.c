/*
 * ADC_Task.c
 *
 * Created: 2016/7/29 上午 11:08:41
 *  Author: tom hsieh
 */ 

#include <avr/io.h>
#include "ADC_Task.h"
#include "var.h"

// ADC initialize.
void ADC_Init( void )
{
	// Reference on VACC.
	// Data right align.
	// ADC3.
	ADMUX = 0x63;
	// Power reduction ADC off, for ADC.
	PRR0 &= 0xfe;
	// Single conversion ( no auto trigger ). 
	// Clock division 128.
	ADCSRA = 0x97;
	// Start conversion.
	ADCSRA |= 0x40;	
}

// ADC task.
void ADC_Task( void )
{
	// ADC interrupt flag bit check.
	temp1 = ADCSRA;
	temp1 &= 0x10;
	if( temp1 == 0 ){
		return;
	}
	// Clear flag bit.
	ADCSRA |= 0x10;
	// Keep ADC value.
	adc_value = ADCH;	
	// Start conversion.
	ADCSRA |= 0x40;	
}
