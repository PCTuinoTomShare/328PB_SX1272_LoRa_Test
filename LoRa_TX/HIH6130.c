/*
 * File:   HIH6130.c
 * Author: tom hsieh
 *
 * Created on 2014?11?1?, ?? 10:05
 */


#include "TWI_Task.h"
#include "HIH6130.h"
#include "var.h"

// Measurement request.
void HIHI6130_Measure_Request( void )
{
	// SLA.
    twi0_addr = HIH6130_WR;        
    // Trigger on write task.
	TWI0_On_Write();
	// Set task count.
	twi0_task_cnt = 19;
}

// Read humidity and temperature data.
void HIH6130_I2C_On_HT( void )
{	
	// SLA.
    twi0_addr = HIH6130_RD;        	
	// 4 byte to read.
	twi0_data_cnt = 4;
	// Trigger on read task.
	TWI0_On_Read();	
}