/*
 * TWI_Task.h
 *
 * Created: 2016/6/27 上午 09:19:33
 *  Author: tom hsieh
 */ 


#ifndef TWI_TASK_H_
#define TWI_TASK_H_

// TWI #1 initialize.
void TWI1_Init( void );
// TWI #1 write task trigger on.
void TWI1_On_Write( void );
// TWI #1 read task trigger on.
void TWI1_On_Read( void );
// TWI #1 task.
void TWI1_Task( void );

#endif /* TWI_TASK_H_ */