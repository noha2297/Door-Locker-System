/*
 * i2c.h
 *
 *  Created on: Jan 22, 2020
 *      Author: Lenovo
 */

#ifndef I2C_H_
#define I2C_H_

#include "common_macros.h"
#include "std_types.h"
#include "macro_config.h"

#define INTERRUPT 0

#define BIT_RATE 2

typedef enum
{
	TWI_1 , TWI_4 , TWI_16 , TWI_64
}TWI_prescaler;


/* I2C Status Bits in the TWSR Register */
#define TW_START         0x08 // start has been sent
#define TW_REP_START     0x10 // repeated start
#define TW_MT_SLA_W_ACK  0x18 // Master transmit ( slave address + Write request ) to slave + Ack received from slave
#define TW_MT_SLA_R_ACK  0x40 // Master transmit ( slave address + Read request ) to slave + Ack received from slave
#define TW_MT_DATA_ACK   0x28 // Master transmit data and ACK has been received from Slave.
#define TW_MR_DATA_ACK   0x50 // Master received data and send ACK to slave
#define TW_MR_DATA_NACK  0x58 // Master received data but doesn't send ACK to slave

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

void TWI_init(uint8 a_deviceAddress , uint8 a_bitRate ,  TWI_prescaler prescaler);
void TWI_start();
void TWI_stop();
void TWI_write(uint8 data);
uint8 TWI_readWithACK();
uint8 TWI_readWithNACK();
uint8 TWI_getStatus();

#endif /* I2C_H_ */


