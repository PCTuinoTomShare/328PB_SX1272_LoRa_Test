/*
 * LoRa_Task.c
 *
 * Created: 2016/8/18 下午 05:22:27
 *  Author: tom hsieh
 */ 

#include <stdint.h>
#include <avr/io.h>
#include "Lora_Task.h"
#include "SPI_Task.h"
#include "var.h"

// SX1272 RF initialize register address and parameter.
// - Address +  data.
// - Address bit #7 :
//  - 1 : write data to register.
//  - 0 : read read data from register.
const uint8_t RF_Init_Data[44] = {
	
	// #1,  write, address : 0x01, data : 0x00 
	// - Sleep.
	0x81, 0x00,		
	// #2,  write, address : 0x01, data : 0x00 
	// - Enable LoRa mode.
	0x81, 0x80,		
	// #3,  write, address : 0x01, data : 0x00 
	// - Wake / LoRa mode.
	0x81, 0x81,		
	
	// Frequency setting, 915MHz.
	// #4,  write, address : 0x06, data : 0xd9
	// - bit 23 ~ 16.
	0x86, 0xd9,		
	// #5,  write, address : 0x07, data : 0x00
	// - bit 15 ~ 8.
	0x87, 0x00,		
	// #6,  write, address : 0x08, data : 0x00
	// - bit 7 ~ 0.
	0x88, 0x00,
	
	0x89, 0x8f,		// #7,  write, address : 0x09, data : 0x8f
	0x8a, 0x19,		// #8,  write, address : 0x0a, data : 0x19
	0x8b, 0x2b,		// #9,  write, address : 0x0b, data : 0x2b
	0x8c, 0x23,		// #10, write, address : 0x0c, data : 0x23
	
	0x9d, 0x23,		// #11, write, address : 0x1d, data : 0x23
	0x9e, 0xb4,		// #12, write, address : 0x1e, data : 0xb4
	0x9f, 0xff,		// #13, write, address : 0x1f, data : 0xff
	
	// #14, write, address : 0x20, data : 0x00
	// - Preamble length ( bit 15 ~ 8 ).
	0xA0, 0x00,			
	// #15, write, address : 0x21, data : 0x08
	// - Preamble length ( bit 7 ~ 0 ).
	0xA1, 0x08,			
	// #16, write, address : 0x22, data : 0x0a
	// - Payload length, 10 byte.
	0xA2, 0x0a,		
	
	0xA3, 0xff,		// #17, write, address : 0x23, data : 0xff
	0xA4, 0x01,		// #18, write, address : 0x24, data : 0x01
	
	0xb1, 0x03,		// #19, write, address : 0x31, data : 0x03
	0xb3, 0x27,		// #20, write, address : 0x33, data : 0x27
	0xb7, 0x0a,		// #21, write, address : 0x37, data : 0x0a
	
	0xda, 0x87,		// #22, write, address : 0x5a, data : 0x87

};

// LoRa RF initialize.
void LoRa_RF_Init( void )
{
	temp3 = 0;
	// Set initialize register write data.
	spi_trn_ptr =  RF_Init_Data;
	while( temp3 < 22 ){		
		// Set SPI received data.
		spi_rec_ptr = spi_rec_data;								
		// Data count.
		// address + data.
		spi_data_cnt = 2;
		// SPI task trigger on.	
		temp2 = 0x01;		
		SPI_Trigger_On();
		// SPI task loop.
		while( temp2 ){
			SPI_Task();
			temp2 = spi_status_flag;
			temp2 &= 0x01;
		}				
		++temp3;		
	}
}

// LoRa TX open.
void LoRa_Tx_Open( void )
{
	spi_trn_data[0] = 0x81;					// write address 0x01
	spi_trn_data[1] = 0x81;					// write Data 0x81.
	SX1272_Reg_Access();
	
	// Load FIFO TX pointer.
	// - Read FIFO base address.
	spi_trn_data[0] = 0x0e;					// read address 0x0e
	spi_trn_data[1] = 0x00;					// dump out for SPI read.
	SX1272_Reg_Access();			
	// - Set FIFO base address.	
	spi_trn_data[0] = 0x8d;					// write address 0x0d
	spi_trn_data[1] = spi_rec_data[1];		// FIFO base address.
	SX1272_Reg_Access();	
	
	// TX switch on.
	SX1272_TX_Switch_On();
	
	spi_trn_data[0] = 0x91;					// write address 0x11
	spi_trn_data[1] = 0x87;					// IRQ.
	SX1272_Reg_Access();

	spi_trn_data[0] = 0x92;					// write address 0x12
	spi_trn_data[1] = 0xff;					// IRQ mask.
	SX1272_Reg_Access();
			
}

// LoRa TX close.
void LoRa_Tx_Close( void )
{
	spi_trn_data[0] = 0x81;					// write address 0x01
	spi_trn_data[1] = 0x82;					// write Data 0x82.
	SX1272_Reg_Access();

	spi_trn_data[0] = 0x81;					// write address 0x01
	spi_trn_data[1] = 0x83;					// write Data 0x83.
	SX1272_Reg_Access();

	// Waiting for TX done.
	spi_rec_data[1] = 0;
	while( spi_rec_data[1] == 0 ){	
		spi_trn_data[0] = 0x12;				// read address 0x12
		spi_trn_data[1] = 0x00;				// dump out for SPI read.
		SX1272_Reg_Access();	
		spi_rec_data[1] &= 0x08;			// Bit #3 = 1, TX done.
	}	
	SX1272_RX_Switch_On();					// RX switch on.
}

