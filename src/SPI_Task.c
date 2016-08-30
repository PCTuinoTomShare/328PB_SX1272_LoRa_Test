/*
 * SPI_Task.c
 *
 * Created: 2016/8/18 下午 02:28:24
 *  Author: tom hsieh
 */ 

#include <avr/io.h>
#include "SPI_Task.h"
#include "var.h"

// SPI initialize.
void SPI_Init( void )
{
	// SPI #1.
	// - MSB first.
	// - SCK = fosc/128.
	// CPOL = 0, CPHA = 0.
	// - Master mode.
	SPCR1 = 0x13;	
	// SPI #1 enable.
	SPCR1 |= 0x40;
	
	// For clear interrupt flag bit.
	temp1 = SPDR1;
}

// SPI task.
void SPI_Task( void )
{
	// Check interrupt flag bit.
	temp1 = SPSR1;
	temp1 &= 0x80;	
	if( temp1 == 0 ){
		return;
	}
	// Hold received data and clear interrupt flag bit.
	*spi_rec_ptr = SPDR1;
	++spi_rec_ptr;
	// Check data count.
	if( spi_data_cnt  ){
		
		// Transmit data.
		SPDR1 = *spi_trn_ptr;		
		// for next data.
		++spi_trn_ptr;
		--spi_data_cnt;
		return;
	}
	// SS output inactive.
	PORTE |= 0x04;
	// Clear busy bit.
	spi_status_flag &= 0xfe;	
}

// SPI trigger on.
void SPI_Trigger_On( void )
{
	// Set busy flag bit.
	spi_status_flag |= 0x01;
	// SS output active.
	PORTE &= 0xfb;
	// Assign pointer to data array.
	//spi_rec_ptr = spi_rec_data;
	//spi_trn_ptr = spi_trn_data;
	// First byte output.
	// Check data count.
	if( spi_data_cnt  ){		
		// Transmit data.
		SPDR1 = *spi_trn_ptr;
		// for next data.
		++spi_trn_ptr;
		--spi_data_cnt;
	}	
}