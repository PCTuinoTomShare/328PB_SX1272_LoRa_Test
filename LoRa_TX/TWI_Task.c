/*
 * TWI_Task.c
 *
 * Created: 2016/6/27 上午 09:26:36
 *  Author: tom hsieh
 */ 

#include <avr/io.h>
#include "TWI_Task.h"
#include "var.h"

// TWI #1 initialize.
void TWI1_Init( void )
{
	//TWI ( I2C also ).
	// - 100KHz @ 8MHz.
	TWBR1 = 32;
}

// TWI #1 write task trigger on.
void TWI1_On_Write( void )
{
	// Set busy flag bit.
	twi1_task_status |= 0x01;
	// Send start condition.
	TWCR1 = 0xa4;	
}

// TWI #1 read task trigger on.
void TWI1_On_Read( void )
{
	// Set busy flag bit.
	twi1_task_status |= 0x01;	
	// Send start condition.
	TWCR1 = 0xa4;	
}

// TWI #1 task.
void TWI1_Task( void )
{
	// Check interrupt flag.
	temp1 = TWCR1;
	temp1 &= 0x80;
	if( temp1 == 0x00 ){
		return;
	}
	// Clear flag bit.
	//TWCR |= 0x80;
	
	// Get status code.
	temp1 = TWSR1;
	temp1 &= 0xf8;
	
	switch( temp1 ){
		
		// Master receiver mode, master NACK send.
		case 0x58:
		// Master receiver mode, master ACK send.
		case 0x50:
		// Hold shifter data.
		twi1_data[ twi1_data_index ] = TWDR1;
		// Next byte data.
		++twi1_data_index;
		--twi1_data_cnt;
		// SLA + R send, received ACK.
		case 0x40:
		// Last byte received.
		if( twi1_data_cnt == 1 ){
			// Last data will receive.
			// Prepare receive first byte data.
			// 1.) Clear interrupt flag.
			// 2.) Received data and return NACK.
			// 3.) Trigger on.
			TWCR1 = 0x84;
		}
		// No more data received, output stop condition.
		else if( twi1_data_cnt == 0 ){
			// Stop condition out.
			// 1.) Clear interrupt flag,
			// 2.) stop condition,
			// 3.) trigger on TWI.
			TWCR1 = 0x94;
			// Clear flag bit.
			twi1_task_status &= 0xfe;
		}
		else{
			// 1.) Clear interrupt flag.
			// 2.) Received data and return ACK.
			// 3.) Trigger on.
			TWCR1 = 0xC4;
		}
		break;
		
		// Master transmitter mode, master SLA+W send NACK received.
		case 0x20:
		// Master transmitter mode, master data send, NACK received.
		case 0x30:
		twi1_data_cnt = 0;
		// Master transmitter mode, master data send, NACK received.
		case 0x28:
		// Master transmitter mode, master master SLA+W send ACK received.
		case 0x18:
		// No more data send, send stop condition.
		if( twi1_data_cnt == 0 ){
			// Stop condition out.
			// 1.) Clear interrupt flag,
			// 2.) stop condition,
			// 3.) trigger on TWI.
			TWCR1 = 0x94;
			// Clear flag bit.
			twi1_task_status &= 0xfe;
			return;
		}
		// Send data.
		TWDR1 = twi1_data[ twi1_data_index ];
		// 1.) Clear interrupt flag,
		// 2.) and trigger on.
		TWCR1 = 0x84;
		// Next data.
		--twi1_data_cnt;
		++twi1_data_index;
		break;
		
		// Start condition done.
		case 0x08:
		// Send SLA + W or SLA + R.
		TWDR1 = twi1_addr;
		// 1.) Clear interrupt flag,
		// 2.) and trigger on.
		TWCR1 = 0x84;
		// Clear data index.
		twi1_data_index = 0;
		break;
	}
}