// LoRa RX open.
void LoRa_RX_Open( void )
{
	spi_trn_data[0] = 0x81;					// write address 0x001
	spi_trn_data[1] = 0x82;					// write Data 0x85.
	SX1272_Reg_Access();

	// Load FIFO TX pointer.
	// - Read FIFO base address.
	spi_trn_data[0] = 0x0e;					// Read data from address 0x0e
	spi_trn_data[1] = 0x00;					// Dump out for SPI read.
	SX1272_Reg_Access();
	// - Set FIFO base address.
	spi_trn_data[0] = 0x8d;					// Write data to address 0x0d
	spi_trn_data[1] = spi_rec_data[1];		// FIFO base address.
	SX1272_Reg_Access();

	// RX switch on.
	SX1272_RX_Switch_On();

	spi_trn_data[0] = 0x91;			// Write data to address 0x11
	spi_trn_data[1] = 0x87;			// Value 0x87, IRQ.
	SX1272_Reg_Access();

	spi_trn_data[0] = 0x92;			// Write data to address 0x12
	spi_trn_data[1] = 0xff;			// Value 0xff, IRQ mask.
	SX1272_Reg_Access();	
}

// LoRa RX Close.
void LoRa_RX_Close( void )
{
	spi_trn_data[0] = 0x92;			// Write data to address 0x12
	spi_trn_data[1] = 0xff;			// Value 0xff, IRQ mask.
	SX1272_Reg_Access();	
}

// LoRa TX task.
void LoRa_Tx_Task( void )
{
	LoRa_Tx_Open();					// Open TX.
	// Write data to FIFO for TX.
	spi_trn_data[0] = 0x80;			// Write data to address 0x00.
	
	// Update TX data.
	temp2 = 0;
	temp3 = 1;
	while( temp2 < 10 ){	
		spi_trn_data[temp3] = rf_tx_data[temp2];	
		++temp2;
		++temp3;
	}
		
	spi_trn_ptr = spi_trn_data;		// Output data pointer.
	spi_rec_ptr = spi_rec_data;     // Received data pointer.
	spi_data_cnt = 11;				// 11 byte data.
	SPI_Trigger_On();				// SPI on.
	SX1272_SPI_Loop();				// SPI loop.	
	LoRa_Tx_Close();				// Close TX.
}

// LoRa RX task.
void LoRa_Rx_Task( void )
{
	LoRa_RX_Open();					// Open RX.
	
	spi_trn_data[0] = 0x92;			// Write data to address 0x12
	spi_trn_data[1] = 0xff;			// value 0xff, IRQ mask.
	SX1272_Reg_Access();
	
	// Wait IRQ triggered.
	temp2 = 0;
	while( temp2 == 0){	
		temp2 = PINB;
		temp2 &= 0x02;
	}
	// IRQ trigger, read RX data packet.
	
	// Get current FIFO RX pointer.
	spi_trn_data[0] = 0x25;			// Read data from address 0x25
	spi_trn_data[1] = 0x00;			// Dump out for read.
	SX1272_Reg_Access();	
	temp3 = spi_rec_data[1];		
	
	// Get received payload size.
	spi_trn_data[0] = 0x13;			// Read data from address 0x13
	spi_trn_data[1] = 0x00;			// Dump out for read.
	SX1272_Reg_Access();	
	temp3 -= spi_rec_data[1];

	// Set FIFO pointer for SPI received.	
	spi_trn_data[0] = 0x8d;			// Write data from address 0x0d
	spi_trn_data[1] = temp3;		// FIFO pointer.
	SX1272_Reg_Access();

	// Read data to FIFO for RX.
	spi_trn_data[0] = 0x00;			// Read address from address 0x00,
	spi_trn_ptr = spi_trn_data;		// Output data pointer.
	spi_rec_ptr = spi_rec_data;     // Received data pointer.
	spi_data_cnt = 11;				// 11 byte data.
	SPI_Trigger_On();				// SPI on.
	SX1272_SPI_Loop();				// SPI loop.
	LoRa_RX_Close();				// Close RX.	
}

// SX1272 RX switch on.
void SX1272_RX_Switch_On( void )
{
	PORTB &= 0xfe;					// TX_Switch output low.	
	PORTD |= 0x80;					// RX_Switch output high.
}

// SX1272 TX switch on.
void SX1272_TX_Switch_On( void )
{
	PORTD &= 0x7f;					// RX_Switch output low.		
	PORTB |= 0x01;					// TX_Switch output high.
}

// Single register access.
void SX1272_Reg_Access( void )
{
	spi_trn_ptr = spi_trn_data;		// Output data pointer.
	spi_rec_ptr = spi_rec_data;     // Received data pointer.
	spi_data_cnt = 2;				// 2 byte data.
	SPI_Trigger_On();				// SPI on.
	SX1272_SPI_Loop();				// SPI loop.
}

// SPI transmit loop.
void SX1272_SPI_Loop( void )
{
	// Task loop.
	temp2 = 0x01;
	while( temp2 ){
		SPI_Task();
		temp2 = spi_status_flag;
		temp2 &= 0x01;
	}	
}