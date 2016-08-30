/*
 * main.c
 *
 * Created: 2016/8/22 下午 12:47:44
 * Author : tom hsieh
 */ 

#include <avr/io.h>
#include <avr/fuse.h>
#include <avr/interrupt.h>
#include "SPI_Task.h"
#include "IO_Task.h"
#include "Timer_Task.h"
#include "Lora_Task.h"
#include "TWI_Task.h"
#include "ADC_Task.h"
#include "Device_Task.h"
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

int main(void)
{
	// System clock prescaler 1.
	CLKPR = 0x80;  // Enable.
	CLKPR = 0x00;  // Write value.
		
	// IO initialize.
	IO_Init();
	// ADC initialize.
	ADC_Init();
	// SPI initialize.
	SPI_Init();
	// TWI initialize.
	TWI0_Init();
	// Timer #0 initialize.
	Timer0_Init();
	// Enable global interrupt.
	sei();
	
	// Power on / Reset hold delay for SX1272.
	// About 30ms.
	delay_cnt = 3;
	while( delay_cnt ){
		Timer0_Task();
	}
	
	// SX1272 reset off.	
	SX1272_Reset_Off();

	// Reset delay for SX1272.
	// About 50ms.
	delay_cnt = 5;
	while( delay_cnt ){
		Timer0_Task();
	}
		
	// SX1272 RF initialize.
	LoRa_RF_Init();
	
	// Data reset.
	// - TX data.
	temp1 = 0;
	if( temp1 < 10 ){
		rf_tx_data[temp1] = 0;
		++temp1;
	}
	// - TX task count.
	lora_tx_task_cnt = 0;
		
	while (1)
	{
		// Timer #0 task.
		Timer0_Task();
		// ADC task.
		ADC_Task();
		// TWI task.
		TWI0_Task();		
		// SPI task.
		SPI_Task();		
		// LoRa task.
		LoRa_Tx_Task_Poll();
		// Peripheral devices task dispatch.
		Device_Task();
	}
}
