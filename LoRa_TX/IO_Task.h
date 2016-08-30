/*
 * IO_Task.h
 *
 * Created: 2016/8/18 下午 03:53:38
 *  Author: tom hsieh
 */ 


#ifndef IO_TASK_H_
#define IO_TASK_H_

// IO initialize.
void IO_Init( void );
// Data LED on.
void IO_Data_LED_On( void );
// Data LED off.
void IO_Data_LED_Off( void );
// Data LED toggle.
void IO_Data_LED_Toggle( void );
// Error LED on.
void IO_Error_LED_On( void );
// Error LED off.
void IO_Error_LED_Off( void );


#endif /* IO_TASK_H_ */