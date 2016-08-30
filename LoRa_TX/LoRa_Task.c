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
#include "IO_Task.h"
#include "var.h"

// SX1272 RF initialize register address and parameter.
// Use default frequency 915MHz, no register setting.
// - Address byte + data byte for one register setting.
// - Address byte bit #7 :
//  - 1 : write data to register.
//  - 0 : read read data from register.
const uint8_t RF_Init_Data[44] = {
	//--------------------------------------------------------	
	// #1,  write, address : 0x01, data : 0x00 
	// - Sleep.
	0x81, 0x00,		
	// #2,  write, address : 0x01, data : 0x80 
	// - Enable LoRa / sleep mode.
	0x81, 0x80,		
	// #3,  write, address : 0x01, data : 0x00 
	// - LoRa / stand by mode.
	0x81, 0x81,		
	
	//--------------------------------------------------------	
	// PA configuration.
	0x89, 0x8f,		// #4,  write, address : 0x09, data : 0x8f
	
	0x8a, 0x19,		// #5,  write, address : 0x0a, data : 0x19

	//--------------------------------------------------------	
	// Over current protect.
	0x8b, 0x2b,		// #6,  write, address : 0x0b, data : 0x2b
	
	0x8c, 0x23,		// #7, write, address : 0x0c, data : 0x23
	
	0x9d, 0x23,		// #8, write, address : 0x1d, data : 0x23
	0x9e, 0xb4,		// #9, write, address : 0x1e, data : 0xb4
	0x9f, 0xff,		// #10, write, address : 0x1f, data : 0xff
	
	//--------------------------------------------------------
	// Preamble length.
	// #11, write, address : 0x20, data : 0x00
	// - Preamble length ( bit 15 ~ 8 ).
	0xA0, 0x00,			
	// #12, write, address : 0x21, data : 0x08
	// - Preamble length ( bit 7 ~ 0 ).
	0xA1, 0x08,			

	//--------------------------------------------------------
	// Payload length.	
	// #13, write, address : 0x22, data : 0x0a
	// - Payload length, 10 byte.
	0xA2, 0x0a,		

	0xA3, 0xff,		// #14, write, address : 0x23, data : 0xff
	0xA4, 0x01,		// #15, write, address : 0x24, data : 0x01
	
	0xb1, 0x03,		// #16, write, address : 0x31, data : 0x03
	0xb3, 0x27,		// #17, write, address : 0x33, data : 0x27
	0xb7, 0x0a,		// #18, write, address : 0x37, data : 0x0a
	
	0xda, 0x87 		// #19, write, address : 0x5a, data : 0x87
};

// Frequency setting value.
const uint8_t RF_Freq_Value[6] = {	
	//--------------------------------------------------------
	// Frequency setting, 915MHz ( Power on reset default value ).
	// #1,  write, address : 0x06, data : 0xe4
	// - bit 23 ~ 16.
	0x86, 0xe4,
	// #2,  write, address : 0x07, data : 0xc0
	// - bit 15 ~ 8.
	0x87, 0xc0,
	// #3,  write, address : 0x08, data : 0x00
	// - bit 7 ~ 0.
	0x88, 0x00
};

// Setting for TX task.
const uint8_t TX_Task_Setting[10] = {
	0x81, 0x81,	// LoRa standby.
	0x8d, 0x00, // FIFI pointer base address.
	0x8e, 0x00, // TX pointer base address.
	0x92, 0xff, // Clear IRQ flag.
	0x81, 0x83	// TX on.
};

// Setting for RX task.
const uint8_t RX_Task_Setting[10] = {
	0x81, 0x81, // LoRa standby.
	0x8d, 0x00, // FIFI pointer base address. 
	0xa5, 0x00, // RX pointer base address. 
	0x92, 0xff, // Clear IRQ flag.
	0x81, 0x85  // RX on.
};

