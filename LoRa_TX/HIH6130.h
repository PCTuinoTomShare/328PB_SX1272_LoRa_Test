/* 
 * File:   HIH6130.h
 * Author: tom hsieh
 *
 * Created on 2014?11?1?, ?? 10:05
 */

#ifndef HIH6130_H
#define	HIH6130_H


#define HIH6130_WR  0x4e    // I2C address + Write.
#define HIH6130_RD  0x4f    // I2C address + Read.

// Measurement request.
void HIHI6130_Measure_Request( void );    
// Read humidity and temperature data.
void HIH6130_I2C_On_HT( void );
// Get humidity and temperature.
//void HIH6130_Get_HT( void );


#endif	/* HIH6130_H */

