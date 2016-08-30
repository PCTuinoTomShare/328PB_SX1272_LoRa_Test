/*
 * TWI_Task.c
 *
 * Created: 2016/6/27 上午 09:26:36
 *  Author: tom hsieh
 */ 

#include <avr/io.h>
#include "TWI_Task.h"
#include "var.h"

// TWI #0 initialize.
void TWI0_Init( void )
{
	//TWI ( I2C also ).
	// - 100KHz @ 8MHz.
	TWBR0 = 32;
}

// TWI #0 write task trigger on.
void TWI0_On_Write( void )
{
	// Reset task counter.
	twi0_task_cnt = 1;
	// Set busy flag bit.
	twi0_task_status |= 0x01;
	// Send start condition.
	TWCR0 = 0xa4;	
}

// TWI #0 read task trigger on.
void TWI0_On_Read( void )
{
	// Reset task counter.
	twi0_task_cnt = 6;
	// Set busy flag bit.
	twi0_task_status |= 0x01;	
	// Send start condition.
	TWCR0 = 0xa4;	
}

// TWI #0 task.
void TWI0_Task( void )
{
	// Check interrupt flag.
	temp1 = TWCR0;
	temp1 &= 0x80;
	if( temp1 == 0x00 ){
		return;
	}
	// Clear flag bit.
	//TWCR |= 0x80;
	
	// Get status code.
	temp1 = TWSR0;
	temp1 &= 0xf8;
	
	switch( twi0_task_cnt ){
		
		// SLA + W was send.
		case 12:
		// Send control word.
		TWDR0 = twi0_word;
		// 1.) Clear interrupt flag,
		// 2.) and trigger on.
		TWCR0 = 0x84;
		// Next task.
		++twi0_task_cnt;
		break;
		
		// Control word was send.
		case 13:
		// Restart condition.
		// 1.) Clear interrupt flag,
		// 2.) Start condition.
		// 3.) and trigger on.
		TWCR0 = 0xa4;
		// Next task.
		++twi0_task_cnt;
		break;
		
		// SLA+R was send.
		case 15:
		case 7:
		// Check ACK or NACK.
		// NACK
		if( temp1 == 0x48){
			// Stop condition out.
			// 1.) Clear interrupt flag,
			// 2.) stop condition,
			// 3.) trigger on TWI.
			TWCR0 = 0x94;
			// Clear flag bit.
			twi0_task_status &= 0xfe;
			// NACK check.
			twi0_task_cnt = 0;
			return;
		}
		// 1.) Clear interrupt flag.
		// 2.) Prepare received data and return ACK.
		// 3.) Trigger on.
		TWCR0 = 0xC4;
		// Next task.
		++twi0_task_cnt;
		break;
		
		// Received data.
		case 16:
		case 8:
		// check status data received and ACK return.
		
		// Hold shifter data.
		twi0_data[twi0_data_index] = TWDR0;
		// Next byte data.
		++twi0_data_index;
		--twi0_data_cnt;
		
		// Check data counter.
		if( twi0_data_cnt == 1 ){
			// Last data will receive.
			// Prepare receive first byte data.
			
			// 1.) Clear interrupt flag.
			// 2.) Received data and return NACK.
			// 3.) Trigger on.
			TWCR0 = 0x84;
			// Next task.
			++twi0_task_cnt;
		}
		else{
			// 1.) Clear interrupt flag.
			// 2.) Received data and return ACK.
			// 3.) Trigger on.
			TWCR0 = 0xC4;
		}
		break;

		// All data received.
		case 17:
		case 9:
		// Hold shifter data.
		twi0_data[twi0_data_index] = TWDR0;
		// All data send.
		case 3:
		// Stop condition out.
		// 1.) Clear interrupt flag,
		// 2.) stop condition,
		// 3.) trigger on TWI.
		TWCR0 = 0x94;
		// Clear flag bit.
		twi0_task_status &= 0xfe;
		// Next task.
		++twi0_task_cnt;
		break;
		
		// SLA+W was send or
		// Data was send.
		case 2:
		// ACK or NACK check.
		// Only ACK.
		if( temp1 == 0x18 || temp1 == 0x28 ){
			// Send data.
			TWDR0 = twi0_data[twi0_data_index];
			// 1.) Clear interrupt flag,
			// 2.) and trigger on.
			TWCR0 = 0x84;
			// Next data.
			--twi0_data_cnt;
			++twi0_data_index;
			if( twi0_data_cnt != 0 ){
				return;
			}
			// Next task.
			++twi0_task_cnt;
			return;
		}
		else if( temp1 == 0x20 ){
			// Stop condition out.
			// 1.) Clear interrupt flag,
			// 2.) stop condition,
			// 3.) trigger on TWI.
			TWCR0 = 0x94;
			// Clear flag bit.
			twi0_task_status &= 0xfe;
		}
		// Error check.
		twi0_task_cnt = 0;
		break;
		
		case 20: // <--- for HIH6130 start measurement.
			// Stop condition out.
			// 1.) Clear interrupt flag,
			// 2.) stop condition,
			// 3.) trigger on TWI.
			TWCR0 = 0x94;
			// Clear flag bit.
			twi0_task_status &= 0xfe;
			// Next task.
			++twi0_task_cnt;			
			break;
		
		// Start condition was send.
		case 14:
		twi0_addr |= 0x01;
		case 19:	// <--- for HIH6130 only write SLA + W to start measure.
		case 11:
		case 6:
		case 1:
		// Error check.
		if( temp1 != 0x08 ){
			twi0_task_cnt = 0;
			return;
		}
		// Send SLA + W or SLA + R.
		TWDR0 = twi0_addr;
		// 1.) Clear interrupt flag,
		// 2.) and trigger on.
		TWCR0 = 0x84;
		// Clear data index.
		twi0_data_index = 0;
		// Next task.
		++twi0_task_cnt;
		break;
	}
}