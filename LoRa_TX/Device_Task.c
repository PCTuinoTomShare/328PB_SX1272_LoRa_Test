/*
 * Device_Task.c
 *
 * Created: 2016/6/27 上午 10:23:49
 *  Author: tom hsieh
 */ 

#include <avr/io.h>
#include "Device_Task.h"
#include "HIH6130.h"
#include "Lora_Task.h"
#include "var.h"

// device task.
void Device_Task( void ){
	
	// check delay counter.
	if( delay_cnt ){
		return;
	}
		
	// TWI task busy check.
	temp1 = twi0_task_status;
	temp1 &= 0x01;
	if( temp1 ){
		// Still busy.
		return;
	}
	
	// Check LoRa TX busy bit.
	temp1 = lora_task_flag;
	temp1 &= 0x01;
	if( temp1 ){
		return;
	}
			
	switch( dev_task_cnt ){
				
		case 0:
			// Start HIH6130 measurement.	
			HIHI6130_Measure_Request();
			// Next task.
			++dev_task_cnt;
			// Reset measurement delay count.
			// 100ms.
			delay_cnt = 10;
			break;
					
		case 1:
			// Read measurement data.
			HIH6130_I2C_On_HT();
			// Next task.
			++dev_task_cnt;			
			break;
		
		case 2:
			// Prepare TX data.
			// HIH6130 humidity.
			temp5 = twi0_data[0];
			temp5 <<= 8;
			temp5 &= 0x3f00;
			temp5 |= twi0_data[1];		
			temp5 *= 100;
			temp5 /= 16383;		
			rf_tx_data[0] = (uint8_t)temp5;		
			// HIH6130 temperature.
			temp5 = twi0_data[2];
			temp5 <<= 8;
			temp5 &= 0xff00;
			temp5 |= twi0_data[3];
			temp5 >>= 2;
			temp5 *= 165;
			temp5 /= 16383;		
			temp1 = 0x00;		// Minus Sign.
			if( temp5 < 40 ){
				temp6 = temp5;
				temp5 = 40;
				temp5 -= temp6;
				temp1 = 0x80;
			}
			else{
				temp5 -= 40;
			}		
			rf_tx_data[1] = (uint8_t)temp5;
			rf_tx_data[1] |= temp1;
			// ADC data.
			rf_tx_data[2] = adc_value;
			// Next task.
			++dev_task_cnt;
			break;
				
		case 3:			
			// CRC.
			rf_tx_data[9] = rf_tx_data[0];
			rf_tx_data[9] += rf_tx_data[1];
			rf_tx_data[9] += rf_tx_data[2];
			// 900ms delay.
			delay_cnt = 90;			
			// Next task.
			++dev_task_cnt;
			break;
			
		case 4:
			// Start LoRa TX task.
			lora_task_flag |= 0x01;
			lora_tx_task_cnt = 0;			
			// Next task.
			dev_task_cnt = 0;
			break;	
	}
}