// LoRa RF initialize.
void LoRa_RF_Init( void )
{
	temp3 = 0;
	// Set initialize register write data.
	while( temp3 < 38 ){			
		spi_trn_data[0] = RF_Init_Data[ temp3 ];
		++temp3;
		spi_trn_data[1] = RF_Init_Data[ temp3 ];
		++temp3;
		SX1272_Reg_Access();					
	}
}

// LoRa TX task, polling style.
void LoRa_Tx_Task_Poll( void )
{
	// Status flag check.
	// - TX turn on flag bit.
	temp2 = lora_task_flag;
	temp2 &= 0x01;
	if( temp2 == 0 ){
		return;
	}
		
	// SPI busy check.
	temp2 = spi_status_flag;
	temp2 &= 0x01;
	if( temp2 ){
		return;
	}
	// Task dispatch.
	switch( lora_tx_task_cnt ){
		
		case 0:
			// Data LED toggle.
			IO_Data_LED_Toggle();
			// Switch TX on.
			SX1272_TX_Switch_On();
			// Parameter setting count.
			lora_para_cnt = 8;
			// Clear index.
			lora_para_index = 0;	
			// Next task;
			++lora_tx_task_cnt;
			break;
			
		case 1:
			// Output RW + register address.
			spi_trn_data[0] = TX_Task_Setting[ lora_para_index ];
			// For next data.		
			++lora_para_index;
			--lora_para_cnt;
			// Write register.			
			spi_trn_data[1] = TX_Task_Setting[ lora_para_index ];		
			// For next data.
			++lora_para_index;
			--lora_para_cnt;
			// Task trigger on.
			SX1272_Reg_Access_On();			
			// Need more data to write.
			if( lora_para_cnt ){
				return;
			}			
			// Data write to FIFO count.
			lora_para_cnt = 10;
			// Clear index.
			lora_para_index = 0;
			// Register address, data FIFO.
			spi_trn_data[0] = 0x80;			
			// Next task.			
			++lora_tx_task_cnt;			
			break;	
		
		case 2:
			spi_trn_data[1] = rf_tx_data[ lora_para_index ];
			// For next data.
			++lora_para_index;
			--lora_para_cnt;
			// Task trigger on.
			SX1272_Reg_Access_On();
			// Need more data to write.
			if( lora_para_cnt ){
				return;
			}
			// Next task.
			++lora_tx_task_cnt;		
			break;
		
		case 3:
			// LoRa TX on.
			// Output RW + register address.
			spi_trn_data[0] = TX_Task_Setting[ 8 ];
			// Write register.
			spi_trn_data[1] = TX_Task_Setting[ 9 ];
			// Task trigger on.
			SX1272_Reg_Access_On();
			// Next task.
			++lora_tx_task_cnt;		
			break;
		
		case 4:
			// IRQ flag read.
			spi_trn_data[0] = 0x12;		// read address 0x12
			spi_trn_data[1] = 0x00;		// dump out for SPI read.
			spi_rec_data[1] = 0;
			// Task trigger on.
			SX1272_Reg_Access_On();
			// Next task.
			++lora_tx_task_cnt;
			break;
			
		case 5:
			// Check flag bit.
			spi_rec_data[1] &= 0x08;	// Bit #3 = 1, TX done.
			if( spi_rec_data[1] == 0 ){
				// Not done.
				// Previous task.
				--lora_tx_task_cnt;
				return;				
			}
			// Next task.
			++lora_tx_task_cnt;
			// Clear status flag bit.
			lora_task_flag &= 0xfe;					
			break;	
	}	
}

