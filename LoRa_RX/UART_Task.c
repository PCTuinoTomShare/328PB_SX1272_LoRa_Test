/*
 * UART_Task.c
 *
 * Created: 2016/6/26 下午 11:40:16
 *  Author: tom hsieh
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "UART_Task.h"
#include "var.h"


// UART #0 received ISR.
ISR( USART0_RX_vect )
{
	// Clear received time out.
	uart0_rec_to = 0;
	// Hold received data.		
	uart0_rec_data[uart0_rec_cnt] = UDR0;
	if( uart0_rec_cnt < 16 ){
		++uart0_rec_cnt;
	} 
}

// UART #0 initialize.
void UART0_Init( void )
{
	// 9600 bps @ 8MHz.
	UBRR0H = 0;
	UBRR0L = 51;
	// RX interrupt enable.
	// RX enable.
	// TX enable.
	// 8 bit data.
	// No parity.
	UCSR0B = 0x98;
	
	uart0_trn_cnt = 0;
}

// UART #0 transmit task.
void UART0_Trn_Task( void )
{
	// Check data counter.
	if( uart0_trn_cnt == 0 ){
		// Check show data update flag bit.				
		temp1 = lora_task_flag;		
		temp1 &= 0x08;
		if( temp1 ){
			// Clear flag bit.
			lora_task_flag &= 0xf7;
			// Transmit data setting.
			uart0_trn_cnt = 18;	
			uart0_trn_ptr = show_data;	
		}
		return;
	}
	// Check transmit buffer.
	temp1 = UCSR0A;
	temp1 &= 0x20;
	if( temp1 == 0 ){
		return;
	}
	// Data out.
	UDR0 = *uart0_trn_ptr;
	// Next data.
	++uart0_trn_ptr;
	--uart0_trn_cnt;
}
