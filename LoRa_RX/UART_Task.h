/*
 * UART_Task.h
 *
 * Created: 2016/6/26 下午 11:36:33
 *  Author: tom hsieh
 */ 


#ifndef UART_TASK_H_
#define UART_TASK_H_

// UART #0 initialize.
void UART0_Init( void );
// UART #0 transmit task.
void UART0_Trn_Task( void );
// UART #0 received check.
void UART0_Rec_Check( void );
// Character to value.
void ToValue( void );
// Value to character.
void ToChar( void );

#endif /* UART_TASK_H_ */