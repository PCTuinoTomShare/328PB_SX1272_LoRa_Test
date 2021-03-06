﻿/*
 * var.h
 *
 * Created: 2016/8/18 下午 03:33:11
 *  Author: tom hsieh
 */ 

#include <stdint.h>

#ifndef VAR_H_
#define VAR_H_

uint8_t temp1;				// Temporary #1.
uint8_t temp2;				// Temporary #2.
uint8_t temp3;				// Temporary #3.
uint8_t temp4;				// Temporary #4.

uint8_t spi_status_flag;	// SPI status flag.

uint8_t spi_rec_data[32];	// SPI received data.
uint8_t spi_trn_data[32];	// SPI transmit data.
uint8_t spi_data_cnt;		// SPI data transmission count.

uint8_t uart0_rec_cnt;		// UART #0 received count.
uint8_t uart0_rec_to;		// UART #0 received time out.
uint8_t uart0_trn_cnt;		// UART #0 transmit count.
uint8_t uart0_rec_data[32]; // UART #0 received data.

uint8_t delay_cnt;			// Delay count.

// LoRa task status flag.
// bit #0, TX task on.
// bit #1, RX task on.
// bit #2, RX task time out.
// bit #3, show data update. 
uint8_t lora_task_flag;		 
uint8_t lora_tx_task_cnt;	// LoRa TX task count.
uint8_t lora_rx_task_cnt;	// LoRa RX task count.
uint8_t lora_para_cnt;		// SX1272 register address / data count.
uint8_t lora_para_index;	// SX1272 register address / data index.
uint8_t rf_tx_data[12];		// TX data.
uint8_t rf_rx_data[12];		// RX data.

uint8_t *spi_trn_ptr;		// SPI transmit data pointer.
uint8_t *spi_rec_ptr;		// SPI received data pointer.
uint8_t *uart0_trn_ptr;		// UART #0 data transmit pointer.

uint8_t show_data[18];

uint32_t temp5;
uint32_t temp6;


#endif /* VAR_H_ */