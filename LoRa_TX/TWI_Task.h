/*
 * TWI_Task.h
 *
 * Created: 2016/6/27 上午 09:19:33
 *  Author: tom hsieh
 */ 


#ifndef TWI_TASK_H_
#define TWI_TASK_H_

// TWI #0 initialize.
void TWI0_Init( void );
// TWI #0 write task trigger on.
void TWI0_On_Write( void );
// TWI #0 read task trigger on.
void TWI0_On_Read( void );
// TWI #0 task.
void TWI0_Task( void );

#endif /* TWI_TASK_H_ */