/*
// LoRa TX task.
void LoRa_Tx_Task( void )
{
	// TX switch on.
	SX1272_TX_Switch_On();
				
	// LoRa / standby.
	spi_trn_data[0] = TX_Task_Setting[0]; // write address 0x01
	spi_trn_data[1] = TX_Task_Setting[1]; // write Data 0x81.
	SX1272_Reg_Access();

	// Set frequency.
	//SX1272_Set_Freq();

	// Set FIFO base address.	
	spi_trn_data[0] = TX_Task_Setting[2]; // write address 0x0d
	spi_trn_data[1] = TX_Task_Setting[3]; // value 0x00.
	SX1272_Reg_Access();
	// Set TX base address.	
	spi_trn_data[0] = TX_Task_Setting[4]; // write address 0x0e
	//spi_trn_data[1] = TX_Task_Setting[5];	// value 0x00.	
	SX1272_Reg_Access();

	// Clear all IRQ flag bit.
	spi_trn_data[0] = TX_Task_Setting[6]; // Write data to address 0x12
	spi_trn_data[1] = TX_Task_Setting[7]; // Value 0xff, Clear IRQ flag bit all.
	SX1272_Reg_Access();
				
	// Update TX data.
	temp3 = 0;
	spi_trn_data[0] = 0x80;			// Write data to address 0x00.	
	while( temp3 < 10 ){
		spi_trn_data[1] = rf_tx_data[temp3];	// Write data to FIFO for TX.
		SX1272_Reg_Access();		
		++temp3;
	}
	
	// LoRa / TX.
	//spi_trn_data[0] = 0x81;					// write address 0x01
	//spi_trn_data[1] = 0x82;					// write Data 0x82.
	//SX1272_Reg_Access();
	
	// LoRa / TX.
	spi_trn_data[0] = TX_Task_Setting[8];		// write address 0x01
	spi_trn_data[1] = TX_Task_Setting[9];		// write Data 0x83.
	SX1272_Reg_Access();

	// Waiting for TX done.
	spi_trn_data[0] = 0x12;				// read address 0x12
	spi_trn_data[1] = 0x00;				// dump out for SPI read.
	spi_rec_data[1] = 0;
	while( spi_rec_data[1] == 0 ){
		SX1272_Reg_Access();
		spi_rec_data[1] &= 0x08;			// Bit #3 = 1, TX done.
	}					
}
*/

// LoRa RX task.
void LoRa_Rx_Task_Poll( void )
{
	// Status flag check.
	// - RX turn on flag bit.
	temp2 = lora_task_flag;
	temp2 &= 0x02;
	if( temp2 == 0 ){
		return;
	}
	
	// SPI busy check.
	temp2 = spi_status_flag;
	temp2 &= 0x01;
	if( temp2 ){
		return;
	}
	// Task dispatch.
	switch( lora_rx_task_cnt ){
		
		case 0:
			// Data LED toggle.
			IO_Data_LED_Toggle();		
			// Switch RX on.
			SX1272_RX_Switch_On();
			// Parameter setting count.
			lora_para_cnt = 10;
			// Clear index.
			lora_para_index = 0;
			// Next task;
			++lora_rx_task_cnt;
			break;
		
		case 1:
			// Output RW + register address.
			spi_trn_data[0] = RX_Task_Setting[ lora_para_index ];
			// For next data.
			++lora_para_index;
			--lora_para_cnt;
			// Write register.
			spi_trn_data[1] = RX_Task_Setting[ lora_para_index ];
			// For next data.
			++lora_para_index;
			--lora_para_cnt;
			// Task trigger on.
			SX1272_Reg_Access_On();
			// Need more data to write.
			if( lora_para_cnt ){
				return;
			}
			// Next task.
			++lora_rx_task_cnt;
			break;
		
		case 2:
			// IRQ flag read.
			spi_trn_data[0] = 0x12;		// read address 0x12
			spi_trn_data[1] = 0x00;		// dump out for SPI read.
			spi_rec_data[1] = 0;
			// Task trigger on.
			SX1272_Reg_Access_On();
			// Next task.
			++lora_rx_task_cnt;
			break;
		
		case 3:
			// Check flag bit.
			spi_rec_data[1] &= 0x40;		// Bit #6 = 1, RX done.
			if( spi_rec_data[1] == 0 ){
				// Not done.
				// Previous task.
				--lora_rx_task_cnt;
				return;
			}
			// Data read for FIFO count.
			lora_para_cnt = 10;
			// Clear index.
			lora_para_index = 0;
			// Read FIFO register address.
			spi_trn_data[0] = 0x00;
			// Next task.
			++lora_rx_task_cnt;
			break;

		case 4:		
			// Task trigger on.
			SX1272_Reg_Access_On();
			// Next task.
			++lora_rx_task_cnt;		
			break;
		
		case 5:
			// Hold FIFO data.
			rf_rx_data[ lora_para_index ] = spi_rec_data[1];
			++lora_para_index;
			--lora_para_cnt;
			// More data need receive.
			if( lora_para_cnt ){
				// Previous task.
				--lora_rx_task_cnt;
				return;
			}
			// Next task.
			++lora_rx_task_cnt;
			// Clear status flag bit.
			lora_task_flag &= 0xfd;		
			break;
	}
}

