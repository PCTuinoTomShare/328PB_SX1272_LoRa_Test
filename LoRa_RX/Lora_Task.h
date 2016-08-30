/*
 * Lora_Task.h
 *
 * Created: 2016/8/18 下午 05:16:59
 *  Author: tom hsieh
 */ 


#ifndef LORA_TASK_H_
#define LORA_TASK_H_

// LoRa RF initialize.
void LoRa_RF_Init( void );

// LoRa TX task, polling style.
void LoRa_Tx_Task_Poll( void );
// LoRa RX task.
void LoRa_Rx_Task_Poll( void );

// SX1272 set frequency.
void SX1272_Set_Freq( void );
// SX1272 RX switch on.
void SX1272_RX_Switch_On( void );
// SX1272 TX switch on.
void SX1272_TX_Switch_On( void );
// SX1272 Reset on.
void SX1272_Reset_On( void );
// SX1272 Reset off.
void SX1272_Reset_Off( void );
// Single register access trigger on, for polling task trigger on.
void SX1272_Reg_Access_On( void );
// Single register access. 
void SX1272_Reg_Access( void );
// SPI transmit loop.
void SX1272_SPI_Loop( void );

#endif /* LORA_TASK_H_ */