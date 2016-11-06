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
    twi1_addr = HIH6130_WR;
	// 0 byte to write.
	twi1_data_cnt = 0;	        
    // Trigger on write task.
	TWI1_On_Write();
}

// Read humidity and temperature data.
void HIH6130_I2C_On_HT( void )
{	
	// SLA.
    twi1_addr = HIH6130_RD;        	
	// 4 byte to read.
	twi1_data_cnt = 4;
	// Trigger on read task.
	TWI1_On_Read();	
}