/*
// LoRa RX task.
void LoRa_Rx_Task( void )
{
	// Start received setting.
	// RX switch on.
	SX1272_RX_Switch_On();

	// LoRa / standby.
	spi_trn_data[0] = RX_Task_Setting[0]; // write address 0x01
	spi_trn_data[1] = RX_Task_Setting[1]; // write Data 0x81.
	SX1272_Reg_Access();

	// FIFO base pointer address.
	spi_trn_data[0] = RX_Task_Setting[2]; // Write data to address 0x0d
	spi_trn_data[1] = RX_Task_Setting[3]; // FIFO base address 0x00.
	SX1272_Reg_Access();
	
	// RX base pointer address.
	spi_trn_data[0] = RX_Task_Setting[4]; // Write data to address 0x25
	//spi_trn_data[1] = RX_Task_Setting[5]; // FIFO base address 0x00.
	SX1272_Reg_Access();

	// Clear all IRQ flag bit.
	spi_trn_data[0] = RX_Task_Setting[6]; // Write data to address 0x12
	spi_trn_data[1] = RX_Task_Setting[7]; // Value 0xff, Clear IRQ flag bit all.
	SX1272_Reg_Access();

	// LoRa / RX continuous.
	spi_trn_data[0] = RX_Task_Setting[8]; // write address 0x01
	spi_trn_data[1] = RX_Task_Setting[9]; // write Data 0x85.
	SX1272_Reg_Access();
		
	// Waiting for RX done.
	spi_trn_data[0] = 0x12;				// read address 0x12
	spi_trn_data[1] = 0x00;				// dump out for SPI read.	
	spi_rec_data[1] = 0;
	while( spi_rec_data[1] == 0 ){
		SX1272_Reg_Access();
		spi_rec_data[1] &= 0x40;		// Bit #6 = 1, RX done.
	}
		
	// Read data to FIFO for RX.
	spi_trn_data[0] = 0x00;			// Read address from address 0x00,
	//spi_trn_data[1] = 0x00;			// Dump output for SPI read,	
	temp3 = 0;
	while( temp3 < 10 ){
		SX1272_Reg_Access();
		rf_rx_data[temp3] = spi_rec_data[1];	// Keep RF RX data.		
		++temp3;
	}
}
*/

// SX1272 set frequency.
void SX1272_Set_Freq( void )
{	
	temp3 = 0;
	// Set frequency register write data.
	while( temp3 < 6 ){
		spi_trn_data[0] = RF_Freq_Value[ temp3 ];
		++temp3;
		spi_trn_data[1] = RF_Freq_Value[ temp3 ];
		++temp3;
		SX1272_Reg_Access();
	}
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

// SX1272 Reset on.
void SX1272_Reset_On( void )
{
	// SX1272 Reset output high.
	PORTC |= 0x04;
	DDRC |= 0x04;	
}

// SX1272 Reset off.
void SX1272_Reset_Off( void )
{
	// Reset inactive, IO port keep High impedance.
	PORTC &= 0xfb;	// Output low.
	DDRC &= 0xfb;   // Input port.	
}

// Single register access trigger on, for polling task  trigger on.
void SX1272_Reg_Access_On( void )
{
	spi_trn_ptr = spi_trn_data;		// Output data pointer.
	spi_rec_ptr = spi_rec_data;     // Received data pointer.
	spi_data_cnt = 2;				// 2 byte data.
	SPI_Trigger_On();				// SPI on.	
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
