/*
 * main.c
 *
 * Created: 2016/8/22 下午 12:51:34
 * Author : tom hsieh
 */ 

#include <avr/io.h>
#include <avr/fuse.h>
#include <avr/interrupt.h>
#include "UART_Task.h"
#include "SPI_Task.h"
#include "IO_Task.h"
#include "Timer_Task.h"
#include "Lora_Task.h"
#include "var.h"

FUSES = {
	// 8MHz internal RC oscillator.
	// Slow rising power.
	.low = (FUSE_SUT_CKSEL0 & FUSE_SUT_CKSEL2 & FUSE_SUT_CKSEL3 & FUSE_SUT_CKSEL4 ),
	// Boot from 0x0000.
	// SPI programming enable.
	// debugWIRE enable.
	.high = (FUSE_BOOTSZ0 & FUSE_BOOTSZ1 & FUSE_SPIEN & FUSE_DWEN ),
	// No boot loader lock bit.
	.extended = EFUSE_DEFAULT,
};

// Get humidity & temperature data for show.
void get_HT_for_Show();

int main(void)
{
	// System clock prescaler 1.
	CLKPR = 0x80;  // Enable.
	CLKPR = 0x00;  // Write value.
	
	// IO initialize.
	IO_Init();
	// SPI initialize.
	SPI_Init();
	// UART #0 initialize.
	UART0_Init();
	// Timer #0 initialize.
	Timer0_Init();
	// Enable global interrupt.
	sei();
	
	// Power on delay for SX1272.
	delay_cnt = 3;
	while( delay_cnt ){
		Timer0_Task();
	}

	// SX1272 reset off.
	SX1272_Reset_Off();

	// Reset delay for SX1272.
	delay_cnt = 3;
	while( delay_cnt ){
		Timer0_Task();
	}
		
	// SX1272 RF initialize.
	LoRa_RF_Init();

	// Initial delay for SX1272.
	delay_cnt = 5;
	while( delay_cnt ){
		Timer0_Task();
	}
			
	// Reset variable.
	// Set flag bit, start LoRa RX task.
	lora_rx_task_cnt = 0;
	lora_task_flag = 0x02;
	// Show data.
	show_data[3] = '%';
	show_data[4] = ' ';	
	show_data[9] = ' ';
	show_data[10] = 'U';
	show_data[11] = 'V';
	show_data[12] = ':';
	show_data[16] = '\r';
	show_data[17] = '\n';

	while (1)
	{
		// UART #0 transmit task.
		UART0_Trn_Task();
		// Timer #0 task.
		Timer0_Task();
		// SPI task.
		SPI_Task();

		// LoRa RX task.
		LoRa_Rx_Task_Poll();
		
		// LoRa RX task task flag check. 
		temp2 = lora_task_flag;
		temp2 &= 0x02;
		if( temp2 == 0 ){
			// Set flag bit, start LoRa RX task.
			lora_task_flag |= 0x02;
			lora_rx_task_cnt = 0;
			get_HT_for_Show();			
		}		
	}
}

// Get humidity & temperature data for show.
void get_HT_for_Show()
{
	// CRC check.
	temp1 = rf_rx_data[0];
	temp1 += rf_rx_data[1];
	temp1 += rf_rx_data[2];
	temp1 -= rf_rx_data[9];
	if( temp1 ){
		return;
	}			
	// Humidity, HIH6130 humidity measurement and
	// converted data.		
	temp1 = rf_rx_data[0];
	temp1 /= 100;
	show_data[0] = (uint8_t)temp1;
	show_data[0] += 0x30;	
	rf_rx_data[0] %= 100;	
	temp1 = rf_rx_data[0];
	temp1 /= 10;
	show_data[1] = (uint8_t)temp1;
	show_data[1] += 0x30;		
	rf_rx_data[0] %= 10;	
	show_data[2] = (uint8_t)rf_rx_data[0];
	show_data[2] += 0x30;		
	// Temperature, HIH6130 temperature and		
	// converted data.
	// - sign.
	show_data[5] = ' ';
	temp2 = rf_rx_data[1];
	temp2 &= 0x80;
	if( temp2 ){
		show_data[5] = '-';
	}
	// value.	
	temp1 = rf_rx_data[1];
	temp1 /= 100;
	show_data[6] = (uint8_t)temp1;
	show_data[6] += 0x30;
	rf_rx_data[1] %= 100;
	temp1 = rf_rx_data[1];
	temp1 /= 10;
	show_data[7] = (uint8_t)temp1;
	show_data[7] += 0x30;
	rf_rx_data[1] %= 10;
	show_data[8] = (uint8_t)rf_rx_data[1];
	show_data[8] += 0x30;
	// UV sensor value.
	// value.
	temp1 = rf_rx_data[2];
	temp1 /= 100;
	show_data[13] = (uint8_t)temp1;
	show_data[13] += 0x30;
	rf_rx_data[2] %= 100;
	temp1 = rf_rx_data[2];
	temp1 /= 10;
	show_data[14] = (uint8_t)temp1;
	show_data[14] += 0x30;
	rf_rx_data[2] %= 10;
	show_data[15] = (uint8_t)rf_rx_data[2];
	show_data[15] += 0x30;	
	// Set flag bit, update show data.
	lora_task_flag |= 0x08;
}
